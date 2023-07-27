/**
 * Copyright (C) 2021，大连商品交易所 大连飞创信息技术有限公司
 *
 * @brief TCP和UDP客户端
 *
 * 封装libuv，实现tcp和udp客户端定义，并提供易用接口。
 *
 */

#include "mdp/net.hpp"

#define NOMINMAX 1

#include <algorithm>
#include <memory.h>
#include <stdlib.h>
#include <vector>

#include "mdp/log.hpp"
#include "mdp/strings.hpp"

namespace mdp {
namespace net {
// ======

typedef struct {
    uv_write_t req;
    uv_buf_t buf;
    uint64_t req_no;
} write_req_t;

tcp_client::tcp_client(void) : tcp_(new uv_tcp_t) { tcp_->data = this; }

tcp_client::tcp_client(int channel_id)
    : channel_id_(channel_id), tcp_(new uv_tcp_t),
      pkg_buffer_(new char[TCP_PKG_BUF_LEN]) {
    tcp_->data = this;
}

tcp_client::~tcp_client(void) {
    if (is_start_) {
        // uv_recv_stop((uv_stream_t*)tcp_.get());
    }
    if (!is_closed_) {
        uv_close((uv_handle_t*)tcp_.get(), nullptr);
        is_closed_ = true;
    }
    if (nullptr != pkg_buffer_) {
        delete[] pkg_buffer_;
        pkg_buffer_ = nullptr;
    }
}

int tcp_client::connect(const std::string& addr) {
    LOG_INFO("connect ip={0}", addr);

    addr_ = addr;

    std::vector<std::string> addrs;
    mdp::strings::split(addrs, addr, ":");

    if (2 > addrs.size()) {
        LOG_ERROR("addr is invalid");
        return 1;
    }
    struct sockaddr_in remote_addr;
    uv_ip4_addr(addrs[0].c_str(), std::strtol(addrs[1].c_str(), nullptr, 10),
                &remote_addr);

    LOG_DEBUG("uv_tcp_init");
    int ret = uv_tcp_init(uv_default_loop(), tcp_.get());
    if (ret != 0) {
        LOG_ERROR("uv_udp_init ret={0}", ret);
        return 1;
    }
    LOG_DEBUG("uv_tcp_init ok");

    LOG_DEBUG("uv_tcp_connect");
    memset(&connect_req_, 0, sizeof(connect_req_));
    connect_req_.data = this;
    try_connecting_ = true;
    ret = uv_tcp_connect(
        &connect_req_, tcp_.get(), (const struct sockaddr*)&remote_addr,
        [](uv_connect_t* req, int status) {
            auto obj_ptr = (tcp_client*)req->data;

            if (status) {
                LOG_ERROR("uv_tcp_connect failed, or disconnected. status={0} "
                          "error={1}",
                          status, uv_err_name(status));
                if (obj_ptr->try_connecting_) {
                    obj_ptr->on_connected(1, "连接失败");
                } else {
                    obj_ptr->on_disconnected(1, "断开连接");
                }
                return;
            }
            LOG_INFO("uv_tcp_connect ok. status={0}", status);
            obj_ptr->on_connected(0, "连接成功");

            int ret = uv_read_start(
                (uv_stream_t*)(req->handle),
                [](uv_handle_t* handle, size_t len, uv_buf_t* buf) {
                    // alloc
                    auto obj_ptr = (tcp_client*)handle->data;
                    obj_ptr->recv_len_ = len;
                    buf->base = (char*)(obj_ptr->recv_buffer_);
                    buf->len = static_cast<unsigned long>(len);
                },
                [](uv_stream_t* client, ssize_t nread, const uv_buf_t* buf) {
                    // recv
                    auto obj_ptr = (tcp_client*)client->data;

                    if (0 == nread) {
                        LOG_DEBUG("n=0 no exception, unused buffer");
                        return;
                    }

                    if (nread < 0) {
                        if (nread != UV_EOF) {
                            LOG_DEBUG("on_disconnected msg={}",
                                      uv_err_name(static_cast<int>(nread)));
                            obj_ptr->on_disconnected(
                                2, uv_err_name(static_cast<int>(nread)));
                        } else {
                            LOG_DEBUG("on_disconnected eof");
                            obj_ptr->on_disconnected(3, "EOF");
                        }
                        return;
                    }

                    if (nread > 0) {
                        obj_ptr->on_recv(buf->base, nread);
                    }
                });
            LOG_INFO("uv_read_start. ret={0}", ret);
        });
    if (ret != 0) {
        LOG_ERROR("uv_tcp_connect ret={0}", ret);
        return 1;
    }
    LOG_INFO("uv_tcp_connect ok. ret=0");

    return 0;
}

int tcp_client::disconnect() {
    LOG_INFO("uv_close ok.");
    if (!is_connected() || is_closed_) {
        return 0;
    }
    uv_close((uv_handle_t*)tcp_.get(), nullptr);
    is_closed_ = true;
    return 0;
}

size_t tcp_client::post(uint64_t req_no, char* data, size_t len) {
    if (len > SEND_DATA_MAX) {
        return 0;
    }

    auto data_copy = malloc(len);
    memcpy(data_copy, data, len);
    write_req_t* req = (write_req_t*)malloc(sizeof(write_req_t));
    req->req.data = this;
    req->req_no = req_no;
    req->buf = uv_buf_init((char*)data_copy, static_cast<unsigned int>(len));
    uv_write((uv_write_t*)req, (uv_stream_t*)tcp_.get(), &req->buf, 1,
             [](uv_write_t* wreq, int status) {
                 write_req_t* wr = (write_req_t*)wreq;
                 auto obj_ptr = (tcp_client*)wr->req.data;
                 if (status) {
                     LOG_ERROR("uv_write failed. status={0}, req_no={1}",
                               status, wr->req_no);
                 }
                 LOG_DEBUG("client uv_write status={0}, req_no={1}", status,
                           wr->req_no);
                 obj_ptr->on_sended(static_cast<uint32_t>(wr->req_no), status);
                 free(wr->buf.base);
                 free(wr);
             });

    return len;
}

size_t tcp_client::send(uint64_t req_no, char* data, size_t len) {
    if (len > SEND_DATA_MAX) {
        return 0;
    }

    std::unique_lock<std::mutex> lock(wlock_);

    auto data_copy = malloc(len);
    memcpy(data_copy, data, len);
    write_req_t* req = (write_req_t*)malloc(sizeof(write_req_t));
    req->req.data = this;
    req->req_no = req_no;
    req->buf = uv_buf_init((char*)data_copy, static_cast<unsigned int>(len));
    LOG_DEBUG("client send req_no={0} len={1}", req_no, len);
    uv_write((uv_write_t*)req, (uv_stream_t*)tcp_.get(), &req->buf, 1,
             [](uv_write_t* wreq, int status) {
                 write_req_t* wr = (write_req_t*)wreq;
                 auto obj_ptr = (tcp_client*)wr->req.data;
                 if (status) {
                     LOG_ERROR("uv_write failed. status={0}, req_no={1}",
                               status, wr->req_no);
                 }
                 LOG_DEBUG("client uv_write status={0}, req_no={1}", status,
                           wr->req_no);
                 obj_ptr->on_sended(static_cast<uint32_t>(wr->req_no), status);
                 free(wr->buf.base);
                 free(wr);
             });

    std::chrono::seconds span_s(10);
    auto ret = cv_.wait_for(lock, span_s);
    if (ret == std::cv_status::timeout) {
        LOG_ERROR("send timeout. timeout={0}s", 10);
        return 0;
    }

    if (status_ != 0) {
        LOG_ERROR("send failed. status={0}", status_);
        return 0;
    }

    return len;
}

void tcp_client::on_sended(uint32_t req_no, int status) {
    status_ = status;
    cv_.notify_all();
}

void tcp_client::on_connected(int code, const std::string& msg) {
    LOG_INFO("on_connected code={0} msg={1}", code, msg);

    try_connecting_ = false;
    if (code != 0) {
        connected_ = false;
    } else {
        connected_ = true;
    }

    if (this->on_connected_) {
        on_connected_(channel_id_, code, msg);
    }
}

void tcp_client::on_disconnected(int code, const std::string& msg) {
    LOG_INFO("on_disconnected code={0} msg={1}", code, msg);

    try_connecting_ = false;
    connected_ = false;
    if (!is_closed_) {
        uv_close((uv_handle_t*)tcp_.get(), nullptr);
        is_closed_ = true;
    }
    if (this->on_disconnected_) {
        on_disconnected_(channel_id_, code, msg);
    }
}
size_t tcp_client::on_recv(char* data, size_t len) {
    // 未注册处理函数，丢弃
    if (nullptr == on_recv_) {
        LOG_DEBUG("on_recv is null. len={0}", len);
        return len;
    }

    size_t remain = len;
    size_t offset = 0;

    while (remain > 0) {
        // 获取可以拷贝到缓存中的长度
        size_t read = std::min((TCP_PKG_BUF_LEN - pkg_offset_), remain);

        // 复制
        memcpy(pkg_buffer_ + pkg_offset_, data + offset, read);
        remain -= read;
        pkg_offset_ += read;
        offset += read;

        if (pkg_offset_ < sizeof(uint16_t)) {
            break;
        }

        pkg_len_ = *(uint16_t*)pkg_buffer_;

        //处理buff中的完整包
        while (pkg_len_ <= pkg_offset_) {
            // 不符合拦截器要求，丢弃
            for (auto& item : filters_) {
                if (!item(data, pkg_len_, addr_)) {
                    // 关闭连接
                    uv_close((uv_handle_t*)tcp_.get(), nullptr);
                    is_closed_ = true;
                    if (this->on_disconnected_) {
                        on_disconnected_(channel_id_, 4,
                                         "协议不匹配，主动断开连接");
                    }
                    LOG_ERROR(
                        "filter and close, 协议不匹配，主动断开连接, len={0}",
                        pkg_len_);
                    return len;
                }
            }

            // 回调
            int ret_recv = on_recv_(channel_id_, pkg_buffer_, pkg_len_, addr_);
            if (0 != ret_recv) {
                // 关闭连接
                uv_close((uv_handle_t*)tcp_.get(), nullptr);
                is_closed_ = true;
                if (this->on_disconnected_) {
                    on_disconnected_(channel_id_, 4,
                                     "协议不匹配，主动断开连接");
                }
                LOG_ERROR("filter and close, 协议不匹配，主动断开连接, len={0}",
                          pkg_len_);
                return len;
            }

            // 整理buffer
            memmove(pkg_buffer_, pkg_buffer_ + pkg_len_,
                    pkg_offset_ - pkg_len_);
            pkg_offset_ -= pkg_len_; // pkg_offset_一定不小于0

            if (pkg_offset_ >= sizeof(uint16_t)) {
                // buffer中仍然有未处理的包
                //获取下一包长度
                pkg_len_ = *(uint16_t*)pkg_buffer_;
            } else {
                pkg_len_ = 0;
                break;
            }
        }
    }
    return len;
}

udp_server::udp_server(void) : udp_(new uv_udp_t) { udp_->data = this; }

udp_server::udp_server(int channel_id)
    : udp_(new uv_udp_t), channel_id_(channel_id) {
    udp_->data = this;
}

udp_server::~udp_server(void) {
    if (is_start_) {
        uv_udp_recv_stop(udp_.get());
    }

    if (!uv_is_closing((uv_handle_t*)udp_.get())) {
        uv_close((uv_handle_t*)udp_.get(), nullptr);
    }
}

int udp_server::listen(const std::string& local_addr,
                       const std::string& multicast_addr) {
    LOG_INFO("listen local={0}, multicast={1}", local_addr, multicast_addr);

    if (is_start_) {
        LOG_INFO("udp already start");
        return 0;
    }

    std::vector<std::string> multicast_addrs;
    mdp::strings::split(multicast_addrs, multicast_addr, ":");

    if (2 > multicast_addrs.size()) {
        LOG_ERROR("addr is invalid");
        return 1;
    }
    struct sockaddr_in addr;
#ifndef WIN32
    uv_ip4_addr(multicast_addrs[0].c_str(),
                std::strtol(multicast_addrs[1].c_str(), nullptr, 10), &addr);
#else
    uv_ip4_addr(local_addr.c_str(),
                std::strtol(multicast_addrs[1].c_str(), nullptr, 10), &addr);
#endif

    LOG_DEBUG("uv_udp_init");
    int ret = uv_udp_init(uv_default_loop(), udp_.get());
    if (ret != 0) {
        LOG_ERROR("uv_udp_init ret={0}", ret);
        return 1;
    }
    LOG_DEBUG("uv_udp_init ok");

    LOG_DEBUG("uv_udp_bind");
    ret = uv_udp_bind(udp_.get(), (const struct sockaddr*)&addr,
                      UV_UDP_REUSEADDR);
    if (ret != 0) {
        LOG_ERROR("uv_udp_bind ret={0}", ret);
        return 1;
    }
    LOG_DEBUG("uv_udp_bind ok\n");

    // 组播  UV_JOIN_GROUP/UV_LEAVE_GROUP
    LOG_DEBUG("uv_udp_set_membership ok");
    ret = uv_udp_set_membership(udp_.get(), multicast_addrs[0].c_str(),
                                local_addr.c_str(), UV_JOIN_GROUP);
    if (ret != 0) {
        LOG_ERROR("uv_udp_set_membership ret={0}", ret);
        return 1;
    }
    LOG_DEBUG("uv_udp_set_membership ok\n");

    return recv_start();
}

int udp_server::recv_start() {
    LOG_DEBUG("uv_udp_recv_start");

    int ret = uv_udp_recv_start(
        udp_.get(),
        [](uv_handle_t* handle, size_t len, uv_buf_t* buf) {
            // alloc
            auto obj_ptr = (udp_server*)handle->data;
            obj_ptr->len_ = len;
            buf->base = (char*)obj_ptr->buffer_;
            buf->len = static_cast<unsigned long>(len);
        },
        [](uv_udp_t* req, ssize_t nread, const uv_buf_t* buf,
           const struct sockaddr* addr, unsigned flags) {
            // on_recv
            auto obj_ptr = (udp_server*)req->data;

            if (0 == nread) {
                // unused buffer
                // addr == nullptr
                return;
            }

            if (nread < 0) {
                LOG_ERROR("error: unexcepted. udp ? n<0");
                return;
            }

            char sz_addr[32] = {0};
            uv_ip4_name((const struct sockaddr_in*)addr, sz_addr, 16);
            int port = ntohs(((const struct sockaddr_in*)addr)->sin_port);

            // 因为是demo, 所以这个addr地址先这样用直观的给了
            obj_ptr->on_recv(buf->base, nread,
                             std::string(sz_addr) + ":" + std::to_string(port));
        });
    if (ret != 0) {
        LOG_ERROR("uv_udp_recv_start ret={0}", ret);
        return 1;
    }
    is_start_ = true;
    LOG_DEBUG("uv_udp_recv_start ok");
    return 0;
}

size_t udp_server::on_recv(char* data, size_t len, const std::string& addr) {
    // 未注册处理函数，丢弃
    if (nullptr == on_recv_) {
        return len;
    }

    // 不符合拦截器要求，丢弃
    for (auto& item : filters_) {
        if (!item((char*)data, len, addr)) {
            LOG_ERROR("filter len={0} addr={1}", len, addr);
            return len;
        }
    }

    // 交由用户层处理
    on_recv_(channel_id_, (char*)data, len, addr);
    return len;
}

// ======
} // namespace net
} // namespace mdp
