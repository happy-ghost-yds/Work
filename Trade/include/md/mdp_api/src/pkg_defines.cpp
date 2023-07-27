/**
 * Copyright (C) 2021，大连商品交易所 大连飞创信息技术有限公司
 *
 * @brief 组播行情包和域定义
 *
 * 组播行情数据包和域定义，打包解包功能等。
 *
 */

#include "mdp/pkg_defines.hpp"
#include "mdp/enc_dec.hpp"
#include "mdp/log.hpp"

#include <memory.h>
#include <sstream>

#define MDP_PROTO_TYPE 4  // 协议包头类型，指开放协议
#define MDP_MAX_SIZE 1400 // 包最大长度

// 版本号规则定义为单个数字
// (启用) 1段 直接用数表示
// (不用) 2段 majar.minor
// (不用) 3段 majar.minor.patch
//#define MDP_PROTO_VERSION_FUNC(majar,minor) (((majar)<<8)+(minor)) // 协议版本
//#define MDP_PROTO_VERSION MDP_PROTO_VERSION_FUNC(0,1) // v0.1
static uint8_t g_version_number = 1; // 当前版本
#define MDP_PROTO_VERSION g_version_number

namespace mdp {
namespace pkg {
// ======

uint8_t proto_version() { return MDP_PROTO_VERSION; }

// 方便测试使用，测试版本号规则
void set_proto_version(uint8_t v) { g_version_number = v; }

size_t max_size() { return MDP_MAX_SIZE; }

uint8_t proto_type() { return MDP_PROTO_TYPE; }

// FID列表
namespace fid {
bool exists(uint16_t fid) {
    switch (fid) {
    case quot_common: // 合约行情头域
        return true;
    case match_done: // 成交域
        return true;
    case high_low_price: // 最高最低价域
        return true;
    case settlement_price: // 结算价域
        return true;
    case buy_best_order: // 买最优委托域
        return true;
    case sell_best_order: // 卖最优委托域
        return true;
    case arbi_match_done: // 套利成交域
        return true;
    case buy_arbi_best_order: // 买套利最优委托域
        return true;
    case sell_arbi_best_order: // 卖套利最优委托域
        return true;
    case opt_param: // 期权参数域
        return true;
    case market_status: // 市场交易状态域
        return true;
    case variety_status: // 品种交易状态域
        return true;
    case contract_status: // 合约交易状态域
        return true;
    case data_notify: // 数据通知域
        return true;
    case mbl_quot: // 深度行情域
        return true;
    case price_pair: // 价格对域
        return true;
    case qty_pair: // 数量对域
        return true;
    case segment_price_qty: // 分价成交量域
        return true;
    case order_statistic_biz6: // 委托统计域
        return true;
    case extend_quot_common: // 扩展合约行情头域
        return true;
    case rsp_msg: // 响应域
        return true;
    case trader_login_req: // 登录请求域
        return true;
    case trader_login_rsp: // 登录应答域
        return true;
    case trader_id: // 席位号域
        return true;
    case flow_ctrl_warning: // 流控二级警告域
        return true;
    case req_contract_basic_info: // 合约基本信息查询请求域
        return true;
    case contract_basic_info: // 合约基本信息域
        return true;
    case contract_param: // 合约参数域
        return true;
    case req_trade_plan: // 交易计划查询请求域
        return true;
    case trade_plan: // 交易计划域
        return true;
    case qry_his_quot: // 查询历史行情域
        return true;
    case qry_quot_snap: // 查询行情快照域
        return true;
    case max_no: // 最大序号域
        return true;
    default:
        return false;
    }
}
} // namespace fid

// TID列表
namespace tid {
bool exists(uint16_t tid) {
    switch (tid) {
    case heart_beat: // 心跳通知报文
        return true;
    case level_1_period: // L1定时行情报文
        return true;
    case level_2_period: // L2定时行情报文
        return true;
    case nty_mkt_status: // 市场状态报文
        return true;
    case nty_trader_status: // 交易状态报文
        return true;
    case nty_close_mkt_notice: // 数据服务通知报文
        return true;
    case req_heart_beat: // 查询网关心跳请求
        return true;
    case rsp_heart_beat: // 查询网关心跳应答
        return true;
    case req_trader_login: // 席位登录请求
        return true;
    case rsp_trader_login: // 席位登录应答
        return true;
    case req_trader_logout: // 席位退出请求
        return true;
    case rsp_trader_logout: // 席位退出应答
        return true;
    case flow_ctrl_warning: // 向柜台发送二级警告
        return true;
    case req_query_contract_basic_info: // 合约基本信息查询请求
        return true;
    case rsp_query_contract_basic_info: // 合约基本信息查询应答
        return true;
    case req_qry_contract_status: // 合约交易状态查询请求
        return true;
    case rsp_qry_contract_status: // 合约交易状态查询应答
        return true;
    case req_qry_contract_param: // 合约参数查询请求
        return true;
    case rsp_qry_contract_param: // 合约参数查询应答
        return true;
    case req_qry_trade_plan: // 合约交易计划查询请求
        return true;
    case rsp_qry_trade_plan: // 交易计划查询应答
        return true;
    case req_query_history_quot: // 历史行情查询请求
        return true;
    case rsp_query_history_quot: // 历史行情查询应答
        return true;
    case req_query_quot_snap: // 行情快照查询请求
        return true;
    case rsp_query_quot_snap: // 行情快照查询应答
        return true;
    case req_query_ln_history_quot: // LN历史行情查询请求
        return true;
    case rsp_query_ln_history_quot: // LN历史行情查询应答
        return true;
    case req_query_ln_quot_snap: // LN行情快照查询请求
        return true;
    case rsp_query_ln_quot_snap: // LN行情快照查询应答
        return true;
    case quot: // 最优行情通知
        return true;
    case quot_snap: // 最优行情快照
        return true;
    case opt_greek_quot: // 期权参数通知
        return true;
    case opt_greek_quot_snap: // 期权参数快照
        return true;
    case ln_mbl_quot_biz6: // 五档深度行情通知
        return true;
    case ln_mbl_quot_biz6_snap: // 五档深度行情快照
        return true;
    case ln_best_level_orders_biz6: // 最优价位前十笔委托通知
        return true;
    case ln_best_level_orders_biz6_snap: // 最优价位前十笔委托快照
        return true;
    case ln_segment_price_match_qty: // 分价成交量通知
        return true;
    case ln_segment_price_match_qty_snap: // 分价成交量快照
        return true;
    case ln_order_statistics: // 委托统计通知
        return true;
    case ln_order_statistics_snap: // 委托统计快照
        return true;
    default:
        return false;
    }
}
} // namespace tid

uint16_t get_tid_by_field_ids(const std::vector<uint16_t>& flds) {
    for (auto& fld : flds) {
        switch (fld) {
        case fid::opt_param: {
            return tid::opt_greek_quot;
        }
        case fid::data_notify: {
            return tid::nty_close_mkt_notice;
        }
        case fid::market_status: {
            return tid::nty_mkt_status;
        }
        case fid::variety_status: {
            return tid::nty_trader_status;
        }
        case fid::contract_status: {
            return tid::nty_trader_status;
        }
        case fid::mbl_quot: {
            return tid::ln_mbl_quot_biz6;
        }
        case fid::price_pair: {
            return tid::ln_best_level_orders_biz6;
        }
        case fid::qty_pair: {
            return tid::ln_best_level_orders_biz6;
        }
        case fid::segment_price_qty: {
            return tid::ln_segment_price_match_qty;
        }
        case fid::order_statistic_biz6: {
            return tid::ln_order_statistics;
        }
        default:
            break;
        }
    }

    // 到这里代表是基本最优行情或者套利最优行情，根据quot_comm判断类型，需要对应到业务，所以统一返回quot，业务自己区分
    return tid::quot;
}

uint16_t get_tid_by_field_ids_snap(const std::vector<uint16_t>& flds) {
    for (auto& fld : flds) {
        switch (fld) {
        case fid::opt_param: {
            return tid::opt_greek_quot_snap;
        }
        case fid::mbl_quot: {
            return tid::ln_mbl_quot_biz6_snap;
        }
        case fid::price_pair: {
            return tid::ln_best_level_orders_biz6_snap;
        }
        case fid::qty_pair: {
            return tid::ln_best_level_orders_biz6_snap;
        }
        case fid::segment_price_qty: {
            return tid::ln_segment_price_match_qty_snap;
        }
        case fid::order_statistic_biz6: {
            return tid::ln_order_statistics_snap;
        }
        default:
            break;
        }
    }

    // 到这里代表是基本最优行情或者套利最优行情，根据quot_comm判断类型，需要对应到业务，所以统一返回quot_snap，业务自己区分
    return tid::quot_snap;
}

// is_relative
bool is_relative(uint16_t tid, uint16_t fid) {
    switch (tid) {
    case tid::heart_beat: { // 心跳通知报文
        return false;
    }
    case tid::level_1_period: { // L1定时行情报文
        switch (fid) {
        case fid::quot_common: // 合约行情头域
            return true;
        case fid::match_done: // 成交域
            return true;
        case fid::high_low_price: // 最高最低价域
            return true;
        case fid::settlement_price: // 结算价域
            return true;
        case fid::buy_best_order: // 买最优委托域
            return true;
        case fid::sell_best_order: // 卖最优委托域
            return true;
        case fid::arbi_match_done: // 套利成交域
            return true;
        case fid::buy_arbi_best_order: // 买套利最优委托域
            return true;
        case fid::sell_arbi_best_order: // 卖套利最优委托域
            return true;
        case fid::opt_param: // 期权参数域
            return true;
        default:
            return false;
        }
    }
    case tid::level_2_period: { // L2定时行情报文
        switch (fid) {
        case fid::quot_common: // 合约行情头域
            return true;
        case fid::match_done: // 成交域
            return true;
        case fid::high_low_price: // 最高最低价域
            return true;
        case fid::settlement_price: // 结算价域
            return true;
        case fid::buy_best_order: // 买最优委托域
            return true;
        case fid::sell_best_order: // 卖最优委托域
            return true;
        case fid::arbi_match_done: // 套利成交域
            return true;
        case fid::buy_arbi_best_order: // 买套利最优委托域
            return true;
        case fid::sell_arbi_best_order: // 卖套利最优委托域
            return true;
        case fid::opt_param: // 期权参数域
            return true;
        case fid::mbl_quot: // 深度行情域
            return true;
        case fid::price_pair: // 价格对域
            return true;
        case fid::qty_pair: // 数量对域
            return true;
        case fid::segment_price_qty: // 分价成交量域
            return true;
        case fid::order_statistic_biz6: // 委托统计域
            return true;
        default:
            return false;
        }
    }
    case tid::nty_mkt_status: { // 市场状态报文
        switch (fid) {
        case fid::market_status: // 市场交易状态域
            return true;
        default:
            return false;
        }
    }
    case tid::nty_trader_status: { // 交易状态报文
        switch (fid) {
        case fid::variety_status: // 品种交易状态域
            return true;
        case fid::contract_status: // 合约交易状态域
            return true;
        default:
            return false;
        }
    }
    case tid::nty_close_mkt_notice: { // 数据服务通知报文
        switch (fid) {
        case fid::data_notify: // 数据通知域
            return true;
        default:
            return false;
        }
    }
    case tid::req_heart_beat: { // 查询网关心跳请求
        return false;
    }
    case tid::rsp_heart_beat: { // 查询网关心跳应答
        return false;
    }
    case tid::req_trader_login: { // 席位登录请求
        switch (fid) {
        case fid::trader_login_req: // 登录请求域
            return true;
        default:
            return false;
        }
    }
    case tid::rsp_trader_login: { // 席位登录应答
        switch (fid) {
        case fid::rsp_msg: // 响应域
            return true;
        case fid::trader_login_rsp: // 登录应答域
            return true;
        default:
            return false;
        }
    }
    case tid::req_trader_logout: { // 席位退出请求
        switch (fid) {
        case fid::trader_id: // 席位号域
            return true;
        default:
            return false;
        }
    }
    case tid::rsp_trader_logout: { // 席位退出应答
        switch (fid) {
        case fid::rsp_msg: // 响应域
            return true;
        case fid::trader_id: // 席位号域
            return true;
        default:
            return false;
        }
    }
    case tid::flow_ctrl_warning: { // 向柜台发送二级警告
        switch (fid) {
        case fid::flow_ctrl_warning: // 流控二级警告域
            return true;
        default:
            return false;
        }
    }
    case tid::req_query_contract_basic_info: { // 合约基本信息查询请求
        switch (fid) {
        case fid::req_contract_basic_info: // 合约基本信息查询请求域
            return true;
        default:
            return false;
        }
    }
    case tid::rsp_query_contract_basic_info: { // 合约基本信息查询应答
        switch (fid) {
        case fid::rsp_msg: // 响应域
            return true;
        case fid::contract_basic_info: // 合约基本信息域
            return true;
        default:
            return false;
        }
    }
    case tid::req_qry_contract_status: { // 合约交易状态查询请求
        switch (fid) {
        case fid::trader_id: // 席位号域
            return true;
        default:
            return false;
        }
    }
    case tid::rsp_qry_contract_status: { // 合约交易状态查询应答
        switch (fid) {
        case fid::rsp_msg: // 响应域
            return true;
        case fid::contract_status: // 合约交易状态域
            return true;
        default:
            return false;
        }
    }
    case tid::req_qry_contract_param: { // 合约参数查询请求
        switch (fid) {
        case fid::trader_id: // 席位号域
            return true;
        default:
            return false;
        }
    }
    case tid::rsp_qry_contract_param: { // 合约参数查询应答
        switch (fid) {
        case fid::rsp_msg: // 响应域
            return true;
        case fid::contract_param: // 合约参数域
            return true;
        default:
            return false;
        }
    }
    case tid::req_qry_trade_plan: { // 合约交易计划查询请求
        switch (fid) {
        case fid::req_trade_plan: // 交易计划查询请求域
            return true;
        default:
            return false;
        }
    }
    case tid::rsp_qry_trade_plan: { // 交易计划查询应答
        switch (fid) {
        case fid::rsp_msg: // 响应域
            return true;
        case fid::trade_plan: // 交易计划域
            return true;
        default:
            return false;
        }
    }
    case tid::req_query_history_quot: { // 历史行情查询请求
        switch (fid) {
        case fid::qry_his_quot: // 查询历史行情域
            return true;
        default:
            return false;
        }
    }
    case tid::rsp_query_history_quot: { // 历史行情查询应答
        switch (fid) {
        case fid::rsp_msg: // 响应域
            return true;
        default:
            return false;
        }
    }
    case tid::req_query_quot_snap: { // 行情快照查询请求
        switch (fid) {
        case fid::qry_quot_snap: // 查询行情快照域
            return true;
        default:
            return false;
        }
    }
    case tid::rsp_query_quot_snap: { // 行情快照查询应答
        switch (fid) {
        case fid::rsp_msg: // 响应域
            return true;
        case fid::max_no: // 最大序号域
            return true;
        case fid::extend_quot_common: // 扩展合约行情头域
            return true;
        case fid::match_done: // 成交域
            return true;
        case fid::high_low_price: // 最高最低价域
            return true;
        case fid::settlement_price: // 结算价域
            return true;
        case fid::buy_best_order: // 买最优委托域
            return true;
        case fid::sell_best_order: // 卖最优委托域
            return true;
        case fid::arbi_match_done: // 套利成交域
            return true;
        case fid::buy_arbi_best_order: // 买套利最优委托域
            return true;
        case fid::sell_arbi_best_order: // 卖套利最优委托域
            return true;
        case fid::opt_param: // 期权参数域
            return true;
        default:
            return false;
        }
    }
    case tid::req_query_ln_history_quot: { // LN历史行情查询请求
        switch (fid) {
        case fid::qry_his_quot: // 查询历史行情域
            return true;
        default:
            return false;
        }
    }
    case tid::rsp_query_ln_history_quot: { // LN历史行情查询应答
        switch (fid) {
        case fid::rsp_msg: // 响应域
            return true;
        default:
            return false;
        }
    }
    case tid::req_query_ln_quot_snap: { // LN行情快照查询请求
        switch (fid) {
        case fid::qry_quot_snap: // 查询行情快照域
            return true;
        default:
            return false;
        }
    }
    case tid::rsp_query_ln_quot_snap: { // LN行情快照查询应答
        switch (fid) {
        case fid::rsp_msg: // 响应域
            return true;
        case fid::max_no: // 最大序号域
            return true;
        case fid::extend_quot_common: // 扩展合约行情头域
            return true;
        case fid::match_done: // 成交域
            return true;
        case fid::high_low_price: // 最高最低价域
            return true;
        case fid::settlement_price: // 结算价域
            return true;
        case fid::buy_best_order: // 买最优委托域
            return true;
        case fid::sell_best_order: // 卖最优委托域
            return true;
        case fid::arbi_match_done: // 套利成交域
            return true;
        case fid::buy_arbi_best_order: // 买套利最优委托域
            return true;
        case fid::sell_arbi_best_order: // 卖套利最优委托域
            return true;
        case fid::opt_param: // 期权参数域
            return true;
        case fid::mbl_quot: // 深度行情域
            return true;
        case fid::price_pair: // 价格对域
            return true;
        case fid::qty_pair: // 数量对域
            return true;
        case fid::segment_price_qty: // 分价成交量域
            return true;
        case fid::order_statistic_biz6: // 委托统计域
            return true;
        default:
            return false;
        }
    }
    case tid::quot: { // 最优行情通知
        return false;
    }
    case tid::quot_snap: { // 最优行情快照
        return false;
    }
    case tid::opt_greek_quot: { // 期权参数通知
        return false;
    }
    case tid::opt_greek_quot_snap: { // 期权参数快照
        return false;
    }
    case tid::ln_mbl_quot_biz6: { // 五档深度行情通知
        return false;
    }
    case tid::ln_mbl_quot_biz6_snap: { // 五档深度行情快照
        return false;
    }
    case tid::ln_best_level_orders_biz6: { // 最优价位前十笔委托通知
        return false;
    }
    case tid::ln_best_level_orders_biz6_snap: { // 最优价位前十笔委托快照
        return false;
    }
    case tid::ln_segment_price_match_qty: { // 分价成交量通知
        return false;
    }
    case tid::ln_segment_price_match_qty_snap: { // 分价成交量快照
        return false;
    }
    case tid::ln_order_statistics: { // 委托统计通知
        return false;
    }
    case tid::ln_order_statistics_snap: { // 委托统计快照
        return false;
    }
    default:
        return false;
    }
}

// field_count_of
size_t field_count_of(uint16_t tid) {
    switch (tid) {
    case tid::heart_beat: // 心跳通知报文
        return 0;
    case tid::level_1_period: // L1定时行情报文
        return 10;
    case tid::level_2_period: // L2定时行情报文
        return 15;
    case tid::nty_mkt_status: // 市场状态报文
        return 1;
    case tid::nty_trader_status: // 交易状态报文
        return 2;
    case tid::nty_close_mkt_notice: // 数据服务通知报文
        return 1;
    case tid::req_heart_beat: // 查询网关心跳请求
        return 0;
    case tid::rsp_heart_beat: // 查询网关心跳应答
        return 0;
    case tid::req_trader_login: // 席位登录请求
        return 1;
    case tid::rsp_trader_login: // 席位登录应答
        return 2;
    case tid::req_trader_logout: // 席位退出请求
        return 1;
    case tid::rsp_trader_logout: // 席位退出应答
        return 2;
    case tid::flow_ctrl_warning: // 向柜台发送二级警告
        return 1;
    case tid::req_query_contract_basic_info: // 合约基本信息查询请求
        return 1;
    case tid::rsp_query_contract_basic_info: // 合约基本信息查询应答
        return 2;
    case tid::req_qry_contract_status: // 合约交易状态查询请求
        return 1;
    case tid::rsp_qry_contract_status: // 合约交易状态查询应答
        return 2;
    case tid::req_qry_contract_param: // 合约参数查询请求
        return 1;
    case tid::rsp_qry_contract_param: // 合约参数查询应答
        return 2;
    case tid::req_qry_trade_plan: // 合约交易计划查询请求
        return 1;
    case tid::rsp_qry_trade_plan: // 交易计划查询应答
        return 2;
    case tid::req_query_history_quot: // 历史行情查询请求
        return 1;
    case tid::rsp_query_history_quot: // 历史行情查询应答
        return 1;
    case tid::req_query_quot_snap: // 行情快照查询请求
        return 1;
    case tid::rsp_query_quot_snap: // 行情快照查询应答
        return 12;
    case tid::req_query_ln_history_quot: // LN历史行情查询请求
        return 1;
    case tid::rsp_query_ln_history_quot: // LN历史行情查询应答
        return 1;
    case tid::req_query_ln_quot_snap: // LN行情快照查询请求
        return 1;
    case tid::rsp_query_ln_quot_snap: // LN行情快照查询应答
        return 17;
    case tid::quot: // 最优行情通知
        return 0;
    case tid::quot_snap: // 最优行情快照
        return 0;
    case tid::opt_greek_quot: // 期权参数通知
        return 0;
    case tid::opt_greek_quot_snap: // 期权参数快照
        return 0;
    case tid::ln_mbl_quot_biz6: // 五档深度行情通知
        return 0;
    case tid::ln_mbl_quot_biz6_snap: // 五档深度行情快照
        return 0;
    case tid::ln_best_level_orders_biz6: // 最优价位前十笔委托通知
        return 0;
    case tid::ln_best_level_orders_biz6_snap: // 最优价位前十笔委托快照
        return 0;
    case tid::ln_segment_price_match_qty: // 分价成交量通知
        return 0;
    case tid::ln_segment_price_match_qty_snap: // 分价成交量快照
        return 0;
    case tid::ln_order_statistics: // 委托统计通知
        return 0;
    case tid::ln_order_statistics_snap: // 委托统计快照
        return 0;
    default:
        return false;
    }
}

// ------
// err

const char* err::msg(int code) {
    // 注：C字符串常量可以返回
    switch (code) {
    case unknow:
        return "unknow";
    case ok:
        return "ok";
    case failed:
        return "failed";
    case timeout:
        return "timeout";
    case length_error:
        return "length error";
    case short_length:
        return "too short length";
    case long_length:
        return "too long length";
    case zero_length:
        return "len must be not zero";
    case null_buf:
        return "buf must be not null";
    case proto_err:
        return "proto error";
    case proto_ver_err:
        return "proto version error";
    case body_err:
        return "body error";
    case body_type_err:
        return "body type error";
    case attr_err:
        return "attr error";
    case attr_type_err:
        return "attr type error";
    case data_err:
        return "data error";
    case data_type_err:
        return "data type error";
    case field_err:
        return "field error";
    case field_type_err:
        return "field type error";
    default:
        return "undefined";
    }
}

// ------
// macros

// attribute

#define PKG_ATTR_TYPEID(fname, type)                                           \
    case tid::fname: {                                                         \
        return (type);                                                         \
    }

#define PKG_ATTR_TYPE(tid)                                                     \
    switch ((tid)) {                                                           \
        PKG_ATTR_TYPEID(heart_beat, nty)                                       \
        PKG_ATTR_TYPEID(level_1_period, nty)                                   \
        PKG_ATTR_TYPEID(level_2_period, nty)                                   \
        PKG_ATTR_TYPEID(nty_mkt_status, nty)                                   \
        PKG_ATTR_TYPEID(nty_trader_status, nty)                                \
        PKG_ATTR_TYPEID(nty_close_mkt_notice, nty)                             \
        PKG_ATTR_TYPEID(req_heart_beat, qry)                                   \
        PKG_ATTR_TYPEID(rsp_heart_beat, qry)                                   \
        PKG_ATTR_TYPEID(req_trader_login, qry)                                 \
        PKG_ATTR_TYPEID(rsp_trader_login, qry)                                 \
        PKG_ATTR_TYPEID(req_trader_logout, qry)                                \
        PKG_ATTR_TYPEID(rsp_trader_logout, qry)                                \
        PKG_ATTR_TYPEID(flow_ctrl_warning, nty)                                \
        PKG_ATTR_TYPEID(req_query_contract_basic_info, qry)                    \
        PKG_ATTR_TYPEID(rsp_query_contract_basic_info, qry)                    \
        PKG_ATTR_TYPEID(req_qry_contract_status, qry)                          \
        PKG_ATTR_TYPEID(rsp_qry_contract_status, qry)                          \
        PKG_ATTR_TYPEID(req_qry_contract_param, qry)                           \
        PKG_ATTR_TYPEID(rsp_qry_contract_param, qry)                           \
        PKG_ATTR_TYPEID(req_qry_trade_plan, qry)                               \
        PKG_ATTR_TYPEID(rsp_qry_trade_plan, qry)                               \
        PKG_ATTR_TYPEID(req_query_history_quot, qry)                           \
        PKG_ATTR_TYPEID(rsp_query_history_quot, qry)                           \
        PKG_ATTR_TYPEID(req_query_quot_snap, qry)                              \
        PKG_ATTR_TYPEID(rsp_query_quot_snap, qry)                              \
        PKG_ATTR_TYPEID(req_query_ln_history_quot, qry)                        \
        PKG_ATTR_TYPEID(rsp_query_ln_history_quot, qry)                        \
        PKG_ATTR_TYPEID(req_query_ln_quot_snap, qry)                           \
        PKG_ATTR_TYPEID(rsp_query_ln_quot_snap, qry)                           \
        PKG_ATTR_TYPEID(quot, qry)                                             \
        PKG_ATTR_TYPEID(quot_snap, qry)                                        \
        PKG_ATTR_TYPEID(opt_greek_quot, qry)                                   \
        PKG_ATTR_TYPEID(opt_greek_quot_snap, qry)                              \
        PKG_ATTR_TYPEID(ln_mbl_quot_biz6, qry)                                 \
        PKG_ATTR_TYPEID(ln_mbl_quot_biz6_snap, qry)                            \
        PKG_ATTR_TYPEID(ln_best_level_orders_biz6, qry)                        \
        PKG_ATTR_TYPEID(ln_best_level_orders_biz6_snap, qry)                   \
        PKG_ATTR_TYPEID(ln_segment_price_match_qty, qry)                       \
        PKG_ATTR_TYPEID(ln_segment_price_match_qty_snap, qry)                  \
        PKG_ATTR_TYPEID(ln_order_statistics, qry)                              \
        PKG_ATTR_TYPEID(ln_order_statistics_snap, qry)                         \
    default: { return unknow; }                                                \
    }

// --------
// dmdp_header
dmdp_header::dmdp_header()
    : header(), pkg_size_(0), pkg_type_(0), version_(proto_version()), flag_(0),
      mdg_no_(0), start_seq_no_(0), seq_num_(0), send_time_(0), reserved_(0) {}

// id 标识
uint16_t dmdp_header::id() const { return pkg_type_; }

// length 长度大小 字节
size_t dmdp_header::length() {
    return 0 + sizeof(pkg_size_) + sizeof(pkg_type_) + sizeof(version_) +
           sizeof(flag_) + sizeof(mdg_no_) + sizeof(start_seq_no_) +
           sizeof(seq_num_) + sizeof(send_time_) + sizeof(reserved_);
}

// is_valid 判断包头是否有效
bool dmdp_header::is_valid() {
    if (pkg_size_ < length()) {
        return false;
    }
    return true;
}

// marshal 序列化
int32_t dmdp_header::marshal(char* buf, size_t len) const {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }
    if (len < length()) {
        return err::short_length;
    }

