/**
 * Copyright (C) 2021，大连商品交易所 大连飞创信息技术有限公司
 *
 * @brief 行情业务示例
 *
 * 行情业务处理和回调类。封装请求，并对回报或者行情消息进行处理。
 *
 */

#include "md/DFITCMarketDataListener.hpp"
#include "mdp/log.hpp"
#include "spdlog/spdlog.h"

#include "mdp/version.hpp"
#include "json/json.hpp"
#include "mdp/errors.hpp"

#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

mdp::options options;
std::string tcp_addr;
std::string udp_addr_1;
std::string udp_addr_2;
std::string trader_id;
std::string passwd;

int log_level = 1;

bool load_config() {
    Env& env = Env::getInstance();
    try {
        options.timeout_ms = env["mdDFITC"]["timeout_ms"]; // ms
        options.heart_interval_ms =
            env["mdDFITC"]["timeout_ms"];                       // ms
        options.heart_max_failed = env["mdDFITC"]["heart_max_failed"]; // 次
        options.cache_win = env["mdDFITC"]["cache_win"];
        options.cache_size = env["mdDFITC"]["cache_size"];
        options.reconnect_max_times = env["mdDFITC"]["reconnect_max_times"];

        trader_id = env["mdDFITC"]["trader_id"];
        passwd = env["mdDFITC"]["passwd"];
#ifdef MDP_LN_QUOTER
        tcp_addr = env["mdDFITC"]["ln_tcp_addr"];
        udp_addr_1 = env["mdDFITC"]["ln_udp_addr_1"];
        udp_addr_2 = env["mdDFITC"]["ln_udp_addr_2"];
#else
        tcp_addr = env["mdDFITC"]["l1_tcp_addr"];
        udp_addr_1 = env["mdDFITC"]["l1_udp_addr_1"];
        udp_addr_2 = env["mdDFITC"]["l1_udp_addr_2"];
#endif
        log_level = env["mdDFITC"]["log_level"];
    } catch (std::exception& e) {
        std::cout << "parse config failed.file=" << " "
                  << " error=" << e.what() << std::endl;
        return false;
    } catch (...) {
        std::cout << "parse config throw exception.file=" << " "
                  << std::endl;
        return false;
    }

    return true;
}

DFITCMarketDataListener::DFITCMarketDataListener(void) : impl_(new mdp::quoter()) {
    impl_->set_rsp(this);
}

DFITCMarketDataListener::~DFITCMarketDataListener(void) {
    // 断开连接
    this->disconnect();
}

void DFITCMarketDataListener::start() {

    // 初始化，读取配置文件
    LOG_INIT(1, "mdp_quoter_");

    if (!load_config()) {
        std::cout << "parse config failed.file=" << " " << std::endl;
        return;
    }

    this->req_no = 0;

    // 选项
    this->set_options(options);

    // ln行情数据结构初始化
    ln_md.bid_md_count = 0;
    ln_md.ask_md_count = 0;
    for (int i = 0; i < 5; i++) {
        ln_md.bid_order_price[i] = -1;
        ln_md.bid_order_qty[i] = 0;
        ln_md.bid_imply_qty[i] = 0;
        ln_md.ask_order_price[i] = -1;
        ln_md.ask_order_qty[i] = 0;
        ln_md.ask_imply_qty[i] = 0;
    }

    m_pushMdThread = std::thread(&DFITCMarketDataListener::_pushMarketData, this);

    // udp开始接收, tcp连接
    auto ret = impl_->connect(udp_addr_1, udp_addr_2, tcp_addr);
    if (ret != 0) {
        std::cout << "connect(" << udp_addr_1 << ", " << udp_addr_2 << ", "
                  << tcp_addr << ")"
                  << "failed. ret=" << ret << std::endl;
        return;
    }

    // 登录
    mdp::mdp_rsp_msg msg;
    this->req_no++;
    mdp::mdp_trader_login_req login_req;
    memset(&login_req, 0, sizeof(login_req));
    strncpy(login_req.trader_id, trader_id.c_str(),
            sizeof(login_req.trader_id));
    strncpy(login_req.pwd, passwd.c_str(), sizeof(login_req.pwd));
    mdp::mdp_trader_login_rsp login_rsp;
    impl_->login(req_no, login_req, msg, login_rsp);
    if (msg.err_code != 0) {
        std::cout << "login failed. code=" << msg.err_code << std::endl;
        return;
    }
    // 查询合约
    this->req_no++;
    mdp::mdp_contract_basic_info_req contract_req;
    memset(&contract_req, 0, sizeof(contract_req));
    strncpy(contract_req.trader_id, trader_id.c_str(),
            sizeof(contract_req.trader_id));
    std::vector<mdp::mdp_contract_basic_info_rsp> contract_basic_info_rsp;
    this->req_query_contract_basic(req_no, contract_req, msg,
                                    contract_basic_info_rsp);
    if (msg.err_code != 0) {
        std::cout << "req_query_contract_basic failed. code=" << msg.err_code
                  << std::endl;
        return;
    }

    // 查询快照
    this->req_no++;
    mdp::mdp_quot_snap_query_req snap_req;
    memset(&snap_req, 0, sizeof(snap_req));
    // snap_req.contract_no = 0; // 所有，会包含max_seq_no
    strncpy(snap_req.trader_id, trader_id.c_str(), sizeof(snap_req.trader_id));
    mdp::mdp_quot_snap_query_req quot_snap_query_rsp;
    this->req_query_quot_snap(req_no, snap_req, msg, quot_snap_query_rsp);
    if (msg.err_code != 0) {
        std::cout << "req_query_quot_snap failed. code=" << msg.err_code
                  << std::endl;
        return;
    }

    // 已准备好
    this->ready();
}

