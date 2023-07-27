/**
 * Copyright (C) 2021，大连商品交易所 大连飞创信息技术有限公司
 *
 * @brief 外部数据结构定义，仅供参考。
 *
 */

#ifndef __MDP_API_TYPES_HPP__
#define __MDP_API_TYPES_HPP__

using ftd_value_t = int64_t;
#define FTD_VALUE_MAX INT64_MAX

namespace mdp {

struct mdp_ln_md {
    // 基本行情
    uint64_t change_no;             /// 合约增量行情编号
    uint32_t mdg_no;                /*行情品种分组号*/
    uint64_t seq_no;                /*发包序列号*/
    uint64_t batch_no;              /*定时发布批次号*/
    uint32_t trade_date;            /*交易日期*/
    char contract_id[129];          /*合约号*/
    uint64_t trans_id;              /*事务编号*/
    char contract_name[61];         /*合约名称*/
    ftd_value_t last_price;         /*最新价*/
    ftd_value_t high_price;         /*最高价*/
    ftd_value_t low_price;          /*最低价*/
    uint32_t last_match_qty;        /*最新成交量*/
    uint32_t match_tot_qty;         /*成交量*/
    ftd_value_t turnover;           /*成交额*/
    uint32_t init_open_interest;    /*初始持仓量*/
    uint32_t open_interest;         /*持仓量*/
    int32_t interest_chg;           /*持仓量变化*/
    ftd_value_t clear_price;        /*今结算价*/
    ftd_value_t life_low;           /*历史最低价*/
    ftd_value_t life_high;          /*历史最高价*/
    ftd_value_t rise_limit;         /*涨停板*/
    ftd_value_t fall_limit;         /*跌停板*/
    ftd_value_t last_clear;         /*上日结算价*/
    ftd_value_t last_close;         /*上日收盘价*/
    ftd_value_t l1_bid_price;       /*最高买*/
    uint32_t l1_bid_qty;            /*申买量*/
    uint32_t l1_bid_imply_qty;      /*申买推导量*/
    ftd_value_t l1_ask_price;       /*最低卖*/
    uint32_t l1_ask_qty;            /*申卖量*/
    uint32_t l1_ask_imply_qty;      /*申卖推导量*/
    ftd_value_t avg_price;          /*当日均价*/
    char send_time[24];             /*生成时间*/
    ftd_value_t open_price;         /*开盘价*/
    ftd_value_t close_price;        /*收盘价*/
    // ln 行情
    uint32_t ln_mdg_no;             /*行情品种分组号*/
    uint64_t ln_seq_no;             /*发包序列号*/
    uint64_t ln_change_no;          /*合约增量行情编号*/
    char ln_contract_id[129];       /*合约号*/
    char ln_send_time[24];          /*发送时间*/
    ftd_value_t bid_order_price[5]; /*买价格*/
    uint32_t bid_order_qty[5];      /*买委托量*/
    uint32_t bid_imply_qty[5];      /*买推导量*/
    ftd_value_t ask_order_price[5]; /*卖价格*/
    uint32_t ask_order_qty[5];      /*卖委托量*/
    uint32_t ask_imply_qty[5];      /*卖推导量*/
    int bid_md_count;               /*买行情数量*/
    int ask_md_count;               /*卖行情数量*/
};

// 选项
struct options {
    uint32_t timeout_ms{30000}; // 通用超时时间, 单位毫秒 默认 30000
    uint32_t heart_interval_ms{5000}; // 心跳时间间隔, 单位毫秒 默认 5000
    uint32_t heart_max_failed{6};    // 心跳最大失败次数 默认6次
    uint32_t reconnect_max_times{5}; // 重连最大次数 默认5次
    uint32_t lost_check_interval_ms{
        1000}; // 丢包超时检测定时器间隔 单位毫秒 默认 1000
    uint32_t lost_check_max_failed{9}; // 丢包超时最大次数 默认9次
    uint64_t cache_size{100000};       // 缓存包的最大个数 默认 100000
    uint32_t cache_win{
        2000}; // 缓存和心跳窗口大小，下一个要处理的序列号和最大缓存或心跳包序列号的差距，达到时会触发恢复机制，默认2000
    uint32_t channel{0}; // 通讯方式 0 全通道（默认）， 1 查询通道，2
                         // 组播双通道，3 组播单通道
    uint32_t mbl_levels{
        5}; // 深度行情档数，需要与交易系统发布的行情档数一致 默认5（仅LN使用）
};

// UPD心跳域
struct mdp_quote_heart {
    uint8_t flag;    // 消息结束标示符
    uint32_t mdg_no; //分组号
    uint64_t seq_no; //发布一次增量行情序列号
    uint64_t extend; //扩展字段
};

// 登录请求域
struct mdp_trader_login_req {
    char member_id[9];    /*会员号*/
    char trader_id[17];   /*席位编号*/
    char pwd[65];         /*席位密码*/
    uint8_t func_type;    /*席位的功能类型*/
    char pos_type[3];     /*席位的位置类型*/
    uint8_t gate_type;    /*网关类型*/
    uint8_t is_prg_trade; /*是否程序化交易*/
    char ip[16];          /*席位IP*/
    char mac[50];         /*席位MAC*/
    char app_version[11]; /*应用版本号*/
    char app_serial[21];  /*应用序列号*/
    char app_name[21];    /*应用名*/
};

// 交易员登录应答
struct mdp_trader_login_rsp {
    char member_id[9];            /*会员号*/
    char trader_id[17];           /*席位编号*/
    uint32_t date;                /*交易核心的日期*/
    char time[13];                /*交易核心的时间*/
    uint64_t latest_order_no;     /*最近成功提交的报单编号*/
    uint8_t is_first_login;       /*是否首次登录*/
    uint8_t is_pwd_expire_prompt; /*密码是否到期提示*/
    int32_t expire_days;          /*距离过期天数*/
    uint8_t last_login_flag;      /*上次登录是否成功*/
    uint32_t last_date;           /*上次登录日期*/
    char last_time[13];           /*上次登录时间*/
    uint8_t ca_style;             /*上次CA认证方法*/
    char last_ip[16];             /*上次登录位置*/
    int32_t failed_times;         /*上次登录失败次数*/
};

//席位密码更改域
struct mdp_trader_pwd_upd {
    char trader_id[17]; /*席位编号*/
    char member_id[9];  /*会员号*/
    char old_pwd[65];   /*旧密码*/
    char new_pwd[65];   /*新密码*/
};

// 基本行情
struct mdp_best_quot {
    uint64_t change_no;          /// 合约增量行情编号
    uint32_t mdg_no;             /*行情品种分组号*/
    uint64_t seq_no;             /*发包序列号*/
    uint64_t batch_no;           /*定时发布批次号*/
    uint32_t trade_date;         /*交易日期*/
    char contract_id[129];       /*合约号*/
    uint64_t trans_id;           /*事务编号*/
    char contract_name[61];      /*合约名称*/
    ftd_value_t last_price;      /*最新价*/
    ftd_value_t high_price;      /*最高价*/
    ftd_value_t low_price;       /*最低价*/
    uint32_t last_match_qty;     /*最新成交量*/
    uint32_t match_tot_qty;      /*成交量*/
    ftd_value_t turnover;        /*成交额*/
    uint32_t init_open_interest; /*初始持仓量*/
    uint32_t open_interest;      /*持仓量*/
    int32_t interest_chg;        /*持仓量变化*/
    ftd_value_t clear_price;     /*今结算价*/
    ftd_value_t life_low;        /*历史最低价*/
    ftd_value_t life_high;       /*历史最高价*/
    ftd_value_t rise_limit;      /*涨停板*/
    ftd_value_t fall_limit;      /*跌停板*/
    ftd_value_t last_clear;      /*上日结算价*/
    ftd_value_t last_close;      /*上日收盘价*/
    ftd_value_t bid_price;       /*最高买*/
    uint32_t bid_qty;            /*申买量*/
    uint32_t bid_imply_qty;      /*申买推导量*/
    ftd_value_t ask_price;       /*最低卖*/
    uint32_t ask_qty;            /*申卖量*/
    uint32_t ask_imply_qty;      /*申卖推导量*/
    ftd_value_t avg_price;       /*当日均价*/
    char send_time[24];          /*生成时间*/
    ftd_value_t open_price;      /*开盘价*/
    ftd_value_t close_price;     /*收盘价*/
};

// 套利行情
struct mdp_arbi_best_quot {
    uint64_t change_no;         /// 合约增量行情编号
    uint32_t mdg_no;            /*行情品种分组号*/
    uint64_t seq_no;            /*发包序列号*/
    uint64_t batch_no;          /*定时发布批次号*/
    uint32_t trade_date;        /*交易日期*/
    char arbi_contract_id[129]; /*套利合约号*/
    uint64_t trans_id;          /*事务编号*/
    ftd_value_t last_price;     /*最新价*/
    uint32_t last_match_qty;    /*最新成交量*/
    ftd_value_t low_price;      /*最低价*/
    ftd_value_t high_price;     /*最高价*/
    ftd_value_t life_low;       /*历史最低价*/
    ftd_value_t life_high;      /*历史最高价*/
    ftd_value_t rise_limit;     /*涨停板*/
    ftd_value_t fall_limit;     /*跌停板*/
    ftd_value_t bid_price;      /*最高买*/
    uint32_t bid_qty;           /*申买量*/
    ftd_value_t ask_price;      /*最低卖*/
    uint32_t ask_qty;           /*申卖量*/
    char send_time[24];         /*生成时间*/
};

// ln行情发送头信息
struct mdp_ln_quot_header {
    uint32_t mdg_no;       /*行情品种分组号*/
    uint64_t seq_no;       /*发包序列号*/
    uint64_t change_no;    /*合约增量行情编号*/
    char contract_id[129]; /*合约号*/
    char send_time[24];    /*发送时间*/
};

// l2 定时N档深度行情通知（基本和套利用一个结构）
struct mdp_mbl_level {
    uint64_t trans_id;       /*事务编号*/
    uint8_t event_type;      /*事件类型*/
    ftd_value_t order_price; /*价格*/
    uint32_t order_qty;      /*委托量*/
    uint32_t imply_qty;      /*推导量*/
    uint8_t bs_flag;         /*买卖标志*/
    char gen_time[24];       /*生成时间*/
};

// 价格对
struct mdp_price_pair {
    ftd_value_t bid; // 买
    ftd_value_t ask; // 卖
};

// 数量对
struct mdp_qty_pair {
    uint32_t bid; // 买
    uint32_t ask; // 卖
};

// l2委托统计通知
struct mdp_ln_order_statistic {
    uint32_t mdg_no;                               /*行情品种分组号*/
    uint64_t seq_no;                               /*发包序列号*/
    uint64_t change_no;                            /*合约增量行情编号*/
    char contract_id[129];                         /*合约号*/
    char send_time[24];                            /*发送时间*/
    uint32_t total_buy_order_num;                  /*买委托总量*/
    uint32_t total_sell_order_num;                 /*卖委托总量*/
    ftd_value_t weighted_average_buy_order_price;  /*加权平均委买价格*/
    ftd_value_t weighted_average_sell_order_price; /*加权平均委卖价格*/
};

// ln分价成交量
struct mdp_ln_segment_price_qty {
    ftd_value_t price;     /* 价格*/
    uint32_t price_bo_qty; /* 买开数量*/
    uint32_t price_be_qty; /* 买平数量*/
    uint32_t price_so_qty; /* 卖开数量*/
    uint32_t price_se_qty; /* 卖平数量*/
};

// 期权参数
struct mdp_option_parameter {
    uint64_t change_no;    /// 合约增量行情编号
    uint32_t mdg_no;       /*行情品种分组号*/
    uint64_t seq_no;       /*发包序列号*/
    uint64_t batch_no;     /*定时发布批次号*/
    uint32_t trade_date;   /*交易日期*/
    char contract_id[129]; /*期权合约号*/
    uint64_t trans_id;     /*事务编号*/
    double delta;          /*delta*/
    double gamma;          /*gama*/
    double rho;            /*rho*/
    double theta;          /*theta*/
    double vega;           /*vega*/
    char send_time[24];    /*生成时间*/
};

// 行情使用的市场状态通知
struct mdp_quot_mkt_status {
    uint32_t mdg_no;       /*行情品种分组号*/
    uint64_t seq_no;       /*发包序列号*/
    uint8_t mkt_status;    /*市场状态*/
    char exch_code[7];     /*交易所代码*/
    char mkt_chg_time[13]; /*状态触发时间*/
};

// 行情使用的合约状态域头
struct mdp_contract_status_header {
    uint32_t mdg_no;   /*行情品种分组号*/
    uint64_t seq_no;   /*发包序列号*/
};

//品种状态域
struct mdp_variety_status {
    char variety_id[5]; /*品种代码*/
    uint8_t trade_type; /*交易类型*/
    uint8_t status;     /*品种交易状态*/
    char chg_time[13];  /*状态触发时间*/
    uint8_t op_style;   /*操作类型*/
};

//合约状态域
struct mdp_contract_status {
    char contract_id[33]; /*合约号*/
    uint8_t status;       /*品种交易状态*/
    char chg_time[13];    /*切换时间*/
    uint8_t op_style;     /*操作类型*/
};

//数据服务盘后应答
struct mdp_mkt_data_notice {
    uint32_t mdg_no;   /*行情品种分组号*/
    uint64_t seq_no;   /*发包序列号*/
    char time[13];     /*时间*/
    uint8_t type;      /*类型*/
    char content[81];  /*内容*/
};

//合约状态查询请求
struct mdp_contract_status_query_req {
    char member_id[9];  /*会员号*/
    char trader_id[17]; /*席位号*/
};

//合约参数查询请求
struct mdp_contract_param_query_req {
    char trader_id[17]; /*席位号*/
};

// 交易计划查询请求域——基本行情、深度行情
struct mdp_trade_plan_query_req {
    char member_id[9];  /*会员号*/
    char trader_id[17]; /*席位号*/
    char variety_id[5]; /*品种号*/
    uint8_t trade_type; /*交易类型*/
};

//历史行情查询（请求/应答公用结果）
struct mdp_history_quot_query {
    char trader_id[17];    /*席位编号*/
    uint32_t mdg_no;       /*行情品种分组号*/
    uint64_t start_seq_no; /*开始序列号*/
    uint64_t end_seq_no;   /*结束序列号*/
};

/* 响应域 */
struct mdp_rsp_msg {
    int32_t err_code;    /*错误码*/
    char rsp_msg[64];    /*错误描述信息*/
    char time_stamp[20]; /*时间戳*/
};

//合约参数查询响应
struct mdp_contract_param {
    char contract_id[129];             /*合约号*/
    uint32_t unit;                     /*交易单位*/
    ftd_value_t tick;                  /*最小变动价位*/
    ftd_value_t buy_spec_margin;       /*买投保证金*/
    ftd_value_t sell_spec_margin;      /*卖投保证金*/
    ftd_value_t buy_hedge_margin;      /*买保保证金*/
    ftd_value_t sell_hedge_margin;     /*卖保保证金*/
    uint32_t contract_expiration_date; /*合约到期日*/
    uint32_t last_trade_date;          /*最后交易日*/
    uint32_t last_deliver_date;        /*最后交割日*/
};

// 交易计划查询应答域——基本行情、深度行情
struct mdp_trade_plan {
    uint32_t trade_date;  /*交易日期*/
    char variety_id[5];   /*品种号*/
    uint8_t trade_type;   /*交易类型*/
    uint8_t state;        /*交易状态*/
    char chg_time[13];    /*切换时间*/
    uint32_t gis_no;      /*交易小节序号*/
    uint32_t next_gis_no; /*下一个交易小节序号*/
};

//流控二级警告域
struct mdp_flow_ctrl_warning {
    char trader_id[17];    /*席位编号*/
    char trigger_time[13]; /*触发时间*/
    uint32_t tgid;         /*tid组号*/
    uint32_t cur_pkg_cnt;  /*瞬时流量值*/
};

// ===========相比七期新增结构请求=============
// 合约基本信息查询请求域
struct mdp_contract_basic_info_req {
    char trader_id[17]; /*席位号*/
    char contract_id[129]; /*合约号。合约号为空时查询所有合约的基本信息。*/
};
//合约基本信息查询应答域
struct mdp_contract_basic_info_rsp {
    uint32_t contract_no;  /*合约号索引*/
    char contract_id[129]; /*合约号*/
    uint32_t trade_date;   /*交易日期*/
    uint8_t contract_type; /*合约类型 0: 期货合约 1：期权合约 2：套利合约
                              3：UDS合约*/
    uint32_t init_open_interest; /*初始持仓量(套利、UDS合约无需关心此值)*/
    ftd_value_t rise_limit; /*涨停板*/
    ftd_value_t fall_limit; /*跌停板*/
    ftd_value_t last_clear; /*上日结算价(套利、UDS合约无需关心此值)*/
    ftd_value_t last_close; /*上日收盘价(套利、UDS合约无需关心此值)*/
    ftd_value_t codec_price; /*编码基准价*/
    ftd_value_t tick;        /*最小变动价位*/
    char contract_name[61]; /*合约名称(套利、UDS合约无需关心此值)*/
};
//行情快照查询请求域
struct mdp_quot_snap_query_req {
    char trader_id[17]; /*席位编号*/
    uint32_t
        mdg_no; /*品种分组号。0代表查询所有mdg组快照，非0代表查询具体mdb组快照。contract_no非0时填写该字段。*/
    char contract_id[129]; /*基础合约编号*/
};

// 行情快照查询应答域
struct mdp_max_no {
    uint32_t mdg_no;     /*品种分组号*/
    uint64_t max_seq_no; /*最大序号*/
};

// 席位号域名
struct mdp_trader_no {
    char trader_id[17]; /*席位编号*/
};
} // namespace mdp
#endif // __MDP_API_TYPES_HPP__