    size_t offset = 0;

    memcpy(buf + offset, &pkg_size_, sizeof(pkg_size_));
    offset += sizeof(pkg_size_);
    memcpy(buf + offset, &pkg_type_, sizeof(pkg_type_));
    offset += sizeof(pkg_type_);
    memcpy(buf + offset, &version_, sizeof(version_));
    offset += sizeof(version_);
    memcpy(buf + offset, &flag_, sizeof(flag_));
    offset += sizeof(flag_);
    memcpy(buf + offset, &mdg_no_, sizeof(mdg_no_));
    offset += sizeof(mdg_no_);
    memcpy(buf + offset, &start_seq_no_, sizeof(start_seq_no_));
    offset += sizeof(start_seq_no_);
    memcpy(buf + offset, &seq_num_, sizeof(seq_num_));
    offset += sizeof(seq_num_);
    memcpy(buf + offset, &send_time_, sizeof(send_time_));
    offset += sizeof(send_time_);
    memcpy(buf + offset, &reserved_, sizeof(reserved_));
    offset += sizeof(reserved_);
    return static_cast<int32_t>(offset);
}

// unmarshal 反序列化
int32_t dmdp_header::unmarshal(const char* buf, size_t len) {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }

    size_t offset = 0;
    memcpy(&pkg_size_, buf + offset, sizeof(pkg_size_));
    offset += sizeof(pkg_size_);
    if (len < pkg_size_) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }

    memcpy(&pkg_type_, buf + offset, sizeof(pkg_type_));
    offset += sizeof(pkg_type_);
    memcpy(&version_, buf + offset, sizeof(version_));
    offset += sizeof(version_);
    memcpy(&flag_, buf + offset, sizeof(flag_));
    offset += sizeof(flag_);
    memcpy(&mdg_no_, buf + offset, sizeof(mdg_no_));
    offset += sizeof(mdg_no_);
    memcpy(&start_seq_no_, buf + offset, sizeof(start_seq_no_));
    offset += sizeof(start_seq_no_);
    memcpy(&seq_num_, buf + offset, sizeof(seq_num_));
    offset += sizeof(seq_num_);
    memcpy(&send_time_, buf + offset, sizeof(send_time_));
    offset += sizeof(send_time_);
    memcpy(&reserved_, buf + offset, sizeof(reserved_));
    offset += sizeof(reserved_);

    return static_cast<int32_t>(offset);
}

void dmdp_header::set_pkg_size(uint16_t pkg_size) { pkg_size_ = pkg_size; }

uint16_t dmdp_header::pkg_size() const { return pkg_size_; }

void dmdp_header::set_pkg_type(uint16_t pkg_type) { pkg_type_ = pkg_type; }

uint16_t dmdp_header::pkg_type() const { return pkg_type_; }

void dmdp_header::set_version(uint8_t version) { version_ = version; }

uint8_t dmdp_header::version() const { return version_; }

void dmdp_header::set_flag(uint8_t flag) { flag_ = flag; }

uint8_t dmdp_header::flag() const { return flag_; }

void dmdp_header::set_mdg_no(uint8_t mdg_no) { mdg_no_ = mdg_no; }

uint8_t dmdp_header::mdg_no() const { return mdg_no_; }

void dmdp_header::set_start_seq_no(uint64_t start_seq_no) {
    start_seq_no_ = start_seq_no;
}

uint64_t dmdp_header::start_seq_no() const { return start_seq_no_; }

void dmdp_header::set_seq_num(uint8_t seq_num) { seq_num_ = seq_num; }

uint8_t dmdp_header::seq_num() const { return seq_num_; }

void dmdp_header::set_send_time(uint64_t send_time) { send_time_ = send_time; }

uint64_t dmdp_header::send_time() const { return send_time_; }

void dmdp_header::set_reserved(int8_t reserved) { reserved_ = reserved; }

int8_t dmdp_header::reserved() const { return reserved_; }

// to_string 生成描述文字
std::string dmdp_header::to_string(void) const {
    std::stringstream oss;
    oss << "{ typename:dmdp_header"
        << ", pkg_size:" << pkg_size() << ", pkg_type:0x" << std::hex
        << pkg_type_ << std::dec << ", version:" << (uint16_t)version_
        << ", flag:" << (uint16_t)flag_ << ", mdg_no:" << (uint16_t)mdg_no_
        << ", start_seq_no:" << start_seq_no_
        << ", seq_num:" << (uint16_t)seq_num_ << ", send_time:" << send_time_
        << ", reserved:" << (int16_t)reserved_ << "}";
    return oss.str();
}

// --------
// dmqp_header
dmqp_header::dmqp_header()
    : header(), pkg_size_(0), pkg_type_(0), version_(proto_version()), flag_(0),
      request_no_(0), reserved_(0) {}

// id 标识
uint16_t dmqp_header::id() const { return pkg_type_; }

// length 长度大小 字节
size_t dmqp_header::length() {
    return 0 + sizeof(pkg_size_) + sizeof(pkg_type_) + sizeof(version_) +
           sizeof(flag_) + sizeof(request_no_) + sizeof(reserved_);
}

// is_valid 判断包头是否有效
bool dmqp_header::is_valid() {
    if (pkg_size_ < length()) {
        return false;
    }
    return true;
}

// marshal 序列化
int32_t dmqp_header::marshal(char* buf, size_t len) const {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }
    if (len < length()) {
        return err::short_length;
    }

    size_t offset = 0;

    memcpy(buf + offset, &pkg_size_, sizeof(pkg_size_));
    offset += sizeof(pkg_size_);
    memcpy(buf + offset, &pkg_type_, sizeof(pkg_type_));
    offset += sizeof(pkg_type_);
    memcpy(buf + offset, &version_, sizeof(version_));
    offset += sizeof(version_);
    memcpy(buf + offset, &flag_, sizeof(flag_));
    offset += sizeof(flag_);
    memcpy(buf + offset, &request_no_, sizeof(request_no_));
    offset += sizeof(request_no_);
    memcpy(buf + offset, &reserved_, sizeof(reserved_));
    offset += sizeof(reserved_);
    return static_cast<int32_t>(offset);
}

// unmarshal 反序列化
int32_t dmqp_header::unmarshal(const char* buf, size_t len) {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }

    size_t offset = 0;
    memcpy(&pkg_size_, buf + offset, sizeof(pkg_size_));
    offset += sizeof(pkg_size_);
    if (len < pkg_size_) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }

    memcpy(&pkg_type_, buf + offset, sizeof(pkg_type_));
    offset += sizeof(pkg_type_);
    memcpy(&version_, buf + offset, sizeof(version_));
    offset += sizeof(version_);
    memcpy(&flag_, buf + offset, sizeof(flag_));
    offset += sizeof(flag_);
    memcpy(&request_no_, buf + offset, sizeof(request_no_));
    offset += sizeof(request_no_);
    memcpy(&reserved_, buf + offset, sizeof(reserved_));
    offset += sizeof(reserved_);

    return static_cast<int32_t>(offset);
}

void dmqp_header::set_pkg_size(uint16_t pkg_size) { pkg_size_ = pkg_size; }

uint16_t dmqp_header::pkg_size() const { return pkg_size_; }

void dmqp_header::set_pkg_type(uint16_t pkg_type) { pkg_type_ = pkg_type; }

uint16_t dmqp_header::pkg_type() const { return pkg_type_; }

void dmqp_header::set_version(uint8_t version) { version_ = version; }

uint8_t dmqp_header::version() const { return version_; }

void dmqp_header::set_flag(uint8_t flag) { flag_ = flag; }

uint8_t dmqp_header::flag() const { return flag_; }

void dmqp_header::set_request_no(uint32_t request_no) {
    request_no_ = request_no;
}

uint32_t dmqp_header::request_no() const { return request_no_; }

void dmqp_header::set_reserved(int8_t reserved) { reserved_ = reserved; }

int8_t dmqp_header::reserved() const { return reserved_; }

// to_string 生成描述文字
std::string dmqp_header::to_string(void) const {
    std::stringstream oss;
    oss << "{ typename:dmqp_header"
        << ", pkg_size:" << pkg_size() << ", pkg_type:0x" << std::hex
        << pkg_type_ << std::dec << ", version:" << (uint16_t)version_
        << ", flag:" << (uint16_t)flag_ << ", request_no:" << request_no_
        << ", reserved:" << (int16_t)reserved_ << "}";
    return oss.str();
}

// -------------
// field

field* field::make(uint16_t fid) {
    switch (fid) {
    case fid::quot_common: {
        return new fld_quot_common();
    }
    case fid::match_done: {
        return new fld_match_done();
    }
    case fid::high_low_price: {
        return new fld_high_low_price();
    }
    case fid::settlement_price: {
        return new fld_settlement_price();
    }
    case fid::buy_best_order: {
        return new fld_buy_best_order();
    }
    case fid::sell_best_order: {
        return new fld_sell_best_order();
    }
    case fid::arbi_match_done: {
        return new fld_arbi_match_done();
    }
    case fid::buy_arbi_best_order: {
        return new fld_buy_arbi_best_order();
    }
    case fid::sell_arbi_best_order: {
        return new fld_sell_arbi_best_order();
    }
    case fid::opt_param: {
        return new fld_opt_param();
    }
    case fid::market_status: {
        return new fld_market_status();
    }
    case fid::variety_status: {
        return new fld_variety_status();
    }
    case fid::contract_status: {
        return new fld_contract_status();
    }
    case fid::data_notify: {
        return new fld_data_notify();
    }
    case fid::mbl_quot: {
        return new fld_mbl_quot();
    }
    case fid::price_pair: {
        return new fld_price_pair();
    }
    case fid::qty_pair: {
        return new fld_qty_pair();
    }
    case fid::segment_price_qty: {
        return new fld_segment_price_qty();
    }
    case fid::order_statistic_biz6: {
        return new fld_order_statistic_biz6();
    }
    case fid::extend_quot_common: {
        return new fld_extend_quot_common();
    }
    case fid::rsp_msg: {
        return new fld_rsp_msg();
    }
    case fid::trader_login_req: {
        return new fld_trader_login_req();
    }
    case fid::trader_login_rsp: {
        return new fld_trader_login_rsp();
    }
    case fid::trader_id: {
        return new fld_trader_id();
    }
    case fid::flow_ctrl_warning: {
        return new fld_flow_ctrl_warning();
    }
    case fid::req_contract_basic_info: {
        return new fld_req_contract_basic_info();
    }
    case fid::contract_basic_info: {
        return new fld_contract_basic_info();
    }
    case fid::contract_param: {
        return new fld_contract_param();
    }
    case fid::req_trade_plan: {
        return new fld_req_trade_plan();
    }
    case fid::trade_plan: {
        return new fld_trade_plan();
    }
    case fid::qry_his_quot: {
        return new fld_qry_his_quot();
    }
    case fid::qry_quot_snap: {
        return new fld_qry_quot_snap();
    }
    case fid::max_no: {
        return new fld_max_no();
    }
    default:
        return nullptr;
    }
}

