/**
 * Copyright (C) 2021，大连商品交易所 大连飞创信息技术有限公司
 *
 * @brief 查询通道
 *
 * 实现查询通道。连接服务器解析数据并回调。
 *
 */

#ifndef __MDP_QUERY_CHANNEL_HPP__
#define __MDP_QUERY_CHANNEL_HPP__

#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include "mdp/net.hpp"

#include "mdp/types.hpp"

#include "mdp/common.hpp"
#include "mdp/pkg_defines.hpp"

namespace mdp {
// ======

/// query_channel 查询通道
class query_channel {
  public:
    query_channel(void);
    ~query_channel(void);

    void set_options(const mdp::options& options);

    bool is_connected();

    int32_t connect(const std::string& addr, uint32_t timeout_ms);
    int32_t connect(const std::string& addr);

    int32_t reconnect();

    int32_t disconnect(void);

    int32_t post(uint64_t req_no, char* buf, size_t len);

    int32_t send(uint64_t req_no, char* buf, size_t len);

    // on_connected 底层回调
    void on_connected(int channel_id, int code, const std::string& msg);

    // on_disconnected 断开连接
    void on_disconnected(int channel_id, int code, const std::string& msg);

    // on_recv 接收数据
    int on_recv(int channel_id, char* data, size_t len,
                const std::string& addr);

    // handle_single_pkg 处理解析的单包
    int handle_single_pkg(int channel_id, char* data, size_t len,
                          const std::string& addr,
                          const pkg::dmqp_header& header);
    int handle_single_pkg(int channel_id, char* data, size_t len,
                          const std::string& addr,
                          const pkg::dmdp_header& header);

    // 回调
    handler_connected_t on_connected_ = nullptr;
    handler_disconnected_t on_disconnected_ = nullptr;
    pkg::tcp_handler_t on_query_data_ = nullptr;
    pkg::udp_handler_t on_quote_data_ = nullptr;

  private:
    void init();

    // 通道
    std::unique_ptr<net::tcp_client> tcp_;
    std::string addr_;

    // 同步
    std::mutex lock_;
    std::condition_variable cv_;

    // 数据
    bool connected_ = false;
    int32_t connect_code_ = 0;
    mdp::options options_;
    bool reconnecting_{false};
};

// ======
} // namespace mdp
#endif // __MDP_QUERY_CHANNEL_HPP__