void DFITCMarketDataListener::_pushMarketData()
{
    while (true)
    {
        mdp::mdp_ln_md depthMarketData = m_MdQueue.pop();
        Mydata mydata = { 0 };
        unsigned int n = sizeof(mdp::mdp_ln_md);
        memcpy(&mydata.datas, &depthMarketData, sizeof(mdp::mdp_ln_md));
        memcpy(&mydata.datas[n], (char*)&seq, sizeof(unsigned int)); 
        clock_gettime(CLOCK_REALTIME, &mydata.time);
        mydata.len = n;
        if ((getTextField(mydata, DATA_FIELD_INSTRUMENT_NAME) != INSTRUMENT1) && (getTextField(mydata, DATA_FIELD_INSTRUMENT_NAME) != INSTRUMENT2))
        {
            // std::cout << "symbol not match " << getTextField(mydata, DATA_FIELD_INSTRUMENT_NAME) << std::endl;
            return;
        }
        // std::cout << "upd time=" << snapshot->UpdTime << std::endl;
        dataTaskManager->push(mydata);
        ++seq;
        // ++pDataIndex;
    }
}

void DFITCMarketDataListener::dumpMarketData(mdp::mdp_ln_md* pDepthMarketData)
{
    m_MdQueue.push(*pDepthMarketData);
}

int32_t DFITCMarketDataListener::set_options(const mdp::options& options) {
    return impl_->set_options(options);
}

int32_t DFITCMarketDataListener::disconnect(void) { return impl_->disconnect(); }


// 登出请求
int32_t DFITCMarketDataListener::logout(void) { 
    this->req_no++;
    return impl_->logout(req_no); 
}

// 交易计划查询请求
int32_t DFITCMarketDataListener::req_query_trade_plan(
    uint64_t req_no,
    const mdp::mdp_trade_plan_query_req& trade_plan_query_req) {
    return impl_->req_query_trade_plan(req_no, trade_plan_query_req);
}

// 合约基本信息查询请求 (同步)
void DFITCMarketDataListener::req_query_contract_basic(
    uint64_t req_no,
    const mdp::mdp_contract_basic_info_req& contract_basic_info_req,
    mdp::mdp_rsp_msg& msg,
    std::vector<mdp::mdp_contract_basic_info_rsp>& contract_basic_info_rsp,
    int timeout_s) {
    impl_->req_query_contract_basic(req_no, contract_basic_info_req, msg,
                                    contract_basic_info_rsp, timeout_s);
}

// 历史行情查询请求
int32_t DFITCMarketDataListener::req_query_history_quot(
    uint64_t req_no,
    const mdp::mdp_history_quot_query& history_quot_query_req) {
    return impl_->req_query_history_quot(req_no, history_quot_query_req);
}

// 行情快照查询请求 （同步）
void DFITCMarketDataListener::req_query_quot_snap(
    uint64_t req_no, const mdp::mdp_quot_snap_query_req& quot_snap_query_req,
    mdp::mdp_rsp_msg& msg, mdp::mdp_quot_snap_query_req& quot_snap_query_rsp,
    int timeout_s) {
    impl_->req_query_quot_snap(req_no, quot_snap_query_req, msg,
                               quot_snap_query_rsp, timeout_s);
}

// ready 已准备好
int32_t DFITCMarketDataListener::ready() { return impl_->ready(); }

// on_connected 查询通道连接成功
void DFITCMarketDataListener::on_connected(int code, const std::string& msg) {
    std::cout << "connect ret = " << msg << std::endl;
}

// on_disconnected 查询通道断开连接
void DFITCMarketDataListener::on_disconnected(int code, const std::string& msg) {
    std::cout << "disconnect ret = " << msg << std::endl;
}

// 登录应答
void DFITCMarketDataListener::on_login(uint64_t req_no, const mdp::mdp_rsp_msg& msg,
                           mdp::mdp_trader_login_rsp& trader_login_rsp,
                           unsigned char chain_flag) {
    std::cout << "login ret = " << mdp::errors::msg(msg.err_code) << std::endl;
}

// 登出应答
void DFITCMarketDataListener::on_logout(uint64_t req_no, const mdp::mdp_rsp_msg& msg,
                            const mdp::mdp_trader_no& rsp_trader_no,
                            unsigned char chain_flag) {
    std::cout << "logout ret = " << mdp::errors::msg(msg.err_code) << std::endl;
}