field::field(uint16_t fid) : field_size_(length()), field_id_(fid) {}

// id 标识
uint16_t field::id() const { return field_id_; }

// length 长度大小 字节
size_t field::length() const { return sizeof(field_size_) + sizeof(field_id_); }

// max_length 长度大小 字节
size_t field::max_length() const {
    return sizeof(field_size_) + sizeof(field_id_);
}

// marshal 序列化
int32_t field::marshal(char* buf, size_t len) const {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }
    if (len < length()) {
        return err::short_length;
    }
    size_t offset = 0;
    memcpy(buf + offset, &field_size_, sizeof(field_size_));
    offset += field_size_;
    memcpy(buf + offset, &field_id_, sizeof(field_id_));
    offset += field_id_;
    return static_cast<int32_t>(offset);
}

// unmarshal 反序列化
int32_t field::unmarshal(const char* buf, size_t len) {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }
    if (len < length()) {
        return err::short_length;
    }

    size_t offset = 0;
    memcpy(&field_size_, buf + offset, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(&field_id_, buf + offset, sizeof(field_id_));
    offset += sizeof(field_id_);
    return static_cast<int32_t>(offset);
}

// to_string 生成描述文字
std::string field::to_string(void) const {
    std::stringstream oss;
    oss << "{ typename:field"
        << ", len:" << length() << ", fid:0x" << std::hex << field_id_
        << std::dec << "}";
    return oss.str();
}

fld_quot_common::fld_quot_common()
    : field(fid::quot_common), contract_no_{0}, contract_seq_no_{0} {
    field_size_ = length();
}

// id 标识
uint16_t fld_quot_common::id() const { return fid::quot_common; }

// length 长度大小 字节
size_t fld_quot_common::length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + sizeof(contract_no_) +
           sizeof(contract_seq_no_);
}

/// max_length 长度大小 字节
size_t fld_quot_common::max_length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + 5 + 10;
}

// marshal 序列化
int32_t fld_quot_common::marshal(char* buf, size_t len) const {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }
    if (len < max_length()) {
        return err::short_length;
    }

    size_t offset = 0;
    memcpy(buf + offset, &field_size_, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(buf + offset, &field_id_, sizeof(field_id_));
    offset += sizeof(field_id_);
    offset += vint_encode(contract_no_, buf + offset);
    offset += vint_encode(contract_seq_no_, buf + offset);
    field_size_ = static_cast<uint16_t>(offset);
    memcpy(buf, &field_size_, sizeof(field_size_));
    return static_cast<int32_t>(offset);
}

// unmarshal 反序列化
int32_t fld_quot_common::unmarshal(const char* buf, size_t len) {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }

    uint16_t ulen = 0;
    uint16_t ufid = 0;

    size_t offset = 0;
    memcpy(&ulen, buf + offset, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(&ufid, buf + offset, sizeof(field_id_));
    offset += sizeof(field_id_);
    if (len < ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }
    if (field_id_ != ufid) {
        return err::field_type_err;
    }
    offset += vint_decode(contract_no_, buf + offset);
    offset += vint_decode(contract_seq_no_, buf + offset);
    if (offset != ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }

    return static_cast<int32_t>(offset);
}

// to_string 生成描述文字
std::string fld_quot_common::to_string(void) const {
    std::stringstream oss;
    oss << "{ typename:fld_quot_common"
        << ", len:" << length() << ", fid:0x" << std::hex << field_id_
        << std::dec << ", contract_no:" << contract_no_
        << ", contract_seq_no:" << contract_seq_no_ << "}";
    return oss.str();
}

fld_match_done::fld_match_done()
    : field(fid::match_done), last_price_{0}, last_qty_{0},
      total_qty_{0}, turnover_{0}, open_interest_{0}, avg_price_{0} {
    field_size_ = length();
}

// id 标识
uint16_t fld_match_done::id() const { return fid::match_done; }

// length 长度大小 字节
size_t fld_match_done::length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + sizeof(last_price_) +
           sizeof(last_qty_) + sizeof(total_qty_) + sizeof(turnover_) +
           sizeof(open_interest_) + sizeof(avg_price_);
}

/// max_length 长度大小 字节
size_t fld_match_done::max_length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + 10 + 5 + 5 +
           sizeof(turnover_) + 5 + sizeof(avg_price_);
}

// marshal 序列化
int32_t fld_match_done::marshal(char* buf, size_t len) const {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }
    if (len < max_length()) {
        return err::short_length;
    }

    size_t offset = 0;
    memcpy(buf + offset, &field_size_, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(buf + offset, &field_id_, sizeof(field_id_));
    offset += sizeof(field_id_);
    offset += vint_encode(last_price_, buf + offset);
    offset += vint_encode(last_qty_, buf + offset);
    offset += vint_encode(total_qty_, buf + offset);
    memcpy(buf + offset, &turnover_, sizeof(turnover_));
    offset += sizeof(turnover_);
    offset += vint_encode(open_interest_, buf + offset);
    memcpy(buf + offset, &avg_price_, sizeof(avg_price_));
    offset += sizeof(avg_price_);
    field_size_ = static_cast<uint16_t>(offset);
    memcpy(buf, &field_size_, sizeof(field_size_));
    return static_cast<int32_t>(offset);
}

// unmarshal 反序列化
int32_t fld_match_done::unmarshal(const char* buf, size_t len) {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }

    uint16_t ulen = 0;
    uint16_t ufid = 0;

    size_t offset = 0;
    memcpy(&ulen, buf + offset, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(&ufid, buf + offset, sizeof(field_id_));
    offset += sizeof(field_id_);
    if (len < ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }
    if (field_id_ != ufid) {
        return err::field_type_err;
    }
    offset += vint_decode(last_price_, buf + offset);
    offset += vint_decode(last_qty_, buf + offset);
    offset += vint_decode(total_qty_, buf + offset);
    memcpy(&turnover_, buf + offset, sizeof(turnover_));
    offset += sizeof(turnover_);
    offset += vint_decode(open_interest_, buf + offset);
    memcpy(&avg_price_, buf + offset, sizeof(avg_price_));
    offset += sizeof(avg_price_);
    if (offset != ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }

    return static_cast<int32_t>(offset);
}

// to_string 生成描述文字
std::string fld_match_done::to_string(void) const {
    std::stringstream oss;
    oss << "{ typename:fld_match_done"
        << ", len:" << length() << ", fid:0x" << std::hex << field_id_
        << std::dec << ", last_price:" << last_price_
        << ", last_qty:" << last_qty_ << ", total_qty:" << total_qty_
        << ", turnover:" << turnover_ << ", open_interest:" << open_interest_
        << ", avg_price:" << avg_price_ << "}";
    return oss.str();
}

fld_high_low_price::fld_high_low_price()
    : field(fid::high_low_price), daily_high_price_{0}, daily_low_price_{0},
      historical_high_{0}, historical_low_{0} {
    field_size_ = length();
}

// id 标识
uint16_t fld_high_low_price::id() const { return fid::high_low_price; }

// length 长度大小 字节
size_t fld_high_low_price::length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) +
           sizeof(daily_high_price_) + sizeof(daily_low_price_) +
           sizeof(historical_high_) + sizeof(historical_low_);
}

/// max_length 长度大小 字节
size_t fld_high_low_price::max_length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + 10 + 10 + 10 + 10;
}

// marshal 序列化
int32_t fld_high_low_price::marshal(char* buf, size_t len) const {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }
    if (len < max_length()) {
        return err::short_length;
    }

    size_t offset = 0;
    memcpy(buf + offset, &field_size_, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(buf + offset, &field_id_, sizeof(field_id_));
    offset += sizeof(field_id_);
    offset += vint_encode(daily_high_price_, buf + offset);
    offset += vint_encode(daily_low_price_, buf + offset);
    offset += vint_encode(historical_high_, buf + offset);
    offset += vint_encode(historical_low_, buf + offset);
    field_size_ = static_cast<uint16_t>(offset);
    memcpy(buf, &field_size_, sizeof(field_size_));
    return static_cast<int32_t>(offset);
}

// unmarshal 反序列化
int32_t fld_high_low_price::unmarshal(const char* buf, size_t len) {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }

    uint16_t ulen = 0;
    uint16_t ufid = 0;

    size_t offset = 0;
    memcpy(&ulen, buf + offset, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(&ufid, buf + offset, sizeof(field_id_));
    offset += sizeof(field_id_);
    if (len < ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }
    if (field_id_ != ufid) {
        return err::field_type_err;
    }
    offset += vint_decode(daily_high_price_, buf + offset);
    offset += vint_decode(daily_low_price_, buf + offset);
    offset += vint_decode(historical_high_, buf + offset);
    offset += vint_decode(historical_low_, buf + offset);
    if (offset != ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }

    return static_cast<int32_t>(offset);
}

// to_string 生成描述文字
std::string fld_high_low_price::to_string(void) const {
    std::stringstream oss;
    oss << "{ typename:fld_high_low_price"
        << ", len:" << length() << ", fid:0x" << std::hex << field_id_
        << std::dec << ", daily_high_price:" << daily_high_price_
        << ", daily_low_price:" << daily_low_price_
        << ", historical_high:" << historical_high_
        << ", historical_low:" << historical_low_ << "}";
    return oss.str();
}

fld_settlement_price::fld_settlement_price()
    : field(fid::settlement_price), settlement_price_{0}, opening_price_{0},
      closing_price_{0} {
    field_size_ = length();
}

// id 标识
uint16_t fld_settlement_price::id() const { return fid::settlement_price; }

// length 长度大小 字节
size_t fld_settlement_price::length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) +
           sizeof(settlement_price_) + sizeof(opening_price_) +
           sizeof(closing_price_);
}

/// max_length 长度大小 字节
size_t fld_settlement_price::max_length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + 10 + 10 + 10;
}

// marshal 序列化
int32_t fld_settlement_price::marshal(char* buf, size_t len) const {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }
    if (len < max_length()) {
        return err::short_length;
    }

    size_t offset = 0;
    memcpy(buf + offset, &field_size_, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(buf + offset, &field_id_, sizeof(field_id_));
    offset += sizeof(field_id_);
    offset += vint_encode(settlement_price_, buf + offset);
    offset += vint_encode(opening_price_, buf + offset);
    offset += vint_encode(closing_price_, buf + offset);
    field_size_ = static_cast<uint16_t>(offset);
    memcpy(buf, &field_size_, sizeof(field_size_));
    return static_cast<int32_t>(offset);
}

// unmarshal 反序列化
int32_t fld_settlement_price::unmarshal(const char* buf, size_t len) {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }

    uint16_t ulen = 0;
    uint16_t ufid = 0;

    size_t offset = 0;
    memcpy(&ulen, buf + offset, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(&ufid, buf + offset, sizeof(field_id_));
    offset += sizeof(field_id_);
    if (len < ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }
    if (field_id_ != ufid) {
        return err::field_type_err;
    }
    offset += vint_decode(settlement_price_, buf + offset);
    offset += vint_decode(opening_price_, buf + offset);
    offset += vint_decode(closing_price_, buf + offset);
    if (offset != ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }

    return static_cast<int32_t>(offset);
}

// to_string 生成描述文字
std::string fld_settlement_price::to_string(void) const {
    std::stringstream oss;
    oss << "{ typename:fld_settlement_price"
        << ", len:" << length() << ", fid:0x" << std::hex << field_id_
        << std::dec << ", settlement_price:" << settlement_price_
        << ", opening_price:" << opening_price_
        << ", closing_price:" << closing_price_ << "}";
    return oss.str();
}

fld_buy_best_order::fld_buy_best_order()
    : field(fid::buy_best_order), bid_price_{0}, bid_qty_{0}, bid_imply_qty_{
                                                                  0} {
    field_size_ = length();
}

// id 标识
uint16_t fld_buy_best_order::id() const { return fid::buy_best_order; }

// length 长度大小 字节
size_t fld_buy_best_order::length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + sizeof(bid_price_) +
           sizeof(bid_qty_) + sizeof(bid_imply_qty_);
}

/// max_length 长度大小 字节
size_t fld_buy_best_order::max_length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + 10 + 5 + 5;
}

// marshal 序列化
int32_t fld_buy_best_order::marshal(char* buf, size_t len) const {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }
    if (len < max_length()) {
        return err::short_length;
    }

    size_t offset = 0;
    memcpy(buf + offset, &field_size_, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(buf + offset, &field_id_, sizeof(field_id_));
    offset += sizeof(field_id_);
    offset += vint_encode(bid_price_, buf + offset);
    offset += vint_encode(bid_qty_, buf + offset);
    offset += vint_encode(bid_imply_qty_, buf + offset);
    field_size_ = static_cast<uint16_t>(offset);
    memcpy(buf, &field_size_, sizeof(field_size_));
    return static_cast<int32_t>(offset);
}

// unmarshal 反序列化
int32_t fld_buy_best_order::unmarshal(const char* buf, size_t len) {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }

    uint16_t ulen = 0;
    uint16_t ufid = 0;

    size_t offset = 0;
    memcpy(&ulen, buf + offset, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(&ufid, buf + offset, sizeof(field_id_));
    offset += sizeof(field_id_);
    if (len < ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }
    if (field_id_ != ufid) {
        return err::field_type_err;
    }
    offset += vint_decode(bid_price_, buf + offset);
    offset += vint_decode(bid_qty_, buf + offset);
    offset += vint_decode(bid_imply_qty_, buf + offset);
    if (offset != ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }

    return static_cast<int32_t>(offset);
}

// to_string 生成描述文字
std::string fld_buy_best_order::to_string(void) const {
    std::stringstream oss;
    oss << "{ typename:fld_buy_best_order"
        << ", len:" << length() << ", fid:0x" << std::hex << field_id_
        << std::dec << ", bid_price:" << bid_price_ << ", bid_qty:" << bid_qty_
        << ", bid_imply_qty:" << bid_imply_qty_ << "}";
    return oss.str();
}

fld_sell_best_order::fld_sell_best_order()
    : field(fid::sell_best_order), ask_price_{0}, ask_qty_{0}, ask_imply_qty_{
                                                                   0} {
    field_size_ = length();
}

// id 标识
uint16_t fld_sell_best_order::id() const { return fid::sell_best_order; }

// length 长度大小 字节
size_t fld_sell_best_order::length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + sizeof(ask_price_) +
           sizeof(ask_qty_) + sizeof(ask_imply_qty_);
}

/// max_length 长度大小 字节
size_t fld_sell_best_order::max_length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + 10 + 5 + 5;
}

// marshal 序列化
int32_t fld_sell_best_order::marshal(char* buf, size_t len) const {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }
    if (len < max_length()) {
        return err::short_length;
    }

    size_t offset = 0;
    memcpy(buf + offset, &field_size_, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(buf + offset, &field_id_, sizeof(field_id_));
    offset += sizeof(field_id_);
    offset += vint_encode(ask_price_, buf + offset);
    offset += vint_encode(ask_qty_, buf + offset);
    offset += vint_encode(ask_imply_qty_, buf + offset);
    field_size_ = static_cast<uint16_t>(offset);
    memcpy(buf, &field_size_, sizeof(field_size_));
    return static_cast<int32_t>(offset);
}

// unmarshal 反序列化
int32_t fld_sell_best_order::unmarshal(const char* buf, size_t len) {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }

    uint16_t ulen = 0;
    uint16_t ufid = 0;

    size_t offset = 0;
    memcpy(&ulen, buf + offset, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(&ufid, buf + offset, sizeof(field_id_));
    offset += sizeof(field_id_);
    if (len < ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }
    if (field_id_ != ufid) {
        return err::field_type_err;
    }
    offset += vint_decode(ask_price_, buf + offset);
    offset += vint_decode(ask_qty_, buf + offset);
    offset += vint_decode(ask_imply_qty_, buf + offset);
    if (offset != ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }

    return static_cast<int32_t>(offset);
}

// to_string 生成描述文字
std::string fld_sell_best_order::to_string(void) const {
    std::stringstream oss;
    oss << "{ typename:fld_sell_best_order"
        << ", len:" << length() << ", fid:0x" << std::hex << field_id_
        << std::dec << ", ask_price:" << ask_price_ << ", ask_qty:" << ask_qty_
        << ", ask_imply_qty:" << ask_imply_qty_ << "}";
    return oss.str();
}

fld_arbi_match_done::fld_arbi_match_done()
    : field(fid::arbi_match_done), last_price_{0}, last_qty_{0} {
    field_size_ = length();
}

// id 标识
uint16_t fld_arbi_match_done::id() const { return fid::arbi_match_done; }

// length 长度大小 字节
size_t fld_arbi_match_done::length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + sizeof(last_price_) +
           sizeof(last_qty_);
}

/// max_length 长度大小 字节
size_t fld_arbi_match_done::max_length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + 10 + 5;
}

// marshal 序列化
int32_t fld_arbi_match_done::marshal(char* buf, size_t len) const {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }
    if (len < max_length()) {
        return err::short_length;
    }

    size_t offset = 0;
    memcpy(buf + offset, &field_size_, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(buf + offset, &field_id_, sizeof(field_id_));
    offset += sizeof(field_id_);
    offset += vint_encode(last_price_, buf + offset);
    offset += vint_encode(last_qty_, buf + offset);
    field_size_ = static_cast<uint16_t>(offset);
    memcpy(buf, &field_size_, sizeof(field_size_));
    return static_cast<int32_t>(offset);
}

// unmarshal 反序列化
int32_t fld_arbi_match_done::unmarshal(const char* buf, size_t len) {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }

    uint16_t ulen = 0;
    uint16_t ufid = 0;

    size_t offset = 0;
    memcpy(&ulen, buf + offset, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(&ufid, buf + offset, sizeof(field_id_));
    offset += sizeof(field_id_);
    if (len < ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }
    if (field_id_ != ufid) {
        return err::field_type_err;
    }
    offset += vint_decode(last_price_, buf + offset);
    offset += vint_decode(last_qty_, buf + offset);
    if (offset != ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }

    return static_cast<int32_t>(offset);
}

// to_string 生成描述文字
std::string fld_arbi_match_done::to_string(void) const {
    std::stringstream oss;
    oss << "{ typename:fld_arbi_match_done"
        << ", len:" << length() << ", fid:0x" << std::hex << field_id_
        << std::dec << ", last_price:" << last_price_
        << ", last_qty:" << last_qty_ << "}";
    return oss.str();
}

fld_buy_arbi_best_order::fld_buy_arbi_best_order()
    : field(fid::buy_arbi_best_order), bid_price_{0}, bid_qty_{0} {
    field_size_ = length();
}

// id 标识
uint16_t fld_buy_arbi_best_order::id() const {
    return fid::buy_arbi_best_order;
}

// length 长度大小 字节
size_t fld_buy_arbi_best_order::length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + sizeof(bid_price_) +
           sizeof(bid_qty_);
}

