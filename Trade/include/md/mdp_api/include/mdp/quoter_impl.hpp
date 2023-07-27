/**
 * Copyright (C) 2021，大连商品交易所 大连飞创信息技术有限公司
 *
 * @brief 具体实现。
 *
 * 组合查询通道和行情通道的数据，并进行内外部转换然后回调给用户。
 *
 */

#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <tuple>
#include <unordered_map>

#include "mdp/net.hpp"
#include "mdp/timers.hpp"

#include "mdp/cache.hpp"
#include "mdp/common.hpp"
#include "mdp/pkg_defines.hpp"

#include "mdp/quoter.hpp"
#include "mdp/types.hpp"

namespace mdp {
// ======
enum qrying_type_e {
    no_qrying = 0,         // 没有正在查询
    user_qrying_quot = 1,  // 用户正在查询历史行情
    user_qrying_snap = 2,  // 用户正在查询行情快照
    api_qrying_recover = 3 // API正在恢复历史行情
};

class query_channel;
class quote_channel;

/// quoter_impl level n 内部实现
class quoter_impl {
  public:
    quoter_impl(void);
    ~quoter_impl(void);

    // 设置回调
    void set_rsp(quoter_rsp* rsp);

    // 生成新的请求号
    uint32_t new_req_no();

    // 设置选项
    int32_t set_options(const options& options);

    // connect 连接
    // @brief udp绑定端口，加入组播组监听。tcp 连接服务端。
    int32_t connect(const std::string& udp_addr_1,
                    const std::string& udp_addr_2, const std::string& tcp_addr);

    // disconnect 关闭所有通道
    int32_t disconnect(void);

    // 登录请求（同步）
    void login(uint64_t req_no, const mdp_trader_login_req& req,
               mdp_rsp_msg& msg, mdp_trader_login_rsp& rsp);

    // 登出请求
    int32_t logout(uint64_t req_no, const mdp_trader_no& req_trader_no);

    // 交易计划查询请求
    int32_t
    req_query_trade_plan(uint64_t req_no,
                         const mdp_trade_plan_query_req& trade_plan_query_req);

    // 合约基本信息查询请求（同步）
    void req_query_contract_basic(
        uint64_t req_no, const mdp_contract_basic_info_req& req,
        mdp_rsp_msg& msg,
        std::vector<mdp_contract_basic_info_rsp>& contract_basic_info_rsp,
        int timeout_s = 30);


    // 历史行情查询请求
    int32_t req_query_history_quot(
        uint64_t req_no, const mdp_history_quot_query& history_quot_query_req);

    // 行情快照查询请求（同步）
    void req_query_quot_snap(uint64_t req_no,
                             const mdp::mdp_quot_snap_query_req& req,
                             mdp::mdp_rsp_msg& msg,
                             mdp::mdp_quot_snap_query_req& quot_snap_query_rsp,
                             int timeout_s = 30);

    // ready 已准备好
    int32_t ready(bool ready = true);

    // post 发送数据（异步）
    int32_t post(mdp::pkg::body* body_ptr);

    // send 发送数据（同步）
    int32_t send(mdp::pkg::body* body_ptr);

    // on_connected 查询通道连接
    void on_connected(int channel_id, int code, const std::string& msg);

    // on_disconnected 查询通道断开连接
    void on_disconnected(int channel_id, int code, const std::string& msg);

    // on_query_data 查询数据
    void on_query_data(int channel_id, const pkg::dmqp_header& header,
                       pkg::body* body_ptr);

    // 登录应答
    void on_login(int channel_id, const pkg::header& header,
                  pkg::body* body_ptr);

    // 登出应答
    void on_logout(int channel_id, const pkg::header& header,
                   pkg::body* body_ptr);

    // 交易计划查询应答
    void on_handler_rsp_query_trade_plan(int channel_id,
                                         const pkg::header& header,
                                         pkg::body* body_ptr);

    // 合约基本信息查询应答
    void on_handler_rsp_query_contract_basic(int channel_id,
                                             const pkg::header& header,
                                             pkg::body* body_ptr);

    // 历史行情查询应答
    void on_handler_rsp_query_history_quot(int channel_id,
                                           const pkg::header& header,
                                           pkg::body* body_ptr);

