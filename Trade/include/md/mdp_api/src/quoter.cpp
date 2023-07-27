/**
 * Copyright (C) 2021，大连商品交易所 大连飞创信息技术有限公司
 *
 * @brief 行情回调接口和封装。
 *
 * 定义用户需要实现的回调接口。封装查询通道和行情通道的连接和数据回调等。
 *
 */

#include "mdp/quoter.hpp"

#include <memory.h>

#include "mdp/quoter_impl.hpp"

namespace mdp {
// ======

quoter::quoter(void) : impl_(new quoter_impl()) {}

quoter::~quoter(void) {
    if (nullptr != impl_) {
        delete impl_;
        impl_ = nullptr;
    }
}

void quoter::set_rsp(quoter_rsp* rsp_ptr) { impl_->set_rsp(rsp_ptr); }

int32_t quoter::set_options(const mdp::options& options) {
    return impl_->set_options(options);
}

int32_t quoter::connect(const std::string& udp_addr_1,
                        const std::string& udp_addr_2,
                        const std::string& tcp_addr) {
    return impl_->connect(udp_addr_1, udp_addr_2, tcp_addr);
}

int32_t quoter::disconnect(void) { return impl_->disconnect(); }

// 登录请求 (同步)
void quoter::login(uint64_t req_no, const mdp_trader_login_req& req,
                   mdp_rsp_msg& msg, mdp_trader_login_rsp& rsp) {
    impl_->login(req_no, req, msg, rsp);
}

// 登出请求
int32_t quoter::logout(uint64_t req_no) {
    mdp_trader_no trader_no;
    memset(&trader_no, 0, sizeof(mdp_trader_no));
    return impl_->logout(req_no, trader_no);
}

// 交易计划查询请求
int32_t quoter::req_query_trade_plan(
    uint64_t req_no, const mdp_trade_plan_query_req& trade_plan_query_req) {
    return impl_->req_query_trade_plan(req_no, trade_plan_query_req);
}

// 合约基本信息查询请求 (同步)
void quoter::req_query_contract_basic(
    uint64_t req_no,
    const mdp::mdp_contract_basic_info_req& contract_basic_info_req,
    mdp::mdp_rsp_msg& msg,
    std::vector<mdp::mdp_contract_basic_info_rsp>& contract_basic_info_rsp,
    int timeout_s) {
    impl_->req_query_contract_basic(req_no, contract_basic_info_req, msg,
                                    contract_basic_info_rsp, timeout_s);
}

// 历史行情查询请求
int32_t quoter::req_query_history_quot(
    uint64_t req_no, const mdp_history_quot_query& history_quot_query_req) {
    return impl_->req_query_history_quot(req_no, history_quot_query_req);
}

// 行情快照查询请求 （同步）
void quoter::req_query_quot_snap(
    uint64_t req_no, const mdp::mdp_quot_snap_query_req& quot_snap_query_req,
    mdp::mdp_rsp_msg& msg, mdp::mdp_quot_snap_query_req& quot_snap_query_rsp,
    int timeout_s) {
    impl_->req_query_quot_snap(req_no, quot_snap_query_req, msg,
                               quot_snap_query_rsp, timeout_s);
}

// ready 已准备好
int32_t quoter::ready() { return impl_->ready(); }

uint32_t quoter::new_req_no() { return impl_->new_req_no(); }

// ======
} // namespace mdp