/// max_length 长度大小 字节
size_t fld_buy_arbi_best_order::max_length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + 10 + 5;
}

// marshal 序列化
int32_t fld_buy_arbi_best_order::marshal(char* buf, size_t len) const {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }
    if (len < max_length()) {
        return err::short_length;
    }

    size_t offset = 0;
    memcpy(buf + offset, &field_size_, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(buf + offset, &field_id_, sizeof(field_id_));
    offset += sizeof(field_id_);
    offset += vint_encode(bid_price_, buf + offset);
    offset += vint_encode(bid_qty_, buf + offset);
    field_size_ = static_cast<uint16_t>(offset);
    memcpy(buf, &field_size_, sizeof(field_size_));
    return static_cast<int32_t>(offset);
}

// unmarshal 反序列化
int32_t fld_buy_arbi_best_order::unmarshal(const char* buf, size_t len) {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }

    uint16_t ulen = 0;
    uint16_t ufid = 0;

    size_t offset = 0;
    memcpy(&ulen, buf + offset, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(&ufid, buf + offset, sizeof(field_id_));
    offset += sizeof(field_id_);
    if (len < ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }
    if (field_id_ != ufid) {
        return err::field_type_err;
    }
    offset += vint_decode(bid_price_, buf + offset);
    offset += vint_decode(bid_qty_, buf + offset);
    if (offset != ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }

    return static_cast<int32_t>(offset);
}

// to_string 生成描述文字
std::string fld_buy_arbi_best_order::to_string(void) const {
    std::stringstream oss;
    oss << "{ typename:fld_buy_arbi_best_order"
        << ", len:" << length() << ", fid:0x" << std::hex << field_id_
        << std::dec << ", bid_price:" << bid_price_ << ", bid_qty:" << bid_qty_
        << "}";
    return oss.str();
}

fld_sell_arbi_best_order::fld_sell_arbi_best_order()
    : field(fid::sell_arbi_best_order), ask_price_{0}, ask_qty_{0} {
    field_size_ = length();
}

// id 标识
uint16_t fld_sell_arbi_best_order::id() const {
    return fid::sell_arbi_best_order;
}

// length 长度大小 字节
size_t fld_sell_arbi_best_order::length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + sizeof(ask_price_) +
           sizeof(ask_qty_);
}

/// max_length 长度大小 字节
size_t fld_sell_arbi_best_order::max_length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + 10 + 5;
}

// marshal 序列化
int32_t fld_sell_arbi_best_order::marshal(char* buf, size_t len) const {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }
    if (len < max_length()) {
        return err::short_length;
    }

    size_t offset = 0;
    memcpy(buf + offset, &field_size_, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(buf + offset, &field_id_, sizeof(field_id_));
    offset += sizeof(field_id_);
    offset += vint_encode(ask_price_, buf + offset);
    offset += vint_encode(ask_qty_, buf + offset);
    field_size_ = static_cast<uint16_t>(offset);
    memcpy(buf, &field_size_, sizeof(field_size_));
    return static_cast<int32_t>(offset);
}

// unmarshal 反序列化
int32_t fld_sell_arbi_best_order::unmarshal(const char* buf, size_t len) {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }

    uint16_t ulen = 0;
    uint16_t ufid = 0;

    size_t offset = 0;
    memcpy(&ulen, buf + offset, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(&ufid, buf + offset, sizeof(field_id_));
    offset += sizeof(field_id_);
    if (len < ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }
    if (field_id_ != ufid) {
        return err::field_type_err;
    }
    offset += vint_decode(ask_price_, buf + offset);
    offset += vint_decode(ask_qty_, buf + offset);
    if (offset != ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }

    return static_cast<int32_t>(offset);
}

// to_string 生成描述文字
std::string fld_sell_arbi_best_order::to_string(void) const {
    std::stringstream oss;
    oss << "{ typename:fld_sell_arbi_best_order"
        << ", len:" << length() << ", fid:0x" << std::hex << field_id_
        << std::dec << ", ask_price:" << ask_price_ << ", ask_qty:" << ask_qty_
        << "}";
    return oss.str();
}

fld_opt_param::fld_opt_param()
    : field(fid::opt_param), delta_{0}, gamma_{0}, rho_{0}, theta_{0}, vega_{
                                                                           0} {
    field_size_ = length();
}

// id 标识
uint16_t fld_opt_param::id() const { return fid::opt_param; }

// length 长度大小 字节
size_t fld_opt_param::length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + sizeof(delta_) +
           sizeof(gamma_) + sizeof(rho_) + sizeof(theta_) + sizeof(vega_);
}

/// max_length 长度大小 字节
size_t fld_opt_param::max_length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + sizeof(delta_) +
           sizeof(gamma_) + sizeof(rho_) + sizeof(theta_) + sizeof(vega_);
}

// marshal 序列化
int32_t fld_opt_param::marshal(char* buf, size_t len) const {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }
    if (len < max_length()) {
        return err::short_length;
    }

    size_t offset = 0;
    memcpy(buf + offset, &field_size_, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(buf + offset, &field_id_, sizeof(field_id_));
    offset += sizeof(field_id_);
    memcpy(buf + offset, &delta_, sizeof(delta_));
    offset += sizeof(delta_);
    memcpy(buf + offset, &gamma_, sizeof(gamma_));
    offset += sizeof(gamma_);
    memcpy(buf + offset, &rho_, sizeof(rho_));
    offset += sizeof(rho_);
    memcpy(buf + offset, &theta_, sizeof(theta_));
    offset += sizeof(theta_);
    memcpy(buf + offset, &vega_, sizeof(vega_));
    offset += sizeof(vega_);
    field_size_ = static_cast<uint16_t>(offset);
    memcpy(buf, &field_size_, sizeof(field_size_));
    return static_cast<int32_t>(offset);
}

// unmarshal 反序列化
int32_t fld_opt_param::unmarshal(const char* buf, size_t len) {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }

    uint16_t ulen = 0;
    uint16_t ufid = 0;

    size_t offset = 0;
    memcpy(&ulen, buf + offset, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(&ufid, buf + offset, sizeof(field_id_));
    offset += sizeof(field_id_);
    if (len < ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }
    if (field_id_ != ufid) {
        return err::field_type_err;
    }
    memcpy(&delta_, buf + offset, sizeof(delta_));
    offset += sizeof(delta_);
    memcpy(&gamma_, buf + offset, sizeof(gamma_));
    offset += sizeof(gamma_);
    memcpy(&rho_, buf + offset, sizeof(rho_));
    offset += sizeof(rho_);
    memcpy(&theta_, buf + offset, sizeof(theta_));
    offset += sizeof(theta_);
    memcpy(&vega_, buf + offset, sizeof(vega_));
    offset += sizeof(vega_);
    if (offset != ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }

    return static_cast<int32_t>(offset);
}

// to_string 生成描述文字
std::string fld_opt_param::to_string(void) const {
    std::stringstream oss;
    oss << "{ typename:fld_opt_param"
        << ", len:" << length() << ", fid:0x" << std::hex << field_id_
        << std::dec << ", delta:" << delta_ << ", gamma:" << gamma_
        << ", rho:" << rho_ << ", theta:" << theta_ << ", vega:" << vega_
        << "}";
    return oss.str();
}

fld_market_status::fld_market_status()
    : field(fid::market_status), mkt_status_{0}, exch_code_{0}, mkt_chg_time_{
                                                                    0} {
    field_size_ = length();
}

// id 标识
uint16_t fld_market_status::id() const { return fid::market_status; }

// length 长度大小 字节
size_t fld_market_status::length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + sizeof(mkt_status_) +
           sizeof(exch_code_) + sizeof(mkt_chg_time_);
}

/// max_length 长度大小 字节
size_t fld_market_status::max_length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + sizeof(mkt_status_) +
           sizeof(exch_code_) + sizeof(mkt_chg_time_);
}

// marshal 序列化
int32_t fld_market_status::marshal(char* buf, size_t len) const {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }
    if (len < max_length()) {
        return err::short_length;
    }

    size_t offset = 0;
    memcpy(buf + offset, &field_size_, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(buf + offset, &field_id_, sizeof(field_id_));
    offset += sizeof(field_id_);
    memcpy(buf + offset, &mkt_status_, sizeof(mkt_status_));
    offset += sizeof(mkt_status_);
    memcpy(buf + offset, &exch_code_, sizeof(exch_code_));
    offset += sizeof(exch_code_);
    memcpy(buf + offset, &mkt_chg_time_, sizeof(mkt_chg_time_));
    offset += sizeof(mkt_chg_time_);
    field_size_ = static_cast<uint16_t>(offset);
    memcpy(buf, &field_size_, sizeof(field_size_));
    return static_cast<int32_t>(offset);
}

// unmarshal 反序列化
int32_t fld_market_status::unmarshal(const char* buf, size_t len) {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }

    uint16_t ulen = 0;
    uint16_t ufid = 0;

    size_t offset = 0;
    memcpy(&ulen, buf + offset, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(&ufid, buf + offset, sizeof(field_id_));
    offset += sizeof(field_id_);
    if (len < ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }
    if (field_id_ != ufid) {
        return err::field_type_err;
    }
    memcpy(&mkt_status_, buf + offset, sizeof(mkt_status_));
    offset += sizeof(mkt_status_);
    memcpy(&exch_code_, buf + offset, sizeof(exch_code_));
    offset += sizeof(exch_code_);
    memcpy(&mkt_chg_time_, buf + offset, sizeof(mkt_chg_time_));
    offset += sizeof(mkt_chg_time_);
    if (offset != ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }

    return static_cast<int32_t>(offset);
}

// to_string 生成描述文字
std::string fld_market_status::to_string(void) const {
    std::stringstream oss;
    oss << "{ typename:fld_market_status"
        << ", len:" << length() << ", fid:0x" << std::hex << field_id_
        << std::dec << ", mkt_status:" << (uint16_t)mkt_status_
        << ", exch_code:" << exch_code_ << ", mkt_chg_time:" << mkt_chg_time_
        << "}";
    return oss.str();
}

fld_variety_status::fld_variety_status()
    : field(fid::variety_status), variety_id_{0},
      trade_type_{0}, status_{0}, op_style_{0}, chg_time_{0} {
    field_size_ = length();
}

// id 标识
uint16_t fld_variety_status::id() const { return fid::variety_status; }

// length 长度大小 字节
size_t fld_variety_status::length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + sizeof(variety_id_) +
           sizeof(trade_type_) + sizeof(status_) + sizeof(op_style_) +
           sizeof(chg_time_);
}

/// max_length 长度大小 字节
size_t fld_variety_status::max_length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + sizeof(variety_id_) +
           sizeof(trade_type_) + sizeof(status_) + sizeof(op_style_) +
           sizeof(chg_time_);
}

// marshal 序列化
int32_t fld_variety_status::marshal(char* buf, size_t len) const {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }
    if (len < max_length()) {
        return err::short_length;
    }

    size_t offset = 0;
    memcpy(buf + offset, &field_size_, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(buf + offset, &field_id_, sizeof(field_id_));
    offset += sizeof(field_id_);
    memcpy(buf + offset, &variety_id_, sizeof(variety_id_));
    offset += sizeof(variety_id_);
    memcpy(buf + offset, &trade_type_, sizeof(trade_type_));
    offset += sizeof(trade_type_);
    memcpy(buf + offset, &status_, sizeof(status_));
    offset += sizeof(status_);
    memcpy(buf + offset, &op_style_, sizeof(op_style_));
    offset += sizeof(op_style_);
    memcpy(buf + offset, &chg_time_, sizeof(chg_time_));
    offset += sizeof(chg_time_);
    field_size_ = static_cast<uint16_t>(offset);
    memcpy(buf, &field_size_, sizeof(field_size_));
    return static_cast<int32_t>(offset);
}

// unmarshal 反序列化
int32_t fld_variety_status::unmarshal(const char* buf, size_t len) {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }

    uint16_t ulen = 0;
    uint16_t ufid = 0;

    size_t offset = 0;
    memcpy(&ulen, buf + offset, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(&ufid, buf + offset, sizeof(field_id_));
    offset += sizeof(field_id_);
    if (len < ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }
    if (field_id_ != ufid) {
        return err::field_type_err;
    }
    memcpy(&variety_id_, buf + offset, sizeof(variety_id_));
    offset += sizeof(variety_id_);
    memcpy(&trade_type_, buf + offset, sizeof(trade_type_));
    offset += sizeof(trade_type_);
    memcpy(&status_, buf + offset, sizeof(status_));
    offset += sizeof(status_);
    memcpy(&op_style_, buf + offset, sizeof(op_style_));
    offset += sizeof(op_style_);
    memcpy(&chg_time_, buf + offset, sizeof(chg_time_));
    offset += sizeof(chg_time_);
    if (offset != ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }

    return static_cast<int32_t>(offset);
}

// to_string 生成描述文字
std::string fld_variety_status::to_string(void) const {
    std::stringstream oss;
    oss << "{ typename:fld_variety_status"
        << ", len:" << length() << ", fid:0x" << std::hex << field_id_
        << std::dec << ", variety_id:" << variety_id_
        << ", trade_type:" << (uint16_t)trade_type_
        << ", status:" << (uint16_t)status_
        << ", op_style:" << (uint16_t)op_style_ << ", chg_time:" << chg_time_
        << "}";
    return oss.str();
}

fld_contract_status::fld_contract_status()
    : field(fid::contract_status),
      contract_no_{0}, status_{0}, op_style_{0}, chg_time_{0} {
    field_size_ = length();
}

// id 标识
uint16_t fld_contract_status::id() const { return fid::contract_status; }

// length 长度大小 字节
size_t fld_contract_status::length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + sizeof(contract_no_) +
           sizeof(status_) + sizeof(op_style_) + sizeof(chg_time_);
}

/// max_length 长度大小 字节
size_t fld_contract_status::max_length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + sizeof(contract_no_) +
           sizeof(status_) + sizeof(op_style_) + sizeof(chg_time_);
}

// marshal 序列化
int32_t fld_contract_status::marshal(char* buf, size_t len) const {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }
    if (len < max_length()) {
        return err::short_length;
    }

    size_t offset = 0;
    memcpy(buf + offset, &field_size_, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(buf + offset, &field_id_, sizeof(field_id_));
    offset += sizeof(field_id_);
    memcpy(buf + offset, &contract_no_, sizeof(contract_no_));
    offset += sizeof(contract_no_);
    memcpy(buf + offset, &status_, sizeof(status_));
    offset += sizeof(status_);
    memcpy(buf + offset, &op_style_, sizeof(op_style_));
    offset += sizeof(op_style_);
    memcpy(buf + offset, &chg_time_, sizeof(chg_time_));
    offset += sizeof(chg_time_);
    field_size_ = static_cast<uint16_t>(offset);
    memcpy(buf, &field_size_, sizeof(field_size_));
    return static_cast<int32_t>(offset);
}

// unmarshal 反序列化
int32_t fld_contract_status::unmarshal(const char* buf, size_t len) {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }

    uint16_t ulen = 0;
    uint16_t ufid = 0;

    size_t offset = 0;
    memcpy(&ulen, buf + offset, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(&ufid, buf + offset, sizeof(field_id_));
    offset += sizeof(field_id_);
    if (len < ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }
    if (field_id_ != ufid) {
        return err::field_type_err;
    }
    memcpy(&contract_no_, buf + offset, sizeof(contract_no_));
    offset += sizeof(contract_no_);
    memcpy(&status_, buf + offset, sizeof(status_));
    offset += sizeof(status_);
    memcpy(&op_style_, buf + offset, sizeof(op_style_));
    offset += sizeof(op_style_);
    memcpy(&chg_time_, buf + offset, sizeof(chg_time_));
    offset += sizeof(chg_time_);
    if (offset != ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }

    return static_cast<int32_t>(offset);
}

// to_string 生成描述文字
std::string fld_contract_status::to_string(void) const {
    std::stringstream oss;
    oss << "{ typename:fld_contract_status"
        << ", len:" << length() << ", fid:0x" << std::hex << field_id_
        << std::dec << ", contract_no:" << contract_no_
        << ", status:" << (uint16_t)status_
        << ", op_style:" << (uint16_t)op_style_ << ", chg_time:" << chg_time_
        << "}";
    return oss.str();
}

fld_data_notify::fld_data_notify()
    : field(fid::data_notify), type_{0}, time_{0} {
    field_size_ = length();
}

// id 标识
uint16_t fld_data_notify::id() const { return fid::data_notify; }

// length 长度大小 字节
size_t fld_data_notify::length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + sizeof(type_) +
           sizeof(time_);
}

/// max_length 长度大小 字节
size_t fld_data_notify::max_length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + sizeof(type_) +
           sizeof(time_);
}

// marshal 序列化
int32_t fld_data_notify::marshal(char* buf, size_t len) const {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }
    if (len < max_length()) {
        return err::short_length;
    }

    size_t offset = 0;
    memcpy(buf + offset, &field_size_, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(buf + offset, &field_id_, sizeof(field_id_));
    offset += sizeof(field_id_);
    memcpy(buf + offset, &type_, sizeof(type_));
    offset += sizeof(type_);
    memcpy(buf + offset, &time_, sizeof(time_));
    offset += sizeof(time_);
    field_size_ = static_cast<uint16_t>(offset);
    memcpy(buf, &field_size_, sizeof(field_size_));
    return static_cast<int32_t>(offset);
}

// unmarshal 反序列化
int32_t fld_data_notify::unmarshal(const char* buf, size_t len) {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }

    uint16_t ulen = 0;
    uint16_t ufid = 0;

    size_t offset = 0;
    memcpy(&ulen, buf + offset, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(&ufid, buf + offset, sizeof(field_id_));
    offset += sizeof(field_id_);
    if (len < ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }
    if (field_id_ != ufid) {
        return err::field_type_err;
    }
    memcpy(&type_, buf + offset, sizeof(type_));
    offset += sizeof(type_);
    memcpy(&time_, buf + offset, sizeof(time_));
    offset += sizeof(time_);
    if (offset != ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }

    return static_cast<int32_t>(offset);
}

// to_string 生成描述文字
std::string fld_data_notify::to_string(void) const {
    std::stringstream oss;
    oss << "{ typename:fld_data_notify"
        << ", len:" << length() << ", fid:0x" << std::hex << field_id_
        << std::dec << ", type:" << (uint16_t)type_ << ", time:" << time_
        << "}";
    return oss.str();
}

fld_mbl_quot::fld_mbl_quot()
    : field(fid::mbl_quot), event_type_{0}, bs_flag_{0}, order_price_{0},
      order_qty_{0}, imply_qty_{0} {
    field_size_ = length();
}

// id 标识
uint16_t fld_mbl_quot::id() const { return fid::mbl_quot; }

// length 长度大小 字节
size_t fld_mbl_quot::length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + sizeof(event_type_) +
           sizeof(bs_flag_) + sizeof(order_price_) + sizeof(order_qty_) +
           sizeof(imply_qty_);
}

/// max_length 长度大小 字节
size_t fld_mbl_quot::max_length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + sizeof(event_type_) +
           sizeof(bs_flag_) + 10 + 5 + 5;
}

