#pragma once
#include <cstdint>

constexpr auto ContractIdLen = 129;

// 与价格/金额有关的字段除以10000后的单位为"元"
struct MarketDataSnapshot {
    int64_t LastPrice = 0;   // 最新价
    int64_t TotalQty = 0;     // 总成交量
    int64_t Turnover = 0;    // 总成交额
    int64_t OpenInterest = 0;  // 持仓
    int64_t AvgPrice = 0;     // 当日成交均价
    int64_t BidPrice = 0;  // 最高买
    int64_t AskPrice = 0;  // 最低卖
    int32_t LastQty = 0;      // 最新成交量
    int32_t BidVolume = 0;  // 买一量
    int32_t AskVolume = 0;  // 卖一量
    int32_t BidImplyVolume = 0;      // 买推导量
    int32_t AskImplyVolume = 0;      // 卖推导量
    uint32_t ContractSeqNo = 0;  // 最后一次序号
    uint64_t UpdTime = 0; // 发送时间(来自报文 UTC纳秒时间戳)
    int64_t SettlePrice = 0;     // 今日结算价
    int64_t OpeningPrice = 0;    // 开盘价
    int64_t ClosingPrice = 0;    // 收盘价
    int64_t DailyHighPrice = 0;  // 当日最高成交价
    int64_t DailyLowPrice = 0;   // 当日最低成交价
};

struct ContractBaseInfo {
    uint32_t ContractNo;     // 合约序号
    uint32_t TradeDate;      // 交易日
    int64_t LimitUpPrice;    // 涨停价
    int64_t LimitDownPrice;  // 跌停价
    int64_t CodecPrice;      // 编码基准价
    int64_t Tick;            // 最小变动价位
    uint8_t ContractType;    // 合约类型
    uint32_t InitOpenInterest;     // 初始持仓量
    int64_t LastSettlementPrice;   // 前一日结算价
    int64_t LastClosingPrice;      // 前一日收盘价
    char ContractID[ContractIdLen]; // 合约代码
};

struct DCEL1MD{
    char ContractID[ContractIdLen];
    MarketDataSnapshot mdSnapshot;
};