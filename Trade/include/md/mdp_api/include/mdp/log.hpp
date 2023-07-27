/**
 * Copyright (C) 2021，大连商品交易所 大连飞创信息技术有限公司
 *
 * @brief 定义日志宏
 *
 * 封装spdlog并提供不同级别的日志记录宏。
 *
 */

#ifndef __MDP_BASE_LOG_HPP__
#define __MDP_BASE_LOG_HPP__

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include <chrono>
#include <iomanip>
#include <sstream>

#define LOG_FATAL(fmt, ...)                                                    \
    do {                                                                       \
        if (nullptr != spdlog::get("log")) {                                   \
            spdlog::get("log")->critical(fmt, ##__VA_ARGS__);                  \
        }                                                                      \
    } while (false)
#define LOG_ERROR(fmt, ...)                                                    \
    do {                                                                       \
        if (nullptr != spdlog::get("log")) {                                   \
            spdlog::get("log")->error(fmt, ##__VA_ARGS__);                     \
        }                                                                      \
    } while (false)
#define LOG_WARN(fmt, ...)                                                     \
    do {                                                                       \
        if (nullptr != spdlog::get("log")) {                                   \
            spdlog::get("log")->warn(fmt, ##__VA_ARGS__);                      \
        }                                                                      \
    } while (false)
#define LOG_INFO(fmt, ...)                                                     \
    do {                                                                       \
        if (nullptr != spdlog::get("log")) {                                   \
            spdlog::get("log")->info(fmt, ##__VA_ARGS__);                      \
        }                                                                      \
    } while (false)
#define LOG_DEBUG(fmt, ...)                                                    \
    do {                                                                       \
        if (nullptr != spdlog::get("log")) {                                   \
            spdlog::get("log")->debug(fmt, ##__VA_ARGS__);                     \
        }                                                                      \
    } while (false)

static inline void init_log(const std::string& prefix, uint8_t log_level) {
    try {
        std::stringstream ss;
        auto t = std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now());
        auto dt = std::localtime(&t);
        ss << std::setfill('0') << std::setw(4) << dt->tm_year + 1900
           << std::setfill('0') << std::setw(2) << dt->tm_mon << std::setw(2)
           << dt->tm_mday << "_" << std::setw(2) << dt->tm_hour << std::setw(2)
           << dt->tm_min << std::setw(2) << dt->tm_sec;
        spdlog::set_pattern("[%Y%m%d %H:%M:%S.%e][%L][T%t] %v");
        auto logger =
            spdlog::basic_logger_mt("log", "./" + prefix + ss.str() + ".log");
        logger->flush_on(spdlog::level::debug);
    } catch (const spdlog::spdlog_ex& ex) {
        return;
    }

    spdlog::set_level(spdlog::level::level_enum(log_level));
}

#define LOG_INIT(log_level, prefix)                                            \
    do {                                                                       \
        init_log(prefix, log_level);                                           \
    } while (false)

#endif // __MDP_BASE_LOG_HPP__
