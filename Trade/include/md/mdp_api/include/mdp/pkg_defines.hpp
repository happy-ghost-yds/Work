/**
 * Copyright (C) 2021，大连商品交易所 大连飞创信息技术有限公司
 *
 * @brief 组播行情包和域定义
 *
 * 组播行情数据包和域定义，打包解包功能等。
 *
 */

#pragma once

#include <climits>
#include <functional>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

namespace mdp {
namespace pkg {
// ======

/// version 版本号
uint8_t proto_version();

/// set_proto_version 方便测试使用，测试版本号规则
void set_proto_version(uint8_t v);

/// max_size 最大包大小，字节数
size_t max_size();

/// proto_type 协议类型
uint8_t proto_type();

// FID列表
namespace fid {
enum : uint16_t {
    unknow = 0x00,
    /// quot_common 合约行情头域
    quot_common = 0x01,
    /// match_done 成交域
    match_done = 0x02,
    /// high_low_price 最高最低价域
    high_low_price = 0x03,
    /// settlement_price 结算价域
    settlement_price = 0x04,
    /// buy_best_order 买最优委托域
    buy_best_order = 0x05,
    /// sell_best_order 卖最优委托域
    sell_best_order = 0x06,
    /// arbi_match_done 套利成交域
    arbi_match_done = 0x07,
    /// buy_arbi_best_order 买套利最优委托域
    buy_arbi_best_order = 0x08,
    /// sell_arbi_best_order 卖套利最优委托域
    sell_arbi_best_order = 0x09,
    /// opt_param 期权参数域
    opt_param = 0x0A,
    /// market_status 市场交易状态域
    market_status = 0x0B,
    /// variety_status 品种交易状态域
    variety_status = 0x0C,
    /// contract_status 合约交易状态域
    contract_status = 0x0D,
    /// data_notify 数据通知域
    data_notify = 0x0E,
    /// mbl_quot 深度行情域
    mbl_quot = 0x0F,
    /// price_pair 价格对域
    price_pair = 0x10,
    /// qty_pair 数量对域
    qty_pair = 0x11,
    /// segment_price_qty 分价成交量域
    segment_price_qty = 0x12,
    /// order_statistic_biz6 委托统计域
    order_statistic_biz6 = 0x13,
    /// extend_quot_common 扩展合约行情头域
    extend_quot_common = 0x14,
    /// rsp_msg 响应域
    rsp_msg = 0x101,
    /// trader_login_req 登录请求域
    trader_login_req = 0x102,
    /// trader_login_rsp 登录应答域
    trader_login_rsp = 0x103,
    /// trader_id 席位号域
    trader_id = 0x104,
    /// flow_ctrl_warning 流控二级警告域
    flow_ctrl_warning = 0x105,
    /// req_contract_basic_info 合约基本信息查询请求域
    req_contract_basic_info = 0x106,
    /// contract_basic_info 合约基本信息域
    contract_basic_info = 0x107,
    /// contract_param 合约参数域
    contract_param = 0x108,
    /// req_trade_plan 交易计划查询请求域
    req_trade_plan = 0x109,
    /// trade_plan 交易计划域
    trade_plan = 0x10A,
    /// qry_his_quot 查询历史行情域
    qry_his_quot = 0x10B,
    /// qry_quot_snap 查询行情快照域
    qry_quot_snap = 0x10C,
    /// max_no 最大序号域
    max_no = 0x10D
};

/// exists 有效的fid
bool exists(uint16_t fid);
} // namespace fid

// TID列表
namespace tid {
enum : uint16_t {
    unknow = 0x00,
    /// heart_beat 心跳通知报文
    heart_beat = 0x01,
    /// level_1_period L1定时行情报文
    level_1_period = 0x02,
    /// level_2_period L2定时行情报文
    level_2_period = 0x03,
    /// nty_mkt_status 市场状态报文
    nty_mkt_status = 0x04,
    /// nty_trader_status 交易状态报文
    nty_trader_status = 0x05,
    /// nty_close_mkt_notice 数据服务通知报文
    nty_close_mkt_notice = 0x06,
    /// req_heart_beat 查询网关心跳请求
    req_heart_beat = 0x101,
    /// rsp_heart_beat 查询网关心跳应答
    rsp_heart_beat = 0x102,
    /// req_trader_login 席位登录请求
    req_trader_login = 0x103,
    /// rsp_trader_login 席位登录应答
    rsp_trader_login = 0x104,
    /// req_trader_logout 席位退出请求
    req_trader_logout = 0x105,
    /// rsp_trader_logout 席位退出应答
    rsp_trader_logout = 0x106,
    /// flow_ctrl_warning 向柜台发送二级警告
    flow_ctrl_warning = 0x107,
    /// req_query_contract_basic_info 合约基本信息查询请求
    req_query_contract_basic_info = 0x109,
    /// rsp_query_contract_basic_info 合约基本信息查询应答
    rsp_query_contract_basic_info = 0x10A,
    /// req_qry_contract_status 合约交易状态查询请求
    req_qry_contract_status = 0x10B,
    /// rsp_qry_contract_status 合约交易状态查询应答
    rsp_qry_contract_status = 0x10C,
    /// req_qry_contract_param 合约参数查询请求
    req_qry_contract_param = 0x10D,
    /// rsp_qry_contract_param 合约参数查询应答
    rsp_qry_contract_param = 0x10E,
    /// req_qry_trade_plan 合约交易计划查询请求
    req_qry_trade_plan = 0x10F,
    /// rsp_qry_trade_plan 交易计划查询应答
    rsp_qry_trade_plan = 0x110,
    /// req_query_history_quot 历史行情查询请求
    req_query_history_quot = 0x111,
    /// rsp_query_history_quot 历史行情查询应答
    rsp_query_history_quot = 0x112,
    /// req_query_quot_snap 行情快照查询请求
    req_query_quot_snap = 0x113,
    /// rsp_query_quot_snap 行情快照查询应答
    rsp_query_quot_snap = 0x114,
    /// req_query_ln_history_quot LN历史行情查询请求
    req_query_ln_history_quot = 0x117,
    /// rsp_query_ln_history_quot LN历史行情查询应答
    rsp_query_ln_history_quot = 0x118,
    /// req_query_ln_quot_snap LN行情快照查询请求
    req_query_ln_quot_snap = 0x119,
    /// rsp_query_ln_quot_snap LN行情快照查询应答
    rsp_query_ln_quot_snap = 0x11A,
    /// quot 最优行情通知
    quot = 0x8001,
    /// quot_snap 最优行情快照
    quot_snap = 0x8002,
    /// opt_greek_quot 期权参数通知
    opt_greek_quot = 0x8003,
    /// opt_greek_quot_snap 期权参数快照
    opt_greek_quot_snap = 0x8004,
    /// ln_mbl_quot_biz6 五档深度行情通知
    ln_mbl_quot_biz6 = 0x8005,
    /// ln_mbl_quot_biz6_snap 五档深度行情快照
    ln_mbl_quot_biz6_snap = 0x8006,
    /// ln_best_level_orders_biz6 最优价位前十笔委托通知
    ln_best_level_orders_biz6 = 0x8007,
    /// ln_best_level_orders_biz6_snap 最优价位前十笔委托快照
    ln_best_level_orders_biz6_snap = 0x8008,
    /// ln_segment_price_match_qty 分价成交量通知
    ln_segment_price_match_qty = 0x8009,
    /// ln_segment_price_match_qty_snap 分价成交量快照
    ln_segment_price_match_qty_snap = 0x800A,
    /// ln_order_statistics 委托统计通知
    ln_order_statistics = 0x800B,
    /// ln_order_statistics_snap 委托统计快照
    ln_order_statistics_snap = 0x800C
};

/// exists 有效的tid
bool exists(uint16_t tid);
} // namespace tid

uint16_t get_tid_by_field_ids(const std::vector<uint16_t>& flds);

uint16_t get_tid_by_field_ids_snap(const std::vector<uint16_t>& flds);

// is_relative
bool is_relative(uint16_t tid, uint16_t fid);

// field_count_of
size_t field_count_of(uint16_t tid);

// err 错误码
class err {
  public:
    // 内置
    static const int unknow = INT_MIN;        /// 未知错误
    static const int undefined = INT_MIN + 1; /// 未定义错误
    static const int ok = 0;                  /// 成功
    static const int failed = -1;             /// 失败
    static const int timeout = -2;            /// 超时

