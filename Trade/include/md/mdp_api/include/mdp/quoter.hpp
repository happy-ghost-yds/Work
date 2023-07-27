/**
 * Copyright (C) 2021，大连商品交易所 大连飞创信息技术有限公司
 *
 * @brief 行情回调接口和封装。
 *
 * 定义用户需要实现的回调接口。封装查询通道和行情通道的连接和数据回调等。
 *
 */

#ifndef __MDP_API_QUOTER_HPP__
#define __MDP_API_QUOTER_HPP__

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "mdp/exports.hpp"
#include "mdp/types.hpp"

namespace mdp {
// ======

// quoter_rsp level n 行情响应
class MDP_API quoter_rsp {
  public:
    virtual ~quoter_rsp(void) {}

    /// 行情心跳
    virtual void on_quote_heart(int channel_id,
                                const mdp_quote_heart& quote_heart) {}

    /// on_connected 查询通道连接成功
    virtual void on_connected(int code, const std::string& msg) = 0;

    // on_disconnected 查询通道断开连接
    virtual void on_disconnected(int code, const std::string& msg) = 0;

    // 登录应答
    virtual void on_login(uint64_t req_no, const mdp_rsp_msg& msg,
                          mdp_trader_login_rsp& trader_login_rsp,
                          unsigned char chain_flag = 0) = 0;

    // 登出应答
    virtual void on_logout(uint64_t req_no, const mdp_rsp_msg& msg,
                           const mdp_trader_no& rsp_trader_no,
                           unsigned char chain_flag = 0) = 0;

    // 交易计划查询应答
    virtual void
    on_rsp_query_trade_plan(uint64_t req_no, const mdp_rsp_msg& msg,
                            std::vector<mdp_trade_plan>& trade_plan_vec,
                            unsigned char chain_flag = 0) = 0;

    // 合约基本信息查询应答
    virtual void on_rsp_query_contract_basic(
        uint64_t req_no, const mdp_rsp_msg& msg,
        std::vector<mdp_contract_basic_info_rsp>& contract_basic_info_rsp_vec,
        unsigned char chain_flag = 0) = 0;

    // 合约参数查询应答
    virtual void on_rsp_query_contract_param(
        uint64_t req_no, const mdp_rsp_msg& msg,
        std::vector<mdp_contract_param>& contract_param_vec,
        unsigned char chain_flag = 0) = 0;

    // 历史行情查询应答
    virtual void
    on_rsp_query_history_quot(uint64_t req_no, const mdp_rsp_msg& msg,
                              const mdp_history_quot_query& history_quot_rsp,
                              unsigned char chain_flag = 0) = 0;

    // 行情快照查询应答
    virtual void on_rsp_query_quot_snap(
        uint64_t req_no, const mdp::mdp_rsp_msg& msg,
        const mdp::mdp_quot_snap_query_req& query_req,
        std::vector<mdp::mdp_max_no>& quot_snap_query_rsp_vec,
        unsigned char chain_flag = 0) = 0;

    // 深度行情合约状态查询应答
    virtual void on_ln_rsp_quot_contract_status(
        uint64_t req_no, const mdp_rsp_msg& msg,
        std::vector<mdp_contract_status>& contract_status_vec,
        unsigned char chain_flag = 0) = 0;

    // 合约交易状态通知
    virtual void on_nty_quot_contract_status(
        const mdp_contract_status_header& status_header,
        std::vector<mdp_variety_status>& variety_status_vec,
        std::vector<mdp_contract_status>& contract_status_vec,
        unsigned char chain_flag = 0) = 0;

    // 市场状态通知
    virtual void on_nty_quot_mkt_status(const mdp_quot_mkt_status& mkt_status,
                                        unsigned char chain_flag = 0) = 0;

    // 数据服务盘后通知
    virtual void
    on_nty_close_mkt_notice(const mdp_mkt_data_notice& mkt_data_notice,
                            unsigned char chain_flag = 0) = 0;