    // 行情快照查询应答
    void on_handler_rsp_query_quot_snap(int channel_id,
                                        const pkg::header& header,
                                        pkg::body* body_ptr);

    // 合约交易状态通知
    void on_handler_nty_quot_contract_status(int channel_id,
                                             const pkg::header& header,
                                             pkg::body* body_ptr);

    // 市场状态通知
    void on_handler_nty_quot_mkt_status(int channel_id,
                                        const pkg::header& header,
                                        pkg::body* body_ptr);

    // 数据服务盘后通知
    void on_handler_nty_close_mkt_notice(int channel_id,
                                         const pkg::header& header,
                                         pkg::body* body_ptr);

    // 最优行情增量域初始化无效值
    void best_quot_set_invalid_value(mdp_best_quot& best_quot);
    // 套利最优行情增量域初始化无效值
    void arbi_best_quot_set_invalid_value(mdp_arbi_best_quot& arbi_best_quot);

    // 行情通知
    void on_handler_best_quot(int channel_id, const pkg::header& header,
                              pkg::body* body_ptr);

    // 套利行情通知
    void on_handler_arbi_best_quot(int channel_id, const pkg::header& header,
                                   pkg::body* body_ptr);

    // 期权参数通知
    void on_handler_option_parameter(int channel_id, const pkg::header& header,
                                     pkg::body* body_ptr);

    // 快照行情通知
    void on_handler_snap_best_quot(int channel_id, const pkg::header& header,
                                   pkg::body* body_ptr);

    // 快照套利行情通知
    void on_handler_snap_arbi_best_quot(int channel_id,
                                        const pkg::header& header,
                                        pkg::body* body_ptr);

    // 快照期权参数通知
    void on_handler_snap_option_parameter(int channel_id,
                                          const pkg::header& header,
                                          pkg::body* body_ptr);
#ifdef MDP_LN_QUOTER
    
    // 合约交易状态查询请求
    int32_t req_query_ln_contract_status(uint64_t req_no,
                                         const mdp_trader_no& req_trader_no);
    // 合约参数查询请求（异步）
    int32_t req_query_contract_param(
        uint64_t req_no,
        const mdp_contract_param_query_req& contract_param_query_req);

    // 定时N档深度行情通知
    void on_handler_ln_mbl_quot(int channel_id, const pkg::header& header,
                                pkg::body* body_ptr);

    // 最优价位前十笔委托通知
    void on_handler_ln_best_level_orders(int channel_id,
                                         const pkg::header& header,
                                         pkg::body* body_ptr);

    // 快照分价成交量通知
    void on_handler_ln_snap_segment_price_qty(int channel_id,
                                              const pkg::header& header,
                                              pkg::body* body_ptr);

    // 委托统计通知
    void on_handler_ln_order_statistic(int channel_id,
                                       const pkg::header& header,
                                       pkg::body* body_ptr);

    // 合约参数查询应答
    void on_handler_rsp_query_contract_param(int channel_id,
                                             const pkg::header& header,
                                             pkg::body* body_ptr);

    // 深度行情合约状态查询应答
    void on_ln_rsp_quot_contract_status(int channel_id,
                                        const pkg::header& header,
                                        pkg::body* body_ptr);

    // 分价成交量通知
    void on_handler_ln_segment_price_qty(int channel_id,
                                         const pkg::header& header,
                                         pkg::body* body_ptr);

    // 快照定时N档深度行情通知
    void on_handler_ln_snap_mbl_quot(int channel_id, const pkg::header& header,
                                     pkg::body* body_ptr);

    // 快照最优价位前十笔委托通知
    void on_handler_ln_snap_best_level_orders(int channel_id,
                                              const pkg::header& header,
                                              pkg::body* body_ptr);

    // 快照委托统计通知
    void on_handler_ln_snap_order_statistic(int channel_id,
                                            const pkg::header& header,
                                            pkg::body* body_ptr);
#endif

    // 流控二级警告通知
    void on_handler_nty_flow_ctrl_warning(int channel_id,
                                          const pkg::header& header,
                                          pkg::body* body_ptr);

    // 行情数据
    void on_quote_data(int channel_id, const pkg::dmdp_header& header,
                       pkg::body* body_ptr);

