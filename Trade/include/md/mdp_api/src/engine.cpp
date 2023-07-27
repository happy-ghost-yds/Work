/**
 * Copyright (C) 2021，大连商品交易所 大连飞创信息技术有限公司
 *
 * @brief 事件引擎
 *
 * 封装libuv，启动新线程执行libuv循环
 */

#include "mdp/engine.hpp"

#include "mdp/log.hpp"
#include "mdp/net.hpp"

namespace mdp {
// ======

std::unique_ptr<engine> engine::ins_;

engine::engine() {}

engine::~engine() {
    stoped = true;
    close();
}

engine& engine::ins() {
    static std::once_flag flag;
    std::call_once(flag, [&] {
        ins_.reset(new engine());
        ins_->start();
    });
    return *ins_;
}

void engine::start() {
    if (running) {
        return;
    }
    std::thread thd(&engine::run, this);
    thd.detach();
}

int engine::join() {
    LOG_DEBUG("uv_run server");
    int ret = uv_run(uv_default_loop(), UV_RUN_DEFAULT);
    if (ret != 0) {
        LOG_DEBUG("uv_run ret={0}", ret);
        return ret;
    }
    LOG_DEBUG("uv_run server ok");
    return 0;
}

int engine::close() {
    int ret = uv_loop_close(uv_default_loop());
    if (ret != 0) {
        return ret;
    }

    // free(uv_default_loop());
    return 0;
}

void engine::run(engine* ptr) {
    LOG_DEBUG("uv_run server");
    ptr->running = true;
    while (!ptr->stoped) {
        int ret = uv_run(uv_default_loop(), UV_RUN_DEFAULT);
        if (ret != 0) {
            LOG_DEBUG("uv_run ret={0}", ret);
            return;
        }
    }
    ptr->running = false;
    LOG_DEBUG("uv_run server ok");
}

// ======
} // namespace mdp