    // 行情通知
    virtual void on_best_quot(const mdp_best_quot& best_quot,
                              unsigned char chain_flag = 0) = 0;

    // 套利行情通知
    virtual void on_arbi_best_quot(const mdp_arbi_best_quot& arbi_best_quot,
                                   unsigned char chain_flag = 0) = 0;

    // 期权参数通知
    virtual void on_option_parameter(const mdp_option_parameter& opt_para,
                                     unsigned char chain_flag = 0) = 0;

    // 分价成交量通知
    virtual void on_ln_segment_price_qty(
        const mdp_ln_quot_header& ln_quot_header,
        std::vector<mdp_ln_segment_price_qty>& ln_segment_price_qty_vec,
        unsigned char chain_flag = 0) = 0;

    // N档深度行情通知
    virtual void
    on_ln_mbl_quot(const mdp_ln_quot_header& ln_quot_header,
                   std::map<int64_t, mdp_mbl_level>& mbl_level_map_buy,
                   std::map<int64_t, mdp_mbl_level>& mbl_level_map_sell,
                   unsigned char chain_flag = 0) = 0;

    // 最优价位前十笔委托通知
    virtual void
    on_ln_best_level_orders(const mdp_ln_quot_header& ln_quot_header,
                            const mdp_price_pair& price_pair,
                            std::vector<mdp_qty_pair>& qty_pair_vec,
                            unsigned char chain_flag = 0) = 0;

    // 委托统计通知
    virtual void
    on_ln_order_statistic(const mdp_ln_order_statistic& ln_order_statistic,
                          unsigned char chain_flag = 0) = 0;

    //流控二级警告通知
    virtual void
    on_nty_flow_ctrl_warning(mdp_flow_ctrl_warning& flow_ctrl_warning,
                             unsigned char chain_flag = 0) = 0;
};

// 内部实现前置声明
class quoter_impl;

/// quoter 行情请求
class MDP_API quoter {
  public:
    quoter(void);
    ~quoter(void);

    /// 设置回调
    void set_rsp(quoter_rsp* rsp_ptr);

    /// 生成新的请求号
    uint32_t new_req_no();

    /// set_options 选项
    int32_t set_options(const options& options);

    /// connect 连接
    /// @brief udp绑定端口，加入组播组监听。tcp 连接服务端。
    int32_t connect(const std::string& udp_addr_1,
                    const std::string& udp_addr_2, const std::string& tcp_addr);

    /// disconnect 关闭所有通道
    int32_t disconnect(void);

    /// 登出请求
    int32_t logout(uint64_t req_no);

    /// 登录请求(同步)
    void login(uint64_t req_no, const mdp_trader_login_req& req,
               mdp_rsp_msg& msg, mdp_trader_login_rsp& rsp);

    /// 交易计划查询请求
    int32_t
    req_query_trade_plan(uint64_t req_no,
                         const mdp_trade_plan_query_req& trade_plan_query_req);

    /// 合约基本信息查询请求（同步）
    void req_query_contract_basic(
        uint64_t req_no,
        const mdp_contract_basic_info_req& contract_basic_info_req,
        mdp_rsp_msg& msg,
        std::vector<mdp_contract_basic_info_rsp>& contract_basic_info_rsp,
        int timeout_s = 30);

    /// 历史行情查询请求
    int32_t req_query_history_quot(
        uint64_t req_no, const mdp_history_quot_query& history_quot_query_req);

    /// 行情快照查询请求（同步）
    void
    req_query_quot_snap(uint64_t req_no,
                        const mdp::mdp_quot_snap_query_req& quot_snap_query_req,
                        mdp::mdp_rsp_msg& msg,
                        mdp::mdp_quot_snap_query_req& quot_snap_query_rsp,
                        int timeout_s = 30);

    /// ready 已准备好
    int32_t ready();

  private:
    quoter(const quoter& cpy) = delete;
    void operator=(const quoter& rhs) = delete;

    quoter_impl* impl_; // 内部实现
};

// ======
} // namespace mdp
#endif // __MDP_API_QUOTER_HPP__