    // -1000 用户错误
    static const int length_error = -1001; /// 长度不符合要求
    static const int short_length = -1002; /// 长度太短
    static const int long_length = -1003;  /// 长度太长
    static const int zero_length = -1004;  /// 长度不能为0
    static const int null_buf = -1005;     /// buffer不能为空
    static const int null_obj = -1006;     /// object不能为空

    // -2000 协议错误
    static const int proto_err = -2001;     /// 协议错误
    static const int proto_ver_err = -2002; /// 协议版本错误

    // -3000 包体错误
    static const int body_err = -3001;      /// 包体错误
    static const int body_type_err = -3002; /// 包体类型错误

    // -4000 属性错误
    static const int attr_err = -4001;      /// 属性错误
    static const int attr_type_err = -4002; /// 属性类型错误

    // -5000 数据错误
    static const int data_err = -5001;      /// 数据错误
    static const int data_type_err = -5002; /// 数据类型错误

    // -6000 域数据错误
    static const int field_err = -6001;      /// 数据错误
    static const int field_type_err = -6002; /// 数据类型错误

    /// msg 错误信息
    static const char* msg(int code);

  private:
    err(){};
};

#pragma pack(push, 1)

/// header
class header {
  public:
    /// id 标识
    virtual uint16_t id() const = 0;
    /// to_string 生成描述文字
    virtual std::string to_string(void) const = 0;
    virtual ~header() = default;
};

/// dmdp_header udp报文头
class dmdp_header : public header {
  public:
    // 构造函数
    dmdp_header();

    /// id 标识
    uint16_t id() const override;

    /// length 长度大小 字节
    static size_t length();

    // is_valid 判断包头是否有效
    bool is_valid();

    /// marshal 序列化
    int32_t marshal(char* buf, size_t len) const;

    /// unmarshal 反序列化
    int32_t unmarshal(const char* buf, size_t len);

    // 设置传输报文长度
    void set_pkg_size(uint16_t pkg_size);
    // 获取传输报文长度
    uint16_t pkg_size() const;

    // 设置报文类型
    void set_pkg_type(uint16_t pkg_type);
    // 获取报文类型
    uint16_t pkg_type() const;

    // 设置版本号
    void set_version(uint8_t version);
    // 获取版本号
    uint8_t version() const;

    // 设置消息结束标示符
    void set_flag(uint8_t flag);
    // 获取消息结束标示符
    uint8_t flag() const;

    // 设置产品组号
    void set_mdg_no(uint8_t mdg_no);
    // 获取产品组号
    uint8_t mdg_no() const;

    // 设置第一个行情消息的编号
    void set_start_seq_no(uint64_t start_seq_no);
    // 获取第一个行情消息的编号
    uint64_t start_seq_no() const;

    // 设置扩展字段
    void set_seq_num(uint8_t seq_num);
    // 获取扩展字段
    uint8_t seq_num() const;

    // 设置发送时间
    void set_send_time(uint64_t send_time);
    // 获取发送时间
    uint64_t send_time() const;

    // 设置扩展字段
    void set_reserved(int8_t reserved);
    // 获取扩展字段
    int8_t reserved() const;

    /// to_string 生成描述文字
    std::string to_string(void) const override;

