/**
 * Copyright (C) 2021，大连商品交易所 大连飞创信息技术有限公司
 *
 * @brief 行情业务示例
 *
 * 行情业务处理和回调类。封装请求，并对回报或者行情消息进行处理。
 *
 */

#pragma once
#include <thread>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "md/baseMarketDataListener.h"
#include "mdp/quoter.hpp"
#include "common/Env.h"
#include "common/customQueue.h"

#define INSTRUMENT1 "rb2305"
#define INSTRUMENT2 "y2309"

/// DFITCMarketDataListener 行情示例
class DFITCMarketDataListener : public mdp::quoter_rsp,public BaseMarketDataListener {
  public:
    DFITCMarketDataListener(void);
    ~DFITCMarketDataListener(void);

    // from BaseMarketDataListener
    void start();
    void setDataManager(CMyDataManager* dataManager) {this->dataManager = dataManager;};
    void setDataTaskManager(CDataTaskManager* dataTaskManager) {this->dataTaskManager = dataTaskManager;};

    void dumpMarketData(mdp::mdp_ln_md* pDepthMarketData);
    
    /// set_options 选项
    int32_t set_options(const mdp::options& options);

    /// disconnect 关闭所有通道
    int32_t disconnect(void);

    /// 登出请求
    int32_t logout(void);

    /// 交易计划查询请求
    int32_t req_query_trade_plan(
        uint64_t req_no,
        const mdp::mdp_trade_plan_query_req& trade_plan_query_req);

    /// 合约基本信息查询请求 (同步)
    void req_query_contract_basic(
        uint64_t req_no,
        const mdp::mdp_contract_basic_info_req& contract_basic_info_req,
        mdp::mdp_rsp_msg& msg,
        std::vector<mdp::mdp_contract_basic_info_rsp>& contract_basic_info_rsp,
        int timeout_s = 30);

    /// 历史行情查询请求
    int32_t req_query_history_quot(
        uint64_t req_no,
        const mdp::mdp_history_quot_query& history_quot_query_req);

    /// 行情快照查询请求（同步）
    void
    req_query_quot_snap(uint64_t req_no,
                        const mdp::mdp_quot_snap_query_req& quot_snap_query_req,
                        mdp::mdp_rsp_msg& msg,
                        mdp::mdp_quot_snap_query_req& quot_snap_query_rsp,
                        int timeout_s = 30);

    /// ready 已准备好
    int32_t ready();

    /// on_connected 查询通道连接成功
    virtual void on_connected(int code, const std::string& msg);

    /// on_disconnected 查询通道断开连接
    virtual void on_disconnected(int code, const std::string& msg);

    /// 登录应答
    virtual void on_login(uint64_t req_no, const mdp::mdp_rsp_msg& msg,
                          mdp::mdp_trader_login_rsp& trader_login_rsp,
                          unsigned char chain_flag);

    /// 登出应答
    virtual void on_logout(uint64_t req_no, const mdp::mdp_rsp_msg& msg,
                           const mdp::mdp_trader_no& rsp_trader_no,
                           unsigned char chain_flag);

    // 交易计划查询应答
    virtual void
    on_rsp_query_trade_plan(uint64_t req_no, const mdp::mdp_rsp_msg& msg,
                            std::vector<mdp::mdp_trade_plan>& trade_plan_vec,
                            unsigned char chain_flag = 0);

    // 合约基本信息查询应答
    virtual void on_rsp_query_contract_basic(
        uint64_t req_no, const mdp::mdp_rsp_msg& msg,
        std::vector<mdp::mdp_contract_basic_info_rsp>& contract_basic_info_rsp,
        unsigned char chain_flag = 0);

    // 合约参数查询应答
    virtual void on_rsp_query_contract_param(
        uint64_t req_no, const mdp::mdp_rsp_msg& msg,
        std::vector<mdp::mdp_contract_param>& contract_param_vec,
        unsigned char chain_flag = 0);

