#pragma once

#include "common/customQueue.h"

typedef unsigned char Base_SideType;
#define Base_SideType_open_long                 1
#define Base_SideType_close_today_long           2
#define Base_SideType_close_today_short          3
#define Base_SideType_open_short                 4
#define Base_SideType_close_ovn_short            5		///< =�򵥣�ƽ��
#define Base_SideType_close_ovn_long             6		///< =������ƽ��
#define Base_SideType_today_opt_exec				11		///< =��Ȩ��ƽ��
#define Base_SideType_ovn_opt_exec				12		///< =��Ȩ��ƽ��
#define Base_SideType_opt_exec					13		///��Ȩ��ƽ�֣�
#define Base_SideType_abandon_today_opt_exec		14		///< =���������Ȩ
#define Base_SideType_abandon_ovn_opt_exec		15		///< =���������Ȩ
#define Base_SideType_abandon_opt_exec	  		16		///������Ȩ��ƽ�֣�
#define Base_SideType_close_short				21		///< =�򵥣�ƽ�֣�
#define Base_SideType_close_long					22

typedef unsigned char Base_ExchangeID;					///< ������ID
#define Base_ExchangeID_sh_cs                    100		///< =�Ͻ���
#define Base_ExchangeID_sz_cs                    101		///< =���
#define Base_ExchangeID_cffex                    102		///< =�н���
#define Base_ExchangeID_shfe                     103		///< =������
#define Base_ExchangeID_dce                      104		///< =������
#define Base_ExchangeID_zcze                     105		///< =֣����
#define Base_ExchangeID_ine						106		///< =��Դ����
#define Base_ExchangeID_sge						107		///< =�Ϻ�����
#define Base_ExchangeID_done_away                255		///< =Done-away 

typedef unsigned int  Base_ClientToken;
typedef int						Base_UserID;
typedef long long int			Base_MarketToken;
typedef int						Base_TradingDate;
typedef unsigned long long int	Base_Nanosecond;
typedef char	Base_MarketExecId[25];

typedef unsigned int Base_Millisecond;

struct Base_OrderExecutionField
{
	Base_UserID        m_Userid;							///< ԭ�����ӵ��û�����Ӧ��LoginID��
	Base_Nanosecond    m_Timestamp;						///< �ɽ�ʱ�䣬��1970��1��1��0ʱ0��0�뿪ʼ������ʱ��
	Base_ClientToken   m_ClientOrderToken;				///< ԭ�����ӵ����token
	Base_MarketToken   m_MarketOrderToken;				///< ʢ��ϵͳ�����ĵ��Ӻţ���ʢ������ʱ���øúš�
	unsigned int      m_Quantity;						///< ���ӳɽ���
	double            m_Price;							///< ���ӳɽ���
	Base_MarketToken   m_ExecutionID;					///< ���ӳɽ���(TAG 1017)
	Base_MarketExecId  m_MarketExecID;					///< �������ɽ���
};

class BaseTrader
{
public:
    BaseTrader() {};
	virtual ~BaseTrader() {};
    virtual int start() {};
	Base_ExchangeID getExchangeId() {return m_exchangeId;};
    virtual int newOrder(std::string instrument, Base_SideType side, double orderPrice, int quantity) {};
	virtual int smartNewOrder(std::string instrument, int direction, double orderPrice, int quantity) {};
	virtual int CxlOrder(Base_ClientToken clientOrderToken) {};
    customQueue<Base_OrderExecutionField>* m_TraderCallbackQueue; //any derived callback shall squeeze in exec message into this

public:
	Base_ExchangeID m_exchangeId;
};