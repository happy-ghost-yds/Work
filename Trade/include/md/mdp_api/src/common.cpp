/**
 * Copyright (C) 2021，大连商品交易所 大连飞创信息技术有限公司
 *
 * @brief 部分通用定义
 *
 */

#include "mdp/common.hpp"

#ifdef WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <time.h>
#include <windows.h>
#else
#include <sys/time.h>
#include <unistd.h>
#endif

namespace mdp {
// ======

namespace datetime {

// format 将时间戳格式化
void format(char* buf, size_t len, int fmt, uint64_t ns) {
    struct tm stm = {0};
    uint64_t stm_s = ns / 1000000000llu; // 转化为秒
#ifdef WIN32
    localtime_s(&stm, (const time_t*)&stm_s);
#else
    localtime_r((const time_t*)&stm_s, &stm);
#endif
    uint32_t ms = static_cast<uint32_t>(ns % 1000000000llu / 1000000llu);
    switch (fmt) {
    case datetime::yyyymmdd: {
        snprintf(buf, len, "%04u%02u%02u", stm.tm_year + 1900, stm.tm_mon + 1,
                 stm.tm_mday);
        return;
    }
    case datetime::yyyymmdd_hhmmss: {
        snprintf(buf, len, "%04u%02u%02u %02u%02u%02u", stm.tm_year + 1900,
                 stm.tm_mon + 1, stm.tm_mday, stm.tm_hour, stm.tm_min,
                 stm.tm_sec);
        return;
    }
    case datetime::yyyymmdd_hhmmss_nnn: {
        snprintf(buf, len, "%04u%02u%02u %02u%02u%02u.%03u", stm.tm_year + 1900,
                 stm.tm_mon + 1, stm.tm_mday, stm.tm_hour, stm.tm_min,
                 stm.tm_sec, ms);
        return;
    }
    case datetime::yyyy_mm_dd: {
        snprintf(buf, len, "%04u-%02u-%02u", stm.tm_year + 1900, stm.tm_mon + 1,
                 stm.tm_mday);
        return;
    }
    case datetime::yyyy_mm_dd_hh_mm_ss: {
        snprintf(buf, len, "%04u-%02u-%02u %02u:%02u:%02u", stm.tm_year + 1900,
                 stm.tm_mon + 1, stm.tm_mday, stm.tm_hour, stm.tm_min,
                 stm.tm_sec);
        return;
    }
    case datetime::yyyy_mm_dd_hh_mm_ss_nnn: {
        snprintf(buf, len, "%04u-%02u-%02u %02u:%02u:%02u.%03u",
                 stm.tm_year + 1900, stm.tm_mon + 1, stm.tm_mday, stm.tm_hour,
                 stm.tm_min, stm.tm_sec, ms);
        return;
    }
    case datetime::hh_mm_ss: {
        snprintf(buf, len, "%02u:%02u:%02u", stm.tm_hour, stm.tm_min,
                 stm.tm_sec);
        return;
    }
    case datetime::hh_mm_ss_nnn: {
        snprintf(buf, len, "%02u:%02u:%02u.%03u", stm.tm_hour, stm.tm_min,
                 stm.tm_sec, ms);
        return;
    }
    default: {
        snprintf(buf, len, "%04u-%02u-%02u", stm.tm_year + 1900, stm.tm_mon + 1,
                 stm.tm_mday);
        return;
    }
    }
}

// date 将时间戳转换为日期（整数形式）
uint32_t date(uint64_t ns) {
    struct tm stm = {0};
    uint64_t stm_s = ns / 1000000000llu; // 转化为秒
#ifdef WIN32
    localtime_s(&stm, (const time_t*)&stm_s);
#else
    localtime_r((const time_t*)&stm_s, &stm);
#endif
    return ((stm.tm_year + 1900) * 10000) + ((stm.tm_mon + 1) * 100) +
           stm.tm_mday;
}

} // namespace datetime

// ======
} // namespace mdp