/**
 * Copyright (C) 2021，大连商品交易所 大连飞创信息技术有限公司
 *
 * @brief 定义错误码
 *
 * 定义错误码。
 *
 */

#ifndef __MDP_API_ERRORS_HPP__
#define __MDP_API_ERRORS_HPP__

#include <memory>
#include <mutex>

#include "mdp/exports.hpp"

namespace mdp {
// ======

/// errors 错误码
class MDP_API errors {
  public:
    enum {
        // inner
        ok = 0,
        failed = 1,
        timeout = 2,
        qrying_quot = 3,
        qrying_snap = 4,
        recovering = 5,

        // 1000 - 9999 兼容pkg解析
        pkg_err = 1000
        // 11000
        // 12000
        // 13000
    };

    /// msg 错误消息
    static const char* msg(int code);

  private:
    errors();
};

/// net_errors 网络错误码
class MDP_API net_errors {
  public:
    enum {
        // inner
        ok = 0,
        failed = -1
    };

    /// msg 错误消息
    static const char* msg(int code);

  private:
    net_errors();
};
// ======
} // namespace mdp
#endif // __MDP_API_ERRORS_HPP__