    uint16_t pkg_size_;     /// 传输报文长度
    uint16_t pkg_type_;     /// 报文类型
    uint8_t version_;       /// 版本号
    uint8_t flag_;          /// 消息结束标示符
    uint8_t mdg_no_;        /// 产品组号
    uint64_t start_seq_no_; /// 第一个行情消息的编号
    uint8_t seq_num_;       /// 扩展字段
    uint64_t send_time_;    /// 发送时间
    int8_t reserved_;       /// 扩展字段
};

/// dmqp_header tcp报文头
class dmqp_header : public header {
  public:
    // 构造函数
    dmqp_header();

    /// id 标识
    uint16_t id() const override;

    /// length 长度大小 字节
    static size_t length();

    // is_valid 判断包头是否有效
    bool is_valid();

    /// marshal 序列化
    int32_t marshal(char* buf, size_t len) const;

    /// unmarshal 反序列化
    int32_t unmarshal(const char* buf, size_t len);

    // 设置传输报文长度
    void set_pkg_size(uint16_t pkg_size);
    // 获取传输报文长度
    uint16_t pkg_size() const;

    // 设置报文类型
    void set_pkg_type(uint16_t pkg_type);
    // 获取报文类型
    uint16_t pkg_type() const;

    // 设置版本号
    void set_version(uint8_t version);
    // 获取版本号
    uint8_t version() const;

    // 设置消息结束标示符
    void set_flag(uint8_t flag);
    // 获取消息结束标示符
    uint8_t flag() const;

    // 设置请求号
    void set_request_no(uint32_t request_no);
    // 获取请求号
    uint32_t request_no() const;

    // 设置扩展字段
    void set_reserved(int8_t reserved);
    // 获取扩展字段
    int8_t reserved() const;

    /// to_string 生成描述文字
    std::string to_string(void) const override;

    uint16_t pkg_size_;   /// 传输报文长度
    uint16_t pkg_type_;   /// 报文类型
    uint8_t version_;     /// 版本号
    uint8_t flag_;        /// 消息结束标示符
    uint32_t request_no_; /// 请求号
    int8_t reserved_;     /// 扩展字段
};

// -------------
// field

/// field 数据域头
class field {
  public:
    static field* make(uint16_t fid);

    explicit field(uint16_t fid = 0);

    virtual ~field() = default;

    /// id 标识
    virtual uint16_t id() const;

    /// length 长度大小 字节
    virtual size_t length() const;

    /// max_length 长度大小 字节
    virtual size_t max_length() const;

    /// marshal 序列化
    virtual int32_t marshal(char* buf, size_t len) const;

    /// unmarshal 反序列化
    virtual int32_t unmarshal(const char* buf, size_t len);

    /// to_string 生成描述文字
    virtual std::string to_string(void) const;

    mutable uint16_t field_size_ = 0; /// 域长度
    uint16_t field_id_ = 0;           /// 域标识
    uint32_t idx_ = 0; /// 排序标识，单个报文里包含多个相同域时用于排序
};

/// fld_quot_common 合约行情头域
class fld_quot_common : public field {
  public:
    fld_quot_common();

    /// id 标识
    uint16_t id() const override;

    /// length 长度大小 字节
    size_t length() const override;

    /// max_length 长度大小 字节
    size_t max_length() const override;

    /// marshal 序列化
    int32_t marshal(char* buf, size_t len) const override;

    /// unmarshal 反序列化
    int32_t unmarshal(const char* buf, size_t len) override;

    /// to_string 生成描述文字
    std::string to_string(void) const override;

    uint32_t contract_no_;     /// 合约号
    uint64_t contract_seq_no_; /// 合约增量行情编号
};

/// fld_match_done 成交域
class fld_match_done : public field {
  public:
    fld_match_done();

    /// id 标识
    uint16_t id() const override;

    /// length 长度大小 字节
    size_t length() const override;

    /// max_length 长度大小 字节
    size_t max_length() const override;

    /// marshal 序列化
    int32_t marshal(char* buf, size_t len) const override;

    /// unmarshal 反序列化
    int32_t unmarshal(const char* buf, size_t len) override;

    /// to_string 生成描述文字
    std::string to_string(void) const override;

    int64_t last_price_;     /// 最新价
    uint32_t last_qty_;      /// 最新成交量
    uint32_t total_qty_;     /// 成交量
    int64_t turnover_;       /// 成交额
    uint32_t open_interest_; /// 持仓量
    int64_t avg_price_;      /// 当日成交均价
};

/// fld_high_low_price 最高最低价域
class fld_high_low_price : public field {
  public:
    fld_high_low_price();

    /// id 标识
    uint16_t id() const override;

    /// length 长度大小 字节
    size_t length() const override;

    /// max_length 长度大小 字节
    size_t max_length() const override;

    /// marshal 序列化
    int32_t marshal(char* buf, size_t len) const override;

    /// unmarshal 反序列化
    int32_t unmarshal(const char* buf, size_t len) override;

    /// to_string 生成描述文字
    std::string to_string(void) const override;

    int64_t daily_high_price_; /// 最高价
    int64_t daily_low_price_;  /// 最低价
    int64_t historical_high_;  /// 历史最高价
    int64_t historical_low_;   /// 历史最低价
};

/// fld_settlement_price 结算价域
class fld_settlement_price : public field {
  public:
    fld_settlement_price();

    /// id 标识
    uint16_t id() const override;

    /// length 长度大小 字节
    size_t length() const override;

    /// max_length 长度大小 字节
    size_t max_length() const override;

    /// marshal 序列化
    int32_t marshal(char* buf, size_t len) const override;

    /// unmarshal 反序列化
    int32_t unmarshal(const char* buf, size_t len) override;

    /// to_string 生成描述文字
    std::string to_string(void) const override;

    int64_t settlement_price_; /// 今结算价
    int64_t opening_price_;    /// 开盘价
    int64_t closing_price_;    /// 收盘价
};

/// fld_buy_best_order 买最优委托域
class fld_buy_best_order : public field {
  public:
    fld_buy_best_order();

    /// id 标识
    uint16_t id() const override;