    // 行情心跳
    void on_quote_heart(int channel_id, const pkg::header& header,
                        pkg::body* body_ptr);
    // 查询心跳
    void on_query_heart(int channel_id, const pkg::header& header,
                        pkg::body* body_ptr);

    // on_timer_dispatch 分发缓存数据
    void on_timer_dispatch();

    // 心跳定时器启动
    void start_heart_timer();
    // 心跳定时器停止    
    void stop_heart_timer();

    // on_timer_heart 心跳定时器
    void on_timer_heart();

    // 重连定时器启动
    void start_reconnect_timer();
    // 重连定时器停止
    void stop_reconnect_timer();

    // on_timer_reconnect 重连定时器
    void on_timer_reconnect();

    // start_recover 请求恢复
    int32_t start_recover(uint32_t mdg_no, uint64_t start_no, uint64_t end_no);

    // stop_recover 完成恢复
    int32_t stop_recover(uint32_t mdg_no);

    // on_timer_lost_check 丢包超时定时器
    void on_timer_lost_check();

    // start_lost_check_timer 启动丢包超时检测定时器
    void start_lost_check_timer();

    // stop_lost_check_timer 停止丢包超时检测定时器
    void stop_lost_check_timer();

  private:
    // get_contract_id 获取合约号
    uint32_t get_contract_no(const char* contract_id);
    const char* get_contract_id(uint32_t contract_no);
    uint32_t get_mdg_no_by_contract_no(uint32_t contract_no);
    std::pair<int64_t, int64_t> get_tick_codec_price(uint32_t contract_no);
    int64_t real_price_by_tick_price(int64_t price, int64_t codec_price,
                                     int64_t tick);

    bool update_cur_seq_no(uint8_t mdg_no, uint64_t seq_no, bool force_update);

    // 回调
    quoter_rsp* rsp_ = nullptr;
    pkg::udp_handler_map_t udp_handler_map_; // 回调映射表
    pkg::tcp_handler_map_t tcp_handler_map_; // 回调映射表

    // 通道
    std::unique_ptr<query_channel> query_channel_; // 查询通道
    std::unique_ptr<quote_channel> quote_channel_; // 组播通道(1,2)
    mdp::timers::timer dispatch_timer_;

    // 同步
    std::mutex lock_;
    std::condition_variable cv_;
    std::mutex qrying_lock_;

    // 心跳
    mdp::timers::timer heart_timer_;
    mdp::timers::stop_watch heart_pong_watch_;
    bool heart_ping_{false};
    bool reconnecting_{false};
    bool reconnected_ok_{false};

    // 丢包超时定时器
    mdp::timers::timer lost_check_timer_;
    std::unordered_map<uint32_t, mdp::timers::stop_watch> lost_check_watch_;

    // 数据
    std::atomic<uint32_t> inner_req_no_{0}; // 内部单独编号的req_no
    options options_;
    bool ready_ = false;
    bool logined_ = false;
    mdp_trader_login_req login_req_;
    mdp_rsp_msg msg_;
    mdp_trader_login_rsp login_rsp_;
    mdp_history_quot_query history_quot_req_;
    std::unordered_map<uint32_t, mdp_contract_basic_info_rsp>
        contract_map_; // 合约基本信息表
    std::unordered_map<uint32_t, uint32_t>
        contract_mdg_map_; // 合约和mdg_no映射
    std::unordered_map<uint32_t, mdp_best_quot>
        best_quot_cache_map_; // 最优行情缓存
    std::unordered_map<uint32_t, mdp_arbi_best_quot>
        arbi_best_quot_cache_map_; // 套利最优行情缓存
    mbl_cache_t mbl_buy_cache_;    // 五档深度行情买队列缓存
    mbl_cache_t mbl_sell_cache_;   // 五档深度行情卖队列缓存
    mdp::mdp_quot_snap_query_req quot_snap_query_rsp_; // 缓存
    cache::pos_t cur_seq_no_;                          // 当前处理的编号
    qrying_type_e qrying_type_ = no_qrying;
    std::unordered_map<uint32_t, bool> recovering_map_;

    uint32_t req_no_;
    uint32_t rsp_no_;
};

// ======
} // namespace mdp
