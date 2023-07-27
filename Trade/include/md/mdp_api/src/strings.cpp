/**
 * Copyright (C) 2021，大连商品交易所 大连飞创信息技术有限公司
 *
 * @brief 字符串处理函数。
 *
 */

#include "mdp/strings.hpp"

#include <cstdarg>
#include <string.h>

#ifndef WIN32
#define stricmp strcasecmp // for linux
#else
#pragma warning(disable : 4996) // win32 去除警告
#endif

namespace mdp {
namespace strings {
// ======

bool starts_with(const char* sz, const char* prefix) {
    return strncmp(sz, prefix, strlen(prefix)) == 0;
}

bool ends_with(const char* sz, const char* suffix) {
    size_t sz_length = strlen(sz);
    size_t suffix_length = strlen(suffix);

    if (suffix_length > sz_length) {
        return false;
    }

    const char* start = sz + (sz_length - suffix_length);
    return strncmp(start, suffix, suffix_length) == 0;
}

std::string trim(const std::string& str, const char* trim) {
    std::string::size_type first = str.find_first_not_of(trim);
    std::string::size_type last = str.find_last_not_of(trim);

    if (first == std::string::npos || last == std::string::npos) {
        return std::string("");
    }

    return str.substr(first, last - first + 1);
}

bool contains(const std::string& str, const char* sub) {
    return str.find(sub) != std::string::npos;
}

bool equal_fold(const char* left, const char* right) {
    return stricmp(left, right) == 0;
}

void split(std::vector<std::string>& dst, const std::string& str,
           const std::string& d) {
    if ("" == str) {
        dst.emplace_back("");
        return;
    }
    std::string::size_type pos1, pos2;
    size_t len = str.length();
    pos2 = str.find(d);
    pos1 = 0;
    while (std::string::npos != pos2) {
        dst.emplace_back(str.substr(pos1, pos2 - pos1));
        pos1 = pos2 + d.size();
        pos2 = str.find(d, pos1);
    }
    if (pos1 != len) {
        dst.emplace_back(str.substr(pos1));
    } else {
        dst.emplace_back("");
    }
}

std::string format(const char* fmt, ...) {
    char buf[2048];              // note: 最长 2048 个字节
    memset(buf, 0, sizeof(buf)); // ？
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    return std::string(buf); // move ?
}

// ======
} // namespace strings
} // namespace mdp