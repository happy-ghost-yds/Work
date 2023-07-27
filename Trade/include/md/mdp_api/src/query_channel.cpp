/**
 * Copyright (C) 2021，大连商品交易所 大连飞创信息技术有限公司
 *
 * @brief 查询通道
 *
 * 实现查询通道。连接服务器解析数据并回调。
 *
 */

#include "mdp/query_channel.hpp"

#include <chrono>

#include "mdp/log.hpp"
#include "mdp/net.hpp"

#include "mdp/common.hpp"
#include "mdp/engine.hpp"
#include "mdp/errors.hpp"
#include "mdp/pkg_defines.hpp"

namespace mdp {
// ======

query_channel::query_channel(void) : tcp_(new net::tcp_client(CHN_TCP)) {
    memset(&options_, 0, sizeof(options_));

    init();

    engine::ins();
}

query_channel::~query_channel(void) {
    if (tcp_->is_connected()) {
        tcp_->disconnect();
    }
}

void query_channel::set_options(const mdp::options& options) {
    options_ = options;
}

void query_channel::init() {
    tcp_->add_filter(pkg::filter_tcp_pkg_max_size);
    tcp_->add_filter(pkg::filter_tcp_pkg_min_size);
    tcp_->on_connected_ =
        std::bind(&query_channel::on_connected, this, std::placeholders::_1,
                  std::placeholders::_2, std::placeholders::_3);
    tcp_->on_disconnected_ =
        std::bind(&query_channel::on_disconnected, this, std::placeholders::_1,
                  std::placeholders::_2, std::placeholders::_3);
    tcp_->on_recv_ = std::bind(&query_channel::on_recv, this,
                               std::placeholders::_1, std::placeholders::_2,
                               std::placeholders::_3, std::placeholders::_4);
}

bool query_channel::is_connected() { return tcp_->is_connected(); }

int32_t query_channel::connect(const std::string& addr, uint32_t timeout_ms) {
    LOG_INFO("connect addr={0} timeout={1}ms", addr, timeout_ms);
    addr_ = addr;

    mdp::engine::ins();

    std::unique_lock<std::mutex> lck(lock_);

    int ret = tcp_->connect(addr);
    if (ret != 0) {
        LOG_ERROR("connect failed. addr={0} timeout={1}ms ret={2}", addr,
                  timeout_ms, ret);
        return ret;
    }

    // 同步等待
    std::chrono::seconds span_s(timeout_ms / 1000);
    auto status =
        cv_.wait_for(lck, span_s, [&] { return !tcp_->is_connecting(); });
    if (!status) {
        // timeout
        LOG_ERROR("connect timeout. addr={0} timeout={1}ms", addr, timeout_ms);
        return errors::timeout;
    }

    LOG_INFO("connect ok. addr={0} timeout={1}ms", addr, timeout_ms);
    return connect_code_;
}

int32_t query_channel::connect(const std::string& addr) {
    LOG_INFO("connect addr={0}", addr);
    addr_ = addr;

    mdp::engine::ins();

    std::unique_lock<std::mutex> lck(lock_);

    int ret = tcp_->connect(addr);
    if (ret != 0) {
        LOG_ERROR("connect failed. addr={0} ret={1}", addr, ret);
        return ret;
    }

    return 0;
}

int32_t query_channel::reconnect() {
    reconnecting_ = true;
    tcp_->disconnect();

    tcp_.reset(new net::tcp_client(CHN_TCP));

    init();

    std::unique_lock<std::mutex> lck(lock_);

    int ret = tcp_->connect(addr_);
    if (ret != 0) {
        LOG_ERROR("reconnect failed. addr={0} timeout={1}ms ret={2}", addr_,
                  options_.timeout_ms, ret);
        return ret;
    }

    // 同步等待
    std::chrono::seconds span_s(options_.timeout_ms / 1000);
    auto status =
        cv_.wait_for(lck, span_s, [&] { return !tcp_->is_connecting(); });
    if (!status) {
        // timeout
        LOG_ERROR("reconnect timeout. addr={0} timeout={1}ms", addr_,
                  options_.timeout_ms);
        return errors::timeout;
    }

    LOG_INFO("reconnect ok. addr={0} timeout={1}ms", addr_,
             options_.timeout_ms);
    return 0;
}

int32_t query_channel::disconnect(void) {
    LOG_INFO("check connect status={0}", tcp_->is_connected() ? 1 : 0);

    if (!tcp_->is_connected()) {
        LOG_ERROR("not connected");
        return errors::failed;
    }
    return tcp_->disconnect();
}

int32_t query_channel::post(uint64_t req_no, char* buf, size_t len) {
    LOG_DEBUG("post req_no={0} len={1}", req_no, len);

    if (!tcp_->is_connected()) {
        LOG_ERROR("have not connected");
        return errors::failed;
    }

    if (tcp_->post(req_no, buf, len) <= 0) {
        LOG_ERROR("tcp post fail req_no={0}", req_no);
        return errors::failed;
    }

    return 0;
}

int32_t query_channel::send(uint64_t req_no, char* buf, size_t len) {
    LOG_DEBUG("send req_no={0} len={1}", req_no, len);

    if (!tcp_->is_connected()) {
        LOG_ERROR("have not connected");
        return errors::failed;
    }

    if (tcp_->send(req_no, buf, len) <= 0) {
        LOG_ERROR("tcp send fail req_no={0}", req_no);
        return errors::failed;
    }

    return 0;
}

void query_channel::on_connected(int channel_id, int code,
                                 const std::string& msg) {
    connect_code_ = code;

    LOG_INFO("on_connected channel={0} code={1} msg={2}", channel_id, code,
             msg);

    cv_.notify_all();

    if (reconnecting_) {
        return;
    }

    if (on_connected_) {
        on_connected_(channel_id, code, msg);
    }
}

void query_channel::on_disconnected(int channel_id, int code,
                                    const std::string& msg) {
    LOG_INFO("on_disconnected channel={0} code={1} msg={2}", channel_id, code,
             msg);

    if (reconnecting_) {
        return;
    }

    if (on_disconnected_) {
        on_disconnected_(channel_id, code, msg);
    }
}

int query_channel::on_recv(int channel_id, char* data, size_t len,
                           const std::string& addr) {
    size_t offset = 0;

    // 解析协议头
    pkg::dmqp_header header;
    int ret = header.unmarshal(data, len);
    if (ret <= 0) {
        LOG_ERROR(
            "unmarshal header failed ret={0} channel={1} len={2} addr={3} "
            "header={4}",
            ret, channel_id, len, addr, header.to_string());
        return -1;
    }
    offset += ret;
    if (!header.is_valid()) {
        LOG_ERROR("header is invalid. channel={0} len={1} addr={2} header={3}",
                  channel_id, len, addr, header.to_string());
        return -1;
    }

    if (!pkg::tid::exists(header.id())) {
        LOG_ERROR("tid is undefined. channel={0} len={1} addr={2} header={3}",
                  channel_id, len, addr, header.to_string());
        return -1;
    }

    LOG_DEBUG("recv query pkg channel={0} len={1} addr={2} header={3}",
              channel_id, len, addr, header.to_string());

    // TCP通道有4个特殊tid需要拆包
    switch (header.id()) {
    case pkg::tid::rsp_query_quot_snap:
    case pkg::tid::rsp_query_ln_quot_snap: {
        pkg::field pre;
        size_t fld_offset = 0;
        // 只有第一个包有响应域，目前来看，max_no要么没有，要么都在第一个包中
        if (header.flag() == pkg::body::single ||
            header.flag() == pkg::body::first) {
            while (offset + fld_offset < len) {
                // 预读
                ret = pre.unmarshal(data + offset + fld_offset,
                                    len - offset - fld_offset);
                if (ret <= 0 || pre.field_size_ <= 0) {
                    LOG_ERROR(
                        "unmarshal field failed ret={0} channel={1} len={2} "
                        "addr={3} header={4}",
                        ret, channel_id, len, addr, header.to_string());
                    return 0;
                }

                // 起始包必须是rsp_msg
                if (fld_offset == 0 && pre.field_id_ != pkg::fid::rsp_msg) {
                    LOG_ERROR(
                        "package must start from rsp_msg. ret={0} channel={1} "
                        "len={2} addr={3} header={4}",
                        ret, channel_id, len, addr, header.to_string());
                    return 0;
                }

                if (pre.field_id_ != pkg::fid::max_no &&
                    pre.field_id_ != pkg::fid::rsp_msg) {
                    break;
                }

                fld_offset += pre.field_size_;
            }

            if (offset + fld_offset > len) {
                LOG_ERROR("unmarshal field failed ret={0} channel={1} len={2} "
                          "addr={3} "
                          "header={4}",
                          ret, channel_id, len, addr, header.to_string());
                return 0;
            }

            handle_single_pkg(channel_id, data + offset, fld_offset, addr,
                              header);
        }

        std::vector<uint16_t> flds;
        offset += fld_offset;
        fld_offset = 0;
        while (offset + fld_offset < len) {
            // 预读
            ret = pre.unmarshal(data + offset + fld_offset,
                                len - offset - fld_offset);
            if (ret <= 0 || pre.field_size_ <= 0) {
                LOG_ERROR("unmarshal field failed ret={0} channel={1} len={2} "
                          "addr={3} "
                          "header={4}",
                          ret, channel_id, len, addr, header.to_string());
                return 0;
            }

            // 起始包必须是extend_quot_common
            if (pre.field_id_ != pkg::fid::extend_quot_common) {
                LOG_ERROR("package must start from extend_quot_common. ret={0} "
                          "channel={1} len={2} addr={3} header={4}",
                          ret, channel_id, len, addr, header.to_string());
                auto fld = pkg::field::make(pre.field_id_);
                if (fld && 0 < fld->unmarshal(data + offset + fld_offset,
                                              len - offset - fld_offset)) {
                    LOG_ERROR("need extend_quot_common. get={0}, fld={1}",
                              pre.field_id_, fld->to_string());
                } else {
                    LOG_ERROR("need extend_quot_common. get={0}, len={1}",
                              pre.field_id_, pre.field_size_);
                }
                offset += pre.field_size_;
                continue;
            }

            fld_offset += pre.field_size_;
            while (offset + fld_offset < len) {
                ret = pre.unmarshal(data + offset + fld_offset,
                                    len - offset - fld_offset);
                if (ret <= 0 || pre.field_size_ <= 0) {
                    LOG_ERROR(
                        "unmarshal field failed ret={0} channel={1} len={2} "
                        "addr={3} header={4}",
                        ret, channel_id, len, addr, header.to_string());
                    return 0;
                }

                if (pre.field_id_ == pkg::fid::extend_quot_common) {
                    break;
                }

                flds.push_back(pre.field_id_);
                fld_offset += pre.field_size_;
            }

            if (offset + fld_offset > len) {
                LOG_ERROR("unmarshal field failed ret={0} channel={1} len={2} "
                          "addr={3} "
                          "header={4}",
                          ret, channel_id, len, addr, header.to_string());
                return 0;
            }

            // 判断包类型，调用对应的处理，需要修改header，设置body的attr
            header.pkg_type_ = pkg::get_tid_by_field_ids_snap(flds);
            handle_single_pkg(channel_id, data + offset, fld_offset, addr,
                              header);

            // 继续处理下一个小包
            offset += fld_offset;
            fld_offset = 0;
            flds.clear();
        }

        if (offset + fld_offset > len) {
            LOG_ERROR(
                "unmarshal field failed ret={0} channel={1} len={2} addr={3} "
                "header={4}",
                ret, channel_id, len, addr, header.to_string());
            return 0;
        }
    } break;
    case pkg::tid::rsp_query_history_quot:
    case pkg::tid::rsp_query_ln_history_quot: {
        // 只有第一个包有响应域
        if (header.flag() == pkg::body::single ||
            header.flag() == pkg::body::first) {
            // 处理响应域
            pkg::field pre;
            // 预读
            ret = pre.unmarshal(data + offset, len - offset);
            if (ret <= 0 || pre.field_size_ <= 0) {
                LOG_ERROR("unmarshal field failed ret={0} channel={1} len={2} "
                          "addr={3} "
                          "header={4}",
                          ret, channel_id, len, addr, header.to_string());
                return 0;
            }

            // 起始包必须是rsp_msg
            if (pre.field_id_ != pkg::fid::rsp_msg) {
                LOG_ERROR(
                    "package must start from rsp_msg. ret={0} channel={1} "
                    "len={2} addr={3} header={4}",
                    ret, channel_id, len, addr, header.to_string());
                return 0;
            }

            handle_single_pkg(channel_id, data + offset, pre.field_size_, addr,
                              header);
            offset += pre.field_size_;
        }

        if (len != offset) {
            // 处理DMDP报文头
            pkg::dmdp_header header;
            int ret = header.unmarshal(data + offset, len - offset);
            if (ret <= 0) {
                LOG_ERROR("unmarshal header failed ret={0} channel={1} len={2} "
                          "addr={3} header={4}",
                          ret, channel_id, len, addr, header.to_string());
                return -1;
            }
            offset += ret;
            if (!header.is_valid()) {
                LOG_ERROR("header is invalid. channel={0} len={1} addr={2} "
                          "header={3}",
                          channel_id, len, addr, header.to_string());
                return -1;
            }

            if (!pkg::tid::exists(header.id())) {
                LOG_ERROR(
                    "tid is undefined. channel={0} len={1} addr={2} header={3}",
                    channel_id, len, addr, header.to_string());
                return -1;
            }

            if (header.id() == pkg::tid::level_1_period ||
                header.id() == pkg::tid::level_2_period) {
                pkg::field pre;
                size_t fld_offset = 0;
                std::vector<uint16_t> flds;
                while (offset + fld_offset < len) {
                    // 预读
                    ret = pre.unmarshal(data + offset + fld_offset,
                                        len - offset - fld_offset);
                    if (ret <= 0 || pre.field_size_ <= 0) {
                        LOG_ERROR("unmarshal field failed ret={0} channel={1} "
                                  "len={2} "
                                  "addr={3} header={4}",
                                  ret, channel_id, len, addr,
                                  header.to_string());
                        return 0;
                    }

                    // 起始包必须是quot_comm
                    if (fld_offset == 0 &&
                        pre.field_id_ != pkg::fid::quot_common) {
                        LOG_ERROR("package must start from quot_comm. ret={0} "
                                  "channel={1} "
                                  "len={2} addr={3} header={4}",
                                  ret, channel_id, len, addr,
                                  header.to_string());
                        return 0;
                    }

                    // 再次找到quot_comm代表一个包结束
                    if (fld_offset != 0 &&
                        pre.field_id_ == pkg::fid::quot_common) {
                        // 判断包类型，调用对应的处理，需要修改header，设置body的attr
                        header.pkg_type_ = pkg::get_tid_by_field_ids(flds);
                        if (header.flag() == 0) {
                            header.set_seq_num(1);
                        }
                        handle_single_pkg(channel_id, data + offset, fld_offset,
                                          addr, header);

                        auto seq = header.start_seq_no();
                        header.set_start_seq_no(seq + 1);

                        // 继续处理下一个小包
                        offset += fld_offset;
                        fld_offset = 0;
                        flds.clear();
                        continue;
                    }

                    flds.push_back(pre.field_id_);
                    fld_offset += pre.field_size_;
                }

                if (offset + fld_offset > len) {
                    LOG_ERROR(
                        "unmarshal field failed ret={0} channel={1} len={2} "
                        "addr={3} header={4}",
                        ret, channel_id, len, addr, header.to_string());
                    return 0;
                }

                // 判断包类型，调用对应的处理，需要修改header，设置body的attr
                header.pkg_type_ = pkg::get_tid_by_field_ids(flds);
                handle_single_pkg(channel_id, data + offset, fld_offset, addr,
                                  header);
            } else {
                handle_single_pkg(channel_id, data + offset, len - offset, addr,
                                  header);
            }
        }
    } break;
    default: {
        handle_single_pkg(channel_id, data + offset, len - offset, addr,
                          header);
    } break;
    }

    return 0;
}

int query_channel::handle_single_pkg(int channel_id, char* data, size_t len,
                                     const std::string& addr,
                                     const pkg::dmdp_header& header) {
    // 解析业务头
    auto body = new pkg::body(header.id());
    body->init(header, false); // tcp通道可靠，直接全部解析
    auto ret = body->unmarshal(data, len);
    if (ret < 0) {
        LOG_ERROR("unmarshal body failed ret={0} channel={1} len={2} addr={3} "
                  "header={4} body={5}",
                  ret, channel_id, len, addr, header.to_string(),
                  body->to_string());

        delete body;
        body = nullptr;
        return -1;
    }

    LOG_DEBUG("recv channel={0} len={1} addr={2} header={3} body={4}",
              channel_id, len, addr, header.to_string(), body->to_string());

    if (nullptr != on_quote_data_) {
        on_quote_data_(channel_id, header, body);
    }

    delete body;
    body = nullptr;
    return 0;
}

int query_channel::handle_single_pkg(int channel_id, char* data, size_t len,
                                     const std::string& addr,
                                     const pkg::dmqp_header& header) {
    // 解析业务头
    auto body = new pkg::body(header.id());
    body->init(header, false); // tcp通道可靠，直接全部解析
    auto ret = body->unmarshal(data, len);
    if (ret < 0) {
        LOG_ERROR("unmarshal body failed ret={0} channel={1} len={2} addr={3} "
                  "header={4} body={5}",
                  ret, channel_id, len, addr, header.to_string(),
                  body->to_string());

        delete body;
        body = nullptr;
        return -1;
    }

    LOG_DEBUG("recv channel={0} len={1} addr={2} header={3} body={4}",
              channel_id, len, addr, header.to_string(), body->to_string());

    if (nullptr != on_query_data_) {
        on_query_data_(channel_id, header, body);
    }

    delete body;
    body = nullptr;
    return 0;
}

// ======
} // namespace mdp
