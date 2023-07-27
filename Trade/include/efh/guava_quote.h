/*!****************************************************************************
 @note   Copyright (coffee), 2005-2019, Shengli Tech. Co., Ltd.
 @file   guava_quote.h
 @date   2019/12/25   14:04
 @author zhou.hu
 
 @brief  本文件是EFH3.2版本行情组播接口的示例程序

 @note 
******************************************************************************/
#ifndef _GUAVAQUOTE_H_
#define _GUAVAQUOTE_H_

#pragma once
#include <vector>

using std::vector;

#define MAX_IP_LEN				32

#define QUOTE_FLAG_SUMMARY		4


#pragma pack(push, 1)

struct efh3_2_fut_lev1
{
	unsigned int	m_sequence;				///<会话编号
	char			m_exchange_id;			///<市场  0 表示中金  1表示上期
	char			m_channel_id;			///<通道编号
	char			m_symbol[8];			///<合约
	char			m_update_time_h;		///<最后更新的时间hh
	char			m_update_time_m;		///<最后更新的时间mm
	char			m_update_time_s;		///<最后更新的时间ss
	unsigned short  m_millisecond;		    ///<最后更新的毫秒数         

	double			m_last_px;				///<最新价
	unsigned int	m_last_share;			///<最新成交量
	double			m_total_value;			///<成交金额
	double			m_total_pos;			///<持仓量
	double			m_bid_px;				///<买一价
	unsigned int	m_bid_share;			///<买一量
	double			m_ask_px;				///<卖一价
	unsigned int	m_ask_share;			///<卖一量
	char            m_reserve;  			///<保留字段             
};

struct efh3_2_opt_lev1
{
	unsigned int	m_sequence;				///<会话编号
	char			m_exchange_id;			///<市场  0 表示中金  1表示上期
	char			m_channel_id;			///<通道编号
	char			m_symbol[16];			///<合约
	char			m_update_time_h;		///<最后更新的时间hh
	char			m_update_time_m;		///<最后更新的时间mm
	char			m_update_time_s;		///<最后更新的时间ss
	unsigned short  m_millisecond;		    ///<最后更新的毫秒数        

	double			m_last_px;				///<最新价
	unsigned int	m_last_share;			///<最新成交量
	double			m_total_value;			///<成交金额
	double			m_total_pos;			///<持仓量
	double			m_bid_px;				///<买一价
	unsigned int	m_bid_share;			///<买一量
	double			m_ask_px;				///<卖一价
	unsigned int	m_ask_share;			///<卖一量
	char            m_reserve;  			///<保留字段             
};

struct efh3_2_fut_lev2
{
	unsigned int	m_sequence;				///<会话编号
	char			m_exchange_id;			///<市场  0 表示中金  1表示上期
	char			m_channel_id;			///<通道编号
	char			m_symbol[8];			///<合约
	char			m_update_time_h;		///<最后更新的时间hh
	char			m_update_time_m;		///<最后更新的时间mm
	char			m_update_time_s;		///<最后更新的时间ss
	unsigned short  m_millisecond;		    ///<最后更新的毫秒数        

	double			m_last_px;				///<最新价
	unsigned int	m_last_share;			///<最新成交量
	double			m_total_value;			///<成交金额
	double			m_total_pos;			///<持仓量

	double			m_bid1_px;				///<买一价
	unsigned int	m_bid1_share;			///<买一量
	double			m_bid2_px;				///<买二价
	unsigned int	m_bid2_share;			///<买二量
	double			m_bid3_px;				///<买三价
	unsigned int	m_bid3_share;			///<买三量
	double			m_bid4_px;				///<买四价
	unsigned int	m_bid4_share;			///<买四量
	double			m_bid5_px;				///<买五价
	unsigned int	m_bid5_share;			///<买五量

	double			m_ask1_px;				///<卖一价
	unsigned int	m_ask1_share;			///<卖一量
	double			m_ask2_px;				///<卖二价
	unsigned int	m_ask2_share;			///<卖二量
	double			m_ask3_px;				///<卖三价
	unsigned int	m_ask3_share;			///<卖三量
	double			m_ask4_px;				///<卖四价
	unsigned int	m_ask4_share;			///<卖四量
	double			m_ask5_px;				///<卖五价
	unsigned int	m_ask5_share;			///<卖五量

	char            m_reserve;  			///<保留字段             
};


struct efh3_2_opt_lev2
{
	unsigned int	m_sequence;				///<会话编号
	char			m_exchange_id;			///<市场  0 表示中金  1表示上期
	char			m_channel_id;			///<通道编号
	char			m_symbol[16];			///<合约
	char			m_update_time_h;		///<最后更新的时间hh
	char			m_update_time_m;		///<最后更新的时间mm
	char			m_update_time_s;		///<最后更新的时间ss
	unsigned short  m_millisecond;		    ///<最后更新的毫秒数        

	double			m_last_px;				///<最新价
	unsigned int	m_last_share;			///<最新成交量
	double			m_total_value;			///<成交金额
	double			m_total_pos;			///<持仓量

	double			m_bid1_px;				///<买一价
	unsigned int	m_bid1_share;			///<买一量
	double			m_bid2_px;				///<买二价
	unsigned int	m_bid2_share;			///<买二量
	double			m_bid3_px;				///<买三价
	unsigned int	m_bid3_share;			///<买三量
	double			m_bid4_px;				///<买四价
	unsigned int	m_bid4_share;			///<买四量
	double			m_bid5_px;				///<买五价
	unsigned int	m_bid5_share;			///<买五量

	double			m_ask1_px;				///<卖一价
	unsigned int	m_ask1_share;			///<卖一量
	double			m_ask2_px;				///<卖二价
	unsigned int	m_ask2_share;			///<卖二量
	double			m_ask3_px;				///<卖三价
	unsigned int	m_ask3_share;			///<卖三量
	double			m_ask4_px;				///<卖四价
	unsigned int	m_ask4_share;			///<卖四量
	double			m_ask5_px;				///<卖五价
	unsigned int	m_ask5_share;			///<卖五量

	char            m_reserve;  			///<保留字段             
};

#pragma pack(pop)
#endif