// marshal 序列化
int32_t fld_mbl_quot::marshal(char* buf, size_t len) const {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }
    if (len < max_length()) {
        return err::short_length;
    }

    size_t offset = 0;
    memcpy(buf + offset, &field_size_, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(buf + offset, &field_id_, sizeof(field_id_));
    offset += sizeof(field_id_);
    memcpy(buf + offset, &event_type_, sizeof(event_type_));
    offset += sizeof(event_type_);
    memcpy(buf + offset, &bs_flag_, sizeof(bs_flag_));
    offset += sizeof(bs_flag_);
    offset += vint_encode(order_price_, buf + offset);
    offset += vint_encode(order_qty_, buf + offset);
    offset += vint_encode(imply_qty_, buf + offset);
    field_size_ = static_cast<uint16_t>(offset);
    memcpy(buf, &field_size_, sizeof(field_size_));
    return static_cast<int32_t>(offset);
}

// unmarshal 反序列化
int32_t fld_mbl_quot::unmarshal(const char* buf, size_t len) {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }

    uint16_t ulen = 0;
    uint16_t ufid = 0;

    size_t offset = 0;
    memcpy(&ulen, buf + offset, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(&ufid, buf + offset, sizeof(field_id_));
    offset += sizeof(field_id_);
    if (len < ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }
    if (field_id_ != ufid) {
        return err::field_type_err;
    }
    memcpy(&event_type_, buf + offset, sizeof(event_type_));
    offset += sizeof(event_type_);
    memcpy(&bs_flag_, buf + offset, sizeof(bs_flag_));
    offset += sizeof(bs_flag_);
    offset += vint_decode(order_price_, buf + offset);
    offset += vint_decode(order_qty_, buf + offset);
    offset += vint_decode(imply_qty_, buf + offset);
    if (offset != ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }

    return static_cast<int32_t>(offset);
}

// to_string 生成描述文字
std::string fld_mbl_quot::to_string(void) const {
    std::stringstream oss;
    oss << "{ typename:fld_mbl_quot"
        << ", len:" << length() << ", fid:0x" << std::hex << field_id_
        << std::dec << ", event_type:" << (uint16_t)event_type_
        << ", bs_flag:" << (uint16_t)bs_flag_
        << ", order_price:" << order_price_ << ", order_qty:" << order_qty_
        << ", imply_qty:" << imply_qty_ << "}";
    return oss.str();
}

fld_price_pair::fld_price_pair()
    : field(fid::price_pair), bid_price_{0}, ask_price_{0} {
    field_size_ = length();
}

// id 标识
uint16_t fld_price_pair::id() const { return fid::price_pair; }

// length 长度大小 字节
size_t fld_price_pair::length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + sizeof(bid_price_) +
           sizeof(ask_price_);
}

/// max_length 长度大小 字节
size_t fld_price_pair::max_length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + 10 + 10;
}

// marshal 序列化
int32_t fld_price_pair::marshal(char* buf, size_t len) const {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }
    if (len < max_length()) {
        return err::short_length;
    }

    size_t offset = 0;
    memcpy(buf + offset, &field_size_, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(buf + offset, &field_id_, sizeof(field_id_));
    offset += sizeof(field_id_);
    offset += vint_encode(bid_price_, buf + offset);
    offset += vint_encode(ask_price_, buf + offset);
    field_size_ = static_cast<uint16_t>(offset);
    memcpy(buf, &field_size_, sizeof(field_size_));
    return static_cast<int32_t>(offset);
}

// unmarshal 反序列化
int32_t fld_price_pair::unmarshal(const char* buf, size_t len) {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }

    uint16_t ulen = 0;
    uint16_t ufid = 0;

    size_t offset = 0;
    memcpy(&ulen, buf + offset, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(&ufid, buf + offset, sizeof(field_id_));
    offset += sizeof(field_id_);
    if (len < ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }
    if (field_id_ != ufid) {
        return err::field_type_err;
    }
    offset += vint_decode(bid_price_, buf + offset);
    offset += vint_decode(ask_price_, buf + offset);
    if (offset != ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }

    return static_cast<int32_t>(offset);
}

// to_string 生成描述文字
std::string fld_price_pair::to_string(void) const {
    std::stringstream oss;
    oss << "{ typename:fld_price_pair"
        << ", len:" << length() << ", fid:0x" << std::hex << field_id_
        << std::dec << ", bid_price:" << bid_price_
        << ", ask_price:" << ask_price_ << "}";
    return oss.str();
}

fld_qty_pair::fld_qty_pair() : field(fid::qty_pair), bid_qty_{0}, ask_qty_{0} {
    field_size_ = length();
}

// id 标识
uint16_t fld_qty_pair::id() const { return fid::qty_pair; }

// length 长度大小 字节
size_t fld_qty_pair::length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + sizeof(bid_qty_) +
           sizeof(ask_qty_);
}

/// max_length 长度大小 字节
size_t fld_qty_pair::max_length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + 5 + 5;
}

// marshal 序列化
int32_t fld_qty_pair::marshal(char* buf, size_t len) const {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }
    if (len < max_length()) {
        return err::short_length;
    }

    size_t offset = 0;
    memcpy(buf + offset, &field_size_, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(buf + offset, &field_id_, sizeof(field_id_));
    offset += sizeof(field_id_);
    offset += vint_encode(bid_qty_, buf + offset);
    offset += vint_encode(ask_qty_, buf + offset);
    field_size_ = static_cast<uint16_t>(offset);
    memcpy(buf, &field_size_, sizeof(field_size_));
    return static_cast<int32_t>(offset);
}

// unmarshal 反序列化
int32_t fld_qty_pair::unmarshal(const char* buf, size_t len) {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }

    uint16_t ulen = 0;
    uint16_t ufid = 0;

    size_t offset = 0;
    memcpy(&ulen, buf + offset, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(&ufid, buf + offset, sizeof(field_id_));
    offset += sizeof(field_id_);
    if (len < ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }
    if (field_id_ != ufid) {
        return err::field_type_err;
    }
    offset += vint_decode(bid_qty_, buf + offset);
    offset += vint_decode(ask_qty_, buf + offset);
    if (offset != ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }

    return static_cast<int32_t>(offset);
}

// to_string 生成描述文字
std::string fld_qty_pair::to_string(void) const {
    std::stringstream oss;
    oss << "{ typename:fld_qty_pair"
        << ", len:" << length() << ", fid:0x" << std::hex << field_id_
        << std::dec << ", bid_qty:" << bid_qty_ << ", ask_qty:" << ask_qty_
        << "}";
    return oss.str();
}

fld_segment_price_qty::fld_segment_price_qty()
    : field(fid::segment_price_qty), match_price_{0}, buy_open_qty_{0},
      buy_offset_qty_{0}, sell_open_qty_{0}, sell_offset_qty_{0} {
    field_size_ = length();
}

// id 标识
uint16_t fld_segment_price_qty::id() const { return fid::segment_price_qty; }

// length 长度大小 字节
size_t fld_segment_price_qty::length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + sizeof(match_price_) +
           sizeof(buy_open_qty_) + sizeof(buy_offset_qty_) +
           sizeof(sell_open_qty_) + sizeof(sell_offset_qty_);
}

/// max_length 长度大小 字节
size_t fld_segment_price_qty::max_length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + sizeof(match_price_) +
           5 + 5 + 5 + 5;
}

// marshal 序列化
int32_t fld_segment_price_qty::marshal(char* buf, size_t len) const {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }
    if (len < max_length()) {
        return err::short_length;
    }

    size_t offset = 0;
    memcpy(buf + offset, &field_size_, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(buf + offset, &field_id_, sizeof(field_id_));
    offset += sizeof(field_id_);
    memcpy(buf + offset, &match_price_, sizeof(match_price_));
    offset += sizeof(match_price_);
    offset += vint_encode(buy_open_qty_, buf + offset);
    offset += vint_encode(buy_offset_qty_, buf + offset);
    offset += vint_encode(sell_open_qty_, buf + offset);
    offset += vint_encode(sell_offset_qty_, buf + offset);
    field_size_ = static_cast<uint16_t>(offset);
    memcpy(buf, &field_size_, sizeof(field_size_));
    return static_cast<int32_t>(offset);
}

// unmarshal 反序列化
int32_t fld_segment_price_qty::unmarshal(const char* buf, size_t len) {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }

    uint16_t ulen = 0;
    uint16_t ufid = 0;

    size_t offset = 0;
    memcpy(&ulen, buf + offset, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(&ufid, buf + offset, sizeof(field_id_));
    offset += sizeof(field_id_);
    if (len < ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }
    if (field_id_ != ufid) {
        return err::field_type_err;
    }
    memcpy(&match_price_, buf + offset, sizeof(match_price_));
    offset += sizeof(match_price_);
    offset += vint_decode(buy_open_qty_, buf + offset);
    offset += vint_decode(buy_offset_qty_, buf + offset);
    offset += vint_decode(sell_open_qty_, buf + offset);
    offset += vint_decode(sell_offset_qty_, buf + offset);
    if (offset != ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }

    return static_cast<int32_t>(offset);
}

// to_string 生成描述文字
std::string fld_segment_price_qty::to_string(void) const {
    std::stringstream oss;
    oss << "{ typename:fld_segment_price_qty"
        << ", len:" << length() << ", fid:0x" << std::hex << field_id_
        << std::dec << ", match_price:" << match_price_
        << ", buy_open_qty:" << buy_open_qty_
        << ", buy_offset_qty:" << buy_offset_qty_
        << ", sell_open_qty:" << sell_open_qty_
        << ", sell_offset_qty:" << sell_offset_qty_ << "}";
    return oss.str();
}

fld_order_statistic_biz6::fld_order_statistic_biz6()
    : field(fid::order_statistic_biz6), total_buy_order_qty_{0},
      total_sell_order_qty_{0}, weighted_average_bid_price_{0},
      weighted_average_ask_price_{0} {
    field_size_ = length();
}

// id 标识
uint16_t fld_order_statistic_biz6::id() const {
    return fid::order_statistic_biz6;
}

// length 长度大小 字节
size_t fld_order_statistic_biz6::length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) +
           sizeof(total_buy_order_qty_) + sizeof(total_sell_order_qty_) +
           sizeof(weighted_average_bid_price_) +
           sizeof(weighted_average_ask_price_);
}

/// max_length 长度大小 字节
size_t fld_order_statistic_biz6::max_length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + 5 + 5 +
           sizeof(weighted_average_bid_price_) +
           sizeof(weighted_average_ask_price_);
}

// marshal 序列化
int32_t fld_order_statistic_biz6::marshal(char* buf, size_t len) const {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }
    if (len < max_length()) {
        return err::short_length;
    }

    size_t offset = 0;
    memcpy(buf + offset, &field_size_, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(buf + offset, &field_id_, sizeof(field_id_));
    offset += sizeof(field_id_);
    offset += vint_encode(total_buy_order_qty_, buf + offset);
    offset += vint_encode(total_sell_order_qty_, buf + offset);
    memcpy(buf + offset, &weighted_average_bid_price_,
           sizeof(weighted_average_bid_price_));
    offset += sizeof(weighted_average_bid_price_);
    memcpy(buf + offset, &weighted_average_ask_price_,
           sizeof(weighted_average_ask_price_));
    offset += sizeof(weighted_average_ask_price_);
    field_size_ = static_cast<uint16_t>(offset);
    memcpy(buf, &field_size_, sizeof(field_size_));
    return static_cast<int32_t>(offset);
}

// unmarshal 反序列化
int32_t fld_order_statistic_biz6::unmarshal(const char* buf, size_t len) {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }

    uint16_t ulen = 0;
    uint16_t ufid = 0;

    size_t offset = 0;
    memcpy(&ulen, buf + offset, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(&ufid, buf + offset, sizeof(field_id_));
    offset += sizeof(field_id_);
    if (len < ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }
    if (field_id_ != ufid) {
        return err::field_type_err;
    }
    offset += vint_decode(total_buy_order_qty_, buf + offset);
    offset += vint_decode(total_sell_order_qty_, buf + offset);
    memcpy(&weighted_average_bid_price_, buf + offset,
           sizeof(weighted_average_bid_price_));
    offset += sizeof(weighted_average_bid_price_);
    memcpy(&weighted_average_ask_price_, buf + offset,
           sizeof(weighted_average_ask_price_));
    offset += sizeof(weighted_average_ask_price_);
    if (offset != ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }

    return static_cast<int32_t>(offset);
}

// to_string 生成描述文字
std::string fld_order_statistic_biz6::to_string(void) const {
    std::stringstream oss;
    oss << "{ typename:fld_order_statistic_biz6"
        << ", len:" << length() << ", fid:0x" << std::hex << field_id_
        << std::dec << ", total_buy_order_qty:" << total_buy_order_qty_
        << ", total_sell_order_qty:" << total_sell_order_qty_
        << ", weighted_average_bid_price:" << weighted_average_bid_price_
        << ", weighted_average_ask_price:" << weighted_average_ask_price_
        << "}";
    return oss.str();
}

fld_extend_quot_common::fld_extend_quot_common()
    : field(fid::extend_quot_common), contract_no_{0},
      contract_seq_no_{0}, mdg_no_{0}, seq_no_{0}, send_time_{0} {
    field_size_ = length();
}

// id 标识
uint16_t fld_extend_quot_common::id() const { return fid::extend_quot_common; }

// length 长度大小 字节
size_t fld_extend_quot_common::length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + sizeof(contract_no_) +
           sizeof(contract_seq_no_) + sizeof(mdg_no_) + sizeof(seq_no_) +
           sizeof(send_time_);
}

/// max_length 长度大小 字节
size_t fld_extend_quot_common::max_length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + 5 + 10 +
           sizeof(mdg_no_) + sizeof(seq_no_) + sizeof(send_time_);
}

// marshal 序列化
int32_t fld_extend_quot_common::marshal(char* buf, size_t len) const {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }
    if (len < max_length()) {
        return err::short_length;
    }

    size_t offset = 0;
    memcpy(buf + offset, &field_size_, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(buf + offset, &field_id_, sizeof(field_id_));
    offset += sizeof(field_id_);
    offset += vint_encode(contract_no_, buf + offset);
    offset += vint_encode(contract_seq_no_, buf + offset);
    memcpy(buf + offset, &mdg_no_, sizeof(mdg_no_));
    offset += sizeof(mdg_no_);
    memcpy(buf + offset, &seq_no_, sizeof(seq_no_));
    offset += sizeof(seq_no_);
    memcpy(buf + offset, &send_time_, sizeof(send_time_));
    offset += sizeof(send_time_);
    field_size_ = static_cast<uint16_t>(offset);
    memcpy(buf, &field_size_, sizeof(field_size_));
    return static_cast<int32_t>(offset);
}

// unmarshal 反序列化
int32_t fld_extend_quot_common::unmarshal(const char* buf, size_t len) {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }

    uint16_t ulen = 0;
    uint16_t ufid = 0;

    size_t offset = 0;
    memcpy(&ulen, buf + offset, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(&ufid, buf + offset, sizeof(field_id_));
    offset += sizeof(field_id_);
    if (len < ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }
    if (field_id_ != ufid) {
        return err::field_type_err;
    }
    offset += vint_decode(contract_no_, buf + offset);
    offset += vint_decode(contract_seq_no_, buf + offset);
    memcpy(&mdg_no_, buf + offset, sizeof(mdg_no_));
    offset += sizeof(mdg_no_);
    memcpy(&seq_no_, buf + offset, sizeof(seq_no_));
    offset += sizeof(seq_no_);
    memcpy(&send_time_, buf + offset, sizeof(send_time_));
    offset += sizeof(send_time_);
    if (offset != ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }

    return static_cast<int32_t>(offset);
}

// to_string 生成描述文字
std::string fld_extend_quot_common::to_string(void) const {
    std::stringstream oss;
    oss << "{ typename:fld_extend_quot_common"
        << ", len:" << length() << ", fid:0x" << std::hex << field_id_
        << std::dec << ", contract_no:" << contract_no_
        << ", contract_seq_no:" << contract_seq_no_
        << ", mdg_no:" << (uint16_t)mdg_no_ << ", seq_no:" << seq_no_
        << ", send_time:" << send_time_ << "}";
    return oss.str();
}

fld_rsp_msg::fld_rsp_msg() : field(fid::rsp_msg), err_code_{0}, time_stamp_{0} {
    field_size_ = length();
}

// id 标识
uint16_t fld_rsp_msg::id() const { return fid::rsp_msg; }

// length 长度大小 字节
size_t fld_rsp_msg::length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + sizeof(err_code_) +
           sizeof(time_stamp_);
}

/// max_length 长度大小 字节
size_t fld_rsp_msg::max_length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + sizeof(err_code_) +
           sizeof(time_stamp_);
}

// marshal 序列化
int32_t fld_rsp_msg::marshal(char* buf, size_t len) const {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }
    if (len < max_length()) {
        return err::short_length;
    }

    size_t offset = 0;
    memcpy(buf + offset, &field_size_, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(buf + offset, &field_id_, sizeof(field_id_));
    offset += sizeof(field_id_);
    memcpy(buf + offset, &err_code_, sizeof(err_code_));
    offset += sizeof(err_code_);
    memcpy(buf + offset, &time_stamp_, sizeof(time_stamp_));
    offset += sizeof(time_stamp_);
    field_size_ = static_cast<uint16_t>(offset);
    memcpy(buf, &field_size_, sizeof(field_size_));
    return static_cast<int32_t>(offset);
}

// unmarshal 反序列化
int32_t fld_rsp_msg::unmarshal(const char* buf, size_t len) {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }

    uint16_t ulen = 0;
    uint16_t ufid = 0;

    size_t offset = 0;
    memcpy(&ulen, buf + offset, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(&ufid, buf + offset, sizeof(field_id_));
    offset += sizeof(field_id_);
    if (len < ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }
    if (field_id_ != ufid) {
        return err::field_type_err;
    }
    memcpy(&err_code_, buf + offset, sizeof(err_code_));
    offset += sizeof(err_code_);
    memcpy(&time_stamp_, buf + offset, sizeof(time_stamp_));
    offset += sizeof(time_stamp_);
    if (offset != ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }

    return static_cast<int32_t>(offset);
}

// to_string 生成描述文字
std::string fld_rsp_msg::to_string(void) const {
    std::stringstream oss;
    oss << "{ typename:fld_rsp_msg"
        << ", len:" << length() << ", fid:0x" << std::hex << field_id_
        << std::dec << ", err_code:" << err_code_
        << ", time_stamp:" << time_stamp_ << "}";
    return oss.str();
}

fld_trader_login_req::fld_trader_login_req()
    : field(fid::trader_login_req), trader_id_{0}, pwd_{0} {
    field_size_ = length();
}

// id 标识
uint16_t fld_trader_login_req::id() const { return fid::trader_login_req; }

// length 长度大小 字节
size_t fld_trader_login_req::length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + sizeof(trader_id_) +
           sizeof(pwd_);
}

