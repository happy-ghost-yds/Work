/**
 * Copyright (C) 2021，大连商品交易所 大连飞创信息技术有限公司
 *
 * @brief vint加解密函数以及密码加密函数
 *
 * vint加解密函数以及密码加密函数。
 *
 */

#ifndef __MDP_ENCDEC_HPP__
#define __MDP_ENCDEC_HPP__

#include "openssl/evp.h"
#include "openssl/opensslv.h"
#include <cstdint>
#include <cstring>
#include <type_traits>

namespace mdp {
// vint加密函数
template <typename T, typename U> size_t vint_encode(T n, U* buff) {
    U* p = buff;

    int64_t x = n;
    x = (x << 1) ^ (x >> (64 - 1));
    uint64_t xx = x;

    while (xx & ~0x7f) {
        *p++ = (xx & 0x7f) | 0x80;
        xx >>= 7;
    }
    *p++ = xx;
    return p - buff;
}

// vint解密函数
template <typename T, typename U> size_t vint_decode(T& n, const U* buff) {
    int64_t ret = 0;
    size_t shift = 0;
    const U* p = buff;

    while (shift < sizeof(int64_t) * 8) {
        ret |= static_cast<int64_t>((*p) & 0x7f) << shift;
        if (((*p) & 0x80) != 0x80) {
            break;
        }
        ++p;
        shift += 7;
    }

    n = static_cast<T>((static_cast<uint64_t>(ret) >> 1) ^ (-(ret & 1)));

    return p - buff + 1;
}

// 密码加密函数，目前使用md5
inline int pwd_encrypt(const char* in_data, size_t in_length, char* out_data,
                       size_t* out_length) {
    constexpr static size_t md5_len = 33;
    if (!in_data || !out_data || !out_length || *out_length < md5_len) {
        return -1;
    }

    const EVP_MD* md = EVP_md5();
    EVP_MD_CTX* ctx = EVP_MD_CTX_create();
    EVP_DigestInit_ex(ctx, md, nullptr);
    EVP_DigestUpdate(ctx, reinterpret_cast<const unsigned char*>(in_data),
                     in_length);
    auto outlen = static_cast<unsigned int>(md5_len);
    unsigned char result[md5_len] = {0};
    EVP_DigestFinal_ex(ctx, result, &outlen);
    EVP_MD_CTX_destroy(ctx);

    *out_length = md5_len - 1;
    for (size_t i = 0; i * 2 < *out_length; i++) {
        sprintf(out_data + i * 2, "%02x", result[i]);
    }
    out_data[*out_length] = '\0';

    return 0;
}

} // namespace mdp
#endif // __MDP_ENCDEC_HPP__
