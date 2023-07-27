#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <string.h>
#include <time.h>
#include <map>

typedef void*		T_DLL_HANDLE;

#include <string>
#include "trade/rem/include/EesTraderApi.h"
#include "common/customQueue.h"
#include "execution/baseTrader.h"
#include "common/Env.h"

#define TRADE_API_CREATE_FAILED 1

struct EESConfig
{
	std::string tradeServerIp;
	unsigned short	tradeServerPort;
	std::string queryServerIp;
	unsigned short	queryServerPort;
	std::string localIp;
	unsigned short localUDPPort;
	std::string	logonId;
	std::string	logonPwd;
	std::string	account;
	std::string	localMac;
	std::string appId;
	EES_ExchangeID exchangeId;

    friend std::ostream& operator <<(std::ostream& os, EESConfig const& config)
    {
        return os << "tradeServerIp=" << config.tradeServerIp << '\n'
                  << "tradeServerPort=" << config.tradeServerPort << '\n'
                  << "queryServerIp=" << config.queryServerIp << '\n'
                  << "queryServerPort=" << config.queryServerPort << '\n'
                  << "localIp=" << config.localIp << '\n'
                  << "localUDPPort=" << config.localUDPPort << '\n'
                  << "logonId=" << config.logonId << '\n'
                  << "logonPwd=" << config.logonPwd << '\n'
                  << "account=" << config.account << '\n'
                  << "localMac=" << config.localMac << '\n'
				  << "appId=" << config.appId << '\n'
				  << "exchangeId=" << config.exchangeId << '\n';
    }
};

struct EESHoldingStatus
{
	int longHolding;
	int longCloseable;
	int shortHolding;
	int shortCloseable;
};

class TraderEES : public EESTraderEvent, public BaseTrader
{
public:
	TraderEES(void);
	virtual ~TraderEES(void);
	void loadConfig(std::string title);
	int start();
    int newOrder(std::string instrument, EES_SideType side, double orderPrice, int quantity);
	int smartNewOrder(std::string instrument, int direction, double orderPrice, int quantity) override;
	int CxlOrder(EES_ClientToken clientOrderToken);
	EES_ExchangeID getExchangeId();
	// customQueue<EES_OrderExecutionField>* m_TraderCallbackQueue;
	void setHoldingStatus(std::string instrument, int volume);
	void updateHoldingStatus(std::string instrument, EES_SideType side, int event, int diff);

private:
	void logon();
	bool loadEESTrader();

private:
	/// \brief �����������¼�
	virtual void OnConnection(ERR_NO errNo, const char* pErrStr);
	/// \brief	�����������Ͽ������յ������Ϣ
	virtual void OnDisConnection(ERR_NO errNo, const char* pErrStr);
	/// \brief	��¼��Ϣ�Ļص�
	virtual void OnUserLogon(EES_LogonResponse* pLogon);
	/// \brief	�µ���REM���ܵ��¼�
	virtual void OnOrderAccept(EES_OrderAcceptField* pAccept);
	/// \brief	�µ����г����ܵ��¼�
	virtual void OnOrderMarketAccept(EES_OrderMarketAcceptField* pAccept);
	///	\brief	�µ���REM�ܾ����¼�
	virtual void OnOrderReject(EES_OrderRejectField* pReject);
	///	\brief	�µ����г��ܾ����¼�
	virtual void OnOrderMarketReject(EES_OrderMarketRejectField* pReject);
	///	\brief	�����ɽ�����Ϣ�¼�
	virtual void OnOrderExecution(EES_OrderExecutionField* pExec);
	///	\brief	�����ɹ������¼�
	virtual void OnOrderCxled(EES_OrderCxled* pCxled);
	///	\brief	�������ܾ�����Ϣ�¼�
	virtual void OnCxlOrderReject(EES_CxlOrderRej* pReject);
	///	\brief  query current position
	virtual void OnQueryAccountPosition(const char* pAccount, EES_AccountPosition* pAccountPosition, int nReqId, bool bFinish);

private:
	EESTraderApi*				m_tradeApi;				///< EES����API�ӿ�
	T_DLL_HANDLE				m_handle;				///< EES����API���
	funcDestroyEESTraderApi		m_distoryFun;			///< EES����API��̬�����ٺ���

	EESConfig* m_config;
	// std::string					m_tradeServerIp;		///< ����ͨ��IP��ַ
	// int							m_tradeServerPort;		///< ����ͨ���˿�
	// std::string					m_queryServerIp;		///< ��ѯͨ��IP��ַ
	// int							m_queryServerPort;		///< ��ѯͨ���˿�
	// std::string					m_logonId;				///< ��¼���׷������
	// std::string					m_logonPwd;				///< ��¼���׷�������
	// std::string					m_account;				///< �����ʺ�
	// std::string					m_appId;
	// std::string					m_symbol;				///< ���׺�Լ
	// std::string					m_local_mac;			///< ����MAC��ַ
	// int							m_quantity;				///< ��������
	EES_MarketToken				m_marketOrderID;		///< �г��������
	int							m_logonStatus;			///< 0-��δ��¼; 1-���������ڵ�¼; 2-��¼�ɹ�; 3-��¼ʧ�ܣ������Ѿ��Ͽ�����
	timespec					m_start_time;
	std::map<EES_ClientToken, EES_MarketToken> m_clientToMarketTokenMap;
	std::map<EES_ClientToken, EES_EnterOrderField> m_clientTokenToOrderInfoMap;
	std::map<std::string, EESHoldingStatus> m_currentHoldingStatus;

	int m_reqId;
};