// 交易计划查询应答
void DFITCMarketDataListener::on_rsp_query_trade_plan(
    uint64_t req_no, const mdp::mdp_rsp_msg& msg,
    std::vector<mdp::mdp_trade_plan>& trade_plan_vec,
    unsigned char chain_flag) {
    std::cout << "\non_rsp_query_trade_plan ret = "
              << mdp::errors::msg(msg.err_code) << std::endl;

    for (auto& info : trade_plan_vec) {
        std::cout << "trade_date=" << info.trade_date << std::endl;
        std::cout << "variety_id=" << info.variety_id << std::endl;
        std::cout << "trade_type=" << (int)info.trade_type << std::endl;
        std::cout << "state=" << (int)info.state << std::endl;
        std::cout << "chg_time=" << info.chg_time << std::endl;
        std::cout << "gis_no=" << info.gis_no << std::endl;
        std::cout << "next_gis_no=" << info.next_gis_no << std::endl;
    }
}

// 合约基本信息查询应答
void DFITCMarketDataListener::on_rsp_query_contract_basic(
    uint64_t req_no, const mdp::mdp_rsp_msg& msg,
    std::vector<mdp::mdp_contract_basic_info_rsp>& contract_basic_info_rsp,
    unsigned char chain_flag) {
    std::cout << "\non_rsp_query_contract_basic ret = "
              << mdp::errors::msg(msg.err_code) << std::endl;

    for (auto& info : contract_basic_info_rsp) {
        std::cout << "contract_no=" << info.contract_no << std::endl;
        std::cout << "contract_id=" << info.contract_id << std::endl;
        std::cout << "trade_date=" << info.trade_date << std::endl;
        std::cout << "contract_type=" << (int)info.contract_type << std::endl;
        std::cout << "init_open_interest=" << info.init_open_interest
                  << std::endl;
        std::cout << "rise_limit=" << info.rise_limit << std::endl;
        std::cout << "fall_limit=" << info.fall_limit << std::endl;
        std::cout << "last_clear=" << info.last_clear << std::endl;
        std::cout << "last_close=" << info.last_close << std::endl;
        std::cout << "codec_price=" << info.codec_price << std::endl;
        std::cout << "tick=" << info.tick << std::endl;
        std::cout << "contract_name=" << info.contract_name << std::endl;
        std::cout << std::endl;
    }
}

// 合约参数查询应答
void DFITCMarketDataListener::on_rsp_query_contract_param(
    uint64_t req_no, const mdp::mdp_rsp_msg& msg,
    std::vector<mdp::mdp_contract_param>& contract_param_vec,
    unsigned char chain_flag) {
    std::cout << "\non_rsp_query_contract_param ret = "
              << mdp::errors::msg(msg.err_code) << std::endl;

    for (auto& info : contract_param_vec) {
        std::cout << "contract_id=" << info.contract_id << std::endl;
        std::cout << "unit=" << info.unit << std::endl;
        std::cout << "tick=" << info.tick << std::endl;
        std::cout << "buy_spec_margin=" << info.buy_spec_margin << std::endl;
        std::cout << "sell_spec_margin=" << info.sell_spec_margin << std::endl;
        std::cout << "buy_hedge_margin=" << info.buy_hedge_margin << std::endl;
        std::cout << "sell_hedge_margin=" << info.sell_hedge_margin
                  << std::endl;
        std::cout << "contract_expiration_date="
                  << info.contract_expiration_date << std::endl;
        std::cout << "last_trade_date=" << info.last_trade_date << std::endl;
        std::cout << "last_deliver_date=" << info.last_deliver_date
                  << std::endl;
    }
}

// 历史行情查询应答
void DFITCMarketDataListener::on_rsp_query_history_quot(
    uint64_t req_no, const mdp::mdp_rsp_msg& msg,
    const mdp::mdp_history_quot_query& history_quot_rsp,
    unsigned char chain_flag) {
    std::cout << "\non_rsp_query_history_quot ret = "
              << mdp::errors::msg(msg.err_code) << std::endl;
}

// 行情快照查询应答
void DFITCMarketDataListener::on_rsp_query_quot_snap(
    uint64_t req_no, const mdp::mdp_rsp_msg& msg,
    const mdp::mdp_quot_snap_query_req& query_req,
    std::vector<mdp::mdp_max_no>& quot_snap_query_rsp,
    unsigned char chain_flag) {
    std::cout << "\non_rsp_query_quot_snap ret = "
              << mdp::errors::msg(msg.err_code) << std::endl;

    for (auto& info : quot_snap_query_rsp) {
        std::cout << "max_seq_no[" << info.mdg_no << "]=" << info.max_seq_no
                  << std::endl;
    }
}

// 深度行情合约状态查询应答
void DFITCMarketDataListener::on_ln_rsp_quot_contract_status(
    uint64_t req_no, const mdp::mdp_rsp_msg& msg,
    std::vector<mdp::mdp_contract_status>& contract_status_vec,
    unsigned char chain_flag) {
    std::cout << "on_ln_rsp_quot_contract_status ret = "
              << mdp::errors::msg(msg.err_code) << std::endl;

    for (auto& info : contract_status_vec) {
        std::cout << "contract_id=" << info.contract_id << std::endl;
        std::cout << "status=" << (int)info.status << std::endl;
        std::cout << "chg_time=" << info.chg_time << std::endl;
        std::cout << "op_style=" << (int)info.op_style << std::endl
                  << std::endl;
    }
}

