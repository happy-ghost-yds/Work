/**
 * Copyright (C) 2021，大连商品交易所 大连飞创信息技术有限公司
 *
 * @brief 行情通道
 *
 * 实现行情通道。连接服务器解析数据并回调。
 *
 */

#ifndef __MDP_QUOTE_CHANNEL_HPP__
#define __MDP_QUOTE_CHANNEL_HPP__

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include "mdp/net.hpp"

#include "mdp/types.hpp"

#include "mdp/cache.hpp"
#include "mdp/common.hpp"
#include "mdp/pkg_defines.hpp"

namespace mdp {
// ======

using req_recover_func_t =
    std::function<void(uint32_t mdg_no, uint64_t start_no, uint64_t end_no)>;
using is_recovring_func_t = std::function<bool(uint32_t mdg_no)>;

/// quote_channel 组播通道
class quote_channel {
  public:
    quote_channel(void);

    /// 设置选项
    void set_options(const mdp::options& options);

    /// 监听组播通道
    int32_t listen(const std::string& udp_addr_1,
                   const std::string& udp_addr_2);

    // on_recv 接收数据
    int on_recv(int channel_id, char* data, size_t len,
                const std::string& addr);

    // handle_single_pkg 处理解析的单包
    int handle_single_pkg(int channel_id, char* data, size_t len,
                          const std::string& addr,
                          const pkg::dmdp_header& header);

    // ready 基础数据已准备好
    void ready(bool ready);

    // recover 进入恢复模式
    void recover(bool recovering);

    // clear_and_next 清理并更新下一个序列号
    void clear_and_next(uint32_t mdg_no, uint64_t seq_no);

    // set_next 下一个序列号
    void set_next(uint32_t mdg_no, uint64_t seq_no);

    // get_next 查看
    uint64_t get_next(uint32_t mdg_no);

    // set_head 最新序列号
    void set_head(uint32_t mdg_no, uint64_t seq_no);

    // get_head 查看最新序列号
    uint64_t get_head(uint32_t mdg_no);

    /// set_lost 设置丢包的seq_no
    void set_lost(uint32_t mdg_no, uint64_t seq_no);

    /// get_lost 获取丢包的seq_no
    uint64_t get_lost(uint32_t mdg_no);

    // get_max_breakpoint 获取最大断点
    uint64_t get_max_breakpoint(uint32_t mdg_no);

    /// 分发mdg组的数据
    uint64_t dispatch(uint32_t mdg_no);

    // 回调
    pkg::udp_handler_t on_handler_ = nullptr;
    req_recover_func_t req_recover_ = nullptr;

  private:
    // 通道
    std::unique_ptr<net::udp_server> udp_1_;
    std::unique_ptr<net::udp_server> udp_2_;

    // 数据
    bool ready_ = false;      // 基础数据是否准备好
    bool recovering_ = false; // 是否正在恢复
    cache cache_;             // 缓存
    mdp::options options_;    // 选项
};

// ======
} // namespace mdp
#endif // __MDP_QUOTE_CHANNEL_HPP__
