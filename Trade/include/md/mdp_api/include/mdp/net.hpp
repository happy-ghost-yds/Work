/**
 * Copyright (C) 2021，大连商品交易所 大连飞创信息技术有限公司
 *
 * @brief TCP和UDP客户端
 *
 * 封装libuv，实现tcp和udp客户端定义，并提供易用接口。
 *
 */

#ifndef __MDP_BASE_NET_HPP__
#define __MDP_BASE_NET_HPP__

#define NOMINMAX 1

#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "uv.h"

namespace mdp {
namespace net {
// ======

using handler_connected_t =
    std::function<void(int channel_id, int code, const std::string& msg)>;
using handler_disconnected_t =
    std::function<void(int channel_id, int code, const std::string& msg)>;
using handler_recv_t = std::function<int(int channel_id, char* buf, size_t len,
                                         const std::string& addr)>;
using filter_recv_t =
    std::function<bool(char* buf, size_t len, const std::string& addr)>;

enum {
    TCP_RECV_BUF_LEN = 1420,
    UDP_RECV_BUF_LEN = 1420,
    SEND_DATA_MAX = 1024,
    RECV_DATA_MAX = 1024
};

/// tcp_client TCP客户端
class tcp_client {
  public:
    tcp_client(void);
    virtual ~tcp_client(void);

    /// tcp_client 给出通道号
    explicit tcp_client(int channel_id);

    /// add_filter 设置拦截器
    void add_filter(filter_recv_t filter) { filters_.push_back(filter); }

    /// connect 连接
    int connect(const std::string& addr);

    /// disconnect 断开
    int disconnect();

    /// is_connected 是否已经连接成功
    bool is_connected() const { return connected_; }

    /// is_connecting 是否正在连接
    bool is_connecting() const { return try_connecting_; }

    /// post 发送数据
    size_t post(uint64_t req_no, char* data, size_t len);

    size_t send(uint64_t req_no, char* data, size_t len);

    // on_connected 底层回调
    void on_connected(int code, const std::string& msg);

    // on_disconnected 断开连接
    void on_disconnected(int code, const std::string& msg);

    // on_recv 接收数据
    size_t on_recv(char* data, size_t len);

    void on_sended(uint32_t req_no, int status);

    // 回调
    handler_connected_t on_connected_ = nullptr;
    handler_disconnected_t on_disconnected_ = nullptr;
    handler_recv_t on_recv_ = nullptr;

  private:
    static const int TCP_PKG_BUF_LEN = UINT16_MAX * 1024;
    int channel_id_ = 0;
    uv_connect_t connect_req_;
    std::unique_ptr<uv_tcp_t> tcp_;
    char* pkg_buffer_ = nullptr;
    char recv_buffer_[TCP_PKG_BUF_LEN];
    size_t recv_len_ = 0;
    size_t pkg_offset_ = 0; // 当前偏移量
    uint16_t pkg_len_ = 0;
    std::string addr_; // 连接地址
    bool try_connecting_ = false;
    bool connected_ = false;
    bool is_start_ = false;
    bool is_closed_ = false;
    std::vector<filter_recv_t> filters_; // 拦截器
    std::mutex wlock_;                   // 同步发送数据
    std::condition_variable cv_;         // 同步发送数据
    int status_ = 0;                     // 同步发送数据
};

/// udp_server UDP服务端
class udp_server {
  public:
    udp_server(void);
    virtual ~udp_server(void);

    /// udp_server 给出通道号
    /// @param channel_id
    explicit udp_server(int channel_id);

    /// add_filter 设置拦截器
    void add_filter(filter_recv_t filter) { filters_.push_back(filter); }

    /// listen 绑定地址并监听
    int listen(const std::string& local_addr, const std::string& multicast_ip);

    // on_recv 接收数据
    size_t on_recv(char* data, size_t len, const std::string& addr);

    // 回调
    handler_recv_t on_recv_{nullptr};

  private:
    int recv_start(void);

    std::unique_ptr<uv_udp_t> udp_;
    int channel_id_{0};
    char buffer_[UDP_RECV_BUF_LEN] = {0};
    size_t len_{0};
    bool is_start_{false};
    std::vector<filter_recv_t> filters_; // 拦截器
};

// ======
} // namespace net
} // namespace mdp
#endif // __MDP_BASE_NET_HPP__