// 合约交易状态通知
void DFITCMarketDataListener::on_nty_quot_contract_status(
    const mdp::mdp_contract_status_header& status_header,
    std::vector<mdp::mdp_variety_status>& variety_status_vec,
    std::vector<mdp::mdp_contract_status>& contract_status_vec,
    unsigned char chain_flag) {
    std::cout << "\non_nty_quot_contract_status seq_no[" << status_header.mdg_no
              << "]=" << status_header.seq_no << std::endl;

    std::cout << "mdg_no=" << status_header.mdg_no << std::endl;
    std::cout << "seq_no=" << status_header.seq_no << std::endl;

    for (auto& info : variety_status_vec) {
        std::cout << "variety_id=" << info.variety_id << std::endl;
        std::cout << "trade_type=" << (int)info.trade_type << std::endl;
        std::cout << "status=" << (int)info.status << std::endl;
        std::cout << "chg_time=" << info.chg_time << std::endl;
        std::cout << "op_style=" << (int)info.op_style << std::endl
                  << std::endl;
    }

    for (auto& info : contract_status_vec) {
        std::cout << "contract_id=" << info.contract_id << std::endl;
        std::cout << "status=" << (int)info.status << std::endl;
        std::cout << "chg_time=" << info.chg_time << std::endl;
        std::cout << "op_style=" << (int)info.op_style << std::endl
                  << std::endl;
    }
}

// 市场状态通知
void DFITCMarketDataListener::on_nty_quot_mkt_status(
    const mdp::mdp_quot_mkt_status& mkt_status, unsigned char chain_flag) {
    std::cout << "\non_nty_quot_mkt_status seq_no[" << mkt_status.mdg_no
              << "]=" << mkt_status.seq_no << std::endl;

    std::cout << "mdg_no=" << mkt_status.mdg_no << std::endl;
    std::cout << "seq_no=" << mkt_status.seq_no << std::endl;
    std::cout << "mkt_status=" << (int)mkt_status.mkt_status << std::endl;
    std::cout << "exch_code=" << mkt_status.exch_code << std::endl;
    std::cout << "mkt_chg_time=" << mkt_status.mkt_chg_time << std::endl;
}

// 数据服务盘后通知
void DFITCMarketDataListener::on_nty_close_mkt_notice(
    const mdp::mdp_mkt_data_notice& mkt_data_notice, unsigned char chain_flag) {
    std::cout << "\non_nty_close_mkt_notice seq_no[" << mkt_data_notice.mdg_no
              << "]=" << mkt_data_notice.seq_no << std::endl;

    std::cout << "mdg_no=" << mkt_data_notice.mdg_no << std::endl;
    std::cout << "seq_no=" << mkt_data_notice.seq_no << std::endl;
    std::cout << "time=" << mkt_data_notice.time << std::endl;
    std::cout << "type=" << (int)mkt_data_notice.type << std::endl;
    std::cout << "content=" << mkt_data_notice.content << std::endl;
}

// 期权参数通知
void DFITCMarketDataListener::on_option_parameter(const mdp::mdp_option_parameter& opt_para,
                                      unsigned char chain_flag) {
    std::cout << "\non_option_parameter seq_no[" << opt_para.mdg_no
              << "]=" << opt_para.seq_no << std::endl;

    std::cout << "change_no=" << opt_para.change_no << std::endl;
    std::cout << "mdg_no=" << opt_para.mdg_no << std::endl;
    std::cout << "seq_no=" << opt_para.seq_no << std::endl;
    std::cout << "batch_no=" << opt_para.batch_no << std::endl;
    std::cout << "trade_date=" << opt_para.trade_date << std::endl;
    std::cout << "contract_id=" << opt_para.contract_id << std::endl;
    std::cout << "trans_id=" << opt_para.trans_id << std::endl;
    std::cout << "delta=" << opt_para.delta << std::endl;
    std::cout << "gamma=" << opt_para.gamma << std::endl;
    std::cout << "rho=" << opt_para.rho << std::endl;
    std::cout << "theta=" << opt_para.theta << std::endl;
    std::cout << "vega=" << opt_para.vega << std::endl;
    std::cout << "send_time=" << opt_para.send_time << std::endl;
}