    /// length 长度大小 字节
    size_t length() const override;

    /// max_length 长度大小 字节
    size_t max_length() const override;

    /// marshal 序列化
    int32_t marshal(char* buf, size_t len) const override;

    /// unmarshal 反序列化
    int32_t unmarshal(const char* buf, size_t len) override;

    /// to_string 生成描述文字
    std::string to_string(void) const override;

    int64_t bid_price_;      /// 最高买
    uint32_t bid_qty_;       /// 申买量
    uint32_t bid_imply_qty_; /// 申买推导量
};

/// fld_sell_best_order 卖最优委托域
class fld_sell_best_order : public field {
  public:
    fld_sell_best_order();

    /// id 标识
    uint16_t id() const override;

    /// length 长度大小 字节
    size_t length() const override;

    /// max_length 长度大小 字节
    size_t max_length() const override;

    /// marshal 序列化
    int32_t marshal(char* buf, size_t len) const override;

    /// unmarshal 反序列化
    int32_t unmarshal(const char* buf, size_t len) override;

    /// to_string 生成描述文字
    std::string to_string(void) const override;

    int64_t ask_price_;      /// 最高卖
    uint32_t ask_qty_;       /// 申卖量
    uint32_t ask_imply_qty_; /// 申卖推导量
};

/// fld_arbi_match_done 套利成交域
class fld_arbi_match_done : public field {
  public:
    fld_arbi_match_done();

    /// id 标识
    uint16_t id() const override;

    /// length 长度大小 字节
    size_t length() const override;

    /// max_length 长度大小 字节
    size_t max_length() const override;

    /// marshal 序列化
    int32_t marshal(char* buf, size_t len) const override;

    /// unmarshal 反序列化
    int32_t unmarshal(const char* buf, size_t len) override;

    /// to_string 生成描述文字
    std::string to_string(void) const override;

    int64_t last_price_; /// 最新价
    uint32_t last_qty_;  /// 最新成交量
};

/// fld_buy_arbi_best_order 买套利最优委托域
class fld_buy_arbi_best_order : public field {
  public:
    fld_buy_arbi_best_order();

    /// id 标识
    uint16_t id() const override;

    /// length 长度大小 字节
    size_t length() const override;

    /// max_length 长度大小 字节
    size_t max_length() const override;

    /// marshal 序列化
    int32_t marshal(char* buf, size_t len) const override;

    /// unmarshal 反序列化
    int32_t unmarshal(const char* buf, size_t len) override;

    /// to_string 生成描述文字
    std::string to_string(void) const override;

    int64_t bid_price_; /// 最高买
    uint32_t bid_qty_;  /// 申买量
};

/// fld_sell_arbi_best_order 卖套利最优委托域
class fld_sell_arbi_best_order : public field {
  public:
    fld_sell_arbi_best_order();

    /// id 标识
    uint16_t id() const override;

    /// length 长度大小 字节
    size_t length() const override;

    /// max_length 长度大小 字节
    size_t max_length() const override;

    /// marshal 序列化
    int32_t marshal(char* buf, size_t len) const override;

    /// unmarshal 反序列化
    int32_t unmarshal(const char* buf, size_t len) override;

    /// to_string 生成描述文字
    std::string to_string(void) const override;

    int64_t ask_price_; /// 最高卖
    uint32_t ask_qty_;  /// 申卖量
};

/// fld_opt_param 期权参数域
class fld_opt_param : public field {
  public:
    fld_opt_param();

    /// id 标识
    uint16_t id() const override;

    /// length 长度大小 字节
    size_t length() const override;

    /// max_length 长度大小 字节
    size_t max_length() const override;

    /// marshal 序列化
    int32_t marshal(char* buf, size_t len) const override;

    /// unmarshal 反序列化
    int32_t unmarshal(const char* buf, size_t len) override;

    /// to_string 生成描述文字
    std::string to_string(void) const override;

    double delta_; /// delta
    double gamma_; /// gamma
    double rho_;   /// rho
    double theta_; /// theta
    double vega_;  /// vega
};

/// fld_market_status 市场交易状态域
class fld_market_status : public field {
  public:
    fld_market_status();

    /// id 标识
    uint16_t id() const override;

    /// length 长度大小 字节
    size_t length() const override;

    /// max_length 长度大小 字节
    size_t max_length() const override;

    /// marshal 序列化
    int32_t marshal(char* buf, size_t len) const override;

    /// unmarshal 反序列化
    int32_t unmarshal(const char* buf, size_t len) override;

    /// to_string 生成描述文字
    std::string to_string(void) const override;

    uint8_t mkt_status_;    /// 市场状态
    char exch_code_[7];     /// 交易所代码
    char mkt_chg_time_[13]; /// 状态触发时间
};

/// fld_variety_status 品种交易状态域
class fld_variety_status : public field {
  public:
    fld_variety_status();

    /// id 标识
    uint16_t id() const override;

    /// length 长度大小 字节
    size_t length() const override;

    /// max_length 长度大小 字节
    size_t max_length() const override;

    /// marshal 序列化
    int32_t marshal(char* buf, size_t len) const override;

    /// unmarshal 反序列化
    int32_t unmarshal(const char* buf, size_t len) override;

    /// to_string 生成描述文字
    std::string to_string(void) const override;

    char variety_id_[5]; /// 品种号
    uint8_t trade_type_; /// 交易类型
    uint8_t status_;     /// 合约状态
    uint8_t op_style_;   /// 控制模式
    char chg_time_[13];  /// 切换时间
};

/// fld_contract_status 合约交易状态域
class fld_contract_status : public field {
  public:
    fld_contract_status();

    /// id 标识
    uint16_t id() const override;

    /// length 长度大小 字节
    size_t length() const override;

    /// max_length 长度大小 字节
    size_t max_length() const override;

    /// marshal 序列化
    int32_t marshal(char* buf, size_t len) const override;

    /// unmarshal 反序列化
    int32_t unmarshal(const char* buf, size_t len) override;

