/**
 * Copyright (C) 2021，大连商品交易所 大连飞创信息技术有限公司
 *
 * @brief 版本号定义。
 *
 */

#include "mdp/version.hpp"

#include "mdp/pkg_defines.hpp"

// version
#define VERSION_CODE "0.1.0"
#define VERSION_FEATURE "dev"
#define VERSION_DESC "for mdp"

namespace mdp {

const char* version::code(void) { return VERSION_CODE; }

const char* version::feature(void) { return VERSION_FEATURE; }

const char* version::desc(void) { return VERSION_DESC; }

uint16_t version::get_pkg_version() { return pkg::proto_version(); }

// set_pkg_version 设置协议版本号，方便测试
void version::set_pkg_version(uint16_t v) { pkg::set_proto_version(v); }

} // namespace mdp