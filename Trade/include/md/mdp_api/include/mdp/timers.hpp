/**
 * Copyright (C) 2021，大连商品交易所 大连飞创信息技术有限公司
 *
 * @brief 计时器和定时器
 *
 * 简单实现的计时器和定时器，提供计时或定时功能。
 *
 */

#ifndef __MDP_BASE_TIMERS_HPP__
#define __MDP_BASE_TIMERS_HPP__

#include <chrono>
#include <functional>
#include <string>

#include "uv.h"

namespace mdp {
namespace timers {
// ======

/// stop_watch 计时器
/// @brief 使用C++11的high_resolution_clock实现，是稳定的高精度时钟。
/// 计时单调递增，不受修改系统时间的影响。
///
/// example:
///
/// mdp::timers::stop_watch watch;
/// func_call();
/// auto span_1 = watch.elapsed();
/// watch.reset();
/// func_call();
/// auto span_2 = watch.elapsed();
/// auto total = span_1 + span_2;
///
class stop_watch {
  public:
    stop_watch(void);
    ~stop_watch(void);

    /// reset 重置
    void reset(void);

    /// elapsed 毫秒 ms
    uint64_t elapsed(void) const;

    /// elapsed_micro 微秒
    uint64_t elapsed_micro(void) const;

    /// elapsed_nano 纳秒
    uint64_t elapsed_nano(void) const;

    /// elapsed_seconds 秒
    uint64_t elapsed_seconds(void) const;

    /// elapsed_minutes 分钟
    uint64_t elapsed_minutes(void) const;

    /// elapsed_hours 小时
    uint64_t elapsed_hours(void) const;

  private:
    std::chrono::time_point<std::chrono::high_resolution_clock>
        begin_time_; // 开始时间
};

/// timer 定时器
class timer {
  public:
    /// 回调函数类型
    using func_t = std::function<void(void)>;
    static const uint64_t infinite = UINT64_MAX;

    timer();
    ~timer();

    /// 初始化定时器
    int init();
    /// 启动定时器
    int start(func_t func, uint64_t interval, uint64_t repeat);
    /// 停止定时器
    int stop();

    /// uv定时器回调
    void on_timer();

  private:
    uv_timer_t timer_;
    func_t func_ = nullptr;
};

// ======
} // namespace timers
} // namespace mdp
#endif // __MDP_BASE_TIMERS_HPP__