// 行情通知
void DFITCMarketDataListener::on_best_quot(const mdp::mdp_best_quot& best_quot,
                               unsigned char chain_flag) {
    // std::cout << "\non_best_quot seq_no[" << best_quot.mdg_no
    //           << "]=" << best_quot.seq_no << std::endl;

    // std::cout << "change_no=" << best_quot.change_no << std::endl;
    ln_md.change_no = best_quot.change_no;
    ln_md.mdg_no = best_quot.mdg_no;
    ln_md.seq_no = best_quot.seq_no;
    ln_md.batch_no = best_quot.batch_no;
    ln_md.trade_date = best_quot.trade_date;
    strcpy(ln_md.contract_id, best_quot.contract_id);
    // std::cout << "mdg_no=" << best_quot.mdg_no << std::endl;
    // std::cout << "seq_no=" << best_quot.seq_no << std::endl;
    // std::cout << "batch_no=" << best_quot.batch_no << std::endl;
    // std::cout << "trade_date=" << best_quot.trade_date << std::endl;
    // std::cout << "contract_id=" << best_quot.contract_id << std::endl;
    ln_md.trans_id = best_quot.trans_id;
    strcpy(ln_md.contract_name, best_quot.contract_name);
    ln_md.last_price = best_quot.last_price;
    ln_md.high_price = best_quot.high_price;
    ln_md.low_price = best_quot.low_price;
    ln_md.last_match_qty = best_quot.last_match_qty;
    // std::cout << "trans_id=" << best_quot.trans_id << std::endl;
    // std::cout << "contract_name=" << best_quot.contract_name << std::endl;
    // std::cout << "last_price=" << best_quot.last_price << std::endl;
    // std::cout << "high_price=" << best_quot.high_price << std::endl;
    // std::cout << "low_price=" << best_quot.low_price << std::endl;
    // std::cout << "last_match_qty=" << best_quot.last_match_qty << std::endl;
    ln_md.match_tot_qty = best_quot.match_tot_qty;
    ln_md.turnover = best_quot.turnover;
    ln_md.init_open_interest = best_quot.init_open_interest;
    ln_md.open_interest = best_quot.open_interest;
    ln_md.interest_chg = best_quot.interest_chg;
    ln_md.clear_price = best_quot.clear_price;
    ln_md.life_low = best_quot.life_low;
    // std::cout << "match_tot_qty=" << best_quot.match_tot_qty << std::endl;
    // std::cout << "turnover=" << best_quot.turnover << std::endl;
    // std::cout << "init_open_interest=" << best_quot.init_open_interest
    //           << std::endl;
    // std::cout << "open_interest=" << best_quot.open_interest << std::endl;
    // std::cout << "interest_chg=" << best_quot.interest_chg << std::endl;
    // std::cout << "clear_price=" << best_quot.clear_price << std::endl;
    // std::cout << "life_low=" << best_quot.life_low << std::endl;
    ln_md.life_high = best_quot.life_high;
    ln_md.rise_limit = best_quot.rise_limit;
    ln_md.fall_limit = best_quot.fall_limit;
    ln_md.last_clear = best_quot.last_clear;
    ln_md.last_close = best_quot.last_close;
    ln_md.l1_bid_price = best_quot.bid_price;
    // std::cout << "life_high=" << best_quot.life_high << std::endl;
    // std::cout << "rise_limit=" << best_quot.rise_limit << std::endl;
    // std::cout << "fall_limit=" << best_quot.fall_limit << std::endl;
    // std::cout << "last_clear=" << best_quot.last_clear << std::endl;
    // std::cout << "last_close=" << best_quot.last_close << std::endl;
    // std::cout << "bid_price=" << best_quot.bid_price << std::endl;
    ln_md.l1_bid_qty = best_quot.bid_qty;
    ln_md.l1_bid_imply_qty = best_quot.bid_imply_qty;
    ln_md.l1_ask_price = best_quot.ask_price;
    ln_md.l1_ask_qty = best_quot.ask_qty;
    ln_md.l1_ask_imply_qty = best_quot.ask_imply_qty;
    ln_md.avg_price = best_quot.avg_price;
    strcpy(ln_md.send_time, best_quot.send_time);
    ln_md.open_price = best_quot.open_price;
    ln_md.close_price = best_quot.close_price;
    // std::cout << "bid_qty=" << best_quot.bid_qty << std::endl;
    // std::cout << "bid_imply_qty=" << best_quot.bid_imply_qty << std::endl;
    // std::cout << "ask_price=" << best_quot.ask_price << std::endl;
    // std::cout << "ask_qty=" << best_quot.ask_qty << std::endl;
    // std::cout << "ask_imply_qty=" << best_quot.ask_imply_qty << std::endl;
    // std::cout << "avg_price=" << best_quot.avg_price << std::endl;
    // std::cout << "send_time=" << best_quot.send_time << std::endl;
    // std::cout << "open_price=" << best_quot.open_price << std::endl;
    // std::cout << "close_price=" << best_quot.close_price << std::endl;
    
    dumpMarketData(&ln_md);
}

