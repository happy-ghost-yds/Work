/**
 * Copyright (C) 2021，大连商品交易所 大连飞创信息技术有限公司
 *
 * @brief 事件引擎
 *
 * 封装libuv，启动新线程执行libuv循环
 */

#ifndef __MDP_ENGINE_HPP__
#define __MDP_ENGINE_HPP__

#include <memory>
#include <mutex>

namespace mdp {
// ======

/// engine 引擎
class engine {
  public:
    ~engine();

    static engine& ins();

    /// start 开始运行
    void start();

    /// join 阻塞处理网络事件，开启事件循环
    int join();

    /// close 关闭事件循环
    int close();

    // run 运行函数
    static void run(engine* ptr);

  private:
    engine();
    engine(const engine& cpy) = delete;
    void operator=(const engine& rhs) = delete;

    static std::unique_ptr<engine> ins_;
    volatile bool running = false;
    volatile bool stoped = false;
};

// ======
} // namespace mdp
#endif // __MDP_ENGINE_HPP__