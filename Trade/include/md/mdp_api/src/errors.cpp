/**
 * Copyright (C) 2021，大连商品交易所 大连飞创信息技术有限公司
 *
 * @brief 定义错误码
 *
 * 定义错误码。
 *
 */

#include "mdp/errors.hpp"

namespace mdp {
// ======

errors::errors() {}

const char* errors::msg(int code) {
    switch (code) {
    case ok:
        return "ok";
    case failed:
        return "failed";
    case timeout:
        return "timeout";
    case qrying_quot:
        return "qrying quot";
    case qrying_snap:
        return "qrying snap";
    case recovering:
        return "recovering";
    case pkg_err:
        return "pkg error";
    default:
        return "unknow";
    };
}

// ======
} // namespace mdp