    /// to_string 生成描述文字
    std::string to_string(void) const override;

    uint32_t contract_no_; /// 合约号
    uint8_t status_;       /// 合约状态
    uint8_t op_style_;     /// 控制模式
    char chg_time_[13];    /// 切换时间
};

/// fld_data_notify 数据通知域
class fld_data_notify : public field {
  public:
    fld_data_notify();

    /// id 标识
    uint16_t id() const override;

    /// length 长度大小 字节
    size_t length() const override;

    /// max_length 长度大小 字节
    size_t max_length() const override;

    /// marshal 序列化
    int32_t marshal(char* buf, size_t len) const override;

    /// unmarshal 反序列化
    int32_t unmarshal(const char* buf, size_t len) override;

    /// to_string 生成描述文字
    std::string to_string(void) const override;

    uint8_t type_;  /// 类型
    char time_[13]; /// 时间
};

/// fld_mbl_quot 深度行情域
class fld_mbl_quot : public field {
  public:
    fld_mbl_quot();

    /// id 标识
    uint16_t id() const override;

    /// length 长度大小 字节
    size_t length() const override;

    /// max_length 长度大小 字节
    size_t max_length() const override;

    /// marshal 序列化
    int32_t marshal(char* buf, size_t len) const override;

    /// unmarshal 反序列化
    int32_t unmarshal(const char* buf, size_t len) override;

    /// to_string 生成描述文字
    std::string to_string(void) const override;

    uint8_t event_type_;  /// 事件类型
    uint8_t bs_flag_;     /// 买卖标志
    int64_t order_price_; /// 价格
    uint32_t order_qty_;  /// 委托量
    uint32_t imply_qty_;  /// 推导量
};

/// fld_price_pair 价格对域
class fld_price_pair : public field {
  public:
    fld_price_pair();

    /// id 标识
    uint16_t id() const override;

    /// length 长度大小 字节
    size_t length() const override;

    /// max_length 长度大小 字节
    size_t max_length() const override;

    /// marshal 序列化
    int32_t marshal(char* buf, size_t len) const override;

    /// unmarshal 反序列化
    int32_t unmarshal(const char* buf, size_t len) override;

    /// to_string 生成描述文字
    std::string to_string(void) const override;

    int64_t bid_price_; /// 买价
    int64_t ask_price_; /// 卖价
};

/// fld_qty_pair 数量对域
class fld_qty_pair : public field {
  public:
    fld_qty_pair();

    /// id 标识
    uint16_t id() const override;

    /// length 长度大小 字节
    size_t length() const override;

    /// max_length 长度大小 字节
    size_t max_length() const override;

    /// marshal 序列化
    int32_t marshal(char* buf, size_t len) const override;

    /// unmarshal 反序列化
    int32_t unmarshal(const char* buf, size_t len) override;

    /// to_string 生成描述文字
    std::string to_string(void) const override;

    uint32_t bid_qty_; /// 买量
    uint32_t ask_qty_; /// 卖量
};

/// fld_segment_price_qty 分价成交量域
class fld_segment_price_qty : public field {
  public:
    fld_segment_price_qty();

    /// id 标识
    uint16_t id() const override;

    /// length 长度大小 字节
    size_t length() const override;

    /// max_length 长度大小 字节
    size_t max_length() const override;

    /// marshal 序列化
    int32_t marshal(char* buf, size_t len) const override;

    /// unmarshal 反序列化
    int32_t unmarshal(const char* buf, size_t len) override;

    /// to_string 生成描述文字
    std::string to_string(void) const override;

    int64_t match_price_;      /// 价格
    uint32_t buy_open_qty_;    /// 买开数量
    uint32_t buy_offset_qty_;  /// 买平数量
    uint32_t sell_open_qty_;   /// 卖开数量
    uint32_t sell_offset_qty_; /// 卖平数量
};

/// fld_order_statistic_biz6 委托统计域
class fld_order_statistic_biz6 : public field {
  public:
    fld_order_statistic_biz6();

    /// id 标识
    uint16_t id() const override;

    /// length 长度大小 字节
    size_t length() const override;

    /// max_length 长度大小 字节
    size_t max_length() const override;

    /// marshal 序列化
    int32_t marshal(char* buf, size_t len) const override;

    /// unmarshal 反序列化
    int32_t unmarshal(const char* buf, size_t len) override;

    /// to_string 生成描述文字
    std::string to_string(void) const override;

    uint32_t total_buy_order_qty_;       /// 买委托总量
    uint32_t total_sell_order_qty_;      /// 卖委托总量
    int64_t weighted_average_bid_price_; /// 加权平均委买价格
    int64_t weighted_average_ask_price_; /// 加权平均委卖价格
};

/// fld_extend_quot_common 扩展合约行情头域
class fld_extend_quot_common : public field {
  public:
    fld_extend_quot_common();

    /// id 标识
    uint16_t id() const override;

    /// length 长度大小 字节
    size_t length() const override;

    /// max_length 长度大小 字节
    size_t max_length() const override;

    /// marshal 序列化
    int32_t marshal(char* buf, size_t len) const override;

    /// unmarshal 反序列化
    int32_t unmarshal(const char* buf, size_t len) override;

    /// to_string 生成描述文字
    std::string to_string(void) const override;

    uint32_t contract_no_;     /// 合约号
    uint64_t contract_seq_no_; /// 合约增量行情编号
    uint8_t mdg_no_;           /// 产品组号
    uint64_t seq_no_;          /// 行情消息的编号
    uint64_t send_time_;       /// 发送时间
};

/// fld_rsp_msg 响应域
class fld_rsp_msg : public field {
  public:
    fld_rsp_msg();

    /// id 标识
    uint16_t id() const override;

    /// length 长度大小 字节
    size_t length() const override;

    /// max_length 长度大小 字节
    size_t max_length() const override;

    /// marshal 序列化
    int32_t marshal(char* buf, size_t len) const override;

