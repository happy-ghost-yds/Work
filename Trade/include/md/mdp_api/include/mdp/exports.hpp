/**
 * Copyright (C) 2021，大连商品交易所 大连飞创信息技术有限公司
 *
 * @brief 定义导出宏
 *
 * 定义导出宏。
 *
 */

#ifndef __MDP_API_EXPORTS_HPP__
#define __MDP_API_EXPORTS_HPP__

#ifdef WIN32
#ifdef MDP_EXPORTS
#define MDP_API __declspec(dllexport)
#elif defined(MDP_TEST)
#define MDP_API
#else
#define MDP_API __declspec(dllimport)
#endif
#else
#define MDP_API __attribute__((visibility("default")))
#endif

#endif // __MDP_API_EXPORTS_HPP__