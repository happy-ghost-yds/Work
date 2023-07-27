/**
 * Copyright (C) 2021，大连商品交易所 大连飞创信息技术有限公司
 *
 * @brief 版本号定义。
 *
 */

#ifndef __MDP_API_VERSION_HPP__
#define __MDP_API_VERSION_HPP__

#include "mdp/exports.hpp"
#include <stdint.h>

namespace mdp {
// ======

// version 版本信息
class MDP_API version {
  public:
    /// code 版本号
    /// @brief 主版本号.次版本号.修正版本号
    /// @return string 版本号
    static const char* code(void);

    /// feature 特征/功能
    /// @brief 一般是代码分支名称，如 dev
    /// @return string 名称
    static const char* feature(void);

    /// desc 版本说明
    /// @return string 说明信息
    static const char* desc(void);

    /// get_pkg_version 查看协议版本号
    static uint16_t get_pkg_version();

    /// set_pkg_version 设置协议版本号，方便测试
    static void set_pkg_version(uint16_t v);
};

// ======
} // namespace mdp
#endif // __MDP_API_VERSION_HPP__