/// max_length 长度大小 字节
size_t fld_trader_login_req::max_length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + sizeof(trader_id_) +
           sizeof(pwd_);
}

// marshal 序列化
int32_t fld_trader_login_req::marshal(char* buf, size_t len) const {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }
    if (len < max_length()) {
        return err::short_length;
    }

    size_t offset = 0;
    memcpy(buf + offset, &field_size_, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(buf + offset, &field_id_, sizeof(field_id_));
    offset += sizeof(field_id_);
    memcpy(buf + offset, &trader_id_, sizeof(trader_id_));
    offset += sizeof(trader_id_);
    memcpy(buf + offset, &pwd_, sizeof(pwd_));
    offset += sizeof(pwd_);
    field_size_ = static_cast<uint16_t>(offset);
    memcpy(buf, &field_size_, sizeof(field_size_));
    return static_cast<int32_t>(offset);
}

// unmarshal 反序列化
int32_t fld_trader_login_req::unmarshal(const char* buf, size_t len) {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }

    uint16_t ulen = 0;
    uint16_t ufid = 0;

    size_t offset = 0;
    memcpy(&ulen, buf + offset, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(&ufid, buf + offset, sizeof(field_id_));
    offset += sizeof(field_id_);
    if (len < ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }
    if (field_id_ != ufid) {
        return err::field_type_err;
    }
    memcpy(&trader_id_, buf + offset, sizeof(trader_id_));
    offset += sizeof(trader_id_);
    memcpy(&pwd_, buf + offset, sizeof(pwd_));
    offset += sizeof(pwd_);
    if (offset != ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }

    return static_cast<int32_t>(offset);
}

// to_string 生成描述文字
std::string fld_trader_login_req::to_string(void) const {
    std::stringstream oss;
    oss << "{ typename:fld_trader_login_req"
        << ", len:" << length() << ", fid:0x" << std::hex << field_id_
        << std::dec << ", trader_id:" << trader_id_ << ", pwd:" << pwd_ << "}";
    return oss.str();
}

fld_trader_login_rsp::fld_trader_login_rsp()
    : field(fid::trader_login_rsp), member_id_{0}, trader_id_{0}, date_{0},
      time_{0}, is_first_login_{0}, pwd_expire_reminder_{0}, days_overdue_{0},
      last_login_flag_{0}, last_login_date_{0}, last_login_time_{0},
      last_login_ip_{0}, failed_times_{0} {
    field_size_ = length();
}

// id 标识
uint16_t fld_trader_login_rsp::id() const { return fid::trader_login_rsp; }

// length 长度大小 字节
size_t fld_trader_login_rsp::length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + sizeof(member_id_) +
           sizeof(trader_id_) + sizeof(date_) + sizeof(time_) +
           sizeof(is_first_login_) + sizeof(pwd_expire_reminder_) +
           sizeof(days_overdue_) + sizeof(last_login_flag_) +
           sizeof(last_login_date_) + sizeof(last_login_time_) +
           sizeof(last_login_ip_) + sizeof(failed_times_);
}

/// max_length 长度大小 字节
size_t fld_trader_login_rsp::max_length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + sizeof(member_id_) +
           sizeof(trader_id_) + sizeof(date_) + sizeof(time_) +
           sizeof(is_first_login_) + sizeof(pwd_expire_reminder_) +
           sizeof(days_overdue_) + sizeof(last_login_flag_) +
           sizeof(last_login_date_) + sizeof(last_login_time_) +
           sizeof(last_login_ip_) + sizeof(failed_times_);
}

// marshal 序列化
int32_t fld_trader_login_rsp::marshal(char* buf, size_t len) const {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }
    if (len < max_length()) {
        return err::short_length;
    }

    size_t offset = 0;
    memcpy(buf + offset, &field_size_, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(buf + offset, &field_id_, sizeof(field_id_));
    offset += sizeof(field_id_);
    memcpy(buf + offset, &member_id_, sizeof(member_id_));
    offset += sizeof(member_id_);
    memcpy(buf + offset, &trader_id_, sizeof(trader_id_));
    offset += sizeof(trader_id_);
    memcpy(buf + offset, &date_, sizeof(date_));
    offset += sizeof(date_);
    memcpy(buf + offset, &time_, sizeof(time_));
    offset += sizeof(time_);
    memcpy(buf + offset, &is_first_login_, sizeof(is_first_login_));
    offset += sizeof(is_first_login_);
    memcpy(buf + offset, &pwd_expire_reminder_, sizeof(pwd_expire_reminder_));
    offset += sizeof(pwd_expire_reminder_);
    memcpy(buf + offset, &days_overdue_, sizeof(days_overdue_));
    offset += sizeof(days_overdue_);
    memcpy(buf + offset, &last_login_flag_, sizeof(last_login_flag_));
    offset += sizeof(last_login_flag_);
    memcpy(buf + offset, &last_login_date_, sizeof(last_login_date_));
    offset += sizeof(last_login_date_);
    memcpy(buf + offset, &last_login_time_, sizeof(last_login_time_));
    offset += sizeof(last_login_time_);
    memcpy(buf + offset, &last_login_ip_, sizeof(last_login_ip_));
    offset += sizeof(last_login_ip_);
    memcpy(buf + offset, &failed_times_, sizeof(failed_times_));
    offset += sizeof(failed_times_);
    field_size_ = static_cast<uint16_t>(offset);
    memcpy(buf, &field_size_, sizeof(field_size_));
    return static_cast<int32_t>(offset);
}

// unmarshal 反序列化
int32_t fld_trader_login_rsp::unmarshal(const char* buf, size_t len) {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }

    uint16_t ulen = 0;
    uint16_t ufid = 0;

    size_t offset = 0;
    memcpy(&ulen, buf + offset, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(&ufid, buf + offset, sizeof(field_id_));
    offset += sizeof(field_id_);
    if (len < ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }
    if (field_id_ != ufid) {
        return err::field_type_err;
    }
    memcpy(&member_id_, buf + offset, sizeof(member_id_));
    offset += sizeof(member_id_);
    memcpy(&trader_id_, buf + offset, sizeof(trader_id_));
    offset += sizeof(trader_id_);
    memcpy(&date_, buf + offset, sizeof(date_));
    offset += sizeof(date_);
    memcpy(&time_, buf + offset, sizeof(time_));
    offset += sizeof(time_);
    memcpy(&is_first_login_, buf + offset, sizeof(is_first_login_));
    offset += sizeof(is_first_login_);
    memcpy(&pwd_expire_reminder_, buf + offset, sizeof(pwd_expire_reminder_));
    offset += sizeof(pwd_expire_reminder_);
    memcpy(&days_overdue_, buf + offset, sizeof(days_overdue_));
    offset += sizeof(days_overdue_);
    memcpy(&last_login_flag_, buf + offset, sizeof(last_login_flag_));
    offset += sizeof(last_login_flag_);
    memcpy(&last_login_date_, buf + offset, sizeof(last_login_date_));
    offset += sizeof(last_login_date_);
    memcpy(&last_login_time_, buf + offset, sizeof(last_login_time_));
    offset += sizeof(last_login_time_);
    memcpy(&last_login_ip_, buf + offset, sizeof(last_login_ip_));
    offset += sizeof(last_login_ip_);
    memcpy(&failed_times_, buf + offset, sizeof(failed_times_));
    offset += sizeof(failed_times_);
    if (offset != ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }

    return static_cast<int32_t>(offset);
}

// to_string 生成描述文字
std::string fld_trader_login_rsp::to_string(void) const {
    std::stringstream oss;
    oss << "{ typename:fld_trader_login_rsp"
        << ", len:" << length() << ", fid:0x" << std::hex << field_id_
        << std::dec << ", member_id:" << member_id_
        << ", trader_id:" << trader_id_ << ", date:" << date_
        << ", time:" << time_
        << ", is_first_login:" << (uint16_t)is_first_login_
        << ", pwd_expire_reminder:" << (uint16_t)pwd_expire_reminder_
        << ", days_overdue:" << days_overdue_
        << ", last_login_flag:" << (uint16_t)last_login_flag_
        << ", last_login_date:" << last_login_date_
        << ", last_login_time:" << last_login_time_
        << ", last_login_ip:" << last_login_ip_
        << ", failed_times:" << failed_times_ << "}";
    return oss.str();
}

fld_trader_id::fld_trader_id() : field(fid::trader_id), trader_id_{0} {
    field_size_ = length();
}

// id 标识
uint16_t fld_trader_id::id() const { return fid::trader_id; }

// length 长度大小 字节
size_t fld_trader_id::length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + sizeof(trader_id_);
}

/// max_length 长度大小 字节
size_t fld_trader_id::max_length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + sizeof(trader_id_);
}

// marshal 序列化
int32_t fld_trader_id::marshal(char* buf, size_t len) const {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }
    if (len < max_length()) {
        return err::short_length;
    }

    size_t offset = 0;
    memcpy(buf + offset, &field_size_, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(buf + offset, &field_id_, sizeof(field_id_));
    offset += sizeof(field_id_);
    memcpy(buf + offset, &trader_id_, sizeof(trader_id_));
    offset += sizeof(trader_id_);
    field_size_ = static_cast<uint16_t>(offset);
    memcpy(buf, &field_size_, sizeof(field_size_));
    return static_cast<int32_t>(offset);
}

// unmarshal 反序列化
int32_t fld_trader_id::unmarshal(const char* buf, size_t len) {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }

    uint16_t ulen = 0;
    uint16_t ufid = 0;

    size_t offset = 0;
    memcpy(&ulen, buf + offset, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(&ufid, buf + offset, sizeof(field_id_));
    offset += sizeof(field_id_);
    if (len < ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }
    if (field_id_ != ufid) {
        return err::field_type_err;
    }
    memcpy(&trader_id_, buf + offset, sizeof(trader_id_));
    offset += sizeof(trader_id_);
    if (offset != ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }

    return static_cast<int32_t>(offset);
}

// to_string 生成描述文字
std::string fld_trader_id::to_string(void) const {
    std::stringstream oss;
    oss << "{ typename:fld_trader_id"
        << ", len:" << length() << ", fid:0x" << std::hex << field_id_
        << std::dec << ", trader_id:" << trader_id_ << "}";
    return oss.str();
}

fld_flow_ctrl_warning::fld_flow_ctrl_warning()
    : field(fid::flow_ctrl_warning), trader_id_{0}, trigger_time_{0},
      pkg_type_gid_{0}, instantaneous_pkg_cnt_{0} {
    field_size_ = length();
}

// id 标识
uint16_t fld_flow_ctrl_warning::id() const { return fid::flow_ctrl_warning; }

// length 长度大小 字节
size_t fld_flow_ctrl_warning::length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + sizeof(trader_id_) +
           sizeof(trigger_time_) + sizeof(pkg_type_gid_) +
           sizeof(instantaneous_pkg_cnt_);
}

/// max_length 长度大小 字节
size_t fld_flow_ctrl_warning::max_length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + sizeof(trader_id_) +
           sizeof(trigger_time_) + sizeof(pkg_type_gid_) +
           sizeof(instantaneous_pkg_cnt_);
}

// marshal 序列化
int32_t fld_flow_ctrl_warning::marshal(char* buf, size_t len) const {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }
    if (len < max_length()) {
        return err::short_length;
    }

    size_t offset = 0;
    memcpy(buf + offset, &field_size_, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(buf + offset, &field_id_, sizeof(field_id_));
    offset += sizeof(field_id_);
    memcpy(buf + offset, &trader_id_, sizeof(trader_id_));
    offset += sizeof(trader_id_);
    memcpy(buf + offset, &trigger_time_, sizeof(trigger_time_));
    offset += sizeof(trigger_time_);
    memcpy(buf + offset, &pkg_type_gid_, sizeof(pkg_type_gid_));
    offset += sizeof(pkg_type_gid_);
    memcpy(buf + offset, &instantaneous_pkg_cnt_,
           sizeof(instantaneous_pkg_cnt_));
    offset += sizeof(instantaneous_pkg_cnt_);
    field_size_ = static_cast<uint16_t>(offset);
    memcpy(buf, &field_size_, sizeof(field_size_));
    return static_cast<int32_t>(offset);
}

// unmarshal 反序列化
int32_t fld_flow_ctrl_warning::unmarshal(const char* buf, size_t len) {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }

    uint16_t ulen = 0;
    uint16_t ufid = 0;

    size_t offset = 0;
    memcpy(&ulen, buf + offset, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(&ufid, buf + offset, sizeof(field_id_));
    offset += sizeof(field_id_);
    if (len < ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }
    if (field_id_ != ufid) {
        return err::field_type_err;
    }
    memcpy(&trader_id_, buf + offset, sizeof(trader_id_));
    offset += sizeof(trader_id_);
    memcpy(&trigger_time_, buf + offset, sizeof(trigger_time_));
    offset += sizeof(trigger_time_);
    memcpy(&pkg_type_gid_, buf + offset, sizeof(pkg_type_gid_));
    offset += sizeof(pkg_type_gid_);
    memcpy(&instantaneous_pkg_cnt_, buf + offset,
           sizeof(instantaneous_pkg_cnt_));
    offset += sizeof(instantaneous_pkg_cnt_);
    if (offset != ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }

    return static_cast<int32_t>(offset);
}

// to_string 生成描述文字
std::string fld_flow_ctrl_warning::to_string(void) const {
    std::stringstream oss;
    oss << "{ typename:fld_flow_ctrl_warning"
        << ", len:" << length() << ", fid:0x" << std::hex << field_id_
        << std::dec << ", trader_id:" << trader_id_
        << ", trigger_time:" << trigger_time_
        << ", pkg_type_gid:" << pkg_type_gid_
        << ", instantaneous_pkg_cnt:" << instantaneous_pkg_cnt_ << "}";
    return oss.str();
}

fld_req_contract_basic_info::fld_req_contract_basic_info()
    : field(fid::req_contract_basic_info), trader_id_{0}, contract_id_{0} {
    field_size_ = length();
}

// id 标识
uint16_t fld_req_contract_basic_info::id() const {
    return fid::req_contract_basic_info;
}

// length 长度大小 字节
size_t fld_req_contract_basic_info::length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + sizeof(trader_id_) +
           sizeof(contract_id_);
}

/// max_length 长度大小 字节
size_t fld_req_contract_basic_info::max_length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + sizeof(trader_id_) +
           sizeof(contract_id_);
}

// marshal 序列化
int32_t fld_req_contract_basic_info::marshal(char* buf, size_t len) const {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }
    if (len < max_length()) {
        return err::short_length;
    }

    size_t offset = 0;
    memcpy(buf + offset, &field_size_, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(buf + offset, &field_id_, sizeof(field_id_));
    offset += sizeof(field_id_);
    memcpy(buf + offset, &trader_id_, sizeof(trader_id_));
    offset += sizeof(trader_id_);
    memcpy(buf + offset, &contract_id_, sizeof(contract_id_));
    offset += sizeof(contract_id_);
    field_size_ = static_cast<uint16_t>(offset);
    memcpy(buf, &field_size_, sizeof(field_size_));
    return static_cast<int32_t>(offset);
}

// unmarshal 反序列化
int32_t fld_req_contract_basic_info::unmarshal(const char* buf, size_t len) {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }

    uint16_t ulen = 0;
    uint16_t ufid = 0;

    size_t offset = 0;
    memcpy(&ulen, buf + offset, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(&ufid, buf + offset, sizeof(field_id_));
    offset += sizeof(field_id_);
    if (len < ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }
    if (field_id_ != ufid) {
        return err::field_type_err;
    }
    memcpy(&trader_id_, buf + offset, sizeof(trader_id_));
    offset += sizeof(trader_id_);
    memcpy(&contract_id_, buf + offset, sizeof(contract_id_));
    offset += sizeof(contract_id_);
    if (offset != ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }

    return static_cast<int32_t>(offset);
}

// to_string 生成描述文字
std::string fld_req_contract_basic_info::to_string(void) const {
    std::stringstream oss;
    oss << "{ typename:fld_req_contract_basic_info"
        << ", len:" << length() << ", fid:0x" << std::hex << field_id_
        << std::dec << ", trader_id:" << trader_id_
        << ", contract_id:" << contract_id_ << "}";
    return oss.str();
}

fld_contract_basic_info::fld_contract_basic_info()
    : field(fid::contract_basic_info), contract_no_{0}, contract_id_{0},
      trade_date_{0}, contract_type_{0}, init_open_interest_{0},
      limit_up_price_{0}, limit_down_price_{0}, last_settlement_price_{0},
      last_closing_price_{0}, codec_price_{0}, tick_{0}, contract_name_{0} {
    field_size_ = length();
}

// id 标识
uint16_t fld_contract_basic_info::id() const {
    return fid::contract_basic_info;
}

// length 长度大小 字节
size_t fld_contract_basic_info::length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + sizeof(contract_no_) +
           sizeof(contract_id_) + sizeof(trade_date_) + sizeof(contract_type_) +
           sizeof(init_open_interest_) + sizeof(limit_up_price_) +
           sizeof(limit_down_price_) + sizeof(last_settlement_price_) +
           sizeof(last_closing_price_) + sizeof(codec_price_) + sizeof(tick_) +
           sizeof(contract_name_);
}

/// max_length 长度大小 字节
size_t fld_contract_basic_info::max_length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + sizeof(contract_no_) +
           sizeof(contract_id_) + sizeof(trade_date_) + sizeof(contract_type_) +
           sizeof(init_open_interest_) + sizeof(limit_up_price_) +
           sizeof(limit_down_price_) + sizeof(last_settlement_price_) +
           sizeof(last_closing_price_) + sizeof(codec_price_) + sizeof(tick_) +
           sizeof(contract_name_);
}

// marshal 序列化
int32_t fld_contract_basic_info::marshal(char* buf, size_t len) const {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }
    if (len < max_length()) {
        return err::short_length;
    }

    size_t offset = 0;
    memcpy(buf + offset, &field_size_, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(buf + offset, &field_id_, sizeof(field_id_));
    offset += sizeof(field_id_);
    memcpy(buf + offset, &contract_no_, sizeof(contract_no_));
    offset += sizeof(contract_no_);
    memcpy(buf + offset, &contract_id_, sizeof(contract_id_));
    offset += sizeof(contract_id_);
    memcpy(buf + offset, &trade_date_, sizeof(trade_date_));
    offset += sizeof(trade_date_);
    memcpy(buf + offset, &contract_type_, sizeof(contract_type_));
    offset += sizeof(contract_type_);
    memcpy(buf + offset, &init_open_interest_, sizeof(init_open_interest_));
    offset += sizeof(init_open_interest_);
    memcpy(buf + offset, &limit_up_price_, sizeof(limit_up_price_));
    offset += sizeof(limit_up_price_);
    memcpy(buf + offset, &limit_down_price_, sizeof(limit_down_price_));
    offset += sizeof(limit_down_price_);
    memcpy(buf + offset, &last_settlement_price_,
           sizeof(last_settlement_price_));
    offset += sizeof(last_settlement_price_);
    memcpy(buf + offset, &last_closing_price_, sizeof(last_closing_price_));
    offset += sizeof(last_closing_price_);
    memcpy(buf + offset, &codec_price_, sizeof(codec_price_));
    offset += sizeof(codec_price_);
    memcpy(buf + offset, &tick_, sizeof(tick_));
    offset += sizeof(tick_);
    memcpy(buf + offset, &contract_name_, sizeof(contract_name_));
    offset += sizeof(contract_name_);
    field_size_ = static_cast<uint16_t>(offset);
    memcpy(buf, &field_size_, sizeof(field_size_));
    return static_cast<int32_t>(offset);
}