    /// unmarshal 反序列化
    int32_t unmarshal(const char* buf, size_t len) override;

    /// to_string 生成描述文字
    std::string to_string(void) const override;

    int32_t err_code_;    /// 错误码
    char time_stamp_[13]; /// 时间戳
};

/// fld_trader_login_req 登录请求域
class fld_trader_login_req : public field {
  public:
    fld_trader_login_req();

    /// id 标识
    uint16_t id() const override;

    /// length 长度大小 字节
    size_t length() const override;

    /// max_length 长度大小 字节
    size_t max_length() const override;

    /// marshal 序列化
    int32_t marshal(char* buf, size_t len) const override;

    /// unmarshal 反序列化
    int32_t unmarshal(const char* buf, size_t len) override;

    /// to_string 生成描述文字
    std::string to_string(void) const override;

    char trader_id_[17]; /// 席位号
    char pwd_[65];       /// 密码
};

/// fld_trader_login_rsp 登录应答域
class fld_trader_login_rsp : public field {
  public:
    fld_trader_login_rsp();

    /// id 标识
    uint16_t id() const override;

    /// length 长度大小 字节
    size_t length() const override;

    /// max_length 长度大小 字节
    size_t max_length() const override;

    /// marshal 序列化
    int32_t marshal(char* buf, size_t len) const override;

    /// unmarshal 反序列化
    int32_t unmarshal(const char* buf, size_t len) override;

    /// to_string 生成描述文字
    std::string to_string(void) const override;

    char member_id_[9];           /// 会员号
    char trader_id_[17];          /// 席位号
    uint32_t date_;               /// 交易核心的日期
    char time_[13];               /// 交易核心的时间
    uint8_t is_first_login_;      /// 是否首次登录
    uint8_t pwd_expire_reminder_; /// 密码是否到期提示
    int32_t days_overdue_;        /// 距离过期天数
    uint8_t last_login_flag_;     /// 上次登录是否成功
    uint32_t last_login_date_;    /// 上次登录日期
    char last_login_time_[13];    /// 上次登录时间
    char last_login_ip_[16];      /// 上次登录位置
    int32_t failed_times_;        /// 上次登录失败次数
};

/// fld_trader_id 席位号域
class fld_trader_id : public field {
  public:
    fld_trader_id();

    /// id 标识
    uint16_t id() const override;

    /// length 长度大小 字节
    size_t length() const override;

    /// max_length 长度大小 字节
    size_t max_length() const override;

    /// marshal 序列化
    int32_t marshal(char* buf, size_t len) const override;

    /// unmarshal 反序列化
    int32_t unmarshal(const char* buf, size_t len) override;

    /// to_string 生成描述文字
    std::string to_string(void) const override;

    char trader_id_[17]; /// 席位号
};

/// fld_flow_ctrl_warning 流控二级警告域
class fld_flow_ctrl_warning : public field {
  public:
    fld_flow_ctrl_warning();

    /// id 标识
    uint16_t id() const override;

    /// length 长度大小 字节
    size_t length() const override;

    /// max_length 长度大小 字节
    size_t max_length() const override;

    /// marshal 序列化
    int32_t marshal(char* buf, size_t len) const override;

    /// unmarshal 反序列化
    int32_t unmarshal(const char* buf, size_t len) override;

    /// to_string 生成描述文字
    std::string to_string(void) const override;

    char trader_id_[17];             /// 席位号
    char trigger_time_[13];          /// 触发时间
    uint32_t pkg_type_gid_;          /// pkg_type组号
    uint32_t instantaneous_pkg_cnt_; /// 瞬时流量值
};

/// fld_req_contract_basic_info 合约基本信息查询请求域
class fld_req_contract_basic_info : public field {
  public:
    fld_req_contract_basic_info();

    /// id 标识
    uint16_t id() const override;

    /// length 长度大小 字节
    size_t length() const override;

    /// max_length 长度大小 字节
    size_t max_length() const override;

    /// marshal 序列化
    int32_t marshal(char* buf, size_t len) const override;

    /// unmarshal 反序列化
    int32_t unmarshal(const char* buf, size_t len) override;

    /// to_string 生成描述文字
    std::string to_string(void) const override;

    char trader_id_[17];    /// 席位号
    char contract_id_[129]; /// 合约号
};

/// fld_contract_basic_info 合约基本信息域
class fld_contract_basic_info : public field {
  public:
    fld_contract_basic_info();

    /// id 标识
    uint16_t id() const override;

    /// length 长度大小 字节
    size_t length() const override;

    /// max_length 长度大小 字节
    size_t max_length() const override;

    /// marshal 序列化
    int32_t marshal(char* buf, size_t len) const override;

    /// unmarshal 反序列化
    int32_t unmarshal(const char* buf, size_t len) override;

    /// to_string 生成描述文字
    std::string to_string(void) const override;

    uint32_t contract_no_;          /// 合约号索引
    char contract_id_[129];         /// 合约号
    uint32_t trade_date_;           /// 交易日期
    uint8_t contract_type_;         /// 合约类型
    uint32_t init_open_interest_;   /// 初始持仓量
    int64_t limit_up_price_;        /// 涨停板
    int64_t limit_down_price_;      /// 跌停板
    int64_t last_settlement_price_; /// 上日结算价
    int64_t last_closing_price_;    /// 上日收盘价
    int64_t codec_price_;           /// 编码基准价
    int64_t tick_;                  /// 最小变动价位(元/吨)
    char contract_name_[61];        /// 合约名称
};

/// fld_contract_param 合约参数域
class fld_contract_param : public field {
  public:
    fld_contract_param();

    /// id 标识
    uint16_t id() const override;

    /// length 长度大小 字节
    size_t length() const override;

    /// max_length 长度大小 字节
    size_t max_length() const override;

    /// marshal 序列化
    int32_t marshal(char* buf, size_t len) const override;

