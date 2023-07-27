/**
 * Copyright (C) 2021，大连商品交易所 大连飞创信息技术有限公司
 *
 * @brief 部分通用定义
 *
 */

#ifndef __MDP_API_COMMON_HPP__
#define __MDP_API_COMMON_HPP__

#include <deque>
#include <functional>
#include <list>
#include <map>
#include <unordered_map>

#include "mdp/pkg_defines.hpp"
#include "mdp/types.hpp"

// 网络通道
#define CHN_TCP 1
#define CHN_UDP_1 2
#define CHN_UDP_2 3
#define CHN_UDP_CACHE 4

namespace mdp {
// ======

// types
// mbl_cache 深度行情缓存 unordered_map<合约号, map<价位, 档位信息>>
using mbl_cache_t =
    std::unordered_map<uint32_t, std::map<int64_t, mdp_mbl_level>>;

// handler
using handler_connected_t =
    std::function<void(int channel_id, int code, const std::string& msg)>;
using handler_disconnected_t =
    std::function<void(int channel_id, int code, const std::string& msg)>;

// 时间格式
namespace datetime {
enum {
    yyyymmdd = 0,            // yyyymmdd
    yyyy_mm_dd,              // yyyy-mm-dd
    hh_mm_ss,                // hh:mm:ss
    hh_mm_ss_nnn,            // hh:mm:ss.nnn
    yyyy_mm_dd_hh_mm_ss,     // yyyy-mm-dd hh:mm:ss
    yyyy_mm_dd_hh_mm_ss_nnn, // yyyy-mm-dd hh:mm:ss.nnn
    yyyymmdd_hhmmss,         // yyyymmdd hhmmss
    yyyymmdd_hhmmss_nnn      // yyyymmdd hhmmss.nnn
};
// format 从纳秒时间格式化出指定格式时间
void format(char* buf, size_t len, int fmt, uint64_t ns);

// date 取出日期 yyyymmdd 的整数形式
uint32_t date(uint64_t ns);
} // namespace datetime

// ======
} // namespace mdp
#endif // __MDP_API_COMMON_HPP__