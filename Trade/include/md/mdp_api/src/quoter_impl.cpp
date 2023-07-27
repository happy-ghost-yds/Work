/**
 * Copyright (C) 2021，大连商品交易所 大连飞创信息技术有限公司
 *
 * @brief 具体实现。
 *
 * 组合查询通道和行情通道的数据，并进行内外部转换然后回调给用户。
 *
 */

#include "mdp/quoter_impl.hpp"

#include "mdp/enc_dec.hpp"
#include "mdp/log.hpp"

#include "mdp/common.hpp"
#include "mdp/query_channel.hpp"
#include "mdp/quote_channel.hpp"

#include "mdp/errors.hpp"
#include "mdp/quoter.hpp"
#include <iomanip>
#include <sstream>

namespace mdp {
// ======

quoter_impl::quoter_impl(void)
    : query_channel_(new query_channel()), quote_channel_(new quote_channel()) {
    memset(&options_, 0, sizeof(options_));
    options_.timeout_ms = 30000;            // ms
    options_.heart_interval_ms = 5000;      // ms
    options_.heart_max_failed = 6;          // 次
    options_.reconnect_max_times = 5;       // 次
    options_.lost_check_interval_ms = 1000; // ms
    options_.lost_check_max_failed = 9;     // 次
    options_.cache_size = 100000;           // 个
    options_.cache_win = 2000;              // 个
    options_.channel = 0;                   // 全通道
    options_.mbl_levels = 5;                // 档
    query_channel_->set_options(options_);
    quote_channel_->set_options(options_);

    memset(&login_req_, 0, sizeof(login_req_));
    memset(&login_rsp_, 0, sizeof(login_rsp_));
    memset(&msg_, 0, sizeof(msg_));

    // 组播通道
    quote_channel_->on_handler_ =
        std::bind(&quoter_impl::on_quote_data, this, std::placeholders::_1,
                  std::placeholders::_2, std::placeholders::_3);
    quote_channel_->req_recover_ =
        std::bind(&quoter_impl::start_recover, this, std::placeholders::_1,
                  std::placeholders::_2, std::placeholders::_3);

    // 查询通道
    query_channel_->on_connected_ =
        std::bind(&quoter_impl::on_connected, this, std::placeholders::_1,
                  std::placeholders::_2, std::placeholders::_3);
    query_channel_->on_disconnected_ =
        std::bind(&quoter_impl::on_disconnected, this, std::placeholders::_1,
                  std::placeholders::_2, std::placeholders::_3);
    query_channel_->on_query_data_ =
        std::bind(&quoter_impl::on_query_data, this, std::placeholders::_1,
                  std::placeholders::_2, std::placeholders::_3);
    query_channel_->on_quote_data_ =
        std::bind(&quoter_impl::on_quote_data, this, std::placeholders::_1,
                  std::placeholders::_2, std::placeholders::_3);

    // 注册
    udp_handler_map_[pkg::tid::heart_beat] =
        std::bind(&quoter_impl::on_quote_heart, this, std::placeholders::_1,
                  std::placeholders::_2, std::placeholders::_3);
    tcp_handler_map_[pkg::tid::rsp_heart_beat] =
        std::bind(&quoter_impl::on_query_heart, this, std::placeholders::_1,
                  std::placeholders::_2, std::placeholders::_3);
    tcp_handler_map_[pkg::tid::rsp_trader_login] =
        std::bind(&quoter_impl::on_login, this, std::placeholders::_1,
                  std::placeholders::_2, std::placeholders::_3);
    tcp_handler_map_[pkg::tid::rsp_trader_logout] =
        std::bind(&quoter_impl::on_logout, this, std::placeholders::_1,
                  std::placeholders::_2, std::placeholders::_3);
    tcp_handler_map_[pkg::tid::rsp_qry_trade_plan] = std::bind(
        &quoter_impl::on_handler_rsp_query_trade_plan, this,
        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    tcp_handler_map_[pkg::tid::rsp_query_contract_basic_info] = std::bind(
        &quoter_impl::on_handler_rsp_query_contract_basic, this,
        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    tcp_handler_map_[pkg::tid::rsp_query_history_quot] = std::bind(
        &quoter_impl::on_handler_rsp_query_history_quot, this,
        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    tcp_handler_map_[pkg::tid::rsp_query_quot_snap] = std::bind(
        &quoter_impl::on_handler_rsp_query_quot_snap, this,
        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    udp_handler_map_[pkg::tid::nty_trader_status] = std::bind(
        &quoter_impl::on_handler_nty_quot_contract_status, this,
        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    udp_handler_map_[pkg::tid::nty_mkt_status] = std::bind(
        &quoter_impl::on_handler_nty_quot_mkt_status, this,
        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    udp_handler_map_[pkg::tid::nty_close_mkt_notice] = std::bind(
        &quoter_impl::on_handler_nty_close_mkt_notice, this,
        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    udp_handler_map_[pkg::tid::quot] = std::bind(
        &quoter_impl::on_handler_best_quot, this, std::placeholders::_1,
        std::placeholders::_2, std::placeholders::_3);
    udp_handler_map_[pkg::tid::opt_greek_quot] = std::bind(
        &quoter_impl::on_handler_option_parameter, this, std::placeholders::_1,
        std::placeholders::_2, std::placeholders::_3);
    tcp_handler_map_[pkg::tid::quot_snap] = std::bind(
        &quoter_impl::on_handler_snap_best_quot, this, std::placeholders::_1,
        std::placeholders::_2, std::placeholders::_3);
    tcp_handler_map_[pkg::tid::opt_greek_quot_snap] = std::bind(
        &quoter_impl::on_handler_snap_option_parameter, this,
        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
#ifdef MDP_LN_QUOTER
    tcp_handler_map_[pkg::tid::rsp_qry_contract_param] = std::bind(
        &quoter_impl::on_handler_rsp_query_contract_param, this,
        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    tcp_handler_map_[pkg::tid::rsp_qry_contract_status] = std::bind(
        &quoter_impl::on_ln_rsp_quot_contract_status, this,
        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    tcp_handler_map_[pkg::tid::rsp_query_ln_history_quot] = std::bind(
        &quoter_impl::on_handler_rsp_query_history_quot, this,
        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    tcp_handler_map_[pkg::tid::rsp_query_quot_snap] = std::bind(
        &quoter_impl::on_handler_rsp_query_quot_snap, this,
        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    tcp_handler_map_[pkg::tid::rsp_query_ln_quot_snap] = std::bind(
        &quoter_impl::on_handler_rsp_query_quot_snap, this,
        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    udp_handler_map_[pkg::tid::ln_segment_price_match_qty] = std::bind(
        &quoter_impl::on_handler_ln_segment_price_qty, this,
        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    udp_handler_map_[pkg::tid::ln_mbl_quot_biz6] = std::bind(
        &quoter_impl::on_handler_ln_mbl_quot, this, std::placeholders::_1,
        std::placeholders::_2, std::placeholders::_3);
    udp_handler_map_[pkg::tid::ln_best_level_orders_biz6] = std::bind(
        &quoter_impl::on_handler_ln_best_level_orders, this,
        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    udp_handler_map_[pkg::tid::ln_order_statistics] = std::bind(
        &quoter_impl::on_handler_ln_order_statistic, this,
        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    tcp_handler_map_[pkg::tid::ln_segment_price_match_qty_snap] = std::bind(
        &quoter_impl::on_handler_ln_snap_segment_price_qty, this,
        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    tcp_handler_map_[pkg::tid::ln_mbl_quot_biz6_snap] = std::bind(
        &quoter_impl::on_handler_ln_snap_mbl_quot, this, std::placeholders::_1,
        std::placeholders::_2, std::placeholders::_3);
    tcp_handler_map_[pkg::tid::ln_best_level_orders_biz6_snap] = std::bind(
        &quoter_impl::on_handler_ln_snap_best_level_orders, this,
        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    tcp_handler_map_[pkg::tid::ln_order_statistics_snap] = std::bind(
        &quoter_impl::on_handler_ln_snap_order_statistic, this,
        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
#endif
    tcp_handler_map_[pkg::tid::flow_ctrl_warning] = std::bind(
        &quoter_impl::on_handler_nty_flow_ctrl_warning, this,
        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
}

quoter_impl::~quoter_impl(void) {
    disconnect();

    // 清理缓存
    contract_map_.clear();
    best_quot_cache_map_.clear();
    arbi_best_quot_cache_map_.clear();
}

void quoter_impl::set_rsp(quoter_rsp* rsp) { rsp_ = rsp; }

int32_t quoter_impl::set_options(const options& options) {
    options_ = options;
    query_channel_->set_options(options);
    quote_channel_->set_options(options);
    return 0;
}

int32_t quoter_impl::connect(const std::string& udp_addr_1,
                             const std::string& udp_addr_2,
                             const std::string& tcp_addr) {
    int ret = query_channel_->connect(tcp_addr, options_.timeout_ms);
    if (ret != 0) {
        return ret;
    }
    ret = quote_channel_->listen(udp_addr_1, udp_addr_2);
    if (ret != 0) {
        return ret;
    }

    // 连接后重置查询类型
    qrying_type_ = no_qrying;
    recovering_map_.clear();

    return 0;
}

int32_t quoter_impl::disconnect(void) {
    dispatch_timer_.stop();
    stop_heart_timer();
    stop_reconnect_timer();
    stop_lost_check_timer();
    return query_channel_->disconnect();
}

void quoter_impl::login(uint64_t req_no, const mdp_trader_login_req& req,
                        mdp_rsp_msg& msg, mdp_trader_login_rsp& rsp) {
    memset(&msg, 0, sizeof(msg));
    memset(&rsp, 0, sizeof(rsp));

    login_req_ = req;

    auto fld = new mdp::pkg::fld_trader_login_req();
    strncpy(fld->trader_id_, req.trader_id, sizeof(fld->trader_id_));
    auto outlen = sizeof(fld->pwd_);
    if (pwd_encrypt(req.pwd, strlen(req.pwd), fld->pwd_, &outlen) != 0) {
        LOG_ERROR("login req encrypt failed.");
        return;
    }
    mdp::pkg::body body;
    body.init(mdp::pkg::tid::req_trader_login);
    body.set_request_no(req_no);
    body.data_.emplace(fld);

    LOG_DEBUG("login body={0}", body.to_string());

    std::unique_lock<std::mutex> lck(lock_);

    int ret = post(&body);
    if (ret != 0) {
        LOG_ERROR("login req failed. ret={0} msg={1}", ret, errors::msg(ret));
        msg.err_code = mdp::errors::failed;
        strncpy(msg.rsp_msg, mdp::errors::msg(msg.err_code),
                sizeof(msg.rsp_msg));
        return;
    }

    std::chrono::seconds span_s(options_.timeout_ms / 1000);
    auto status = cv_.wait_for(lck, span_s, [&] { return rsp_no_ == req_no_; });
    if (!status) {
        LOG_ERROR("login req timeout. timeout={0}ms", options_.timeout_ms);
        msg.err_code = errors::timeout;
        strncpy(msg.rsp_msg, errors::msg(msg.err_code), sizeof(msg.rsp_msg));
        return;
    }
    msg = msg_;
    rsp = login_rsp_;
}

// 登出请求
int32_t quoter_impl::logout(uint64_t req_no,
                            const mdp_trader_no& req_trader_no) {
    stop_heart_timer();
    stop_lost_check_timer();

    auto fld = new mdp::pkg::fld_trader_id();
    strncpy(fld->trader_id_, req_trader_no.trader_id, sizeof(fld->trader_id_));

    mdp::pkg::body body;
    body.init(mdp::pkg::tid::req_trader_logout);
    body.set_request_no(req_no);
    body.data_.emplace(fld);
    LOG_DEBUG("logout body={0}", body.to_string());
    return post(&body);
}

// 交易计划查询请求
int32_t quoter_impl::req_query_trade_plan(
    uint64_t req_no, const mdp_trade_plan_query_req& trade_plan_query_req) {
    auto fld = new mdp::pkg::fld_req_trade_plan();
    strncpy(fld->trader_id_, trade_plan_query_req.trader_id,
            sizeof(fld->trader_id_));
    strncpy(fld->variety_id_, trade_plan_query_req.variety_id,
            sizeof(fld->variety_id_));
    fld->trade_type_ = trade_plan_query_req.trade_type;

    mdp::pkg::body body;
    body.init(mdp::pkg::tid::req_qry_trade_plan);
    body.set_request_no(req_no);
    body.data_.emplace(fld);

    LOG_DEBUG("req_qry_trade_plan body={0}", body.to_string());
    return post(&body);
}

// 合约基本信息查询请求 (同步)
void quoter_impl::req_query_contract_basic(
    uint64_t req_no, const mdp::mdp_contract_basic_info_req& req,
    mdp::mdp_rsp_msg& msg,
    std::vector<mdp::mdp_contract_basic_info_rsp>& contract_basic_info_rsp,
    int timeout_s) {
    memset(&msg, 0, sizeof(msg));
    contract_basic_info_rsp.clear();

    auto fld = new mdp::pkg::fld_req_contract_basic_info();
    strncpy(fld->contract_id_, req.contract_id, sizeof(fld->contract_id_));
    strncpy(fld->trader_id_, req.trader_id, sizeof(fld->trader_id_));
    mdp::pkg::body body;
    body.init(mdp::pkg::tid::req_query_contract_basic_info);
    body.set_request_no(req_no);
    body.data_.emplace(fld);

    LOG_DEBUG("req_query_contract_basic body={0}", body.to_string());

    std::unique_lock<std::mutex> lck(lock_);

    int ret = post(&body);
    if (ret != 0) {
        LOG_ERROR("req_query_contract_basic req failed. ret={0} msg={1}", ret,
                  errors::msg(ret));
        msg.err_code = mdp::errors::failed;
        strncpy(msg.rsp_msg, mdp::errors::msg(msg.err_code),
                sizeof(msg.rsp_msg));
        return;
    }

    std::chrono::seconds span_s(timeout_s);
    auto status = cv_.wait_for(lck, span_s, [&] { return rsp_no_ == req_no_; });
    if (!status) {
        LOG_ERROR("req_query_contract_basic req timeout. timeout={0}ms",
                  timeout_s);
        msg.err_code = errors::timeout;
        strncpy(msg.rsp_msg, errors::msg(msg.err_code), sizeof(msg.rsp_msg));
        return;
    }
    msg = msg_;
    for (auto& item : contract_map_) {
        contract_basic_info_rsp.push_back(item.second);
    }
}

// 历史行情查询请求
int32_t quoter_impl::req_query_history_quot(
    uint64_t req_no, const mdp_history_quot_query& history_quot_query_req) {
    auto fld = new mdp::pkg::fld_qry_his_quot();
    strncpy(fld->trader_id_, history_quot_query_req.trader_id,
            sizeof(fld->trader_id_));
    fld->mdg_no_ = history_quot_query_req.mdg_no; /*行情品种分组号*/
    fld->start_seq_no_ = history_quot_query_req.start_seq_no; /*开始序列号*/
    fld->end_seq_no_ = history_quot_query_req.end_seq_no; /*结束序列号*/

    mdp::pkg::body body;
#ifdef MDP_LN_QUOTER
    body.init(mdp::pkg::tid::req_query_ln_history_quot);
#else
    body.init(mdp::pkg::tid::req_query_history_quot);
#endif
    body.set_request_no(req_no);
    body.data_.emplace(fld);

    LOG_DEBUG("req_query_history_quot body={0}", body.to_string());
    int32_t ret = post(&body);
    if (0 == ret) {
        history_quot_req_ = history_quot_query_req;
    }

    return ret;
}

// 行情快照查询请求 （同步）
void quoter_impl::req_query_quot_snap(
    uint64_t req_no, const mdp::mdp_quot_snap_query_req& req,
    mdp::mdp_rsp_msg& msg, mdp::mdp_quot_snap_query_req& quot_snap_query_rsp,
    int timeout_s) {
    memset(&msg, 0, sizeof(msg));
    memset(&quot_snap_query_rsp, 0, sizeof(quot_snap_query_rsp));

    auto fld = new mdp::pkg::fld_qry_quot_snap();
    fld->mdg_no_ = req.mdg_no;
    fld->contract_no_ = get_contract_no(req.contract_id);
    strncpy(fld->trader_id_, req.trader_id, sizeof(fld->trader_id_));

    memset(&quot_snap_query_rsp_, 0, sizeof(quot_snap_query_rsp_));
    quot_snap_query_rsp_.mdg_no = req.mdg_no;
    strncpy(quot_snap_query_rsp_.contract_id, req.contract_id,
            sizeof(quot_snap_query_rsp_.contract_id));
    strncpy(quot_snap_query_rsp_.trader_id, req.trader_id,
            sizeof(quot_snap_query_rsp_.trader_id));

    mdp::pkg::body body;
#ifdef MDP_LN_QUOTER
    body.init(mdp::pkg::tid::req_query_ln_quot_snap);
#else
    body.init(mdp::pkg::tid::req_query_quot_snap);
#endif
    body.set_request_no(req_no);
    body.data_.emplace(fld);

    LOG_DEBUG("req_query_quot_snap body={0}", body.to_string());

    // 暂停行情数据处理
    ready(false);

    std::unique_lock<std::mutex> lck(lock_);

    int32_t ret = post(&body);
    if (ret != 0) {
        LOG_ERROR("req_query_quot_snap req failed. ret={0} msg={1}", ret,
                  errors::msg(ret));
        msg.err_code = mdp::errors::failed;
        strncpy(msg.rsp_msg, mdp::errors::msg(msg.err_code),
                sizeof(msg.rsp_msg));
        return;
    }

    std::chrono::seconds span_s(timeout_s);
    auto status = cv_.wait_for(lck, span_s, [&] { return rsp_no_ == req_no_; });
    if (!status) {
        LOG_ERROR("req_query_quot_snap req timeout. timeout={0}s", timeout_s);
        msg.err_code = errors::timeout;
        strncpy(msg.rsp_msg, errors::msg(msg.err_code), sizeof(msg.rsp_msg));
        return;
    }
    msg = msg_;
    quot_snap_query_rsp = quot_snap_query_rsp_;
}

#ifdef MDP_LN_QUOTER

// 合约交易状态查询请求
int32_t
quoter_impl::req_query_ln_contract_status(uint64_t req_no,
                                          const mdp_trader_no& req_trader_no) {

    auto fld = new mdp::pkg::fld_trader_id();
    strncpy(fld->trader_id_, req_trader_no.trader_id, sizeof(fld->trader_id_));
    mdp::pkg::body body;
    body.init(mdp::pkg::tid::req_qry_contract_status);
    body.set_request_no(req_no);
    body.data_.emplace(fld);

    LOG_DEBUG("req_query_ln_contract_status body={0}", body.to_string());

    return post(&body);
}

// 合约参数查询请求（异步）
int32_t quoter_impl::req_query_contract_param(
    uint64_t req_no,
    const mdp_contract_param_query_req& contract_param_query_req) {
    auto fld = new mdp::pkg::fld_trader_id();
    strncpy(fld->trader_id_, contract_param_query_req.trader_id,
            sizeof(fld->trader_id_));
    mdp::pkg::body body;
    body.init(mdp::pkg::tid::req_qry_contract_param);
    body.set_request_no(req_no);
    body.data_.emplace(fld);

    LOG_DEBUG("req_qry_contract_param body={0}", body.to_string());

    return post(&body);
}

#endif

// ready 已准备好
int32_t quoter_impl::ready(bool ready) {
    ready_ = ready;
    quote_channel_->ready(ready_);
    return 0;
}

int32_t quoter_impl::post(mdp::pkg::body* body_ptr) {
    if (nullptr != body_ptr) {
        LOG_DEBUG("post req_no={0}", body_ptr->request_no());
    } else {
        LOG_DEBUG("post nullptr");
    }

    if (!query_channel_->is_connected()) {
        LOG_DEBUG("post not connected");
        return net_errors::failed;
    }

    int ret = 0;
    size_t offset = 0;
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    mdp::pkg::dmqp_header header;
    header.pkg_type_ = body_ptr->id();
    header.set_request_no(body_ptr->request_no());
    header.pkg_size_ = header.length() + body_ptr->length();

    ret = header.marshal(buffer + offset, header.pkg_size_ - offset);
    if (ret <= 0) {
        LOG_ERROR("header marshal error, ret={0}", ret);
        return ret;
    }
    offset += ret;

    ret = body_ptr->marshal(buffer + offset, header.pkg_size_ - offset);

    offset += ret;
    if (header.pkg_size_ != offset) {
        LOG_ERROR("body marshal error, len={0},offset={1}", header.pkg_size_,
                  offset);
        return net_errors::failed;
    }

    LOG_DEBUG("post header={0} body={1}", header.to_string(),
              body_ptr->to_string());

    ret = query_channel_->post(body_ptr->request_no(), buffer, offset);

    if (pkg::tid::req_heart_beat != header.pkg_type_) {
        req_no_ = body_ptr->request_no();
    }

    return ret;
}

int32_t quoter_impl::send(mdp::pkg::body* body_ptr) {
    if (nullptr != body_ptr) {
        LOG_DEBUG("send req_no={0}", body_ptr->request_no());
    } else {
        LOG_DEBUG("send nullptr");
    }

    if (!query_channel_->is_connected()) {
        LOG_DEBUG("send not connected");
        return net_errors::failed;
    }

    int ret = 0;
    size_t offset = 0;
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    mdp::pkg::dmqp_header header;
    header.pkg_type_ = body_ptr->id();
    header.set_request_no(body_ptr->request_no());
    header.pkg_size_ = header.length() + body_ptr->length();

    ret = header.marshal(buffer + offset, sizeof(buffer) - offset);
    if (ret <= 0) {
        return ret;
    }
    offset += ret;
    ret = body_ptr->marshal(buffer + offset, sizeof(buffer) - offset);
    if (ret <= 0) {
        return ret;
    }
    offset += ret;

    LOG_DEBUG("send header={0} body={1}", header.to_string(),
              body_ptr->to_string());

    ret = query_channel_->send(body_ptr->request_no(), buffer, offset);

    if (pkg::tid::req_heart_beat != header.pkg_type_) {
        req_no_ = body_ptr->request_no();
    }

    return ret;
}

// on_connected 查询通道连接
void quoter_impl::on_connected(int channel_id, int code,
                               const std::string& msg) {
    if (nullptr == rsp_) {
        return;
    }
    rsp_->on_connected(code, msg);
}

void quoter_impl::on_disconnected(int channel_id, int code,
                                  const std::string& msg) {
    if (nullptr == rsp_) {
        return;
    }
    rsp_->on_disconnected(code, msg);
}

bool quoter_impl::update_cur_seq_no(uint8_t mdg_no, uint64_t seq_no,
                                    bool force_update) {
    auto it = cur_seq_no_.find(mdg_no);
    if (it == cur_seq_no_.end()) {
        cur_seq_no_.emplace(mdg_no, seq_no);
        return true;
    }

    if (force_update || it->second + 1 == seq_no) {
        it->second = seq_no;
        return true;
    }

    return false;
}

void quoter_impl::on_quote_data(int channel_id, const pkg::dmdp_header& header,
                                pkg::body* body_ptr) {
    // 回调业务
    auto it = udp_handler_map_.find(header.id());
    if (it == udp_handler_map_.end()) {
        return;
    }

    if (pkg::tid::heart_beat != header.id()) {
        if (!update_cur_seq_no(body_ptr->mdg_no(), body_ptr->start_seq_no(),
                               false)) {
            return;
        }
    }

    LOG_DEBUG("on_quote_data tid=0x{0:x}, cid={1}, mdgno={2}, seqno={3}",
              header.id(), channel_id, body_ptr->mdg_no(),
              body_ptr->start_seq_no());

    it->second(channel_id, header, body_ptr);

    // 历史查询结果处理完要更新udp缓存
    if (CHN_TCP == channel_id) {
        quote_channel_->clear_and_next(body_ptr->mdg_no(),
                                       body_ptr->start_seq_no() + 1);
    }
}

void quoter_impl::on_query_data(int channel_id, const pkg::dmqp_header& header,
                                pkg::body* body_ptr) {
    heart_pong_watch_.reset();

    auto it = tcp_handler_map_.find(header.id());
    if (it == tcp_handler_map_.end()) {
        LOG_WARN("tid unsupported=0x{0:x}", header.id());
        return;
    }

    if (pkg::tid::req_heart_beat != header.id()) {
        std::lock_guard<std::mutex> lck(lock_);
        rsp_no_ = body_ptr->request_no();
    }

    LOG_DEBUG("on_query_data tid=0x{0:x}, cid={1}", header.id(), channel_id);

    it->second(channel_id, header, body_ptr);
}

// 登录应答
void quoter_impl::on_login(int channel_id, const pkg::header& header,
                           pkg::body* body_ptr) {
    auto fld_msg = (pkg::fld_rsp_msg*)body_ptr->data_.at(pkg::fid::rsp_msg);
    msg_.err_code = fld_msg->err_code_;
    strncpy(msg_.rsp_msg, "", sizeof(msg_.rsp_msg));
    strncpy(msg_.time_stamp, "", sizeof(msg_.time_stamp));

    auto fld_rsp = (pkg::fld_trader_login_rsp*)body_ptr->data_.at(
        pkg::fid::trader_login_rsp);
    strncpy(login_rsp_.trader_id, fld_rsp->trader_id_,
            sizeof(login_rsp_.trader_id));
    strncpy(login_rsp_.member_id, fld_rsp->member_id_,
            sizeof(login_rsp_.member_id));
    login_rsp_.date = fld_rsp->date_; // 交易核心的日期
    strncpy(login_rsp_.time, fld_rsp->time_,
            sizeof(login_rsp_.time)); // 交易核心的时间
    login_rsp_.is_first_login = fld_rsp->is_first_login_; // 是否首次登录
    login_rsp_.is_pwd_expire_prompt =
        fld_rsp->pwd_expire_reminder_;               // 密码是否到期提示
    login_rsp_.expire_days = fld_rsp->days_overdue_; // 距离过期天数
    login_rsp_.last_login_flag = fld_rsp->last_login_flag_; // 上次登录是否成功
    login_rsp_.last_date = fld_rsp->last_login_date_; // 上次登录日期
    strncpy(login_rsp_.last_time, fld_rsp->last_login_time_,
            sizeof(login_rsp_.last_time)); // 上次登录时间
    strncpy(login_rsp_.last_ip, fld_rsp->last_login_ip_,
            sizeof(login_rsp_.last_ip));              // 上次登录位置
    login_rsp_.failed_times = fld_rsp->failed_times_; // 上次登录失败次数

    if (msg_.err_code == 0) {
        logined_ = true;
    }

    cv_.notify_all();

    // 开启心跳、超时检测定时器
    if (!reconnecting_ && 0 == msg_.err_code) {
        start_heart_timer();
        start_lost_check_timer();
    }

    if (nullptr == rsp_) {
        return;
    }
    rsp_->on_login(body_ptr->request_no(), msg_, login_rsp_, body_ptr->flag());
}

// 登出应答
void quoter_impl::on_logout(int channel_id, const pkg::header& header,
                            pkg::body* body_ptr) {
    stop_heart_timer();
    stop_lost_check_timer();

    if (nullptr == rsp_) {
        return;
    }
    mdp_rsp_msg msg;
    mdp_trader_no trader_no;
    auto fld_msg = (pkg::fld_rsp_msg*)body_ptr->data_.at(pkg::fid::rsp_msg);
    msg.err_code = fld_msg->err_code_;
    strncpy(msg.time_stamp, fld_msg->time_stamp_, sizeof(msg.time_stamp));
    auto fld_rsp = (pkg::fld_trader_id*)body_ptr->data_.at(pkg::fid::trader_id);
    strncpy(trader_no.trader_id, fld_rsp->trader_id_,
            sizeof(trader_no.trader_id));

    if (msg_.err_code == 0) {
        logined_ = false;
    }

    rsp_->on_logout(body_ptr->request_no(), msg, trader_no, body_ptr->flag());
}

// 交易计划查询应答
void quoter_impl::on_handler_rsp_query_trade_plan(int channel_id,
                                                  const pkg::header& header,
                                                  pkg::body* body_ptr) {
    if (nullptr == rsp_) {
        return;
    }
    std::vector<mdp_trade_plan> ret_nty;
    std::map<uint32_t, pkg::field*> sort_map;

    auto fld_msg = (pkg::fld_rsp_msg*)body_ptr->data_.at(pkg::fid::rsp_msg);
    if (fld_msg) {
        msg_.err_code = fld_msg->err_code_;
        strncpy(msg_.time_stamp, fld_msg->time_stamp_, sizeof(msg_.time_stamp));
    }
    // 排序
    for (auto it = body_ptr->data_.begin(); it != body_ptr->data_.end(); ++it) {
        if (it->second->field_id_ != pkg::fid::trade_plan) {
            continue;
        }

        sort_map.insert(std::make_pair(it->second->idx_, it->second));
    }

    mdp_trade_plan rsp;
    memset(&rsp, 0, sizeof(rsp));
    for (auto iter = sort_map.begin(); iter != sort_map.end(); ++iter) {
        auto fld_rsp = (pkg::fld_trade_plan*)iter->second;
        rsp.trade_date = fld_rsp->trade_date_;
        strncpy(rsp.variety_id, fld_rsp->variety_id_, sizeof(rsp.variety_id));
        rsp.trade_type = fld_rsp->trade_type_;
        rsp.state = fld_rsp->state_;
        strncpy(rsp.chg_time, fld_rsp->chg_time_, sizeof(rsp.chg_time));
        rsp.gis_no = fld_rsp->trading_session_no_;
        rsp.next_gis_no = fld_rsp->next_session_no_;

        ret_nty.push_back(rsp);
    }

    rsp_->on_rsp_query_trade_plan(body_ptr->request_no(), msg_, ret_nty,
                                  body_ptr->flag());
}

// 合约基本信息查询应答
void quoter_impl::on_handler_rsp_query_contract_basic(int channel_id,
                                                      const pkg::header& header,
                                                      pkg::body* body_ptr) {
    if (body_ptr->flag() == pkg::body::single ||
        body_ptr->flag() == pkg::body::first) {
        contract_map_.clear();
    }
    auto fld_msg = (pkg::fld_rsp_msg*)body_ptr->data_.at(pkg::fid::rsp_msg);
    if (fld_msg) {
        msg_.err_code = fld_msg->err_code_;
        strncpy(msg_.time_stamp, fld_msg->time_stamp_, sizeof(msg_.time_stamp));
    }

    std::vector<mdp::mdp_contract_basic_info_rsp> contract_rsp_cache;
    std::map<uint32_t, pkg::field*> sort_map;

    // 排序
    for (auto it = body_ptr->data_.begin(); it != body_ptr->data_.end(); ++it) {
        if (it->second->field_id_ != pkg::fid::contract_basic_info) {
            continue;
        }

        sort_map.insert(std::make_pair(it->second->idx_, it->second));
    }

    mdp_contract_basic_info_rsp rsp;
    memset(&rsp, 0, sizeof(rsp));
    for (auto iter = sort_map.begin(); iter != sort_map.end(); ++iter) {
        auto fld_rsp = (pkg::fld_contract_basic_info*)iter->second;
        rsp.contract_no = fld_rsp->contract_no_;
        strncpy(rsp.contract_id, fld_rsp->contract_id_,
                sizeof(rsp.contract_id));
        rsp.trade_date = fld_rsp->trade_date_;
        rsp.init_open_interest = fld_rsp->init_open_interest_;
        rsp.rise_limit = fld_rsp->limit_up_price_;
        rsp.fall_limit = fld_rsp->limit_down_price_;
        rsp.last_clear = fld_rsp->last_settlement_price_;
        rsp.last_close = fld_rsp->last_closing_price_;
        rsp.codec_price = fld_rsp->codec_price_;
        rsp.tick = fld_rsp->tick_;
        strncpy(rsp.contract_name, fld_rsp->contract_name_,
                sizeof(rsp.contract_name));
        rsp.contract_type = fld_rsp->contract_type_;
        //响应域
        contract_rsp_cache.push_back(rsp);
        //合约索引表
        contract_map_[rsp.contract_no] = rsp;

        // 0: 期货合约 1::期权合约 2: 套利合约 3: UDS合约
        if (0 == fld_rsp->contract_type_ || 1 == fld_rsp->contract_type_) {
            //最优行情表
            auto bit = best_quot_cache_map_.find(fld_rsp->contract_no_);
            if (bit == best_quot_cache_map_.end()) {
                best_quot_cache_map_[fld_rsp->contract_no_] = mdp_best_quot();
                //设置合约号--对应到最新的行情缓存中，以备后面增量行情使用
                strncpy(best_quot_cache_map_[fld_rsp->contract_no_].contract_id,
                        fld_rsp->contract_id_,
                        sizeof(best_quot_cache_map_[fld_rsp->contract_no_]
                                   .contract_id));
                best_quot_cache_map_[fld_rsp->contract_no_].trade_date =
                    fld_rsp->trade_date_;
                best_quot_cache_map_[fld_rsp->contract_no_].init_open_interest =
                    fld_rsp->init_open_interest_;
                best_quot_cache_map_[fld_rsp->contract_no_].rise_limit =
                    fld_rsp->limit_up_price_;
                best_quot_cache_map_[fld_rsp->contract_no_].fall_limit =
                    fld_rsp->limit_down_price_;
                best_quot_cache_map_[fld_rsp->contract_no_].last_clear =
                    fld_rsp->last_settlement_price_;
                best_quot_cache_map_[fld_rsp->contract_no_].last_close =
                    fld_rsp->last_closing_price_;
            }
        } else {
            //套利最优行情表
            auto bit = arbi_best_quot_cache_map_.find(fld_rsp->contract_no_);
            if (bit == arbi_best_quot_cache_map_.end()) {
                arbi_best_quot_cache_map_[fld_rsp->contract_no_] =
                    mdp_arbi_best_quot();
                //设置合约号--对应到最新的行情缓存中，以备后面增量行情使用
                strncpy(arbi_best_quot_cache_map_[fld_rsp->contract_no_]
                            .arbi_contract_id,
                        fld_rsp->contract_id_,
                        sizeof(arbi_best_quot_cache_map_[fld_rsp->contract_no_]
                                   .arbi_contract_id));
                arbi_best_quot_cache_map_[fld_rsp->contract_no_].trade_date =
                    fld_rsp->trade_date_;
                arbi_best_quot_cache_map_[fld_rsp->contract_no_].rise_limit =
                    fld_rsp->limit_up_price_;
                arbi_best_quot_cache_map_[fld_rsp->contract_no_].fall_limit =
                    fld_rsp->limit_down_price_;
            }
        }
    }

    if (body_ptr->flag() == pkg::body::single ||
        body_ptr->flag() == pkg::body::last) {
        cv_.notify_all();
    }

    if (nullptr == rsp_) {
        return;
    }
    rsp_->on_rsp_query_contract_basic(body_ptr->request_no(), msg_,
                                      contract_rsp_cache, body_ptr->flag());
}

// 历史行情查询应答
void quoter_impl::on_handler_rsp_query_history_quot(int channel_id,
                                                    const pkg::header& header,
                                                    pkg::body* body_ptr) {
    auto fld_msg = (pkg::fld_rsp_msg*)body_ptr->data_.at(pkg::fid::rsp_msg);
    if (fld_msg) {
        msg_.err_code = fld_msg->err_code_;
        strncpy(msg_.time_stamp, fld_msg->time_stamp_, sizeof(msg_.time_stamp));
    }

    if (nullptr == rsp_) {
        return;
    }
    rsp_->on_rsp_query_history_quot(body_ptr->request_no(), msg_,
                                    history_quot_req_, body_ptr->flag());
}

// 行情快照查询应答
void quoter_impl::on_handler_rsp_query_quot_snap(int channel_id,
                                                 const pkg::header& header,
                                                 pkg::body* body_ptr) {
    auto fld_msg = (pkg::fld_rsp_msg*)body_ptr->data_.at(pkg::fid::rsp_msg);
    if (fld_msg) {
        msg_.err_code = fld_msg->err_code_;
        strncpy(msg_.time_stamp, fld_msg->time_stamp_, sizeof(msg_.time_stamp));
    }

    std::vector<mdp_max_no> max_no_vec;
    std::map<uint32_t, pkg::field*> sort_map;

    // 排序
    for (auto it = body_ptr->data_.begin(); it != body_ptr->data_.end(); ++it) {
        if (it->second->field_id_ != pkg::fid::max_no) {
            continue;
        }

        sort_map.insert(std::make_pair(it->second->idx_, it->second));
    }

    mdp_max_no ret_nty;
    memset(&ret_nty, 0, sizeof(ret_nty));

    for (auto iter = sort_map.begin(); iter != sort_map.end(); ++iter) {
        auto fld_nty = (pkg::fld_max_no*)iter->second;
        ret_nty.mdg_no = fld_nty->mdg_no_;
        ret_nty.max_seq_no = fld_nty->max_seq_no_;
        max_no_vec.push_back(ret_nty);

        quote_channel_->set_next(fld_nty->mdg_no_, fld_nty->max_seq_no_);
        quote_channel_->set_head(fld_nty->mdg_no_,
                                 fld_nty->max_seq_no_); // 防止head小于next
        update_cur_seq_no(fld_nty->mdg_no_, fld_nty->max_seq_no_, true);

        LOG_INFO("rsp_query_quot_snap maxno[{0}]={1}", fld_nty->mdg_no_,
                 fld_nty->max_seq_no_);
    }

    // 无论成功还是失败，都停止恢复模式
    // 如果是失败，会再次触发恢复模式时发快照查询请求
    stop_recover(quot_snap_query_rsp_.mdg_no);

    // 继续行情数据处理
    ready(true);

    cv_.notify_all();
    if (nullptr == rsp_) {
        return;
    }
    rsp_->on_rsp_query_quot_snap(body_ptr->request_no(), msg_,
                                 quot_snap_query_rsp_, max_no_vec,
                                 body_ptr->flag());
}

#ifdef MDP_LN_QUOTER
void quoter_impl::on_handler_rsp_query_contract_param(int channel_id,
                                                      const pkg::header& header,
                                                      pkg::body* body_ptr) {
    if (nullptr == rsp_) {
        return;
    }
    std::vector<mdp_contract_param> contract_param_vec;
    std::map<uint32_t, pkg::field*> sort_map;

    auto fld_msg = (pkg::fld_rsp_msg*)body_ptr->data_.at(pkg::fid::rsp_msg);
    if (fld_msg) {
        msg_.err_code = fld_msg->err_code_;
        strncpy(msg_.time_stamp, fld_msg->time_stamp_, sizeof(msg_.time_stamp));
    }

    // 排序
    for (auto it = body_ptr->data_.begin(); it != body_ptr->data_.end(); ++it) {
        if (it->second->field_id_ != pkg::fid::contract_param) {
            continue;
        }

        sort_map.insert(std::make_pair(it->second->idx_, it->second));
    }

    mdp_contract_param rsp;
    memset(&rsp, 0, sizeof(rsp));
    for (auto iter = sort_map.begin(); iter != sort_map.end(); ++iter) {
        auto fld_rsp = (mdp::pkg::fld_contract_param*)iter->second;
        //合约号转换
        strncpy(rsp.contract_id, get_contract_id(fld_rsp->contract_no_),
                sizeof(rsp.contract_id));
        rsp.unit = fld_rsp->unit_;
        rsp.tick = fld_rsp->tick_;
        rsp.buy_spec_margin = fld_rsp->buy_spec_margin_;
        rsp.sell_spec_margin = fld_rsp->sell_spec_margin_;
        rsp.buy_hedge_margin = fld_rsp->buy_hedge_margin_;
        rsp.sell_hedge_margin = fld_rsp->sell_hedge_margin_;
        rsp.contract_expiration_date = fld_rsp->contract_expiration_date_;
        rsp.last_trade_date = fld_rsp->last_trade_date_;
        rsp.last_deliver_date = fld_rsp->last_deliver_date_;

        contract_param_vec.push_back(rsp);
    }

    rsp_->on_rsp_query_contract_param(body_ptr->request_no(), msg_,
                                      contract_param_vec, body_ptr->flag());
}

// 深度行情合约状态查询应答
void quoter_impl::on_ln_rsp_quot_contract_status(int channel_id,
                                                 const pkg::header& header,
                                                 pkg::body* body_ptr) {
    if (nullptr == rsp_) {
        return;
    }
    std::vector<mdp_contract_status> contract_status_vec;
    std::map<uint32_t, pkg::field*> sort_map;

    auto fld_msg = (pkg::fld_rsp_msg*)body_ptr->data_.at(pkg::fid::rsp_msg);
    if (fld_msg) {
        msg_.err_code = fld_msg->err_code_;
        strncpy(msg_.time_stamp, fld_msg->time_stamp_, sizeof(msg_.time_stamp));
    }

    // 排序
    for (auto it = body_ptr->data_.begin(); it != body_ptr->data_.end(); ++it) {
        if (it->second->field_id_ == pkg::fid::contract_status) {
            sort_map.insert(std::make_pair(it->second->idx_, it->second));
        }
    }

    mdp_contract_status ret_nty;
    memset(&ret_nty, 0, sizeof(ret_nty));
    for (auto iter = sort_map.begin(); iter != sort_map.end(); ++iter) {
        auto fld_nty = (pkg::fld_contract_status*)iter->second;
        strncpy(ret_nty.contract_id, get_contract_id(fld_nty->contract_no_),
                sizeof(ret_nty.contract_id));
        strncpy(ret_nty.chg_time, fld_nty->chg_time_,
                sizeof(ret_nty.chg_time)); //状态触发时间
        ret_nty.op_style = fld_nty->op_style_;
        ret_nty.status = fld_nty->status_;

        contract_status_vec.push_back(ret_nty);
    }

    rsp_->on_ln_rsp_quot_contract_status(body_ptr->request_no(), msg_,
                                         contract_status_vec, body_ptr->flag());
}

#endif

// 合约交易状态通知
void quoter_impl::on_handler_nty_quot_contract_status(int channel_id,
                                                      const pkg::header& header,
                                                      pkg::body* body_ptr) {
    if (nullptr == rsp_) {
        return;
    }
    mdp_contract_status_header status_header;
    status_header.mdg_no = body_ptr->mdg_no();
    status_header.seq_no = body_ptr->start_seq_no();

    std::vector<mdp_variety_status> variety_status_vec;
    std::vector<mdp_contract_status> contract_status_vec;
    std::map<uint32_t, pkg::field*> sort_map1;
    std::map<uint32_t, pkg::field*> sort_map2;

    // 排序
    for (auto it = body_ptr->data_.begin(); it != body_ptr->data_.end(); ++it) {
        if (it->second->field_id_ == pkg::fid::variety_status) {
            sort_map1.insert(std::make_pair(it->second->idx_, it->second));
        }

        if (it->second->field_id_ == pkg::fid::contract_status) {
            sort_map2.insert(std::make_pair(it->second->idx_, it->second));
        }
    }

    mdp_variety_status ret_nty1;
    memset(&ret_nty1, 0, sizeof(ret_nty1));
    for (auto iter = sort_map1.begin(); iter != sort_map1.end(); ++iter) {
        auto fld_nty = (pkg::fld_variety_status*)iter->second;
        strncpy(ret_nty1.variety_id, fld_nty->variety_id_,
                sizeof(ret_nty1.variety_id));
        ret_nty1.trade_type = fld_nty->trade_type_;
        strncpy(ret_nty1.chg_time, fld_nty->chg_time_,
                sizeof(ret_nty1.chg_time)); //状态触发时间
        ret_nty1.op_style = fld_nty->op_style_;
        ret_nty1.status = fld_nty->status_;

        variety_status_vec.push_back(ret_nty1);
    }

    mdp_contract_status ret_nty2;
    memset(&ret_nty2, 0, sizeof(ret_nty2));
    for (auto iter = sort_map2.begin(); iter != sort_map2.end(); ++iter) {
        auto fld_nty = (pkg::fld_contract_status*)iter->second;
        strncpy(ret_nty2.contract_id, get_contract_id(fld_nty->contract_no_),
                sizeof(ret_nty2.contract_id));
        strncpy(ret_nty2.chg_time, fld_nty->chg_time_,
                sizeof(ret_nty2.chg_time)); //状态触发时间
        ret_nty2.op_style = fld_nty->op_style_;
        ret_nty2.status = fld_nty->status_;

        contract_status_vec.push_back(ret_nty2);
    }

    rsp_->on_nty_quot_contract_status(status_header, variety_status_vec,
                                      contract_status_vec, body_ptr->flag());
}

// 市场状态通知
void quoter_impl::on_handler_nty_quot_mkt_status(int channel_id,
                                                 const pkg::header& header,
                                                 pkg::body* body_ptr) {
    if (nullptr == rsp_) {
        return;
    }
    auto fld_nty =
        (pkg::fld_market_status*)body_ptr->data_.at(pkg::fid::market_status);
    mdp_quot_mkt_status ret_nty;
    ret_nty.mdg_no = body_ptr->mdg_no();
    ret_nty.seq_no = body_ptr->start_seq_no();

    ret_nty.mkt_status = fld_nty->mkt_status_; //市场状态
    strncpy(ret_nty.exch_code, fld_nty->exch_code_,
            sizeof(ret_nty.exch_code)); //交易所代码
    strncpy(ret_nty.mkt_chg_time, fld_nty->mkt_chg_time_,
            sizeof(ret_nty.mkt_chg_time)); //状态触发时间

    rsp_->on_nty_quot_mkt_status(ret_nty, body_ptr->flag());
}

// 数据服务盘后通知
void quoter_impl::on_handler_nty_close_mkt_notice(int channel_id,
                                                  const pkg::header& header,
                                                  pkg::body* body_ptr) {
    if (nullptr == rsp_) {
        return;
    }
    auto fld_nty =
        (pkg::fld_data_notify*)body_ptr->data_.at(pkg::fid::data_notify);
    mdp_mkt_data_notice ret_nty;
    ret_nty.mdg_no = body_ptr->mdg_no();
    ret_nty.seq_no = body_ptr->start_seq_no();

    ret_nty.type = fld_nty->type_;
    strncpy(ret_nty.time, fld_nty->time_, sizeof(ret_nty.time)); //状态触发时间

    rsp_->on_nty_close_mkt_notice(ret_nty, body_ptr->flag());
}

void quoter_impl::best_quot_set_invalid_value(mdp_best_quot& best_quot) {
    // match_done
    best_quot.last_price = FTD_VALUE_MAX;
    best_quot.last_match_qty = UINT32_MAX;
    best_quot.match_tot_qty = UINT32_MAX;
    best_quot.turnover = FTD_VALUE_MAX;
    best_quot.open_interest = UINT32_MAX;
    best_quot.interest_chg = INT32_MAX;
    best_quot.avg_price = FTD_VALUE_MAX;
    // high_low_price
    best_quot.high_price = FTD_VALUE_MAX;
    best_quot.low_price = FTD_VALUE_MAX;
    best_quot.life_low = FTD_VALUE_MAX;
    best_quot.life_high = FTD_VALUE_MAX;
    // clear_price
    best_quot.clear_price = FTD_VALUE_MAX;
    best_quot.open_price = FTD_VALUE_MAX;
    best_quot.close_price = FTD_VALUE_MAX;
    // buy_best_order
    best_quot.bid_price = FTD_VALUE_MAX;
    best_quot.bid_qty = UINT32_MAX;
    best_quot.bid_imply_qty = UINT32_MAX;
    // sell_best_order
    best_quot.ask_price = FTD_VALUE_MAX;
    best_quot.ask_qty = UINT32_MAX;
    best_quot.ask_imply_qty = UINT32_MAX;
}

// 行情通知
void quoter_impl::on_handler_best_quot(int channel_id,
                                       const pkg::header& header,
                                       pkg::body* body_ptr) {
    if (nullptr == rsp_) {
        return;
    }

    auto fld_common =
        (pkg::fld_quot_common*)body_ptr->data_.at(pkg::fid::quot_common);
    uint32_t contract_no = fld_common->contract_no_;

    // 目前没有途径知道合约和mdg组的映射关系，
    // 但是在恢复时是以mdg组为单位的，在清理缓存时，需要区分出来
    // 这里利用启动时必须查询一次快照行情来保证得到所有合约的映射
    contract_mdg_map_[contract_no] = body_ptr->mdg_no();

    auto bit = best_quot_cache_map_.find(contract_no);
    if (bit == best_quot_cache_map_.end()) {
        // 从部分域无法分辨具体类型，所以这里再查找一下套利
        if (arbi_best_quot_cache_map_.find(contract_no) !=
            arbi_best_quot_cache_map_.end()) {
            return on_handler_arbi_best_quot(channel_id, header, body_ptr);
        }

        // 正常是先进行合约基本信息进行查询，在其应答时创建，并给出基本信息
        // best_quot_cache_map_[contract_no] = mdp_best_quot();
        //设置合约号
        // strncpy(best_quot_cache_map_[contract_no].contract_id,
        // get_contract_id(contract_no),
        // sizeof(best_quot_cache_map_[contract_no].contract_id));
        LOG_ERROR(
            "on_handler_best_quot-NON-existent contract_no contract_no={0}",
            contract_no);
        return;
    }

    int64_t codec_price = 0;
    int64_t tick = 0;
    std::tie(tick, codec_price) = get_tick_codec_price(contract_no);

    mdp_best_quot tmp = best_quot_cache_map_[contract_no];
    // 用户主动查询场景按增量域形式通知，设置无效值
    if (CHN_TCP == channel_id) {
        best_quot_set_invalid_value(tmp);
    }
    tmp.mdg_no = body_ptr->mdg_no();
    tmp.seq_no = body_ptr->start_seq_no();
    datetime::format(tmp.send_time, sizeof(tmp.send_time),
                     datetime::yyyy_mm_dd_hh_mm_ss_nnn, body_ptr->send_time());

    // 把fld_rsp中的信息，对应到最新的行情缓存中，以备后面增量行情使用
    for (auto it = body_ptr->data_.begin(); it != body_ptr->data_.end(); ++it) {
        switch (it->second->field_id_) {
        case pkg::fid::quot_common: {
            auto ptr = (pkg::fld_quot_common*)it->second;
            tmp.change_no = ptr->contract_seq_no_;
            break;
        }
        case pkg::fid::match_done: {
            auto ptr = (pkg::fld_match_done*)it->second;
            tmp.last_price =
                real_price_by_tick_price(ptr->last_price_, codec_price, tick);
            tmp.last_match_qty = ptr->last_qty_;
            tmp.match_tot_qty = ptr->total_qty_;
            tmp.turnover = ptr->turnover_;
            tmp.open_interest = ptr->open_interest_;
            tmp.avg_price = ptr->avg_price_;
            break;
        }
        case pkg::fid::high_low_price: {
            auto ptr = (pkg::fld_high_low_price*)it->second;
            tmp.high_price = real_price_by_tick_price(ptr->daily_high_price_,
                                                      codec_price, tick);
            tmp.low_price = real_price_by_tick_price(ptr->daily_low_price_,
                                                     codec_price, tick);
            tmp.life_low = real_price_by_tick_price(ptr->historical_low_,
                                                    codec_price, tick);
            tmp.life_high = real_price_by_tick_price(ptr->historical_high_,
                                                     codec_price, tick);
            break;
        }
        case pkg::fid::settlement_price: {
            auto ptr = (pkg::fld_settlement_price*)it->second;
            tmp.clear_price = real_price_by_tick_price(ptr->settlement_price_,
                                                       codec_price, tick);
            tmp.open_price = real_price_by_tick_price(ptr->opening_price_,
                                                      codec_price, tick);
            tmp.close_price = real_price_by_tick_price(ptr->closing_price_,
                                                       codec_price, tick);
            break;
        }
        case pkg::fid::buy_best_order: {
            auto ptr = (pkg::fld_buy_best_order*)it->second;
            tmp.bid_price =
                real_price_by_tick_price(ptr->bid_price_, codec_price, tick);
            tmp.bid_qty = ptr->bid_qty_;
            tmp.bid_imply_qty = ptr->bid_imply_qty_;
            break;
        }
        case pkg::fid::sell_best_order: {
            auto ptr = (pkg::fld_sell_best_order*)it->second;
            tmp.ask_price =
                real_price_by_tick_price(ptr->ask_price_, codec_price, tick);
            tmp.ask_qty = ptr->ask_qty_;
            tmp.ask_imply_qty = ptr->ask_imply_qty_;
            break;
        }
        default: { break; }
        }
    }

    best_quot_cache_map_[contract_no] = tmp;
    rsp_->on_best_quot(tmp, body_ptr->flag());
}

void quoter_impl::arbi_best_quot_set_invalid_value(
    mdp_arbi_best_quot& arbi_best_quot) {
    // high_low_price
    arbi_best_quot.high_price = FTD_VALUE_MAX;
    arbi_best_quot.low_price = FTD_VALUE_MAX;
    arbi_best_quot.life_low = FTD_VALUE_MAX;
    arbi_best_quot.life_high = FTD_VALUE_MAX;
    // arbi_match_done
    arbi_best_quot.last_price = FTD_VALUE_MAX;
    arbi_best_quot.last_match_qty = UINT32_MAX;
    // buy_arbi_best_order
    arbi_best_quot.bid_price = FTD_VALUE_MAX;
    arbi_best_quot.bid_qty = UINT32_MAX;
    // sell_arbi_best_order
    arbi_best_quot.ask_price = FTD_VALUE_MAX;
    arbi_best_quot.ask_qty = UINT32_MAX;
}

// 套利行情通知
void quoter_impl::on_handler_arbi_best_quot(int channel_id,
                                            const pkg::header& header,
                                            pkg::body* body_ptr) {
    if (nullptr == rsp_) {
        return;
    }

    // 然后回调通知用户
    auto fld_common =
        (pkg::fld_quot_common*)body_ptr->data_.at(pkg::fid::quot_common);
    uint32_t contract_no = fld_common->contract_no_;

    auto bit = arbi_best_quot_cache_map_.find(contract_no);
    if (bit == arbi_best_quot_cache_map_.end()) {
        // 正常是先进行合约基本信息进行查询，在其应答时创建，并给出基本信息
        LOG_ERROR("on_handler_ln_arbi_best_quot-NON-existent contract_no "
                  "contract_no={0}",
                  contract_no);
        return;
    }

    int64_t codec_price = 0;
    int64_t tick = 0;
    std::tie(tick, codec_price) = get_tick_codec_price(contract_no);

    mdp_arbi_best_quot tmp = arbi_best_quot_cache_map_[contract_no];
    // 用户主动查询场景按增量域形式通知，设置无效值
    if (CHN_TCP == channel_id) {
        arbi_best_quot_set_invalid_value(tmp);
    }
    tmp.mdg_no = body_ptr->mdg_no();
    tmp.seq_no = body_ptr->start_seq_no();
    datetime::format(tmp.send_time, sizeof(tmp.send_time),
                     datetime::yyyy_mm_dd_hh_mm_ss_nnn, body_ptr->send_time());

    // 把fld_rsp中的信息，对应到最新的行情缓存中，以备后面增量行情使用
    for (auto it = body_ptr->data_.begin(); it != body_ptr->data_.end(); ++it) {
        switch (it->second->field_id_) {
        case pkg::fid::quot_common: {
            auto ptr = (pkg::fld_quot_common*)it->second;
            tmp.change_no = ptr->contract_seq_no_;
            break;
        }
        case pkg::fid::high_low_price: {
            auto ptr = (pkg::fld_high_low_price*)it->second;
            tmp.high_price = real_price_by_tick_price(ptr->daily_high_price_,
                                                      codec_price, tick);
            tmp.low_price = real_price_by_tick_price(ptr->daily_low_price_,
                                                     codec_price, tick);
            tmp.life_low = real_price_by_tick_price(ptr->historical_low_,
                                                    codec_price, tick);
            tmp.life_high = real_price_by_tick_price(ptr->historical_high_,
                                                     codec_price, tick);
            break;
        }
        case pkg::fid::arbi_match_done: {
            auto ptr = (pkg::fld_arbi_match_done*)it->second;
            tmp.last_price =
                real_price_by_tick_price(ptr->last_price_, codec_price, tick);
            tmp.last_match_qty = ptr->last_qty_;
            break;
        }
        case pkg::fid::buy_arbi_best_order: {
            auto ptr = (pkg::fld_buy_arbi_best_order*)it->second;
            tmp.bid_price =
                real_price_by_tick_price(ptr->bid_price_, codec_price, tick);
            tmp.bid_qty = ptr->bid_qty_;
            break;
        }
        case pkg::fid::sell_arbi_best_order: {
            auto ptr = (pkg::fld_sell_arbi_best_order*)it->second;
            tmp.ask_price =
                real_price_by_tick_price(ptr->ask_price_, codec_price, tick);
            tmp.ask_qty = ptr->ask_qty_;
            break;
        }
        default: { break; }
        }
    }

    arbi_best_quot_cache_map_[contract_no] = tmp;
    rsp_->on_arbi_best_quot(tmp, body_ptr->flag());
}

// 期权参数通知
void quoter_impl::on_handler_option_parameter(int channel_id,
                                              const pkg::header& header,
                                              pkg::body* body_ptr) {
    if (nullptr == rsp_) {
        return;
    }

    auto fld_nty = (pkg::fld_opt_param*)body_ptr->data_.at(pkg::fid::opt_param);
    mdp_option_parameter ret_nty;

    ret_nty.mdg_no = body_ptr->mdg_no();
    ret_nty.seq_no = body_ptr->start_seq_no();

    auto fld_common =
        (pkg::fld_quot_common*)body_ptr->data_.at(pkg::fid::quot_common);
    ret_nty.change_no = fld_common->contract_seq_no_;
    //合约号转换
    strncpy(ret_nty.contract_id, get_contract_id(fld_common->contract_no_),
            sizeof(ret_nty.contract_id));

    ret_nty.delta = fld_nty->delta_;
    ret_nty.gamma = fld_nty->gamma_;
    ret_nty.rho = fld_nty->rho_;
    ret_nty.theta = fld_nty->theta_;
    ret_nty.vega = fld_nty->vega_;
    datetime::format(ret_nty.send_time, sizeof(ret_nty.send_time),
                     datetime::yyyy_mm_dd_hh_mm_ss_nnn, body_ptr->send_time());

    rsp_->on_option_parameter(ret_nty, body_ptr->flag());
}

#ifdef MDP_LN_QUOTER
// 分价成交量通知
void quoter_impl::on_handler_ln_segment_price_qty(int channel_id,
                                                  const pkg::header& header,
                                                  pkg::body* body_ptr) {
    if (nullptr == rsp_) {
        return;
    }
    mdp_ln_quot_header ln_quot_header;
    std::vector<mdp_ln_segment_price_qty> ln_segment_price_qty_vec;
    std::map<uint32_t, pkg::field*> sort_map;

    // 业务头转换
    unsigned char chain_flag = body_ptr->flag();
    ln_quot_header.mdg_no = body_ptr->mdg_no();
    ln_quot_header.seq_no = body_ptr->start_seq_no();

    auto fld_segment_price_qty_ptr =
        (pkg::fld_quot_common*)body_ptr->data_.at(pkg::fid::quot_common);
    ln_quot_header.change_no = fld_segment_price_qty_ptr->contract_seq_no_;
    //合约号转换
    strncpy(ln_quot_header.contract_id,
            get_contract_id(fld_segment_price_qty_ptr->contract_no_),
            sizeof(ln_quot_header.contract_id));
    // 时间转换
    datetime::format(ln_quot_header.send_time, sizeof(ln_quot_header.send_time),
                     datetime::yyyy_mm_dd_hh_mm_ss_nnn, body_ptr->send_time());

    // 排序
    for (auto it = body_ptr->data_.begin(); it != body_ptr->data_.end(); ++it) {
        if (it->second->field_id_ != pkg::fid::segment_price_qty) {
            continue;
        }

        sort_map.insert(std::make_pair(it->second->idx_, it->second));
    }

    // 数据转换
    for (auto iter = sort_map.begin(); iter != sort_map.end(); ++iter) {
        mdp_ln_segment_price_qty segment_price_qty_temp;
        mdp::pkg::fld_segment_price_qty* fld_segment_price_qty_ptr =
            (mdp::pkg::fld_segment_price_qty*)iter->second;
        segment_price_qty_temp.price = fld_segment_price_qty_ptr->match_price_;
        segment_price_qty_temp.price_bo_qty =
            fld_segment_price_qty_ptr->buy_open_qty_;
        segment_price_qty_temp.price_be_qty =
            fld_segment_price_qty_ptr->buy_offset_qty_;
        segment_price_qty_temp.price_so_qty =
            fld_segment_price_qty_ptr->sell_open_qty_;
        segment_price_qty_temp.price_se_qty =
            fld_segment_price_qty_ptr->sell_offset_qty_;
        ln_segment_price_qty_vec.push_back(segment_price_qty_temp);
    }
    rsp_->on_ln_segment_price_qty(ln_quot_header, ln_segment_price_qty_vec,
                                  chain_flag);
}

// 定时N档深度行情通知
void quoter_impl::on_handler_ln_mbl_quot(int channel_id,
                                         const pkg::header& header,
                                         pkg::body* body_ptr) {
    if (nullptr == rsp_) {
        return;
    }
    mdp_ln_quot_header ln_quot_header;
    // 业务头转换
    unsigned char chain_flag = body_ptr->flag();
    ln_quot_header.mdg_no = body_ptr->mdg_no();
    ln_quot_header.seq_no = body_ptr->start_seq_no();

    auto fld_common =
        (pkg::fld_quot_common*)body_ptr->data_.at(pkg::fid::quot_common);
    ln_quot_header.change_no = fld_common->contract_seq_no_;
    //合约号转换
    strncpy(ln_quot_header.contract_id,
            get_contract_id(fld_common->contract_no_),
            sizeof(ln_quot_header.contract_id));
    // 时间转换
    datetime::format(ln_quot_header.send_time, sizeof(ln_quot_header.send_time),
                     datetime::yyyy_mm_dd_hh_mm_ss_nnn, body_ptr->send_time());

    std::map<int64_t, mdp_mbl_level>* buy_mbl_level_map_ptr =
        &mbl_buy_cache_[fld_common->contract_no_];
    std::map<int64_t, mdp_mbl_level>* sell_mbl_level_map_ptr =
        &mbl_sell_cache_[fld_common->contract_no_];

    int64_t codec_price = 0;
    int64_t tick = 0;
    std::tie(tick, codec_price) =
        get_tick_codec_price(fld_common->contract_no_);

    // 排序
    std::map<uint32_t, pkg::field*> sort_map;
    for (auto it = body_ptr->data_.begin(); it != body_ptr->data_.end(); ++it) {
        if (it->second->field_id_ != pkg::fid::mbl_quot) {
            continue;
        }

        sort_map.insert(std::make_pair(it->second->idx_, it->second));
    }

    // 数据转换
    std::map<int64_t, mdp_mbl_level>* mbl_level_map_ptr = nullptr;
    for (auto iter = sort_map.begin(); iter != sort_map.end(); ++iter) {
        mdp::pkg::fld_mbl_quot* fld_mbl_quot_ptr =
            (mdp::pkg::fld_mbl_quot*)iter->second;
        auto order_price = real_price_by_tick_price(
            fld_mbl_quot_ptr->order_price_, codec_price, tick);

        LOG_DEBUG("on_handler_ln_mbl_quot {2} order_price={0},id={1},bs={3}",
                  order_price, ln_quot_header.contract_id,
                  fld_mbl_quot_ptr->event_type_, fld_mbl_quot_ptr->bs_flag_);

        // 1:买 3:卖
        if (1 == fld_mbl_quot_ptr->bs_flag_) {
            mbl_level_map_ptr = buy_mbl_level_map_ptr;
        } else {
            mbl_level_map_ptr = sell_mbl_level_map_ptr;
        }

        //实时行情通知将事件还原成完整MBL
        switch (fld_mbl_quot_ptr->event_type_) {
        case 1: { //增加
            auto mbl_quot_ptr = (*mbl_level_map_ptr).find(order_price);
            if (mbl_quot_ptr != (*mbl_level_map_ptr).end()) {
                LOG_ERROR("on_handler_ln_mbl_quot-existent order_price "
                          "order_price={0},addid={1}",
                          order_price, ln_quot_header.contract_id);
                return;
            }

            (*mbl_level_map_ptr)[order_price] = mdp_mbl_level();
            (*mbl_level_map_ptr)[order_price].bs_flag =
                fld_mbl_quot_ptr->bs_flag_;
            (*mbl_level_map_ptr)[order_price].order_price = order_price;
            (*mbl_level_map_ptr)[order_price].order_qty =
                fld_mbl_quot_ptr->order_qty_;
            (*mbl_level_map_ptr)[order_price].imply_qty =
                fld_mbl_quot_ptr->imply_qty_;
            strncpy((*mbl_level_map_ptr)[order_price].gen_time,
                    ln_quot_header.send_time,
                    sizeof((*mbl_level_map_ptr)[order_price].gen_time));
            (*mbl_level_map_ptr)[order_price].event_type = 0;

            // 超过档数删除第一档（买删除价格最小的档位、卖删除价格最大的档位）
            if ((*mbl_level_map_ptr).size() > options_.mbl_levels) {
                if (1 == fld_mbl_quot_ptr->bs_flag_) {
                    auto diter = (*mbl_level_map_ptr).begin();
                    (*mbl_level_map_ptr).erase(diter->second.order_price);
                    LOG_DEBUG("on_handler_ln_mbl_quot rm "
                              "order_price={0},id={1},bs={2}",
                              diter->second.order_price,
                              ln_quot_header.contract_id,
                              fld_mbl_quot_ptr->bs_flag_);
                    LOG_DEBUG("on_handler_ln_mbl_quot rm size={0},{1}",
                              (*mbl_level_map_ptr).size(), options_.mbl_levels);
                } else {
                    auto diter = (*mbl_level_map_ptr).rbegin();
                    (*mbl_level_map_ptr).erase(diter->second.order_price);
                    LOG_DEBUG("on_handler_ln_mbl_quot rm "
                              "order_price={0},id={1},bs={2}",
                              diter->second.order_price,
                              ln_quot_header.contract_id,
                              fld_mbl_quot_ptr->bs_flag_);
                }
            }
        } break;
        case 2: { //修改
            auto mbl_quot_ptr = (*mbl_level_map_ptr).find(order_price);
            if (mbl_quot_ptr == (*mbl_level_map_ptr).end()) {
                LOG_ERROR("on_handler_ln_mbl_quot-NON-existent order_price "
                          "order_price={0},modid={1}",
                          order_price, ln_quot_header.contract_id);

                for (auto& x : (*mbl_level_map_ptr)) {
                    LOG_ERROR("on_handler_ln_mbl_quot order_price={0}",
                              x.first);
                }
                return;
            }

            (*mbl_level_map_ptr)[order_price].bs_flag =
                fld_mbl_quot_ptr->bs_flag_;
            (*mbl_level_map_ptr)[order_price].order_price = order_price;
            (*mbl_level_map_ptr)[order_price].order_qty =
                fld_mbl_quot_ptr->order_qty_;
            (*mbl_level_map_ptr)[order_price].imply_qty =
                fld_mbl_quot_ptr->imply_qty_;
            strncpy((*mbl_level_map_ptr)[order_price].gen_time,
                    ln_quot_header.send_time,
                    sizeof((*mbl_level_map_ptr)[order_price].gen_time));
            (*mbl_level_map_ptr)[order_price].event_type = 0;
        } break;
        case 3: { //删除
            auto mbl_quot_ptr = (*mbl_level_map_ptr).find(order_price);
            if (mbl_quot_ptr == (*mbl_level_map_ptr).end()) {
                LOG_ERROR("on_handler_ln_mbl_quot-NON-existent order_price "
                          "order_price={0},delid={1}",
                          order_price, ln_quot_header.contract_id);
                return;
            }

            (*mbl_level_map_ptr).erase(order_price);
            LOG_DEBUG("on_handler_ln_mbl_quot rm order_price={0},id={1},bs={2}",
                      order_price, ln_quot_header.contract_id,
                      fld_mbl_quot_ptr->bs_flag_);
        } break;
        default:
            break;
        }
    }

    // 因为API有对MBL恢复机制，所以first、middle报文只缓存，不回调用户。收到last报文时统一回调用户，并且以single标志通知。
    if (pkg::body::last == chain_flag) {
        chain_flag = pkg::body::single;
    }
    if (pkg::body::single == chain_flag) {
        rsp_->on_ln_mbl_quot(ln_quot_header, *buy_mbl_level_map_ptr,
                             *sell_mbl_level_map_ptr, chain_flag);
    }
}

// 最优价位前十笔委托通知
void quoter_impl::on_handler_ln_best_level_orders(int channel_id,
                                                  const pkg::header& header,
                                                  pkg::body* body_ptr) {
    if (nullptr == rsp_) {
        return;
    }
    mdp_ln_quot_header ln_quot_header;
    mdp_price_pair price_pair;
    std::vector<mdp_qty_pair> qty_pair_vec;
    std::map<uint32_t, pkg::field*> sort_map;

    // 业务头转换
    unsigned char chain_flag = body_ptr->flag();
    ln_quot_header.mdg_no = body_ptr->mdg_no();
    ln_quot_header.seq_no = body_ptr->start_seq_no();

    auto ptr = (pkg::fld_quot_common*)body_ptr->data_.at(pkg::fid::quot_common);

    ln_quot_header.change_no = ptr->contract_seq_no_;
    //合约号转换
    strncpy(ln_quot_header.contract_id, get_contract_id(ptr->contract_no_),
            sizeof(ln_quot_header.contract_id));
    // 时间转换
    datetime::format(ln_quot_header.send_time, sizeof(ln_quot_header.send_time),
                     datetime::yyyy_mm_dd_hh_mm_ss_nnn, body_ptr->send_time());

    int64_t codec_price = 0;
    int64_t tick = 0;
    std::tie(tick, codec_price) = get_tick_codec_price(ptr->contract_no_);

    auto fld_price_pair_ptr =
        (pkg::fld_price_pair*)body_ptr->data_.at(pkg::fid::price_pair);
    price_pair.bid = real_price_by_tick_price(fld_price_pair_ptr->bid_price_,
                                              codec_price, tick);
    price_pair.ask = real_price_by_tick_price(fld_price_pair_ptr->ask_price_,
                                              codec_price, tick);

    // 排序
    for (auto it = body_ptr->data_.begin(); it != body_ptr->data_.end(); ++it) {
        if (it->second->field_id_ != pkg::fid::qty_pair) {
            continue;
        }

        sort_map.insert(std::make_pair(it->second->idx_, it->second));
    }

    for (auto iter = sort_map.begin(); iter != sort_map.end(); ++iter) {
        mdp_qty_pair mdp_qty_pair_temp;
        mdp::pkg::fld_qty_pair* fld_qty_pair_ptr =
            (mdp::pkg::fld_qty_pair*)iter->second;
        mdp_qty_pair_temp.bid = fld_qty_pair_ptr->bid_qty_;
        mdp_qty_pair_temp.ask = fld_qty_pair_ptr->ask_qty_;
        qty_pair_vec.push_back(mdp_qty_pair_temp);
    }

    rsp_->on_ln_best_level_orders(ln_quot_header, price_pair, qty_pair_vec,
                                  chain_flag);
}

// 委托统计通知
void quoter_impl::on_handler_ln_order_statistic(int channel_id,
                                                const pkg::header& header,
                                                pkg::body* body_ptr) {
    if (nullptr == rsp_) {
        return;
    }

    mdp_ln_order_statistic ln_order_statistic;

    // 业务头转换
    unsigned char chain_flag = body_ptr->flag();
    ln_order_statistic.mdg_no = body_ptr->mdg_no();
    ln_order_statistic.seq_no = body_ptr->start_seq_no();

    auto ptr = (pkg::fld_quot_common*)body_ptr->data_.at(pkg::fid::quot_common);
    ln_order_statistic.change_no = ptr->contract_seq_no_;
    //合约号转换
    strncpy(ln_order_statistic.contract_id, get_contract_id(ptr->contract_no_),
            sizeof(ln_order_statistic.contract_id));
    // 时间转换
    datetime::format(ln_order_statistic.send_time,
                     sizeof(ln_order_statistic.send_time),
                     datetime::yyyy_mm_dd_hh_mm_ss_nnn, body_ptr->send_time());

    // 数据转换
    mdp::pkg::fld_order_statistic_biz6* fld_order_statistic_ptr =
        (mdp::pkg::fld_order_statistic_biz6*)(body_ptr->data_.find(
                                                  mdp::pkg::fid::
                                                      order_statistic_biz6))
            ->second;
    ln_order_statistic.total_buy_order_num =
        fld_order_statistic_ptr->total_buy_order_qty_;
    ln_order_statistic.total_sell_order_num =
        fld_order_statistic_ptr->total_sell_order_qty_;
    ln_order_statistic.weighted_average_buy_order_price =
        fld_order_statistic_ptr->weighted_average_bid_price_;
    ln_order_statistic.weighted_average_sell_order_price =
        fld_order_statistic_ptr->weighted_average_ask_price_;

    rsp_->on_ln_order_statistic(ln_order_statistic, chain_flag);
}

#endif

// 快照行情通知
void quoter_impl::on_handler_snap_best_quot(int channel_id,
                                            const pkg::header& header,
                                            pkg::body* body_ptr) {
    if (nullptr == rsp_) {
        return;
    }
    auto fld_common = (pkg::fld_extend_quot_common*)body_ptr->data_.at(
        pkg::fid::extend_quot_common);
    uint32_t contract_no = fld_common->contract_no_;

    auto bit = best_quot_cache_map_.find(contract_no);
    if (bit == best_quot_cache_map_.end()) {
        // 从部分域无法分辨具体类型，所以这里再查找一下套利
        if (arbi_best_quot_cache_map_.find(contract_no) !=
            arbi_best_quot_cache_map_.end()) {
            return on_handler_snap_arbi_best_quot(channel_id, header, body_ptr);
        }

        LOG_ERROR("on_handler_snap_best_quot-NON-existent contract_no "
                  "contract_no={0}",
                  contract_no);
        return;
    }

    int64_t codec_price = 0;
    int64_t tick = 0;
    std::tie(tick, codec_price) = get_tick_codec_price(contract_no);

    // change by 20200612 单合约快照不应更新缓存行情
    mdp_best_quot tmp = best_quot_cache_map_[contract_no];

    // 把fld_rsp中的信息，对应到最新的行情缓存中，以备后面增量行情使用
    for (auto it = body_ptr->data_.begin(); it != body_ptr->data_.end(); ++it) {
        switch (it->second->field_id_) {
        case pkg::fid::extend_quot_common: {
            auto ptr = (pkg::fld_extend_quot_common*)it->second;
            tmp.change_no = ptr->contract_seq_no_;
            datetime::format(tmp.send_time, sizeof(tmp.send_time),
                             datetime::yyyy_mm_dd_hh_mm_ss_nnn,
                             ptr->send_time_);
            tmp.mdg_no = ptr->mdg_no_;
            tmp.seq_no = ptr->seq_no_;
            break;
        }
        case pkg::fid::match_done: {
            auto ptr = (pkg::fld_match_done*)it->second;
            tmp.last_price =
                real_price_by_tick_price(ptr->last_price_, codec_price, tick);
            tmp.last_match_qty = ptr->last_qty_;
            tmp.match_tot_qty = ptr->total_qty_;
            tmp.turnover = ptr->turnover_;
            tmp.open_interest = ptr->open_interest_;
            tmp.avg_price = ptr->avg_price_;
            break;
        }

        case pkg::fid::high_low_price: {
            auto ptr = (pkg::fld_high_low_price*)it->second;
            tmp.high_price = real_price_by_tick_price(ptr->daily_high_price_,
                                                      codec_price, tick);
            tmp.low_price = real_price_by_tick_price(ptr->daily_low_price_,
                                                     codec_price, tick);
            tmp.life_low = real_price_by_tick_price(ptr->historical_low_,
                                                    codec_price, tick);
            tmp.life_high = real_price_by_tick_price(ptr->historical_high_,
                                                     codec_price, tick);
            break;
        }

        case pkg::fid::settlement_price: {
            auto ptr = (pkg::fld_settlement_price*)it->second;
            tmp.clear_price = real_price_by_tick_price(ptr->settlement_price_,
                                                       codec_price, tick);
            tmp.open_price = real_price_by_tick_price(ptr->opening_price_,
                                                      codec_price, tick);
            tmp.close_price = real_price_by_tick_price(ptr->closing_price_,
                                                       codec_price, tick);
            break;
        }

        case pkg::fid::buy_best_order: {
            auto ptr = (pkg::fld_buy_best_order*)it->second;
            tmp.bid_price =
                real_price_by_tick_price(ptr->bid_price_, codec_price, tick);
            tmp.bid_qty = ptr->bid_qty_;
            tmp.bid_imply_qty = ptr->bid_imply_qty_;
            break;
        }

        case pkg::fid::sell_best_order: {
            auto ptr = (pkg::fld_sell_best_order*)it->second;
            tmp.ask_price =
                real_price_by_tick_price(ptr->ask_price_, codec_price, tick);
            tmp.ask_qty = ptr->ask_qty_;
            tmp.ask_imply_qty = ptr->ask_imply_qty_;
            break;
        }
        default: { break; }
        }
    }

    best_quot_cache_map_[contract_no] = tmp;

    rsp_->on_best_quot(tmp, body_ptr->flag());
}

// 快照套利行情通知
void quoter_impl::on_handler_snap_arbi_best_quot(int channel_id,
                                                 const pkg::header& header,
                                                 pkg::body* body_ptr) {
    if (nullptr == rsp_) {
        return;
    }

    // 然后回调通知用户
    auto fld_common = (pkg::fld_extend_quot_common*)body_ptr->data_.at(
        pkg::fid::extend_quot_common);
    uint32_t contract_no = fld_common->contract_no_;

    auto bit = arbi_best_quot_cache_map_.find(contract_no);
    if (bit == arbi_best_quot_cache_map_.end()) {
        LOG_ERROR("on_handler_ln_snap_arbi_best_quot-NON-existent contract_no "
                  "contract_no={0}",
                  contract_no);
        return;
    }

    int64_t codec_price = 0;
    int64_t tick = 0;
    std::tie(tick, codec_price) = get_tick_codec_price(contract_no);

    // change by 20200612 单合约快照不应更新缓存行情
    mdp_arbi_best_quot tmp = arbi_best_quot_cache_map_[contract_no];

    // 把fld_rsp中的信息，对应到最新的行情缓存中，以备后面增量行情使用
    for (auto it = body_ptr->data_.begin(); it != body_ptr->data_.end(); ++it) {
        switch (it->second->field_id_) {
        case pkg::fid::extend_quot_common: {
            auto ptr = (pkg::fld_extend_quot_common*)it->second;
            tmp.change_no = ptr->contract_seq_no_;
            datetime::format(tmp.send_time, sizeof(tmp.send_time),
                             datetime::yyyy_mm_dd_hh_mm_ss_nnn,
                             ptr->send_time_);
            tmp.mdg_no = ptr->mdg_no_;
            tmp.seq_no = ptr->seq_no_;
            break;
        }
        case pkg::fid::high_low_price: {
            auto ptr = (pkg::fld_high_low_price*)it->second;
            tmp.high_price = real_price_by_tick_price(ptr->daily_high_price_,
                                                      codec_price, tick);
            tmp.low_price = real_price_by_tick_price(ptr->daily_low_price_,
                                                     codec_price, tick);
            tmp.life_low = real_price_by_tick_price(ptr->historical_low_,
                                                    codec_price, tick);
            tmp.life_high = real_price_by_tick_price(ptr->historical_high_,
                                                     codec_price, tick);
            break;
        }
        case pkg::fid::arbi_match_done: {
            auto ptr = (pkg::fld_arbi_match_done*)it->second;
            tmp.last_price =
                real_price_by_tick_price(ptr->last_price_, codec_price, tick);
            tmp.last_match_qty = ptr->last_qty_;
            break;
        }
        case pkg::fid::buy_arbi_best_order: {
            auto ptr = (pkg::fld_buy_arbi_best_order*)it->second;
            tmp.bid_price =
                real_price_by_tick_price(ptr->bid_price_, codec_price, tick);
            tmp.bid_qty = ptr->bid_qty_;
            break;
        }
        case pkg::fid::sell_arbi_best_order: {
            auto ptr = (pkg::fld_sell_arbi_best_order*)it->second;
            tmp.ask_price =
                real_price_by_tick_price(ptr->ask_price_, codec_price, tick);
            tmp.ask_qty = ptr->ask_qty_;
            break;
        }
        default: { break; }
        }
    }

    arbi_best_quot_cache_map_[contract_no] = tmp;

    rsp_->on_arbi_best_quot(tmp, body_ptr->flag());
}

// 快照期权参数通知
void quoter_impl::on_handler_snap_option_parameter(int channel_id,
                                                   const pkg::header& header,
                                                   pkg::body* body_ptr) {
    if (nullptr == rsp_) {
        return;
    }

    mdp_option_parameter ret_nty;
    auto ptr = (pkg::fld_extend_quot_common*)body_ptr->data_.at(
        pkg::fid::extend_quot_common);

    ret_nty.change_no = ptr->contract_seq_no_;
    datetime::format(ret_nty.send_time, sizeof(ret_nty.send_time),
                     datetime::yyyy_mm_dd_hh_mm_ss_nnn, ptr->send_time_);
    ret_nty.mdg_no = ptr->mdg_no_;
    ret_nty.seq_no = ptr->seq_no_;
    strncpy(ret_nty.contract_id, get_contract_id(ptr->contract_no_),
            sizeof(ret_nty.contract_id));

    auto fld_nty = (pkg::fld_opt_param*)body_ptr->data_.at(pkg::fid::opt_param);

    ret_nty.delta = fld_nty->delta_;
    ret_nty.gamma = fld_nty->gamma_;
    ret_nty.rho = fld_nty->rho_;
    ret_nty.theta = fld_nty->theta_;
    ret_nty.vega = fld_nty->vega_;

    rsp_->on_option_parameter(ret_nty, body_ptr->flag());
}

#ifdef MDP_LN_QUOTER

// 快照分价成交量通知
void quoter_impl::on_handler_ln_snap_segment_price_qty(
    int channel_id, const pkg::header& header, pkg::body* body_ptr) {
    if (nullptr == rsp_) {
        return;
    }
    mdp_ln_quot_header ln_quot_header;
    std::vector<mdp_ln_segment_price_qty> ln_segment_price_qty_vec;
    std::map<uint32_t, pkg::field*> sort_map;

    auto ptr = (pkg::fld_extend_quot_common*)body_ptr->data_.at(
        pkg::fid::extend_quot_common);

    ln_quot_header.change_no = ptr->contract_seq_no_;
    //合约号转换
    strncpy(ln_quot_header.contract_id, get_contract_id(ptr->contract_no_),
            sizeof(ln_quot_header.contract_id));
    datetime::format(ln_quot_header.send_time, sizeof(ln_quot_header.send_time),
                     datetime::yyyy_mm_dd_hh_mm_ss_nnn, ptr->send_time_);
    ln_quot_header.mdg_no = ptr->mdg_no_;
    ln_quot_header.seq_no = ptr->seq_no_;

    // 排序
    for (auto it = body_ptr->data_.begin(); it != body_ptr->data_.end(); ++it) {
        if (it->second->field_id_ != pkg::fid::segment_price_qty) {
            continue;
        }

        sort_map.insert(std::make_pair(it->second->idx_, it->second));
    }

    for (auto iter = sort_map.begin(); iter != sort_map.end(); ++iter) {
        mdp_ln_segment_price_qty segment_price_qty_temp;
        mdp::pkg::fld_segment_price_qty* fld_segment_price_qty_ptr =
            (mdp::pkg::fld_segment_price_qty*)iter->second;
        segment_price_qty_temp.price = fld_segment_price_qty_ptr->match_price_;
        segment_price_qty_temp.price_bo_qty =
            fld_segment_price_qty_ptr->buy_open_qty_;
        segment_price_qty_temp.price_be_qty =
            fld_segment_price_qty_ptr->buy_offset_qty_;
        segment_price_qty_temp.price_so_qty =
            fld_segment_price_qty_ptr->sell_open_qty_;
        segment_price_qty_temp.price_se_qty =
            fld_segment_price_qty_ptr->sell_offset_qty_;
        ln_segment_price_qty_vec.push_back(segment_price_qty_temp);
    }

    rsp_->on_ln_segment_price_qty(ln_quot_header, ln_segment_price_qty_vec,
                                  body_ptr->flag());
}

// 快照定时N档深度行情通知
void quoter_impl::on_handler_ln_snap_mbl_quot(int channel_id,
                                              const pkg::header& header,
                                              pkg::body* body_ptr) {
    if (nullptr == rsp_) {
        return;
    }
    mdp_ln_quot_header ln_quot_header;

    auto fld_common = (pkg::fld_extend_quot_common*)body_ptr->data_.at(
        pkg::fid::extend_quot_common);

    ln_quot_header.change_no = fld_common->contract_seq_no_;
    //合约号转换
    strncpy(ln_quot_header.contract_id,
            get_contract_id(fld_common->contract_no_),
            sizeof(ln_quot_header.contract_id));
    datetime::format(ln_quot_header.send_time, sizeof(ln_quot_header.send_time),
                     datetime::yyyy_mm_dd_hh_mm_ss_nnn, fld_common->send_time_);
    ln_quot_header.mdg_no = fld_common->mdg_no_;
    ln_quot_header.seq_no = fld_common->seq_no_;

    int64_t codec_price = 0;
    int64_t tick = 0;
    std::tie(tick, codec_price) =
        get_tick_codec_price(fld_common->contract_no_);

    std::map<int64_t, mdp_mbl_level>* buy_mbl_level_map_ptr =
        &mbl_buy_cache_[fld_common->contract_no_];
    std::map<int64_t, mdp_mbl_level>* sell_mbl_level_map_ptr =
        &mbl_sell_cache_[fld_common->contract_no_];
    buy_mbl_level_map_ptr->clear();
    sell_mbl_level_map_ptr->clear();

    // 排序
    std::map<uint32_t, pkg::field*> sort_map;
    for (auto it = body_ptr->data_.begin(); it != body_ptr->data_.end(); ++it) {
        if (it->second->field_id_ != pkg::fid::mbl_quot) {
            continue;
        }

        sort_map.insert(std::make_pair(it->second->idx_, it->second));
    }

    // 数据转换
    for (auto iter = sort_map.begin(); iter != sort_map.end(); ++iter) {
        mdp::pkg::fld_mbl_quot* fld_mbl_quot_ptr =
            (mdp::pkg::fld_mbl_quot*)iter->second;

        if (fld_mbl_quot_ptr->event_type_ != 1) {
            LOG_ERROR("on_handler_ln_snap_mbl_quot-event_type={0}",
                      fld_mbl_quot_ptr->event_type_);
            return;
        }

        auto order_price = real_price_by_tick_price(
            fld_mbl_quot_ptr->order_price_, codec_price, tick);
        LOG_DEBUG("on_handler_ln_snap_mbl_quot order_price={0},id={1},bs={2}",
                  order_price, ln_quot_header.contract_id,
                  fld_mbl_quot_ptr->bs_flag_);

        std::map<int64_t, mdp_mbl_level>* mbl_level_map_ptr = nullptr;
        // 1:买 3:卖
        if (1 == fld_mbl_quot_ptr->bs_flag_) {
            mbl_level_map_ptr = buy_mbl_level_map_ptr;
        } else {
            mbl_level_map_ptr = sell_mbl_level_map_ptr;
        }

        auto mbl_quot_ptr = mbl_level_map_ptr->find(order_price);
        if (mbl_quot_ptr != mbl_level_map_ptr->end()) {
            LOG_ERROR(
                "on_handler_ln_mbl_quot-existent order_price order_price={0}",
                order_price);
            return;
        }

        (*mbl_level_map_ptr)[order_price] = mdp_mbl_level();
        (*mbl_level_map_ptr)[order_price].bs_flag = fld_mbl_quot_ptr->bs_flag_;
        (*mbl_level_map_ptr)[order_price].order_price = order_price;
        (*mbl_level_map_ptr)[order_price].order_qty =
            fld_mbl_quot_ptr->order_qty_;
        (*mbl_level_map_ptr)[order_price].imply_qty =
            fld_mbl_quot_ptr->imply_qty_;
        strncpy((*mbl_level_map_ptr)[order_price].gen_time,
                ln_quot_header.send_time,
                sizeof((*mbl_level_map_ptr)[order_price].gen_time));
        (*mbl_level_map_ptr)[order_price].event_type = 0;
    }

    rsp_->on_ln_mbl_quot(ln_quot_header, *buy_mbl_level_map_ptr,
                         *sell_mbl_level_map_ptr, body_ptr->flag());
}

// 快照最优价位前十笔委托通知
void quoter_impl::on_handler_ln_snap_best_level_orders(
    int channel_id, const pkg::header& header, pkg::body* body_ptr) {
    if (nullptr == rsp_) {
        return;
    }
    mdp_ln_quot_header ln_quot_header;
    mdp_price_pair price_pair;
    std::vector<mdp_qty_pair> qty_pair_vec;
    std::map<uint32_t, pkg::field*> sort_map;

    // 业务头转换
    auto ptr = (pkg::fld_extend_quot_common*)body_ptr->data_.at(
        pkg::fid::extend_quot_common);

    ln_quot_header.change_no = ptr->contract_seq_no_;
    //合约号转换
    strncpy(ln_quot_header.contract_id, get_contract_id(ptr->contract_no_),
            sizeof(ln_quot_header.contract_id));
    datetime::format(ln_quot_header.send_time, sizeof(ln_quot_header.send_time),
                     datetime::yyyy_mm_dd_hh_mm_ss_nnn, ptr->send_time_);
    ln_quot_header.mdg_no = ptr->mdg_no_;
    ln_quot_header.seq_no = ptr->seq_no_;

    int64_t codec_price = 0;
    int64_t tick = 0;
    std::tie(tick, codec_price) = get_tick_codec_price(ptr->contract_no_);

    auto fld_price_pair_ptr =
        (pkg::fld_price_pair*)body_ptr->data_.at(pkg::fid::price_pair);
    price_pair.bid = real_price_by_tick_price(fld_price_pair_ptr->bid_price_,
                                              codec_price, tick);
    price_pair.ask = real_price_by_tick_price(fld_price_pair_ptr->ask_price_,
                                              codec_price, tick);

    // 排序
    for (auto it = body_ptr->data_.begin(); it != body_ptr->data_.end(); ++it) {
        if (it->second->field_id_ != pkg::fid::qty_pair) {
            continue;
        }

        sort_map.insert(std::make_pair(it->second->idx_, it->second));
    }

    // 数据转换
    for (auto iter = sort_map.begin(); iter != sort_map.end(); ++iter) {
        mdp_qty_pair mdp_qty_pair_temp;
        mdp::pkg::fld_qty_pair* fld_qty_pair_ptr =
            (mdp::pkg::fld_qty_pair*)iter->second;
        mdp_qty_pair_temp.bid = fld_qty_pair_ptr->bid_qty_;
        mdp_qty_pair_temp.ask = fld_qty_pair_ptr->ask_qty_;
        qty_pair_vec.push_back(mdp_qty_pair_temp);
    }

    rsp_->on_ln_best_level_orders(ln_quot_header, price_pair, qty_pair_vec,
                                  body_ptr->flag());
}

// 快照委托统计通知
void quoter_impl::on_handler_ln_snap_order_statistic(int channel_id,
                                                     const pkg::header& header,
                                                     pkg::body* body_ptr) {
    if (nullptr == rsp_) {
        return;
    }
    mdp_ln_order_statistic ln_order_statistic;

    // 业务头转换
    auto ptr = (pkg::fld_extend_quot_common*)body_ptr->data_.at(
        pkg::fid::extend_quot_common);

    ln_order_statistic.change_no = ptr->contract_seq_no_;
    //合约号转换
    strncpy(ln_order_statistic.contract_id, get_contract_id(ptr->contract_no_),
            sizeof(ln_order_statistic.contract_id));
    datetime::format(ln_order_statistic.send_time,
                     sizeof(ln_order_statistic.send_time),
                     datetime::yyyy_mm_dd_hh_mm_ss_nnn, ptr->send_time_);
    ln_order_statistic.mdg_no = ptr->mdg_no_;
    ln_order_statistic.seq_no = ptr->seq_no_;

    // 数据转换
    mdp::pkg::fld_order_statistic_biz6* fld_order_statistic_ptr =
        (mdp::pkg::fld_order_statistic_biz6*)(body_ptr->data_.find(
                                                  mdp::pkg::fid::
                                                      order_statistic_biz6))
            ->second;
    ln_order_statistic.total_buy_order_num =
        fld_order_statistic_ptr->total_buy_order_qty_;
    ln_order_statistic.total_sell_order_num =
        fld_order_statistic_ptr->total_sell_order_qty_;
    ln_order_statistic.weighted_average_buy_order_price =
        fld_order_statistic_ptr->weighted_average_bid_price_;
    ln_order_statistic.weighted_average_sell_order_price =
        fld_order_statistic_ptr->weighted_average_ask_price_;

    rsp_->on_ln_order_statistic(ln_order_statistic, body_ptr->flag());
}

#endif

//流控二级警告通知
void quoter_impl::on_handler_nty_flow_ctrl_warning(int channel_id,
                                                   const pkg::header& header,
                                                   pkg::body* body_ptr) {
    if (nullptr == rsp_) {
        return;
    }
    auto fld_nty = (pkg::fld_flow_ctrl_warning*)body_ptr->data_.at(
        pkg::fid::flow_ctrl_warning);
    mdp_flow_ctrl_warning ret_nty;

    strncpy(ret_nty.trader_id, fld_nty->trader_id_, sizeof(ret_nty.trader_id));
    strncpy(ret_nty.trigger_time, fld_nty->trigger_time_,
            sizeof(ret_nty.trigger_time));
    ret_nty.tgid = fld_nty->pkg_type_gid_;                 /// tid组号
    ret_nty.cur_pkg_cnt = fld_nty->instantaneous_pkg_cnt_; /// 瞬时流量值

    rsp_->on_nty_flow_ctrl_warning(ret_nty, body_ptr->flag());
}

// on_dispatch 分发缓存数据
void quoter_impl::on_timer_dispatch() {
    if (!ready_) {
        return;
    }
    if (reconnecting_) {
        return;
    }
    // 防止长时间没有收到数据包时，不进行消息分发
    for (auto& item : cur_seq_no_) {
        quote_channel_->dispatch(item.first);
    }
}

void quoter_impl::on_quote_heart(int channel_id, const pkg::header& header,
                                 pkg::body* body_ptr) {
    LOG_DEBUG("heart channel={0} header={1} attr={2}", channel_id,
              header.to_string(), body_ptr->to_string());

    quote_channel_->set_head(body_ptr->mdg_no(), body_ptr->start_seq_no());
    // 为方便测试 回调心跳通知给客户
    if (nullptr != rsp_) {
        mdp_quote_heart quote_heart;

        quote_heart.flag = body_ptr->flag();
        quote_heart.mdg_no = body_ptr->mdg_no();
        quote_heart.seq_no = body_ptr->start_seq_no();
        quote_heart.extend = body_ptr->seq_num();
        rsp_->on_quote_heart(channel_id, quote_heart);
    }

    auto next_no = quote_channel_->get_next(body_ptr->mdg_no());
    auto distance =
        quote_channel_->get_head(body_ptr->mdg_no()) - (next_no - 1);
    if (distance < options_.cache_win) {
        return;
    }

    auto max_no = quote_channel_->get_max_breakpoint(body_ptr->mdg_no());

    // 触发恢复
    int ret = start_recover(body_ptr->mdg_no(), next_no, max_no);
    if (ret < 0) {
        LOG_ERROR("recover request failed. ret={0}", ret);
        return;
    }
    LOG_INFO("recover request sent. mdg_no={0} head={1} next={2}",
             body_ptr->mdg_no(), quote_channel_->get_head(body_ptr->mdg_no()),
             next_no);
}

void quoter_impl::on_query_heart(int channel_id, const pkg::header& header,
                                 pkg::body* body_ptr) {
    heart_pong_watch_.reset();
}

void quoter_impl::start_heart_timer() {
    heart_ping_ = true;
    heart_pong_watch_.reset();
    heart_timer_.start(std::bind(&quoter_impl::on_timer_heart, this), 0,
                       options_.heart_interval_ms);
}

void quoter_impl::stop_heart_timer() {
    heart_timer_.stop();
    heart_ping_ = false;
}

// on_timer_heart 心跳定时器
void quoter_impl::on_timer_heart() {
    // 重连状态下，不处理
    if (reconnecting_) {
        LOG_WARN("on_timer_heart reconnecting_={0}", reconnecting_);
        return;
    }

    // 收到心跳时，会重置定时器
    // 超过N次总时长，则触发重连机制
    if (heart_pong_watch_.elapsed() >
        options_.heart_max_failed * options_.heart_interval_ms) {
        LOG_WARN("心跳超时，断开连接，1周期后尝试重新连接");
        query_channel_->disconnect(); // 在 start_reconnect_timer
                                      // 前调用，会关闭socket，变相保证线程安全

        // 1周期后重连
        start_reconnect_timer();
        return;
    }

    // 心跳规则, 约定如下：
    // 服务端要求客户端每5秒发送一次心跳，超过30秒未收到则认为连接异常。
    // 注：不识别空闲时间;
    pkg::body body;
    body.init(pkg::tid::req_heart_beat);
    body.set_request_no(new_req_no());
    int ret = post(&body);
    if (ret != 0) {
        LOG_WARN("发送心跳失败，断开连接，1周期后尝试重新连接");
        query_channel_->disconnect(); // 在 start_reconnect_timer
                                      // 前调用，会关闭socket，变相保证线程安全

        // 1周期后重连
        start_reconnect_timer();
        return;
    }
}

void quoter_impl::start_reconnect_timer() {
    reconnecting_ = true;
    reconnected_ok_ = false;
    std::thread thd(std::bind(&quoter_impl::on_timer_reconnect, this));
    thd.detach();
}

void quoter_impl::stop_reconnect_timer() {
    reconnected_ok_ = false;
    reconnecting_ = false;
}

// on_timer_reconnect 重连定时器
void quoter_impl::on_timer_reconnect() {
    LOG_INFO("等待1周期后尝试重新连接");
    mdp::timers::stop_watch delay;

    uint32_t count = 0;
    while (reconnecting_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if (delay.elapsed() < options_.heart_interval_ms) {
            continue;
        } else {
            delay.reset();
        }
        count++;
        if (count > options_.reconnect_max_times) {
            LOG_ERROR("重连超过最大次数 count={0}", count);
            break;
        }
        LOG_INFO("重新连接... count={0}", count);
        int ret = query_channel_->reconnect();
        if (ret != 0) {
            LOG_WARN("重连失败 count={0}", count);
            continue;
        }
        LOG_INFO("重连成功 count={0}", count);
        LOG_INFO("重新登录... count={0}", count);
        login(new_req_no(), login_req_, msg_, login_rsp_);
        if (0 == msg_.err_code) {
            LOG_INFO("登录成功 count={0}", count);
            reconnected_ok_ = true;
            break;
        } else {
            LOG_WARN("登录失败 count={0}", count);
        }
    }
    reconnecting_ = false;
}

// start_recover 请求恢复
int32_t quoter_impl::start_recover(uint32_t mdg_no, uint64_t start_no,
                                   uint64_t end_no) {
    LOG_INFO("start recover mdg_no={0}", mdg_no);

    // 发送查询历史行情查询
    auto fld = new mdp::pkg::fld_qry_his_quot();
    strncpy(fld->trader_id_, login_rsp_.trader_id, sizeof(fld->trader_id_));
    fld->mdg_no_ = mdg_no;
    // 历史行情查询交易系统返回的是开区间查询结果，所以start_no需要减1，end_no需要加1
    if (start_no > 0) {
        --start_no;
    }
    fld->start_seq_no_ = start_no;
    fld->end_seq_no_ = end_no + 1;
    mdp::pkg::body body;
#ifdef MDP_LN_QUOTER
    body.init(mdp::pkg::tid::req_query_ln_history_quot);
#else
    body.init(mdp::pkg::tid::req_query_history_quot);
#endif

    body.set_request_no(new_req_no());
    body.data_.emplace(fld);

    LOG_DEBUG("start_recover req_query_history_quot body={0}",
              body.to_string());
    int32_t ret = post(&body);
    if (ret != 0) {
        LOG_ERROR("request failed when recovering. req_no={0} mdg_no={1}",
                  body.request_no(), mdg_no);
        return 1;
    }

    strncpy(history_quot_req_.trader_id, fld->trader_id_,
            sizeof(history_quot_req_.trader_id));
    history_quot_req_.mdg_no = fld->mdg_no_; /*行情品种分组号*/
    history_quot_req_.start_seq_no = fld->start_seq_no_; /*开始序列号*/
    history_quot_req_.end_seq_no = fld->end_seq_no_;     /*结束序列号*/

    LOG_INFO("request ok when recovering. req_no={0} mdg_no={1}",
             body.request_no(), mdg_no);
    return 0;
}

// stop_recover 完成恢复
int32_t quoter_impl::stop_recover(uint32_t mdg_no) {
    if (0 == mdg_no) {
        for (auto& item : cur_seq_no_) {
            quote_channel_->set_next(item.first, item.second + 1);
        }
    } else {
        quote_channel_->set_next(mdg_no, cur_seq_no_[mdg_no] + 1);
    }

    return 0;
}

// get_contract_id 获取合约号
const char* quoter_impl::get_contract_id(uint32_t contract_no) {
    if (0 == contract_no) {
        return "";
    }
    auto it = contract_map_.find(contract_no);
    if (it == contract_map_.end()) {
        return "";
    }
    return it->second.contract_id;
}

// get_contract_id 获取合约号
uint32_t quoter_impl::get_contract_no(const char* contract_id) {
    if (nullptr == contract_id || strcmp(contract_id, "") == 0) {
        return 0;
    }
    for (auto& item : contract_map_) {
        if (strcmp(contract_id, item.second.contract_id) == 0) {
            return item.second.contract_no;
        }
    }
    return 0;
}

uint32_t quoter_impl::get_mdg_no_by_contract_no(uint32_t contract_no) {
    if (0 == contract_no) {
        return 0;
    }
    auto it = contract_mdg_map_.find(contract_no);
    if (it == contract_mdg_map_.end()) {
        return 0;
    }
    return it->second;
}

std::pair<int64_t, int64_t>
quoter_impl::get_tick_codec_price(uint32_t contract_no) {
    if (0 == contract_no) {
        return std::make_pair(0, 0);
    }
    auto it = contract_map_.find(contract_no);
    if (it == contract_map_.end()) {
        return std::make_pair(0, 0);
    }
    return std::make_pair(it->second.tick, it->second.codec_price);
}

int64_t quoter_impl::real_price_by_tick_price(int64_t price,
                                              int64_t codec_price,
                                              int64_t tick) {
    if (INT64_MAX == price) {
        return price;
    }

    return price * tick + codec_price;
}

uint32_t quoter_impl::new_req_no() { return inner_req_no_++; }

// on_timer_lost_check 丢包检测定时器
void quoter_impl::on_timer_lost_check() {
    if (!ready_) {
        return;
    }

    if (reconnecting_) {
        return;
    }

    // 定时器分发消息（防止收到丢失报文，后续无新报文，导致丢失报文后的消息一直不分发）
    // 该操作也可以另起一个定时器，从而灵活配置定时器分发消息间隔
    on_timer_dispatch();

    for (auto& item : cur_seq_no_) {
        auto head = quote_channel_->get_head(item.first);
        auto next = quote_channel_->get_next(item.first);

        if (head + 1 == next) {
            // 未丢包设置丢包seq_no为0
            quote_channel_->set_lost(item.first, 0);
            lost_check_watch_[item.first].reset();
        } else if (head > next) {
            if (quote_channel_->get_lost(item.first) != next) {
                // 新的seq_no丢包，重新计时
                quote_channel_->set_lost(item.first, next);
                lost_check_watch_[item.first].reset();
            } else {
                // 同一个seq_no丢包，判断是否超过时限
                if (lost_check_watch_[item.first].elapsed() >
                    options_.lost_check_interval_ms *
                        options_.lost_check_max_failed) {
                    // 触发恢复
                    auto max_no =
                        quote_channel_->get_max_breakpoint(item.first);
                    int ret = start_recover(item.first, next, max_no);
                    if (ret < 0) {
                        LOG_ERROR("[lost_check]recover request failed. ret={0}",
                                  ret);
                        return;
                    }
                    LOG_INFO(
                        "[lost_check]recover request sent. mdg_no={0} head={1} "
                        "next={2} max_no={3}",
                        item.first, head, next, max_no);
                }
            }
        } else {
            // 正常不应该进入该分支
            quote_channel_->set_lost(item.first, 0);
            lost_check_watch_[item.first].reset();
        }
    }
}

// start_lost_check_timer 启动丢包超时检测定时器
void quoter_impl::start_lost_check_timer() {
    lost_check_watch_.clear();
    lost_check_timer_.start(std::bind(&quoter_impl::on_timer_lost_check, this),
                            0, options_.lost_check_interval_ms);
}

// stop_lost_check_timer 停止丢包超时检测定时器
void quoter_impl::stop_lost_check_timer() { lost_check_timer_.stop(); }

// ======
} // namespace mdp
