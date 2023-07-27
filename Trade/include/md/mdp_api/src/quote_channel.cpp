/**
 * Copyright (C) 2021，大连商品交易所 大连飞创信息技术有限公司
 *
 * @brief 行情通道
 *
 * 实现行情通道。连接服务器解析数据并回调。
 *
 */

#include "mdp/quote_channel.hpp"

#include "mdp/log.hpp"
#include "mdp/net.hpp"
#include "mdp/strings.hpp"

#include "mdp/cache.hpp"
#include "mdp/common.hpp"
#include "mdp/engine.hpp"
#include "mdp/pkg_defines.hpp"

#include <vector>

namespace mdp {
// ======

quote_channel::quote_channel(void)
    : udp_1_(new net::udp_server(CHN_UDP_1)),
      udp_2_(new net::udp_server(CHN_UDP_2)) {
    memset(&options_, 0, sizeof(options_));

    udp_1_->add_filter(pkg::filter_udp_pkg_max_size);
    udp_1_->add_filter(pkg::filter_udp_pkg_min_size);
    udp_1_->on_recv_ = std::bind(&quote_channel::on_recv, this,
                                 std::placeholders::_1, std::placeholders::_2,
                                 std::placeholders::_3, std::placeholders::_4);

    udp_2_->add_filter(pkg::filter_udp_pkg_max_size);
    udp_2_->add_filter(pkg::filter_udp_pkg_min_size);
    udp_2_->on_recv_ = std::bind(&quote_channel::on_recv, this,
                                 std::placeholders::_1, std::placeholders::_2,
                                 std::placeholders::_3, std::placeholders::_4);

    engine::ins();
}

void quote_channel::set_options(const mdp::options& options) {
    options_ = options;
}

int32_t quote_channel::listen(const std::string& udp_addr_1,
                              const std::string& udp_addr_2) {
    // addr
    std::vector<std::string> addr_1;
    mdp::strings::split(addr_1, udp_addr_1, ",");
    std::vector<std::string> addr_2;
    mdp::strings::split(addr_2, udp_addr_2, ",");

    if (addr_1.size() < 2 || addr_2.size() < 2) {
        return -1;
    }

    // listen
    int ret = udp_1_->listen(addr_1[1], addr_1[0]);
    if (ret != 0) {
        return ret;
    }
    ret = udp_2_->listen(addr_2[1], addr_2[0]);
    if (ret != 0) {
        return ret;
    }
    return 0;
}

int quote_channel::on_recv(int channel_id, char* data, size_t len,
                           const std::string& addr) {
    size_t offset_ = 0;

    // 解析协议头
    pkg::dmdp_header header;
    int ret = header.unmarshal(data, len);
    if (ret <= 0) {
        LOG_ERROR(
            "unmarshal header failed ret={0} channel={1} len={2} addr={3} "
            "header={4}",
            ret, channel_id, len, addr, header.to_string());
        return 0;
    }
    offset_ += ret;

    if (!header.is_valid()) {
        LOG_ERROR("header is invalid. channel={0} len={1} addr={2} header={3}",
                  channel_id, len, addr, header.to_string());
        return 0;
    }

    if (!pkg::tid::exists(header.id())) {
        LOG_ERROR("tid is undefined. channel={0} len={1} addr={2} header={3}",
                  channel_id, len, addr, header.to_string());
        return 0;
    }

    LOG_DEBUG("recv quote pkg channel={0} len={1} addr={2} header={3}",
              channel_id, len, addr, header.to_string());

    // 组播通道有两个特殊tid需要拆包
    switch (header.id()) {
    case pkg::tid::level_1_period:
    case pkg::tid::level_2_period: {
        pkg::field pre;
        size_t fld_offset = 0;
        std::vector<uint16_t> flds;
        while (offset_ + fld_offset < len) {
            // 预读
            ret = pre.unmarshal(data + offset_ + fld_offset,
                                len - offset_ - fld_offset);
            if (ret <= 0 || pre.field_size_ <= 0) {
                LOG_ERROR("unmarshal field failed ret={0} channel={1} len={2} "
                          "addr={3} "
                          "header={4}",
                          ret, channel_id, len, addr, header.to_string());
                return 0;
            }

            // 起始包必须是quot_comm
            if (fld_offset == 0 && pre.field_id_ != pkg::fid::quot_common) {
                LOG_ERROR(
                    "package must start from quot_comm. ret={0} channel={1} "
                    "len={2} addr={3} header={4}",
                    ret, channel_id, len, addr, header.to_string());
                return 0;
            }

            // 再次找到quot_comm代表一个包结束
            if (fld_offset != 0 && pre.field_id_ == pkg::fid::quot_common) {
                // 判断包类型，调用对应的处理，需要修改header，设置body的attr
                header.pkg_type_ = pkg::get_tid_by_field_ids(flds);
                if (header.flag() == pkg::body::single) {
                    header.set_seq_num(1);
                }

                handle_single_pkg(channel_id, data + offset_, fld_offset, addr,
                                  header);

                auto seq = header.start_seq_no();
                header.set_start_seq_no(seq + 1);

                // 继续处理下一个小包
                offset_ += fld_offset;
                fld_offset = 0;
                flds.clear();
                continue;
            }

            flds.push_back(pre.field_id_);
            fld_offset += pre.field_size_;
        }

        if (offset_ + fld_offset > len) {
            LOG_ERROR(
                "unmarshal field failed ret={0} channel={1} len={2} addr={3} "
                "header={4}",
                ret, channel_id, len, addr, header.to_string());
            return 0;
        }

        // 判断包类型，调用对应的处理，需要修改header，设置body的attr
        header.pkg_type_ = pkg::get_tid_by_field_ids(flds);
        handle_single_pkg(channel_id, data + offset_, fld_offset, addr, header);
    } break;
    default: {
        handle_single_pkg(channel_id, data + offset_, len - offset_, addr,
                          header);
    } break;
    }

    // printf("dispatch pkg 0x%0x\n", header.id());

    // 触发其他逻辑
    dispatch(header.mdg_no());

    return 0;
}

int quote_channel::handle_single_pkg(int channel_id, char* data, size_t len,
                                     const std::string& addr,
                                     const pkg::dmdp_header& header) {
    // 解析业务头
    auto body = new pkg::body(header.id());
    body->init(header, true);

    // 心跳包，不缓存
    if (header.id() == pkg::tid::heart_beat) {
        // ready后才将心跳给用户，ready前丢弃心跳
        if (ready_) {
            if (nullptr != on_handler_) {
                on_handler_(channel_id, header, body);
            }
        }
        delete body;
        body = nullptr;
        return 0;
    }

    // 解析包体
    auto ret = body->unmarshal(data, len);
    if (ret <= 0) {
        LOG_ERROR("unmarshal body failed ret={0} channel={1} len={2} addr={3} "
                  "header={4} body={5}",
                  ret, channel_id, len, addr, header.to_string(),
                  body->to_string());
        delete body;
        body = nullptr;
        return 0;
    }

    LOG_DEBUG("recv channel={0} len={1} addr={2} header={3} body={4}",
              channel_id, len, addr, header.to_string(), body->to_string());

    // next以前的包丢弃
    if (header.start_seq_no() < cache_.get_next(header.mdg_no())) {
        LOG_DEBUG("old pkg lost channel={0} header={1}", channel_id,
                  header.to_string());
        delete body;
        body = nullptr;
        return 0;
    }

    // 数据包，缓存，双通道去重
    cache::data_key_t key;
    key.mdg_no = header.mdg_no();
    key.start_seq_no = header.start_seq_no();
    key.seq_num = header.seq_num();
    auto it = cache_.find(key);
    if (it == cache_.end()) {
        if (cache_.size() < options_.cache_size) {
            LOG_DEBUG("cache channel={0} header={1} size={2}", channel_id,
                      header.to_string(), cache_.size() + 1);
            cache_.emplace(std::make_pair(key, body)); // 不存在，直接添加
            set_head(key.mdg_no, key.start_seq_no);
        } else {
            LOG_DEBUG("beyond lost channel={0} header={1} size={2}", channel_id,
                      header.to_string(), cache_.size());
            delete body; // 超过缓存最大值则丢弃
            body = nullptr;
            return 0;
        }
    } else {
        LOG_DEBUG("repeat lost channel={0} header={1} size={2}", channel_id,
                  header.to_string(), cache_.size());
        delete body; // 已存在则清理当前包
        body = nullptr;
        return 0;
    }

    return 0;
}

void quote_channel::ready(bool ready) { ready_ = ready; }

// recover 进入恢复模式
void quote_channel::recover(bool recovering) { recovering_ = recovering; }

void quote_channel::clear_and_next(uint32_t mdg_no, uint64_t seq_no) {
    cache_.clear_and_next(mdg_no, seq_no);
}

void quote_channel::set_next(uint32_t mdg_no, uint64_t seq_no) {
    cache_.set_next(mdg_no, seq_no);
}

uint64_t quote_channel::get_next(uint32_t mdg_no) {
    return cache_.get_next(mdg_no);
}

// head 最新序列号
void quote_channel::set_head(uint32_t mdg_no, uint64_t seq_no) {
    cache_.set_head(mdg_no, seq_no);
}

// get_head 查看最新序列号
uint64_t quote_channel::get_head(uint32_t mdg_no) {
    return cache_.get_head(mdg_no);
}

/// set_lost 设置丢包的seq_no
void quote_channel::set_lost(uint32_t mdg_no, uint64_t seq_no) {
    cache_.set_lost(mdg_no, seq_no);
}

/// get_lost 获取丢包的seq_no
uint64_t quote_channel::get_lost(uint32_t mdg_no) {
    return cache_.get_lost(mdg_no);
}

// get_max_breakpoint 获取最大断点
uint64_t quote_channel::get_max_breakpoint(uint32_t mdg_no) {
    return cache_.get_max_breakpoint(mdg_no);
}

uint64_t quote_channel::dispatch(uint32_t mdg_no) {
    if (!ready_) {
        return 0;
    }

    while (1) {
        // 查看下一个seq_no应该收到的总包个数
        auto size = cache_.get_psize(mdg_no, cache_.get_next(mdg_no));

        // 查看下一个seq_no已经收到的包个数
        auto count = cache_.get_pcount(mdg_no, cache_.get_next(mdg_no));

        // 期望的next报文没有收到，跳出循环
        if (count == 0 || size == 0) {
            break;
        }

        // 如果相同，则说明已接收完整，回调通知用户
        if (size == count) {
            cache::data_key_t key;
            key.mdg_no = mdg_no;
            key.start_seq_no = get_next(mdg_no); // 下一个要处理的包seq_no
            for (uint64_t i = 1; i <= size; i++) {
                key.seq_num = i;
                auto body = cache_.at(key);
                auto ret = body->unmarshal(); // 将后面数据部分进行反序列化
                if (ret <= 0) {
                    LOG_ERROR("cache body unmarshal failed. body={0}",
                              body->to_string());
                    // 协议不匹配，暂时不处理，丢弃
                    cache_.erase(key);
                    continue;
                }
                if (nullptr != on_handler_) {
                    pkg::dmdp_header header;
                    header.pkg_type_ = body->id();
                    header.pkg_size_ =
                        static_cast<uint16_t>(sizeof(header) + body->length());
                    on_handler_(CHN_UDP_CACHE, header, body);
                }
                cache_.erase(key);
            }
            set_next(mdg_no, key.start_seq_no + 1);
        } else {
            LOG_DEBUG(
                "[dispatch]count!=size. mdg_no={0} next={1} size={2} count={3}",
                mdg_no, cache_.get_next(mdg_no), size, count);
            // 多包未收完整，跳出循环
            break;
        }
    }

    // 调用窗口处理
    auto next_no = cache_.get_next(mdg_no);
    auto distance = cache_.get_head(mdg_no) - (cache_.get_next(mdg_no) - 1);
    // printf("dispach msg distance %ld head %lu, next %lu \n", distance,
    // cache_.get_head(mdg_no), cache_.get_next(mdg_no));
    if (distance < options_.cache_win) {
        return 0;
    }
    auto max_no = cache_.get_max_breakpoint(mdg_no);
    // 已经达到窗口最大值，则请求进入恢复模式
    if (nullptr != req_recover_) {
        req_recover_(mdg_no, next_no, max_no);
        LOG_INFO("[dispatch]recover request sent. mdg_no={0} head={1} next={2} "
                 "max_no={3}",
                 mdg_no, cache_.get_head(mdg_no), next_no, max_no);
    }
    return 0;
}

// ======
} // namespace mdp