// unmarshal 反序列化
int32_t fld_contract_basic_info::unmarshal(const char* buf, size_t len) {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }

    uint16_t ulen = 0;
    uint16_t ufid = 0;

    size_t offset = 0;
    memcpy(&ulen, buf + offset, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(&ufid, buf + offset, sizeof(field_id_));
    offset += sizeof(field_id_);
    if (len < ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }
    if (field_id_ != ufid) {
        return err::field_type_err;
    }
    memcpy(&contract_no_, buf + offset, sizeof(contract_no_));
    offset += sizeof(contract_no_);
    memcpy(&contract_id_, buf + offset, sizeof(contract_id_));
    offset += sizeof(contract_id_);
    memcpy(&trade_date_, buf + offset, sizeof(trade_date_));
    offset += sizeof(trade_date_);
    memcpy(&contract_type_, buf + offset, sizeof(contract_type_));
    offset += sizeof(contract_type_);
    memcpy(&init_open_interest_, buf + offset, sizeof(init_open_interest_));
    offset += sizeof(init_open_interest_);
    memcpy(&limit_up_price_, buf + offset, sizeof(limit_up_price_));
    offset += sizeof(limit_up_price_);
    memcpy(&limit_down_price_, buf + offset, sizeof(limit_down_price_));
    offset += sizeof(limit_down_price_);
    memcpy(&last_settlement_price_, buf + offset,
           sizeof(last_settlement_price_));
    offset += sizeof(last_settlement_price_);
    memcpy(&last_closing_price_, buf + offset, sizeof(last_closing_price_));
    offset += sizeof(last_closing_price_);
    memcpy(&codec_price_, buf + offset, sizeof(codec_price_));
    offset += sizeof(codec_price_);
    memcpy(&tick_, buf + offset, sizeof(tick_));
    offset += sizeof(tick_);
    memcpy(&contract_name_, buf + offset, sizeof(contract_name_));
    offset += sizeof(contract_name_);
    if (offset != ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }

    return static_cast<int32_t>(offset);
}

// to_string 生成描述文字
std::string fld_contract_basic_info::to_string(void) const {
    std::stringstream oss;
    oss << "{ typename:fld_contract_basic_info"
        << ", len:" << length() << ", fid:0x" << std::hex << field_id_
        << std::dec << ", contract_no:" << contract_no_
        << ", contract_id:" << contract_id_ << ", trade_date:" << trade_date_
        << ", contract_type:" << (uint16_t)contract_type_
        << ", init_open_interest:" << init_open_interest_
        << ", limit_up_price:" << limit_up_price_
        << ", limit_down_price:" << limit_down_price_
        << ", last_settlement_price:" << last_settlement_price_
        << ", last_closing_price:" << last_closing_price_
        << ", codec_price:" << codec_price_ << ", tick:" << tick_
        << ", contract_name:" << contract_name_ << "}";
    return oss.str();
}

fld_contract_param::fld_contract_param()
    : field(fid::contract_param), contract_no_{0}, unit_{0}, tick_{0},
      buy_spec_margin_{0}, sell_spec_margin_{0}, buy_hedge_margin_{0},
      sell_hedge_margin_{0}, contract_expiration_date_{0}, last_trade_date_{0},
      last_deliver_date_{0} {
    field_size_ = length();
}

// id 标识
uint16_t fld_contract_param::id() const { return fid::contract_param; }

// length 长度大小 字节
size_t fld_contract_param::length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + sizeof(contract_no_) +
           sizeof(unit_) + sizeof(tick_) + sizeof(buy_spec_margin_) +
           sizeof(sell_spec_margin_) + sizeof(buy_hedge_margin_) +
           sizeof(sell_hedge_margin_) + sizeof(contract_expiration_date_) +
           sizeof(last_trade_date_) + sizeof(last_deliver_date_);
}

/// max_length 长度大小 字节
size_t fld_contract_param::max_length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + sizeof(contract_no_) +
           sizeof(unit_) + sizeof(tick_) + sizeof(buy_spec_margin_) +
           sizeof(sell_spec_margin_) + sizeof(buy_hedge_margin_) +
           sizeof(sell_hedge_margin_) + sizeof(contract_expiration_date_) +
           sizeof(last_trade_date_) + sizeof(last_deliver_date_);
}

// marshal 序列化
int32_t fld_contract_param::marshal(char* buf, size_t len) const {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }
    if (len < max_length()) {
        return err::short_length;
    }

    size_t offset = 0;
    memcpy(buf + offset, &field_size_, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(buf + offset, &field_id_, sizeof(field_id_));
    offset += sizeof(field_id_);
    memcpy(buf + offset, &contract_no_, sizeof(contract_no_));
    offset += sizeof(contract_no_);
    memcpy(buf + offset, &unit_, sizeof(unit_));
    offset += sizeof(unit_);
    memcpy(buf + offset, &tick_, sizeof(tick_));
    offset += sizeof(tick_);
    memcpy(buf + offset, &buy_spec_margin_, sizeof(buy_spec_margin_));
    offset += sizeof(buy_spec_margin_);
    memcpy(buf + offset, &sell_spec_margin_, sizeof(sell_spec_margin_));
    offset += sizeof(sell_spec_margin_);
    memcpy(buf + offset, &buy_hedge_margin_, sizeof(buy_hedge_margin_));
    offset += sizeof(buy_hedge_margin_);
    memcpy(buf + offset, &sell_hedge_margin_, sizeof(sell_hedge_margin_));
    offset += sizeof(sell_hedge_margin_);
    memcpy(buf + offset, &contract_expiration_date_,
           sizeof(contract_expiration_date_));
    offset += sizeof(contract_expiration_date_);
    memcpy(buf + offset, &last_trade_date_, sizeof(last_trade_date_));
    offset += sizeof(last_trade_date_);
    memcpy(buf + offset, &last_deliver_date_, sizeof(last_deliver_date_));
    offset += sizeof(last_deliver_date_);
    field_size_ = static_cast<uint16_t>(offset);
    memcpy(buf, &field_size_, sizeof(field_size_));
    return static_cast<int32_t>(offset);
}

// unmarshal 反序列化
int32_t fld_contract_param::unmarshal(const char* buf, size_t len) {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }

    uint16_t ulen = 0;
    uint16_t ufid = 0;

    size_t offset = 0;
    memcpy(&ulen, buf + offset, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(&ufid, buf + offset, sizeof(field_id_));
    offset += sizeof(field_id_);
    if (len < ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }
    if (field_id_ != ufid) {
        return err::field_type_err;
    }
    memcpy(&contract_no_, buf + offset, sizeof(contract_no_));
    offset += sizeof(contract_no_);
    memcpy(&unit_, buf + offset, sizeof(unit_));
    offset += sizeof(unit_);
    memcpy(&tick_, buf + offset, sizeof(tick_));
    offset += sizeof(tick_);
    memcpy(&buy_spec_margin_, buf + offset, sizeof(buy_spec_margin_));
    offset += sizeof(buy_spec_margin_);
    memcpy(&sell_spec_margin_, buf + offset, sizeof(sell_spec_margin_));
    offset += sizeof(sell_spec_margin_);
    memcpy(&buy_hedge_margin_, buf + offset, sizeof(buy_hedge_margin_));
    offset += sizeof(buy_hedge_margin_);
    memcpy(&sell_hedge_margin_, buf + offset, sizeof(sell_hedge_margin_));
    offset += sizeof(sell_hedge_margin_);
    memcpy(&contract_expiration_date_, buf + offset,
           sizeof(contract_expiration_date_));
    offset += sizeof(contract_expiration_date_);
    memcpy(&last_trade_date_, buf + offset, sizeof(last_trade_date_));
    offset += sizeof(last_trade_date_);
    memcpy(&last_deliver_date_, buf + offset, sizeof(last_deliver_date_));
    offset += sizeof(last_deliver_date_);
    if (offset != ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }

    return static_cast<int32_t>(offset);
}

// to_string 生成描述文字
std::string fld_contract_param::to_string(void) const {
    std::stringstream oss;
    oss << "{ typename:fld_contract_param"
        << ", len:" << length() << ", fid:0x" << std::hex << field_id_
        << std::dec << ", contract_no:" << contract_no_ << ", unit:" << unit_
        << ", tick:" << tick_ << ", buy_spec_margin:" << buy_spec_margin_
        << ", sell_spec_margin:" << sell_spec_margin_
        << ", buy_hedge_margin:" << buy_hedge_margin_
        << ", sell_hedge_margin:" << sell_hedge_margin_
        << ", contract_expiration_date:" << contract_expiration_date_
        << ", last_trade_date:" << last_trade_date_
        << ", last_deliver_date:" << last_deliver_date_ << "}";
    return oss.str();
}

fld_req_trade_plan::fld_req_trade_plan()
    : field(fid::req_trade_plan), trader_id_{0}, variety_id_{0}, trade_type_{
                                                                     0} {
    field_size_ = length();
}

// id 标识
uint16_t fld_req_trade_plan::id() const { return fid::req_trade_plan; }

// length 长度大小 字节
size_t fld_req_trade_plan::length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + sizeof(trader_id_) +
           sizeof(variety_id_) + sizeof(trade_type_);
}

/// max_length 长度大小 字节
size_t fld_req_trade_plan::max_length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + sizeof(trader_id_) +
           sizeof(variety_id_) + sizeof(trade_type_);
}

// marshal 序列化
int32_t fld_req_trade_plan::marshal(char* buf, size_t len) const {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }
    if (len < max_length()) {
        return err::short_length;
    }

    size_t offset = 0;
    memcpy(buf + offset, &field_size_, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(buf + offset, &field_id_, sizeof(field_id_));
    offset += sizeof(field_id_);
    memcpy(buf + offset, &trader_id_, sizeof(trader_id_));
    offset += sizeof(trader_id_);
    memcpy(buf + offset, &variety_id_, sizeof(variety_id_));
    offset += sizeof(variety_id_);
    memcpy(buf + offset, &trade_type_, sizeof(trade_type_));
    offset += sizeof(trade_type_);
    field_size_ = static_cast<uint16_t>(offset);
    memcpy(buf, &field_size_, sizeof(field_size_));
    return static_cast<int32_t>(offset);
}

// unmarshal 反序列化
int32_t fld_req_trade_plan::unmarshal(const char* buf, size_t len) {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }

    uint16_t ulen = 0;
    uint16_t ufid = 0;

    size_t offset = 0;
    memcpy(&ulen, buf + offset, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(&ufid, buf + offset, sizeof(field_id_));
    offset += sizeof(field_id_);
    if (len < ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }
    if (field_id_ != ufid) {
        return err::field_type_err;
    }
    memcpy(&trader_id_, buf + offset, sizeof(trader_id_));
    offset += sizeof(trader_id_);
    memcpy(&variety_id_, buf + offset, sizeof(variety_id_));
    offset += sizeof(variety_id_);
    memcpy(&trade_type_, buf + offset, sizeof(trade_type_));
    offset += sizeof(trade_type_);
    if (offset != ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }

    return static_cast<int32_t>(offset);
}

// to_string 生成描述文字
std::string fld_req_trade_plan::to_string(void) const {
    std::stringstream oss;
    oss << "{ typename:fld_req_trade_plan"
        << ", len:" << length() << ", fid:0x" << std::hex << field_id_
        << std::dec << ", trader_id:" << trader_id_
        << ", variety_id:" << variety_id_
        << ", trade_type:" << (uint16_t)trade_type_ << "}";
    return oss.str();
}

fld_trade_plan::fld_trade_plan()
    : field(fid::trade_plan), trade_date_{0}, variety_id_{0}, trade_type_{0},
      state_{0}, chg_time_{0}, trading_session_no_{0}, next_session_no_{0} {
    field_size_ = length();
}

// id 标识
uint16_t fld_trade_plan::id() const { return fid::trade_plan; }

// length 长度大小 字节
size_t fld_trade_plan::length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + sizeof(trade_date_) +
           sizeof(variety_id_) + sizeof(trade_type_) + sizeof(state_) +
           sizeof(chg_time_) + sizeof(trading_session_no_) +
           sizeof(next_session_no_);
}

/// max_length 长度大小 字节
size_t fld_trade_plan::max_length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + sizeof(trade_date_) +
           sizeof(variety_id_) + sizeof(trade_type_) + sizeof(state_) +
           sizeof(chg_time_) + sizeof(trading_session_no_) +
           sizeof(next_session_no_);
}

// marshal 序列化
int32_t fld_trade_plan::marshal(char* buf, size_t len) const {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }
    if (len < max_length()) {
        return err::short_length;
    }

    size_t offset = 0;
    memcpy(buf + offset, &field_size_, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(buf + offset, &field_id_, sizeof(field_id_));
    offset += sizeof(field_id_);
    memcpy(buf + offset, &trade_date_, sizeof(trade_date_));
    offset += sizeof(trade_date_);
    memcpy(buf + offset, &variety_id_, sizeof(variety_id_));
    offset += sizeof(variety_id_);
    memcpy(buf + offset, &trade_type_, sizeof(trade_type_));
    offset += sizeof(trade_type_);
    memcpy(buf + offset, &state_, sizeof(state_));
    offset += sizeof(state_);
    memcpy(buf + offset, &chg_time_, sizeof(chg_time_));
    offset += sizeof(chg_time_);
    memcpy(buf + offset, &trading_session_no_, sizeof(trading_session_no_));
    offset += sizeof(trading_session_no_);
    memcpy(buf + offset, &next_session_no_, sizeof(next_session_no_));
    offset += sizeof(next_session_no_);
    field_size_ = static_cast<uint16_t>(offset);
    memcpy(buf, &field_size_, sizeof(field_size_));
    return static_cast<int32_t>(offset);
}

// unmarshal 反序列化
int32_t fld_trade_plan::unmarshal(const char* buf, size_t len) {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }

    uint16_t ulen = 0;
    uint16_t ufid = 0;

    size_t offset = 0;
    memcpy(&ulen, buf + offset, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(&ufid, buf + offset, sizeof(field_id_));
    offset += sizeof(field_id_);
    if (len < ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }
    if (field_id_ != ufid) {
        return err::field_type_err;
    }
    memcpy(&trade_date_, buf + offset, sizeof(trade_date_));
    offset += sizeof(trade_date_);
    memcpy(&variety_id_, buf + offset, sizeof(variety_id_));
    offset += sizeof(variety_id_);
    memcpy(&trade_type_, buf + offset, sizeof(trade_type_));
    offset += sizeof(trade_type_);
    memcpy(&state_, buf + offset, sizeof(state_));
    offset += sizeof(state_);
    memcpy(&chg_time_, buf + offset, sizeof(chg_time_));
    offset += sizeof(chg_time_);
    memcpy(&trading_session_no_, buf + offset, sizeof(trading_session_no_));
    offset += sizeof(trading_session_no_);
    memcpy(&next_session_no_, buf + offset, sizeof(next_session_no_));
    offset += sizeof(next_session_no_);
    if (offset != ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }

    return static_cast<int32_t>(offset);
}

// to_string 生成描述文字
std::string fld_trade_plan::to_string(void) const {
    std::stringstream oss;
    oss << "{ typename:fld_trade_plan"
        << ", len:" << length() << ", fid:0x" << std::hex << field_id_
        << std::dec << ", trade_date:" << trade_date_
        << ", variety_id:" << variety_id_
        << ", trade_type:" << (uint16_t)trade_type_
        << ", state:" << (uint16_t)state_ << ", chg_time:" << chg_time_
        << ", trading_session_no:" << trading_session_no_
        << ", next_session_no:" << next_session_no_ << "}";
    return oss.str();
}

fld_qry_his_quot::fld_qry_his_quot()
    : field(fid::qry_his_quot), trader_id_{0}, mdg_no_{0}, start_seq_no_{0},
      end_seq_no_{0} {
    field_size_ = length();
}

// id 标识
uint16_t fld_qry_his_quot::id() const { return fid::qry_his_quot; }

// length 长度大小 字节
size_t fld_qry_his_quot::length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + sizeof(trader_id_) +
           sizeof(mdg_no_) + sizeof(start_seq_no_) + sizeof(end_seq_no_);
}

/// max_length 长度大小 字节
size_t fld_qry_his_quot::max_length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + sizeof(trader_id_) +
           sizeof(mdg_no_) + sizeof(start_seq_no_) + sizeof(end_seq_no_);
}

// marshal 序列化
int32_t fld_qry_his_quot::marshal(char* buf, size_t len) const {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }
    if (len < max_length()) {
        return err::short_length;
    }

    size_t offset = 0;
    memcpy(buf + offset, &field_size_, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(buf + offset, &field_id_, sizeof(field_id_));
    offset += sizeof(field_id_);
    memcpy(buf + offset, &trader_id_, sizeof(trader_id_));
    offset += sizeof(trader_id_);
    memcpy(buf + offset, &mdg_no_, sizeof(mdg_no_));
    offset += sizeof(mdg_no_);
    memcpy(buf + offset, &start_seq_no_, sizeof(start_seq_no_));
    offset += sizeof(start_seq_no_);
    memcpy(buf + offset, &end_seq_no_, sizeof(end_seq_no_));
    offset += sizeof(end_seq_no_);
    field_size_ = static_cast<uint16_t>(offset);
    memcpy(buf, &field_size_, sizeof(field_size_));
    return static_cast<int32_t>(offset);
}

// unmarshal 反序列化
int32_t fld_qry_his_quot::unmarshal(const char* buf, size_t len) {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }

    uint16_t ulen = 0;
    uint16_t ufid = 0;

    size_t offset = 0;
    memcpy(&ulen, buf + offset, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(&ufid, buf + offset, sizeof(field_id_));
    offset += sizeof(field_id_);
    if (len < ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }
    if (field_id_ != ufid) {
        return err::field_type_err;
    }
    memcpy(&trader_id_, buf + offset, sizeof(trader_id_));
    offset += sizeof(trader_id_);
    memcpy(&mdg_no_, buf + offset, sizeof(mdg_no_));
    offset += sizeof(mdg_no_);
    memcpy(&start_seq_no_, buf + offset, sizeof(start_seq_no_));
    offset += sizeof(start_seq_no_);
    memcpy(&end_seq_no_, buf + offset, sizeof(end_seq_no_));
    offset += sizeof(end_seq_no_);
    if (offset != ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }

    return static_cast<int32_t>(offset);
}

// to_string 生成描述文字
std::string fld_qry_his_quot::to_string(void) const {
    std::stringstream oss;
    oss << "{ typename:fld_qry_his_quot"
        << ", len:" << length() << ", fid:0x" << std::hex << field_id_
        << std::dec << ", trader_id:" << trader_id_
        << ", mdg_no:" << (uint16_t)mdg_no_
        << ", start_seq_no:" << start_seq_no_ << ", end_seq_no:" << end_seq_no_
        << "}";
    return oss.str();
}