// 套利行情通知
void DFITCMarketDataListener::on_arbi_best_quot(
    const mdp::mdp_arbi_best_quot& arbi_best_quot, unsigned char chain_flag) {
    std::cout << "\non_arbi_best_quot seq_no[" << arbi_best_quot.mdg_no
              << "]=" << arbi_best_quot.seq_no << std::endl;

    std::cout << "change_no=" << arbi_best_quot.change_no << std::endl;
    std::cout << "mdg_no=" << arbi_best_quot.mdg_no << std::endl;
    std::cout << "seq_no=" << arbi_best_quot.seq_no << std::endl;
    std::cout << "batch_no=" << arbi_best_quot.batch_no << std::endl;
    std::cout << "trade_date=" << arbi_best_quot.trade_date << std::endl;
    std::cout << "arbi_contract_id=" << arbi_best_quot.arbi_contract_id
              << std::endl;
    std::cout << "trans_id=" << arbi_best_quot.trans_id << std::endl;
    std::cout << "last_price=" << arbi_best_quot.last_price << std::endl;
    std::cout << "last_match_qty=" << arbi_best_quot.last_match_qty
              << std::endl;
    std::cout << "low_price=" << arbi_best_quot.low_price << std::endl;
    std::cout << "high_price=" << arbi_best_quot.high_price << std::endl;
    std::cout << "life_low=" << arbi_best_quot.life_low << std::endl;
    std::cout << "life_high=" << arbi_best_quot.life_high << std::endl;
    std::cout << "rise_limit=" << arbi_best_quot.rise_limit << std::endl;
    std::cout << "fall_limit=" << arbi_best_quot.fall_limit << std::endl;
    std::cout << "bid_price=" << arbi_best_quot.bid_price << std::endl;
    std::cout << "bid_qty=" << arbi_best_quot.bid_qty << std::endl;
    std::cout << "ask_price=" << arbi_best_quot.ask_price << std::endl;
    std::cout << "ask_qty=" << arbi_best_quot.ask_qty << std::endl;
    std::cout << "send_time=" << arbi_best_quot.send_time << std::endl;
}

// 分价成交量通知
void DFITCMarketDataListener::on_ln_segment_price_qty(
    const mdp::mdp_ln_quot_header& ln_quot_header,
    std::vector<mdp::mdp_ln_segment_price_qty>& ln_segment_price_qty_vec,
    unsigned char chain_flag) {
    std::cout << "\non_ln_segment_price_qty seq_no[" << ln_quot_header.mdg_no
              << "]=" << ln_quot_header.seq_no << std::endl;
    std::cout << "mdg_no=" << ln_quot_header.mdg_no << std::endl;
    std::cout << "seq_no=" << ln_quot_header.seq_no << std::endl;
    std::cout << "change_no=" << ln_quot_header.change_no << std::endl;
    std::cout << "contract_id=" << ln_quot_header.contract_id << std::endl;
    std::cout << "send_time=" << ln_quot_header.send_time << std::endl;

    for (auto& info : ln_segment_price_qty_vec) {
        std::cout << "price=" << info.price << std::endl;
        std::cout << "price_bo_qty=" << info.price_bo_qty << std::endl;
        std::cout << "price_be_qty=" << info.price_be_qty << std::endl;
        std::cout << "price_so_qty=" << info.price_so_qty << std::endl;
        std::cout << "price_se_qty=" << info.price_se_qty << std::endl;
    }
}

