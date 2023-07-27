/**
 * Copyright (C) 2021，大连商品交易所 大连飞创信息技术有限公司
 *
 * @brief 计时器和定时器
 *
 * 简单实现的计时器和定时器，提供计时或定时功能。
 *
 */

#include "mdp/timers.hpp"

namespace mdp {
namespace timers {
// ======

stop_watch::stop_watch()
    : begin_time_(std::chrono::high_resolution_clock::now()) {}

stop_watch::~stop_watch() {}

void stop_watch::reset(void) {
    begin_time_ = std::chrono::high_resolution_clock::now();
}

uint64_t stop_watch::elapsed(void) const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::high_resolution_clock::now() - begin_time_)
        .count();
}

uint64_t stop_watch::elapsed_micro(void) const {
    return std::chrono::duration_cast<std::chrono::microseconds>(
               std::chrono::high_resolution_clock::now() - begin_time_)
        .count();
}

uint64_t stop_watch::elapsed_nano(void) const {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
               std::chrono::high_resolution_clock::now() - begin_time_)
        .count();
}

uint64_t stop_watch::elapsed_seconds(void) const {
    return std::chrono::duration_cast<std::chrono::seconds>(
               std::chrono::high_resolution_clock::now() - begin_time_)
        .count();
}

uint64_t stop_watch::elapsed_minutes(void) const {
    return std::chrono::duration_cast<std::chrono::minutes>(
               std::chrono::high_resolution_clock::now() - begin_time_)
        .count();
}

uint64_t stop_watch::elapsed_hours(void) const {
    return std::chrono::duration_cast<std::chrono::hours>(
               std::chrono::high_resolution_clock::now() - begin_time_)
        .count();
}

// ------
// timer

timer::timer() { init(); }

timer::~timer() {
    stop();
    uv_close((uv_handle_t*)&timer_, [](uv_handle_t* handle) {});
}

int timer::init() {
    int ret = uv_timer_init(uv_default_loop(), &timer_);
    if (ret != 0) {
        return ret;
    }
    timer_.data = this;
    return 0;
}

int timer::start(func_t func, uint64_t interval, uint64_t repeat) {
    if (1 == uv_is_active((uv_handle_t*)&timer_)) {
        return 0;
    }
    func_ = func;
    int ret = uv_timer_start(&timer_,
                             [](uv_timer_t* handle) {
                                 auto obj_ptr =
                                     static_cast<timer*>(handle->data);
                                 obj_ptr->on_timer();
                             },
                             interval, repeat);
    if (ret != 0) {
        return ret;
    }
    return 0;
}

int timer::stop() {
    if (0 == uv_is_active((uv_handle_t*)&timer_)) {
        return 0;
    }
    int ret = uv_timer_stop(&timer_);
    if (ret != 0) {
        return ret;
    }
    return 0;
}

void timer::on_timer() {
    if (nullptr == func_) {
        return;
    }
    func_();
}

// ======
} // namespace timers
} // namespace mdp
