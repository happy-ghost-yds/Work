/**
 * Copyright (C) 2021，大连商品交易所 大连飞创信息技术有限公司
 *
 * @brief 缓存类
 *
 * 用于缓存组播数据，用于排序、去重和查失
 *
 */

#include "mdp/cache.hpp"

namespace mdp {
// ======

cache::cache() {}

cache::~cache() { clear(); }

auto cache::find(const key_type& key) -> iterator { return datas_.find(key); }

auto cache::begin() -> iterator { return datas_.begin(); }

auto cache::end() -> iterator { return datas_.end(); }

size_t cache::size() { return datas_.size(); }

bool cache::empty() { return datas_.empty(); }

size_t cache::count(const key_type& key) { return datas_.count(key); }

auto cache::at(const key_type& key) -> second_type { return datas_.at(key); }

auto cache::operator[](const key_type& key) -> second_type {
    return datas_[key];
}

size_t cache::remove(const key_type& key) { return datas_.erase(key); }

size_t cache::erase(const key_type& key) {
    auto it = datas_.find(key);
    if (it != datas_.end() && nullptr != it->second) {
        delete it->second;
        it->second = nullptr;
    }
    return datas_.erase(key);
}

void cache::clear() {
    for (auto& item : datas_) {
        if (nullptr != item.second) {
            delete item.second;
            item.second = nullptr;
        }
    }
    datas_.clear();
}

void cache::emplace(const value_type& val) {
    datas_.emplace(val);

    auto body = std::get<1>(val);

    // 维护 psize pcount
    psize_key_t key;
    key.mdg_no = body->mdg_no();
    key.start_seq_no = body->start_seq_no();
    auto pit = pcount_.find(key);
    if (pit == pcount_.end()) {
        // 因为只比较了一个pcount_，所以是需要psize_和pcount_一致
        psize_[key] = 0;
        pcount_[key] = 0;
    }
    if (body->flag() == pkg::body::single) {
        psize_[key] = 1;
        pcount_[key] = 1;
    } else if (body->flag() == pkg::body::last) {
        psize_[key] = body->seq_num();
        pcount_[key] += 1;
    } else {
        pcount_[key] += 1;
        psize_[key] = 0;
    }

    // 维护 head
    auto hit = head_.find(key.mdg_no);
    if (hit == head_.end()) {
        head_[key.mdg_no] = 0;
    }
    if (head_[key.mdg_no] < key.start_seq_no) {
        head_[key.mdg_no] = key.start_seq_no;
    }
}

void cache::insert(const value_type& val) { emplace(val); }

auto cache::cbegin() -> citerator { return datas_.cbegin(); }

auto cache::cend() -> citerator { return datas_.cend(); }

void cache::reserve(size_t size) { datas_.reserve(size); }

// clear_and_next 清理并更新下一个序列号
void cache::clear_and_next(uint32_t mdg_no, uint64_t seq_no) {
    if (seq_no <= get_next(mdg_no)) {
        return;
    }

    // 清除这个序列号之前的所有包
    // 多包数据的包个数可能未知
    auto it = datas_.begin();
    while (it != datas_.end()) {
        if (it->first.mdg_no == mdg_no && it->first.start_seq_no < seq_no) {
            it = datas_.erase(it);
        } else {
            ++it;
        }
    }

    // 更新
    next_[mdg_no] = seq_no;
}

void cache::set_next(uint32_t mdg_no, uint64_t seq_no) {
    if (seq_no <= get_next(mdg_no)) {
        return;
    }

    // 更新
    next_[mdg_no] = seq_no;
}

uint64_t cache::get_next(uint32_t mdg_no) {
    auto it = next_.find(mdg_no);
    if (it == next_.end()) {
        next_[mdg_no] = 0;
        return 0;
    }
    return it->second;
}

// set_head 设置最新的seq_no
void cache::set_head(uint32_t mdg_no, uint64_t seq_no) {
    if (seq_no <= get_head(mdg_no)) {
        return;
    }

    // 更新
    head_[mdg_no] = seq_no;
}

// get_head 当前最新的seq_no
auto cache::get_head(uint32_t mdg_no) -> uint64_t {
    auto it = head_.find(mdg_no);
    if (it == head_.end()) {
        head_[mdg_no] = 0;
        return 0;
    }
    return head_[mdg_no];
}

/// set_lost 设置丢包的seq_no
void cache::set_lost(uint32_t mdg_no, uint64_t seq_no) {
    // 更新
    lost_[mdg_no] = seq_no;
}

/// get_lost 当前丢包的seq_no
auto cache::get_lost(uint32_t mdg_no) -> uint64_t {
    auto it = lost_.find(mdg_no);
    if (it == lost_.end()) {
        lost_[mdg_no] = 0;
        return 0;
    }
    return lost_[mdg_no];
}

// get_max_breakpoint 获取最大断点
uint64_t cache::get_max_breakpoint(uint32_t mdg_no) {
    auto next_no = get_next(mdg_no);
    auto head_no = get_head(mdg_no);

    uint64_t breakpoint = 0;
    for (auto i = head_no; i >= next_no; i--) {
        auto psize = get_psize(mdg_no, i);
        auto pcount = get_pcount(mdg_no, i);
        if ((0 == pcount || psize != pcount)) {
            breakpoint = i;
            break;
        }
    }
    return breakpoint;
}

// get_psize 获取序列号对应的总包数
uint64_t cache::get_psize(uint32_t mdg_no, uint64_t seq_no) {
    psize_key_t key;
    key.mdg_no = mdg_no;
    key.start_seq_no = seq_no;
    auto pit = psize_.find(key);
    if (pit == psize_.end()) {
        return 0;
    }
    return pit->second;
}

// get_pcount 获取当前序列号已经收到的包数
uint64_t cache::get_pcount(uint32_t mdg_no, uint64_t seq_no) {
    psize_key_t key;
    key.mdg_no = mdg_no;
    key.start_seq_no = seq_no;
    auto pit = pcount_.find(key);
    if (pit == pcount_.end()) {
        return 0;
    }
    return pit->second;
}

// ======
} // namespace mdp