    /// unmarshal 反序列化
    int32_t unmarshal(const char* buf, size_t len) override;

    /// to_string 生成描述文字
    std::string to_string(void) const override;

    uint32_t contract_no_;              /// 合约号
    uint16_t unit_;                     /// 交易单位
    int64_t tick_;                      /// 最小变动单位
    int64_t buy_spec_margin_;           /// 买投保证金
    int64_t sell_spec_margin_;          /// 卖投保证金
    int64_t buy_hedge_margin_;          /// 买保保证金
    int64_t sell_hedge_margin_;         /// 卖保保证金
    uint32_t contract_expiration_date_; /// 合约到期日
    uint32_t last_trade_date_;          /// 最后到期日
    uint32_t last_deliver_date_;        /// 最后交割日
};

/// fld_req_trade_plan 交易计划查询请求域
class fld_req_trade_plan : public field {
  public:
    fld_req_trade_plan();

    /// id 标识
    uint16_t id() const override;

    /// length 长度大小 字节
    size_t length() const override;

    /// max_length 长度大小 字节
    size_t max_length() const override;

    /// marshal 序列化
    int32_t marshal(char* buf, size_t len) const override;

    /// unmarshal 反序列化
    int32_t unmarshal(const char* buf, size_t len) override;

    /// to_string 生成描述文字
    std::string to_string(void) const override;

    char trader_id_[17]; /// 席位号
    char variety_id_[5]; /// 品种号
    uint8_t trade_type_; /// 交易类型
};

/// fld_trade_plan 交易计划域
class fld_trade_plan : public field {
  public:
    fld_trade_plan();

    /// id 标识
    uint16_t id() const override;

    /// length 长度大小 字节
    size_t length() const override;

    /// max_length 长度大小 字节
    size_t max_length() const override;

    /// marshal 序列化
    int32_t marshal(char* buf, size_t len) const override;

    /// unmarshal 反序列化
    int32_t unmarshal(const char* buf, size_t len) override;

    /// to_string 生成描述文字
    std::string to_string(void) const override;

    uint32_t trade_date_;         /// 交易日期
    char variety_id_[5];          /// 品种号
    uint8_t trade_type_;          /// 交易类型
    uint8_t state_;               /// 合约状态
    char chg_time_[13];           /// 切换时间
    uint32_t trading_session_no_; /// 交易小节编号
    uint32_t next_session_no_;    /// 下一交易小节编号
};

/// fld_qry_his_quot 查询历史行情域
class fld_qry_his_quot : public field {
  public:
    fld_qry_his_quot();

    /// id 标识
    uint16_t id() const override;

    /// length 长度大小 字节
    size_t length() const override;

    /// max_length 长度大小 字节
    size_t max_length() const override;

    /// marshal 序列化
    int32_t marshal(char* buf, size_t len) const override;

    /// unmarshal 反序列化
    int32_t unmarshal(const char* buf, size_t len) override;

    /// to_string 生成描述文字
    std::string to_string(void) const override;

    char trader_id_[17];    /// 席位编号
    uint8_t mdg_no_;        /// 行情品种分组号
    uint64_t start_seq_no_; /// 开始序列号
    uint64_t end_seq_no_;   /// 结束序列号
};

/// fld_qry_quot_snap 查询行情快照域
class fld_qry_quot_snap : public field {
  public:
    fld_qry_quot_snap();

    /// id 标识
    uint16_t id() const override;

    /// length 长度大小 字节
    size_t length() const override;

    /// max_length 长度大小 字节
    size_t max_length() const override;

    /// marshal 序列化
    int32_t marshal(char* buf, size_t len) const override;

    /// unmarshal 反序列化
    int32_t unmarshal(const char* buf, size_t len) override;

    /// to_string 生成描述文字
    std::string to_string(void) const override;

    char trader_id_[17];   /// 席位编号
    uint8_t mdg_no_;       /// 行情品种分组号
    uint32_t contract_no_; /// 合约号索引
};

/// fld_max_no 最大序号域
class fld_max_no : public field {
  public:
    fld_max_no();

    /// id 标识
    uint16_t id() const override;

    /// length 长度大小 字节
    size_t length() const override;

    /// max_length 长度大小 字节
    size_t max_length() const override;

    /// marshal 序列化
    int32_t marshal(char* buf, size_t len) const override;

    /// unmarshal 反序列化
    int32_t unmarshal(const char* buf, size_t len) override;

    /// to_string 生成描述文字
    std::string to_string(void) const override;

    uint8_t mdg_no_;      /// 行情品种分组号
    uint64_t max_seq_no_; /// 最大序号
};

// ------
// fields_t

/// fields_t 域容器，key为fid
using fields_t = std::multimap<uint16_t, field*>;

/// field_list_t 域容器
using field_list_t = std::vector<field*>;

/// id 标识
uint16_t id(const fields_t& objs);

/// length 大小，字节
size_t length(const fields_t& objs);

/// max_length 大小，字节
size_t max_length(const fields_t& objs);

/// init 初始化
void init(fields_t& objs, uint16_t id);

/// marshal 序列化
int32_t marshal(const fields_t& objs, char* buf, size_t len);

/// unmarshal 反序列化
int32_t unmarshal(fields_t& objs, const char* buf, size_t len);

/// to_string 生成描述文字
std::string to_string(const fields_t& objs);

#pragma pack(pop)

// ------
// data

class data {
  public:
    using iterator = fields_t::iterator;
    using citerator = fields_t::const_iterator;
    using value_type = fields_t::value_type;
    using second_type = fields_t::value_type::second_type;

    data();

    ~data();

    // 禁止使用
    data(const data& cpy) = delete;
    data(const data&& cpy) = delete;
    void operator=(const data& rhs) {}
    void operator=(const data&& rhs) {}

    /// id 标识 只为接口兼容
    uint16_t id() const;

    /// length 大小 字节
    size_t length() const;

    /// init 初始化
    void init(uint16_t id);