    // 历史行情查询应答
    virtual void on_rsp_query_history_quot(
        uint64_t req_no, const mdp::mdp_rsp_msg& msg,
        const mdp::mdp_history_quot_query& history_quot_rsp,
        unsigned char chain_flag = 0);

    // 行情快照查询应答
    virtual void
    on_rsp_query_quot_snap(uint64_t req_no, const mdp::mdp_rsp_msg& msg,
                           const mdp::mdp_quot_snap_query_req& query_req,
                           std::vector<mdp::mdp_max_no>& quot_snap_query_rsp,
                           unsigned char chain_flag = 0);

    // 深度行情合约状态查询应答
    virtual void on_ln_rsp_quot_contract_status(
        uint64_t req_no, const mdp::mdp_rsp_msg& msg,
        std::vector<mdp::mdp_contract_status>& contract_status_vec,
        unsigned char chain_flag = 0);

    // 合约交易状态通知
    virtual void on_nty_quot_contract_status(
        const mdp::mdp_contract_status_header& status_header,
        std::vector<mdp::mdp_variety_status>& variety_status_vec,
        std::vector<mdp::mdp_contract_status>& contract_status_vec,
        unsigned char chain_flag = 0);

    // 市场状态通知
    virtual void
    on_nty_quot_mkt_status(const mdp::mdp_quot_mkt_status& mkt_status,
                           unsigned char chain_flag = 0);

    // 数据服务盘后通知
    virtual void
    on_nty_close_mkt_notice(const mdp::mdp_mkt_data_notice& mkt_data_notice,
                            unsigned char chain_flag = 0);

    // 行情通知
    virtual void on_best_quot(const mdp::mdp_best_quot& best_quot,
                              unsigned char chain_flag = 0);

    // 套利行情通知
    virtual void
    on_arbi_best_quot(const mdp::mdp_arbi_best_quot& arbi_best_quot,
                      unsigned char chain_flag = 0);

    // 期权参数通知
    virtual void on_option_parameter(const mdp::mdp_option_parameter& opt_para,
                                     unsigned char chain_flag = 0);

    // 分价成交量通知
    virtual void on_ln_segment_price_qty(
        const mdp::mdp_ln_quot_header& ln_quot_header,
        std::vector<mdp::mdp_ln_segment_price_qty>& ln_segment_price_qty_vec,
        unsigned char chain_flag = 0);

    // N档深度行情通知
    virtual void
    on_ln_mbl_quot(const mdp::mdp_ln_quot_header& ln_quot_header,
                   std::map<int64_t, mdp::mdp_mbl_level>& mbl_level_map_buy,
                   std::map<int64_t, mdp::mdp_mbl_level>& mbl_level_map_sell,
                   unsigned char chain_flag = 0);

    // 最优价位前十笔委托通知
    virtual void
    on_ln_best_level_orders(const mdp::mdp_ln_quot_header& ln_quot_header,
                            const mdp::mdp_price_pair& price_pair,
                            std::vector<mdp::mdp_qty_pair>& qty_pair_vec,
                            unsigned char chain_flag = 0);

    // 委托统计通知
    virtual void
    on_ln_order_statistic(const mdp::mdp_ln_order_statistic& ln_order_statistic,
                          unsigned char chain_flag = 0);

    //流控二级警告通知
    virtual void
    on_nty_flow_ctrl_warning(mdp::mdp_flow_ctrl_warning& flow_ctrl_warning,
                             unsigned char chain_flag = 0);

  private:
    DFITCMarketDataListener(const DFITCMarketDataListener& cpy) = delete;
    void operator=(const DFITCMarketDataListener& rhs) = delete;

    std::unique_ptr<mdp::quoter> impl_; // 实际的api
    uint64_t req_no;
    CMyDataManager* dataManager;
    CDataTaskManager* dataTaskManager;
    unsigned int seq = 0;
    std::thread m_pushMdThread;
    void _pushMarketData();
    customQueue<mdp::mdp_ln_md> m_MdQueue;
    mdp::mdp_ln_md ln_md; // ln行情数据结构
};