fld_qry_quot_snap::fld_qry_quot_snap()
    : field(fid::qry_quot_snap), trader_id_{0}, mdg_no_{0}, contract_no_{0} {
    field_size_ = length();
}

// id 标识
uint16_t fld_qry_quot_snap::id() const { return fid::qry_quot_snap; }

// length 长度大小 字节
size_t fld_qry_quot_snap::length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + sizeof(trader_id_) +
           sizeof(mdg_no_) + sizeof(contract_no_);
}

/// max_length 长度大小 字节
size_t fld_qry_quot_snap::max_length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + sizeof(trader_id_) +
           sizeof(mdg_no_) + sizeof(contract_no_);
}

// marshal 序列化
int32_t fld_qry_quot_snap::marshal(char* buf, size_t len) const {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }
    if (len < max_length()) {
        return err::short_length;
    }

    size_t offset = 0;
    memcpy(buf + offset, &field_size_, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(buf + offset, &field_id_, sizeof(field_id_));
    offset += sizeof(field_id_);
    memcpy(buf + offset, &trader_id_, sizeof(trader_id_));
    offset += sizeof(trader_id_);
    memcpy(buf + offset, &mdg_no_, sizeof(mdg_no_));
    offset += sizeof(mdg_no_);
    memcpy(buf + offset, &contract_no_, sizeof(contract_no_));
    offset += sizeof(contract_no_);
    field_size_ = static_cast<uint16_t>(offset);
    memcpy(buf, &field_size_, sizeof(field_size_));
    return static_cast<int32_t>(offset);
}

// unmarshal 反序列化
int32_t fld_qry_quot_snap::unmarshal(const char* buf, size_t len) {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }

    uint16_t ulen = 0;
    uint16_t ufid = 0;

    size_t offset = 0;
    memcpy(&ulen, buf + offset, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(&ufid, buf + offset, sizeof(field_id_));
    offset += sizeof(field_id_);
    if (len < ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }
    if (field_id_ != ufid) {
        return err::field_type_err;
    }
    memcpy(&trader_id_, buf + offset, sizeof(trader_id_));
    offset += sizeof(trader_id_);
    memcpy(&mdg_no_, buf + offset, sizeof(mdg_no_));
    offset += sizeof(mdg_no_);
    memcpy(&contract_no_, buf + offset, sizeof(contract_no_));
    offset += sizeof(contract_no_);
    if (offset != ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }

    return static_cast<int32_t>(offset);
}

// to_string 生成描述文字
std::string fld_qry_quot_snap::to_string(void) const {
    std::stringstream oss;
    oss << "{ typename:fld_qry_quot_snap"
        << ", len:" << length() << ", fid:0x" << std::hex << field_id_
        << std::dec << ", trader_id:" << trader_id_
        << ", mdg_no:" << (uint16_t)mdg_no_ << ", contract_no:" << contract_no_
        << "}";
    return oss.str();
}

fld_max_no::fld_max_no() : field(fid::max_no), mdg_no_{0}, max_seq_no_{0} {
    field_size_ = length();
}

// id 标识
uint16_t fld_max_no::id() const { return fid::max_no; }

// length 长度大小 字节
size_t fld_max_no::length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + sizeof(mdg_no_) +
           sizeof(max_seq_no_);
}

/// max_length 长度大小 字节
size_t fld_max_no::max_length() const {
    return 0 + sizeof(field_size_) + sizeof(field_id_) + sizeof(mdg_no_) +
           sizeof(max_seq_no_);
}

// marshal 序列化
int32_t fld_max_no::marshal(char* buf, size_t len) const {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }
    if (len < max_length()) {
        return err::short_length;
    }

    size_t offset = 0;
    memcpy(buf + offset, &field_size_, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(buf + offset, &field_id_, sizeof(field_id_));
    offset += sizeof(field_id_);
    memcpy(buf + offset, &mdg_no_, sizeof(mdg_no_));
    offset += sizeof(mdg_no_);
    memcpy(buf + offset, &max_seq_no_, sizeof(max_seq_no_));
    offset += sizeof(max_seq_no_);
    field_size_ = static_cast<uint16_t>(offset);
    memcpy(buf, &field_size_, sizeof(field_size_));
    return static_cast<int32_t>(offset);
}

// unmarshal 反序列化
int32_t fld_max_no::unmarshal(const char* buf, size_t len) {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }

    uint16_t ulen = 0;
    uint16_t ufid = 0;

    size_t offset = 0;
    memcpy(&ulen, buf + offset, sizeof(field_size_));
    offset += sizeof(field_size_);
    memcpy(&ufid, buf + offset, sizeof(field_id_));
    offset += sizeof(field_id_);
    if (len < ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }
    if (field_id_ != ufid) {
        return err::field_type_err;
    }
    memcpy(&mdg_no_, buf + offset, sizeof(mdg_no_));
    offset += sizeof(mdg_no_);
    memcpy(&max_seq_no_, buf + offset, sizeof(max_seq_no_));
    offset += sizeof(max_seq_no_);
    if (offset != ulen) {
        return err::
            length_error; // 可能是协议版本问题或攻击，应该丢弃并断开连接
    }

    return static_cast<int32_t>(offset);
}

// to_string 生成描述文字
std::string fld_max_no::to_string(void) const {
    std::stringstream oss;
    oss << "{ typename:fld_max_no"
        << ", len:" << length() << ", fid:0x" << std::hex << field_id_
        << std::dec << ", mdg_no:" << (uint16_t)mdg_no_
        << ", max_seq_no:" << max_seq_no_ << "}";
    return oss.str();
}

// ------
// fields_t

// id 标识
uint16_t id(const fields_t& objs) { return 0; }

// length 大小，字节
size_t length(const fields_t& objs) {
    size_t len = 0;
    for (auto& item : objs) {
        if (nullptr == item.second) {
            continue;
        }
        len += item.second->length();
    }
    return len;
}

// max_length 大小，字节
size_t max_length(const fields_t& objs) {
    size_t len = 0;
    for (auto& item : objs) {
        if (nullptr == item.second) {
            continue;
        }
        len += item.second->max_length();
    }
    return len;
}

// init 初始化
void init(fields_t& objs, uint16_t id) {
    // unused(id)
    for (auto& item : objs) {
        if (nullptr != item.second) {
            delete item.second;
            item.second = nullptr;
        }
    }
    objs.clear();
}

// marshal 序列化
int32_t marshal(const fields_t& objs, char* buf, size_t len) {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }
    if (len < pkg::max_length(objs)) {
        return err::short_length;
    }

    int ret = 0;
    size_t offset = 0;

    for (auto& item : objs) {
        if (nullptr == item.second) {
            continue;
        }
        ret = item.second->marshal(buf + offset, len - offset);
        if (ret <= 0) {
            return ret;
        }
        offset += ret;
    }

    return static_cast<int32_t>(offset);
}

// unmarshal 反序列化
int32_t unmarshal(fields_t& objs, const char* buf, size_t len) {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return err::zero_length;
    }

    int ret = 0;
    size_t offset = 0;

    for (auto& item : objs) {
        if (nullptr == item.second) {
            continue;
        }
        ret = item.second->unmarshal(buf + offset, len - offset);
        if (ret <= 0) {
            return ret;
        }
        offset += ret;
    }

    return static_cast<int32_t>(offset);
}

// to_string 生成描述文字
std::string to_string(const fields_t& objs) {
    std::stringstream oss;
    oss << "{ typename:fields";
    for (auto& item : objs) {
        if (nullptr == item.second) {
            continue;
        }
        oss << ", " << item.second->to_string();
    }
    oss << "}";
    return oss.str();
}

// ------
// data

data::data() {}

data::~data() {
    if (nullptr != raw_data_) {
        delete raw_data_;
        raw_data_ = nullptr;
    }
    for (auto& item : fields_) {
        if (nullptr != item.second) {
            delete item.second;
            item.second = nullptr;
        }
    }
    fields_.clear();
}

// id 标识 只为接口兼容
uint16_t data::id() const { return pkg_type_; }

// length 大小 字节
size_t data::length() const {
    // 因最大包大小目前定义很小，此处没考虑溢出
    if (raw_) {
        return raw_len_;
    }

    size_t len = 0;
    for (auto& item : fields_) {
        if (nullptr == item.second) {
            continue;
        }
        len += item.second->length();
    }
    return len;
}

// init 初始化
void data::init(uint16_t id) {
    if (nullptr != raw_data_) {
        delete raw_data_;
        raw_data_ = nullptr;
    }
    raw_len_ = 0;
    raw_ = false;
    clear();
    pkg_type_ = id;
}

// init 初始化
void data::init(uint16_t id, bool raw) {
    if (nullptr != raw_data_) {
        delete raw_data_;
        raw_data_ = nullptr;
    }
    raw_len_ = 0;
    raw_ = raw;
    clear();
    pkg_type_ = id;
}

// marshal 序列化
int32_t data::marshal(char* buf, size_t len) const {
    if (len < (size_t)this->length()) {
        return err::short_length;
    }

    if (raw_) {
        memcpy(buf, raw_data_, raw_len_);
        return static_cast<int32_t>(raw_len_);
    }

    int ret = 0;
    size_t offset = 0;

    for (auto& item : fields_) {
        if (nullptr == item.second) {
            continue;
        }
        ret = item.second->marshal(buf + offset, len - offset);
        if (ret <= 0) {
            return ret;
        }
        offset += ret;
    }

    return static_cast<int32_t>(offset);
}

// unmarshal 反序列化
int32_t data::unmarshal(const char* buf, size_t len) {
    clear();

    if (raw_) {
        if (nullptr != raw_data_) {
            delete raw_data_;
        }
        raw_len_ = len;
        raw_data_ = new char[len];
        memcpy(raw_data_, buf, len);
        return static_cast<int32_t>(raw_len_);
    }

    int ret = 0;
    size_t offset = 0;
    field pre;

    while (offset < len) {
        // 预读
        ret = pre.unmarshal(buf + offset, len - offset);
        if (ret <= 0) {
            return ret;
        }

        // 读取
        auto fld = field::make(pre.field_id_);
        if (!fld) {
            LOG_ERROR("unknown field type. id = 0x{0:x}", pre.field_id_);
            offset += pre.field_size_;
            continue;
            // return err::field_type_err;
        }

        ret = fld->unmarshal(buf + offset, len - offset);
        if (ret <= 0) {
            return ret;
        }
        offset += ret;

        this->emplace(fld);
    }

    return static_cast<int32_t>(offset);
}

// unmarshal 反序列化
int32_t data::unmarshal() {
    clear();

    raw_ = false;
    int ret = unmarshal(raw_data_, raw_len_);
    if (ret <= 0) {
        raw_ = true;
        return ret;
    }
    if (nullptr != raw_data_) {
        delete raw_data_;
        raw_data_ = nullptr;
    }
    raw_len_ = 0;

    return ret;
}

// to_string 生成描述文字
std::string data::to_string() const {
    std::stringstream oss;
    oss << "{ typename:data"
        << ", len:" << length();
    if (!raw_) {
        oss << ", fields:" << pkg::to_string(fields_);
    } else {
        oss << ", raw:" << raw_len_ << "B";
    }
    oss << "}";
    return oss.str();
}

// find 查询某个fid
data::iterator data::find(uint16_t fid) { return fields_.find(fid); }

// begin 容器开始迭代器
data::iterator data::begin() { return fields_.begin(); }

// end 容器结束迭代器
data::iterator data::end() { return fields_.end(); }

// size 元素个数
size_t data::size() { return fields_.size(); }

// empty
bool data::empty() { return fields_.empty(); }

// count 统计以fid为key的元素个数，0 为不存在， 1为存在， 不可能大于1
size_t data::count(uint16_t fid) { return fields_.count(fid); }

// at 找到fid对应的域
data::second_type data::at(uint16_t key) {
    auto it = fields_.find(key);
    if (it != fields_.end()) {
        return it->second;
    }
    return nullptr;
}

// [] 下标定位
data::second_type data::operator[](uint16_t key) {
    auto it = fields_.find(key);
    if (it != fields_.end()) {
        return it->second;
    }
    return nullptr;
}

// equal_range 查询所有key
std::pair<data::iterator, data::iterator> data::equal_range(uint16_t key) {
    return fields_.equal_range(key);
}

// erase 删除一个元素
size_t data::erase(uint16_t fid) {
    auto it = fields_.find(fid);
    if (it == fields_.end()) {
        return 0;
    }
    if (nullptr != it->second) {
        delete it->second;
        it->second = nullptr;
    }
    return fields_.erase(fid);
}

// clear 清理
void data::clear() {
    for (auto& item : fields_) {
        if (nullptr != item.second) {
            delete item.second;
            item.second = nullptr;
        }
    }
    fields_.clear();
}

// emplace 插入元素
void data::emplace(data::value_type&& val) {
    val.second->idx_ = idx_++;
    fields_.emplace(val);
}

// emplace 插入元素
void data::emplace(data::second_type val) {
    val->idx_ = idx_++;
    fields_.emplace(std::make_pair(val->field_id_, val));
}

// insert 插入元素
void data::insert(data::iterator it, data::value_type& val) {
    val.second->idx_ = idx_++;
    fields_.insert(it, val);
}

// insert 插入元素
void data::insert(data::iterator it, const data::value_type& val) {
    val.second->idx_ = idx_++;
    fields_.insert(it, val);
}

// insert 插入元素
void data::insert(data::value_type& val) {
    val.second->idx_ = idx_++;
    fields_.insert(val);
}

// insert 插入元素
void data::insert(const data::value_type& val) {
    val.second->idx_ = idx_++;
    fields_.insert(val);
}

// insert 插入元素
void data::insert(data::second_type val) {
    val->idx_ = idx_++;
    fields_.insert(std::make_pair(val->field_id_, val));
}

// cbegin const
data::citerator data::cbegin() { return fields_.cbegin(); }

// end const
data::citerator data::cend() { return fields_.cend(); }

// reserve 预分配大小
void data::reserve(size_t size) {
    // return fields_.reserve(size);
}

// ------------
// body

body::body() : data_() {}

body::body(uint16_t tid) : pkg_type_(tid), data_() { data_.init(tid); }

body::~body() { data_.clear(); }

// id 标识
uint16_t body::id() const { return pkg_type_; }

// length 大小 字节
size_t body::length() const {
    // 因最大包大小目前定义很小，此处没考虑溢出
    return data_.length();
}

// init 初始化
void body::init(uint16_t id) {
    pkg_type_ = id;
    data_.init(id);
}

/// init 初始化
void body::init(const dmdp_header& head, bool raw) {
    pkg_type_ = head.pkg_type_;         /// 报文类型
    version_ = head.version_;           /// 版本号
    flag_ = head.flag_;                 /// 消息结束标示符
    mdg_no_ = head.mdg_no_;             /// 产品组号
    start_seq_no_ = head.start_seq_no_; /// 第一个行情消息的编号
    seq_num_ = head.seq_num_;           /// 扩展字段
    send_time_ = head.send_time_;       /// 发送时间

    data_.init(pkg_type_, raw);
}

/// init 初始化
void body::init(const dmqp_header& head, bool raw) {
    pkg_type_ = head.pkg_type_;     /// 报文类型
    version_ = head.version_;       /// 版本号
    flag_ = head.flag_;             /// 消息结束标示符
    request_no_ = head.request_no_; /// 请求号

    data_.init(pkg_type_, raw);
}

// init 初始化
void body::init(uint16_t id, bool raw) {
    pkg_type_ = id;
    data_.init(id, raw);
}

// marshal 序列化
int32_t body::marshal(char* buf, size_t len) const {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return len;
    }
    if (len < (size_t)this->length()) {
        return err::short_length;
    }

    size_t offset = 0;

    // data
    auto ret = data_.marshal(buf + offset, len - offset);
    if (ret <= 0) {
        return ret;
    }
    offset += ret;

    return static_cast<int32_t>(offset);
}

// unmarshal 反序列化
int32_t body::unmarshal(const char* buf, size_t len) {
    if (nullptr == buf) {
        return err::null_buf;
    }
    if (0 == len) {
        return 0;
    }

    size_t offset = 0;

    // data
    auto ret = data_.unmarshal(buf + offset, len - offset);
    if (ret <= 0) {
        return ret;
    }
    offset += ret;

    return static_cast<int32_t>(offset);
}

// unmarshal 反序列化
int32_t body::unmarshal() {
    // data
    int ret = data_.unmarshal();
    if (ret <= 0) {
        return ret;
    }
    return ret;
}

// to_string 生成描述文字
std::string body::to_string() const {
    std::stringstream oss;
    oss << "{ typename:body"
        << ", len:" << length() << ", tid:0x" << std::hex << pkg_type_
        << std::dec << ", fields:" << data_.to_string() << "}";
    return oss.str();
}

// is_relative 是否相关域
bool body::is_relative(uint16_t fid) {
    return pkg::is_relative(pkg_type_, fid);
}

// has_all 是否包含所有域
bool body::has_all() { return true; }

void body::set_pkg_type(uint16_t pkg_type) { pkg_type_ = pkg_type; }

uint16_t body::pkg_type() const { return pkg_type_; }

void body::set_version(uint8_t version) { version_ = version; }

uint8_t body::version() const { return version_; }

void body::set_flag(uint8_t flag) { flag_ = flag; }

uint8_t body::flag() const { return flag_; }

void body::set_mdg_no(uint8_t mdg_no) { mdg_no_ = mdg_no; }

uint8_t body::mdg_no() const { return mdg_no_; }

void body::set_start_seq_no(uint64_t start_seq_no) {
    start_seq_no_ = start_seq_no;
}

uint64_t body::start_seq_no() const { return start_seq_no_; }

void body::set_seq_num(uint8_t seq_num) { seq_num_ = seq_num; }

uint8_t body::seq_num() const { return seq_num_; }

void body::set_send_time(uint64_t send_time) { send_time_ = send_time; }

uint64_t body::send_time() const { return send_time_; }

void body::set_request_no(uint32_t request_no) { request_no_ = request_no; }

uint32_t body::request_no() const { return request_no_; }

int body::type(uint16_t tid) { PKG_ATTR_TYPE(tid); }

// -----------
// filters

bool filter_udp_pkg_max_size(char* buf, size_t len, const std::string& addr) {
    if (len > MDP_MAX_SIZE) {
        return false;
    }
    return true;
}

bool filter_udp_pkg_min_size(char* buf, size_t len, const std::string& addr) {
    if (len < dmdp_header::length()) {
        return false;
    }
    return true;
}

bool filter_tcp_pkg_max_size(char* buf, size_t len, const std::string& addr) {
    if (len > MDP_MAX_SIZE) {
        return false;
    }
    return true;
}

bool filter_tcp_pkg_min_size(char* buf, size_t len, const std::string& addr) {
    if (len < dmqp_header::length()) {
        return false;
    }
    return true;
}

// ======
} // namespace pkg
} // namespace mdp
