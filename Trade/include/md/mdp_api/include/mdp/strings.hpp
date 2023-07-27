/**
 * Copyright (C) 2021，大连商品交易所 大连飞创信息技术有限公司
 *
 * @brief 字符串处理函数。
 *
 */

#ifndef __MDP_BASE_STRINGS_HPP__
#define __MDP_BASE_STRINGS_HPP__

#include <string>
#include <vector>

namespace mdp {
namespace strings {
// ======

/// starts_with 是否以指定字符串开始
/// @param sz string 需要判断的字符串
/// @param prefix string 前缀
/// @return bool false 不以该前缀开始，true 以该前缀开始
bool starts_with(const char* sz, const char* prefix);

/// ends_with 是否以指定字符串结束
/// @param sz string 需要判断的字符串
/// @param suffix string 后缀
/// @return bool false 不以该后缀结束，true 以该后缀结束
bool ends_with(const char* sz, const char* suffix);

/// trim 去除头和尾部的空白符或其他字符
/// @param str string 需要处理的字符串
/// @param trim string 需要消除的字符串
/// @return return string 处理完成的字符串
std::string trim(const std::string& str, const char* trim = " \t\r\n");

/// contains 是否存在指定字符串
/// @param sz string 需要判断的字符串
/// @param sub string 子串
/// @return bool false 不包含该字符串，true 包含该字符串。
bool contains(const char* sz, const char* sub);

/// equal_fold 比较是否相等，同时忽略大小写
/// @param left string 字符串
/// @param right string 字符串
/// @return bool false 不相等，true 相等
bool equal_fold(const char* left, const char* right);

/// split 切分字符串
/// @param dst std::vector<std::string> 切分结果；
/// @param sz string 需要切分的字符串；
/// @param d string 分隔符；
void split(std::vector<std::string>& dst, const std::string& str,
           const std::string& d = " ");

/// format 格式化字符串
/// @param fmt string 格式
/// @param ... 变长参数
/// @return string 格式化后的字符串
std::string format(const char* fmt, ...);

// ======
} // namespace strings
} // namespace mdp
#endif // __MDP_BASE_STRINGS_HPP__