    /// init 初始化
    void init(uint16_t id, bool raw);

    /// marshal 序列化
    int32_t marshal(char* buf, size_t len) const;

    /// unmarshal 反序列化
    int32_t unmarshal(const char* buf, size_t len);

    /// unmarshal 反序列化
    int32_t unmarshal();

    /// to_string 生成描述文字
    std::string to_string() const;

    /// find 查询某个fid
    iterator find(uint16_t fid);

    /// begin 容器开始迭代器
    iterator begin();

    /// end 容器结束迭代器
    iterator end();

    /// size 元素个数
    size_t size();

    /// empty 是否为空
    bool empty();

    /// count 统计以fid为key的元素个数，0 为不存在， 1为存在， 不可能大于1
    size_t count(uint16_t fid);

    /// at 找到fid对应的域，多个时只返回第一个
    second_type at(uint16_t key);

    /// [] 下标定位，多个时只返回第一个
    second_type operator[](uint16_t key);

    /// equal_range 返回key对应所有元素
    std::pair<iterator, iterator> equal_range(uint16_t key);

    /// erase 删除一个元素
    size_t erase(uint16_t fid);

    /// clear 清空
    void clear();

    /// emplace 插入元素
    void emplace(value_type&& val);

    /// emplace 插入元素
    void emplace(second_type val);

    /// insert 插入元素
    void insert(iterator it, value_type& val);

    /// insert 插入元素
    void insert(iterator it, const value_type& val);

    /// insert 插入元素
    void insert(value_type& val);

    /// insert 插入元素
    void insert(const value_type& val);

    /// insert 插入元素
    void insert(second_type val);

    /// cbegin const
    citerator cbegin();

    /// end const
    citerator cend();

    /// reserve 预分配大小
    void reserve(size_t size);

    fields_t fields_; // 业务数据
    size_t raw_len_ = 0;
    char* raw_data_ =
        nullptr; // 通道接收进行缓存时、组包和恢复等，不解析data部分
    bool raw_ = false; // 数据部分不反序列化
    uint32_t idx_ = 0; // 排序标识，单个报文里包含多个相同域时用于排序

  private:
    uint16_t pkg_type_ = 0;
};

// ------------
// body

class body {
  public:
    enum {
        unknow = 0,
        /// nty 通知业务属性（业务头）
        nty,
        /// qry 查询业务属性（业务头）
        qry
    };
    enum { single = 0, first = 1, mid = 2, last = 3 };

    body();

    explicit body(uint16_t tid);

    ~body();

    /// id 标识
    uint16_t id() const;

    /// length 大小 字节
    size_t length() const;

    /// init 初始化
    void init(uint16_t id);

    /// init 初始化
    void init(const dmdp_header& head, bool raw);

    /// init 初始化
    void init(const dmqp_header& head, bool raw);

    /// init 初始化
    void init(uint16_t id, bool raw);

    /// marshal 序列化
    int32_t marshal(char* buf, size_t len) const;

    /// unmarshal 反序列化
    int32_t unmarshal(const char* buf, size_t len);

    /// unmarshal 反序列化
    int32_t unmarshal();

    /// to_string 生成描述文字
    std::string to_string() const;

    /// is_relative 是否相关域
    bool is_relative(uint16_t fid);

    /// has_all 是否包含所有域
    bool has_all();

    uint16_t pkg_type_;     /// 报文类型
    uint8_t version_;       /// 版本号
    uint8_t flag_;          /// 消息结束标示符
    uint8_t mdg_no_;        /// 产品组号
    uint64_t start_seq_no_; /// 第一个行情消息的编号
    uint8_t seq_num_;       /// 扩展字段
    uint64_t send_time_;    /// 发送时间
    uint32_t request_no_;   /// 请求号

    // 设置报文类型
    void set_pkg_type(uint16_t pkg_type);
    // 获取报文类型
    uint16_t pkg_type() const;

    // 设置版本号
    void set_version(uint8_t version);
    // 获取版本号
    uint8_t version() const;

    // 设置消息结束标示符
    void set_flag(uint8_t flag);
    // 获取消息结束标示符
    uint8_t flag() const;

    // 设置产品组号
    void set_mdg_no(uint8_t mdg_no);
    // 获取产品组号
    uint8_t mdg_no() const;

    // 设置第一个行情消息的编号
    void set_start_seq_no(uint64_t start_seq_no);
    // 获取第一个行情消息的编号
    uint64_t start_seq_no() const;

    // 设置扩展字段
    void set_seq_num(uint8_t seq_num);
    // 获取扩展字段
    uint8_t seq_num() const;

    // 设置发送时间
    void set_send_time(uint64_t send_time);
    // 获取发送时间
    uint64_t send_time() const;

    // 设置请求号
    void set_request_no(uint32_t request_no);
    // 获取请求号
    uint32_t request_no() const;

    static int type(uint16_t tid);

    data data_; // 业务数据
};

// -----------
// handler

using udp_handler_t = std::function<void(
    int channel_id, const dmdp_header& header, body* body_ptr)>;
using udp_handler_map_t = std::unordered_map<uint32_t, udp_handler_t>;
using tcp_handler_t = std::function<void(
    int channel_id, const dmqp_header& header, body* body_ptr)>;
using tcp_handler_map_t = std::unordered_map<uint32_t, tcp_handler_t>;
using filter_t =
    std::function<bool(int channel_id, const header& header, body* body_ptr)>;
using filter_map_t = std::unordered_map<uint32_t, filter_t>;
using filters_t = std::vector<filter_t>;

// -----------
// filters

bool filter_udp_pkg_max_size(char* buf, size_t len, const std::string& addr);

bool filter_udp_pkg_min_size(char* buf, size_t len, const std::string& addr);

bool filter_tcp_pkg_max_size(char* buf, size_t len, const std::string& addr);

bool filter_tcp_pkg_min_size(char* buf, size_t len, const std::string& addr);

// ======
} // namespace pkg
} // namespace mdp
