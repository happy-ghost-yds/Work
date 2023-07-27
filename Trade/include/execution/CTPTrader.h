#pragma once

#include <map>

#include "ctp/ThostFtdcTraderApi.h"
#include "execution/baseTrader.h"
#include "common/Env.h"

struct CTPTraderConfig
{
    std::string brokerID;
    std::string userID;
    std::string authCode;
    std::string appID;
    std::string password;
    std::string exchangeID;
    std::string frontTdAddr;
};

class CCTPTrader : public CThostFtdcTraderSpi, public BaseTrader
{
public:
    int start();
    int newOrder(std::string instrument, Base_SideType side, double orderPrice, int quantity);
    int CxlOrder(Base_ClientToken clientOrderToken);

public:
	// CCTPTrader(CThostFtdcTraderApi* instance) {m_ptrTraderApi = instance;};
    CCTPTrader();
	~CCTPTrader() {};
	virtual void OnFrontConnected();
	virtual void OnFrontDisConnected(int nReason);
	virtual void OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);
	virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);
	virtual void OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo);
	virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspQrySettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspUserPasswordUpdate(CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

private:
	CThostFtdcTraderApi* m_ptrTraderApi;
    int nRequestID;
    int m_orderRef;
    int m_FrontID;
    int m_SessionID;

private:
    CTPTraderConfig m_config;
    std::map<Base_ClientToken, std::string> m_orderRefToSysID;
};