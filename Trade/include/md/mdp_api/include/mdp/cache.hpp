/**
 * Copyright (C) 2021，大连商品交易所 大连飞创信息技术有限公司
 *
 * @brief 缓存类
 *
 * 用于缓存组播数据，用于排序、去重和查失
 *
 */

#pragma once

#include <deque>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <unordered_map>

#include "mdp/pkg_defines.hpp"

namespace mdp {
// ======

/// cache 缓存
class cache {
  public:
    struct data_key_t {
        uint8_t mdg_no;
        uint64_t start_seq_no;
        uint8_t seq_num;
    };
    struct data_hash_func {
        size_t operator()(const data_key_t& arg) const {
            size_t h1 = std::hash<uint32_t>()(arg.mdg_no);
            size_t h2 = std::hash<uint64_t>()(arg.start_seq_no);
            size_t h3 = std::hash<uint64_t>()(arg.seq_num);
            size_t seed = 0;
            seed ^= h1 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= h2 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= h3 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            return seed;
        }
    };
    struct data_equal_func {
        bool operator()(const data_key_t& arg_1,
                        const data_key_t& arg_2) const {
            // memcmp
            if (arg_1.mdg_no != arg_2.mdg_no) {
                return false;
            }
            if (arg_1.start_seq_no != arg_2.start_seq_no) {
                return false;
            }
            if (arg_1.seq_num != arg_2.seq_num) {
                return false;
            }
            return true;
        }
    };
    using data_t = std::unordered_map<data_key_t, mdp::pkg::body*,
                                      data_hash_func, data_equal_func>;
    using iterator = data_t::iterator;
    using citerator = data_t::const_iterator;
    using key_type = data_t::key_type;
    using value_type = data_t::value_type;
    using second_type = data_t::value_type::second_type;
    struct psize_key_t {
        uint8_t mdg_no;
        uint64_t start_seq_no;
    };
    struct psize_hash_func {
        size_t operator()(const psize_key_t& arg) const {
            size_t h1 = std::hash<uint32_t>()(arg.mdg_no);
            size_t h2 = std::hash<uint64_t>()(arg.start_seq_no);
            size_t seed = 0;
            seed ^= h1 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= h2 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            return seed;
        }
    };
    struct psize_equal_func {
        bool operator()(const psize_key_t& arg_1,
                        const psize_key_t& arg_2) const {
            // memcmp
            if (arg_1.mdg_no != arg_2.mdg_no) {
                return false;
            }
            if (arg_1.start_seq_no != arg_2.start_seq_no) {
                return false;
            }
            return true;
        }
    };
    using psize_t = std::unordered_map<psize_key_t, uint64_t, psize_hash_func,
                                       psize_equal_func>;
    using completed_t =
        std::unordered_map<uint32_t,
                           std::deque<uint64_t>>; // key: mdg_no, value:
                                                  // deque<seq_no>
    using pos_t =
        std::unordered_map<uint32_t, uint64_t>; // key: mdg_no, value: seq_no

    cache();
    ~cache();

    /// find 查找
    auto find(const key_type& key) -> iterator;

    /// begin 容器开始迭代器
    auto begin() -> iterator;

    /// end 容器结束迭代器
    auto end() -> iterator;

    /// size 元素个数
    size_t size();

    /// empty 是否为空
    bool empty();

    /// count 统计以fid为key的元素个数，0 为不存在， 1为存在， 不可能大于1
    size_t count(const key_type& key);

    /// at 找到fid对应的域
    auto at(const key_type& key) -> second_type;

    /// [] 下标定位
    auto operator[](const key_type& key) -> second_type;

    /// remove 只移除一个元素，不删除对应的指针内存
    size_t remove(const key_type& key);

    /// erase 删除一个元素，同时删除对应的指针内存
    size_t erase(const key_type& key);

    /// clear 清空
    void clear();

    /// emplace 插入元素
    void emplace(const value_type& val);

    /// insert 插入元素
    void insert(const value_type& val);

    /// cbegin const
    auto cbegin() -> citerator;

    /// end const
    auto cend() -> citerator;

    /// reserve 预分配大小
    void reserve(size_t size);

    /// clear_and_next 清理并更新下一个序列号
    void clear_and_next(uint32_t mdg_no, uint64_t seq_no);

    /// set_next 直接更新下一个序列号
    void set_next(uint32_t mdg_no, uint64_t seq_no);

    /// get_next 下一个需要处理的包
    uint64_t get_next(uint32_t mdg_no);

    /// set_head 设置最新的seq_no
    void set_head(uint32_t mdg_no, uint64_t seq_no);

    /// get_head 当前最新的seq_no
    uint64_t get_head(uint32_t mdg_no);

    /// set_lost 设置丢包的seq_no
    void set_lost(uint32_t mdg_no, uint64_t seq_no);

    /// get_lost 当前丢包的seq_no
    uint64_t get_lost(uint32_t mdg_no);

    /// get_max_breakpoint 获取最大断点
    uint64_t get_max_breakpoint(uint32_t mdg_no);

    /// get_psize 获取序列号对应的总包数
    uint64_t get_psize(uint32_t mdg_no, uint64_t seq_no);

    /// get_pcount 获取当前序列号已经收到的包数
    uint64_t get_pcount(uint32_t mdg_no, uint64_t seq_no);

  private:
    cache(const cache& cpy) = delete;
    void operator=(const cache& rhs) = delete;

    // 数据
    size_t datas_max_size_ = 10000;
    data_t datas_; // 缓存的数据
    psize_t psize_; // mdg_no和seq_no对应的应该有多少个包; 单包 1；多包 0
                    // 未知，非0 最大包数（flag=last时的extend）
    psize_t pcount_; // mdg_no和seq_no对应的已经收到多少个包
    pos_t next_; // mdg_no组，下一个该处理的包的seq_no，初始值为tcp通道查询的值
    pos_t head_; // mdg_no组，当前收到的最大seq_no
    pos_t lost_; // mdg_no组，丢失的第一个包的seq_no
};

// ======
} // namespace mdp