// N档深度行情通知
void DFITCMarketDataListener::on_ln_mbl_quot(
    const mdp::mdp_ln_quot_header& ln_quot_header,
    std::map<int64_t, mdp::mdp_mbl_level>& mbl_level_map_buy,
    std::map<int64_t, mdp::mdp_mbl_level>& mbl_level_map_sell,
    unsigned char chain_flag) {
    ln_md.ln_mdg_no = ln_quot_header.mdg_no;
    ln_md.ln_seq_no = ln_quot_header.seq_no;
    ln_md.ln_change_no = ln_quot_header.change_no;
    strcpy(ln_md.ln_contract_id, ln_quot_header.contract_id);
    strcpy(ln_md.ln_send_time, ln_quot_header.send_time);
    // std::cout << "\non_ln_mbl_quot seq_no[" << ln_quot_header.mdg_no
    //           << "]=" << ln_quot_header.seq_no << std::endl;
    // std::cout << "mdg_no=" << ln_quot_header.mdg_no << std::endl;
    // std::cout << "seq_no=" << ln_quot_header.seq_no << std::endl;
    // std::cout << "change_no=" << ln_quot_header.change_no << std::endl;
    // std::cout << "contract_id=" << ln_quot_header.contract_id << std::endl;
    // std::cout << "send_time=" << ln_quot_header.send_time << std::endl;

    // std::cout << "buy side" << std::endl;
    for (auto& info : mbl_level_map_buy) {
        // std::cout << "\nprice=" << info.first << std::endl;
        // std::cout << "trans_id=" << info.second.trans_id << std::endl;
        // std::cout << "event_type=" << (int)info.second.event_type << std::endl;
        // std::cout << "order_price=" << info.second.order_price << std::endl;
        // std::cout << "order_qty=" << info.second.order_qty << std::endl;
        // std::cout << "imply_qty=" << info.second.imply_qty << std::endl;
        // std::cout << "bs_flag=" << (int)info.second.bs_flag << std::endl;
        // std::cout << "gen_time=" << info.second.gen_time << std::endl;
        if ((int)info.second.event_type == 1) { // 增加

            if (ln_md.bid_md_count == 0) {
                ln_md.bid_order_price[0] = info.second.order_price;
                ln_md.bid_order_qty[0] = info.second.order_qty;
                ln_md.bid_imply_qty[0] = info.second.imply_qty;
                ln_md.bid_md_count++;
                continue;
            }

            int temp_order_price, temp_order_qty, temp_imply_qty;

            bool find_flag = false;
            for (int i = 0; i < ln_md.bid_md_count; i++) {
                if (ln_md.bid_order_price[i] < info.second.order_price) {
                    find_flag = true;
                    if (ln_md.bid_md_count < 5) {
                        ln_md.bid_order_price[ln_md.bid_md_count] = ln_md.bid_order_price[ln_md.bid_md_count-1];
                        
                    }
                    for (int j = ln_md.bid_md_count - 1; j >= i; j--) {
                        if (j != i) {
                            ln_md.bid_order_price[j] = ln_md.bid_order_price[j - 1];
                            ln_md.bid_order_qty[j] = ln_md.bid_order_qty[j - 1];
                            ln_md.bid_imply_qty[j] = ln_md.bid_imply_qty[j - 1];
                        }
                        else {
                            ln_md.bid_order_price[j] = info.second.order_price;
                            ln_md.bid_order_qty[j] = info.second.order_qty;
                            ln_md.bid_imply_qty[j] = info.second.imply_qty;
                        }
                    }
                    if (ln_md.bid_md_count < 5) ln_md.bid_md_count++;
                    break;
                }
            }
            if (find_flag == false && ln_md.bid_md_count < 5) {
                ln_md.bid_order_price[ln_md.bid_md_count] = info.second.order_price;
                ln_md.bid_order_qty[ln_md.bid_md_count] = info.second.order_qty;
                ln_md.bid_imply_qty[ln_md.bid_md_count] = info.second.imply_qty;
                ln_md.bid_md_count++;
            }
        }
        else if ((int)info.second.event_type == 2) { // 修改
            for (int i = 0; i < ln_md.bid_md_count; i++) {
                if (ln_md.bid_order_price[i] == info.second.order_price) {
                    ln_md.bid_order_qty[i] = info.second.order_qty;
                    ln_md.bid_imply_qty[i] = info.second.imply_qty;
                }
            }
        }
        else if ((int)info.second.event_type == 3) { // 删除
            if (ln_md.bid_md_count == 0) continue;
            for (int i = 0; i < ln_md.bid_md_count; i++) {
                if (ln_md.bid_order_price[i] == info.second.order_price) {
                    if (i != 4) {
                        for (int j = i; j < 4; j++) {
                            ln_md.bid_order_price[j] = ln_md.bid_order_price[j+1];
                            ln_md.bid_order_qty[j] = ln_md.bid_order_qty[j+1];
                            ln_md.bid_imply_qty[j] = ln_md.bid_imply_qty[j+1];
                        }
                    }
                    ln_md.bid_order_price[4] = -1;
                    ln_md.bid_order_qty[4] = 0;
                    ln_md.bid_imply_qty[4] = 0;
                    ln_md.bid_md_count--;
                    break;
                }
            }
        }
    }

    // std::cout << "sell side" << std::endl;
    for (auto& info : mbl_level_map_sell) {
        // std::cout << "\nprice=" << info.first << std::endl;
        // std::cout << "trans_id=" << info.second.trans_id << std::endl;
        // std::cout << "event_type=" << (int)info.second.event_type << std::endl;
        // std::cout << "order_price=" << info.second.order_price << std::endl;
        // std::cout << "order_qty=" << info.second.order_qty << std::endl;
        // std::cout << "imply_qty=" << info.second.imply_qty << std::endl;
        // std::cout << "bs_flag=" << (int)info.second.bs_flag << std::endl;
        // std::cout << "gen_time=" << info.second.gen_time << std::endl;
        if ((int)info.second.event_type == 1) { // 增加

            if (ln_md.ask_md_count == 0) {
                ln_md.ask_order_price[0] = info.second.order_price;
                ln_md.ask_order_qty[0] = info.second.order_qty;
                ln_md.ask_imply_qty[0] = info.second.imply_qty;
                ln_md.ask_md_count++;
                continue;
            }

            int temp_order_price, temp_order_qty, temp_imply_qty;

            bool find_flag = false;
            for (int i = 0; i < ln_md.ask_md_count; i++) {
                if (ln_md.ask_order_price[i] > info.second.order_price) {
                    find_flag = true;
                    if (ln_md.ask_md_count < 5) {
                        ln_md.ask_order_price[ln_md.ask_md_count] = ln_md.ask_order_price[ln_md.ask_md_count-1];
                        ln_md.ask_order_qty[ln_md.ask_md_count] = ln_md.ask_order_qty[ln_md.ask_md_count-1];
                        ln_md.ask_imply_qty[ln_md.ask_md_count] = ln_md.ask_imply_qty[ln_md.ask_md_count-1];
                    }
                    for (int j = ln_md.ask_md_count - 1; j >= i; j--) {
                        if (j != i) {
                            ln_md.ask_order_price[j] = ln_md.ask_order_price[j - 1];
                            ln_md.ask_order_qty[j] = ln_md.ask_order_qty[j - 1];
                            ln_md.ask_imply_qty[j] = ln_md.ask_imply_qty[j - 1];
                        }
                        else {
                            ln_md.ask_order_price[j] = info.second.order_price;
                            ln_md.ask_order_qty[j] = info.second.order_qty;
                            ln_md.ask_imply_qty[j] = info.second.imply_qty;
                        }
                    }
                    if (ln_md.ask_md_count < 5) ln_md.ask_md_count++;
                    break;
                }
            }
            if (find_flag == false && ln_md.ask_md_count < 5) {
                ln_md.ask_order_price[ln_md.ask_md_count] = info.second.order_price;
                ln_md.ask_order_qty[ln_md.ask_md_count] = info.second.order_qty;
                ln_md.ask_imply_qty[ln_md.ask_md_count] = info.second.imply_qty;
                ln_md.ask_md_count++;
            }
        }
        else if ((int)info.second.event_type == 2) { // 修改
            for (int i = 0; i < ln_md.ask_md_count; i++) {
                if (ln_md.ask_order_price[i] == info.second.order_price) {
                    ln_md.ask_order_qty[i] = info.second.order_qty;
                    ln_md.ask_imply_qty[i] = info.second.imply_qty;
                }
            }
        }
        else if ((int)info.second.event_type == 3) { // 删除
            if (ln_md.ask_md_count == 0) continue;
            for (int i = 0; i < ln_md.ask_md_count; i++) {
                if (ln_md.ask_order_price[i] == info.second.order_price) {
                    if (i != 4) {
                        for (int j = i; j < 4; j++) {
                            ln_md.ask_order_price[j] = ln_md.ask_order_price[j+1];
                            ln_md.ask_order_qty[j] = ln_md.ask_order_qty[j+1];
                            ln_md.ask_imply_qty[j] = ln_md.ask_imply_qty[j+1];
                        }
                    }
                    ln_md.ask_order_price[4] = -1;
                    ln_md.ask_order_qty[4] = 0;
                    ln_md.ask_imply_qty[4] = 0;
                }
            }
            ln_md.ask_md_count--;
        }
    }

    // std::cout << "\non_ln_mbl_quot end" << std::endl;
    dumpMarketData(&ln_md);
}

// 最优价位前十笔委托通知
void DFITCMarketDataListener::on_ln_best_level_orders(
    const mdp::mdp_ln_quot_header& ln_quot_header,
    const mdp::mdp_price_pair& price_pair,
    std::vector<mdp::mdp_qty_pair>& qty_pair_vec, unsigned char chain_flag) {
    std::cout << "\non_ln_best_level_orders seq_no[" << ln_quot_header.mdg_no
              << "]=" << ln_quot_header.seq_no << std::endl;
    std::cout << "mdg_no=" << ln_quot_header.mdg_no << std::endl;
    std::cout << "seq_no=" << ln_quot_header.seq_no << std::endl;
    std::cout << "change_no=" << ln_quot_header.change_no << std::endl;
    std::cout << "contract_id=" << ln_quot_header.contract_id << std::endl;
    std::cout << "send_time=" << ln_quot_header.send_time << std::endl;

    std::cout << "bid=" << price_pair.bid << std::endl;
    std::cout << "ask=" << price_pair.ask << std::endl;

    std::cout << "qty pair" << std::endl;
    for (auto& info : qty_pair_vec) {
        std::cout << "bid=" << info.bid << std::endl;
        std::cout << "ask=" << info.ask << std::endl;
    }
}

// 委托统计通知
void DFITCMarketDataListener::on_ln_order_statistic(
    const mdp::mdp_ln_order_statistic& ln_order_statistic,
    unsigned char chain_flag) {
    std::cout << "\non_ln_order_statistic seq_no[" << ln_order_statistic.mdg_no
              << "]=" << ln_order_statistic.seq_no << std::endl;

    std::cout << "mdg_no=" << ln_order_statistic.mdg_no << std::endl;
    std::cout << "seq_no=" << ln_order_statistic.seq_no << std::endl;
    std::cout << "change_no=" << ln_order_statistic.change_no << std::endl;
    std::cout << "contract_id=" << ln_order_statistic.contract_id << std::endl;
    std::cout << "send_time=" << ln_order_statistic.send_time << std::endl;
    std::cout << "total_buy_order_num="
              << ln_order_statistic.total_buy_order_num << std::endl;
    std::cout << "total_sell_order_num="
              << ln_order_statistic.total_sell_order_num << std::endl;
    std::cout << "weighted_average_buy_order_price="
              << ln_order_statistic.weighted_average_buy_order_price
              << std::endl;
    std::cout << "weighted_average_sell_order_price="
              << ln_order_statistic.weighted_average_sell_order_price
              << std::endl;
}

//流控二级警告通知
void DFITCMarketDataListener::on_nty_flow_ctrl_warning(
    mdp::mdp_flow_ctrl_warning& flow_ctrl_warning, unsigned char chain_flag) {
    std::cout << "\non_nty_flow_ctrl_warning" << std::endl;

    std::cout << "trader_id=" << flow_ctrl_warning.trader_id << std::endl;
    std::cout << "trigger_time=" << flow_ctrl_warning.trigger_time << std::endl;
    std::cout << "tgid=" << flow_ctrl_warning.tgid << std::endl;
    std::cout << "cur_pkg_cnt=" << flow_ctrl_warning.cur_pkg_cnt << std::endl;
}
