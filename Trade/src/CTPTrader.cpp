#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "execution/CTPTrader.h"

static const int orderRefOffset = 40000;

CCTPTrader::CCTPTrader() {};

int CCTPTrader::start()
{
	Env& env = Env::getInstance();
	m_config = {};
	m_config.brokerID = env["tdCTP"]["brokerID"];
	m_config.userID = env["tdCTP"]["userID"];
	m_config.authCode = env["tdCTP"]["authCode"];
	m_config.appID = env["tdCTP"]["appID"];
	m_config.password = env["tdCTP"]["password"];
	m_config.exchangeID = env["tdCTP"]["exchangeID"];
	m_config.frontTdAddr = env["tdCTP"]["frontTdAddr"];

    m_ptrTraderApi = CThostFtdcTraderApi::CreateFtdcTraderApi();

    // printf("CTPTrader1\n");
    m_ptrTraderApi->RegisterSpi(this);
    // printf("CTPTrader2\n");
	m_ptrTraderApi->SubscribePrivateTopic(THOST_TERT_RESUME);
    // printf("CTPTrader3\n");
	m_ptrTraderApi->SubscribePublicTopic(THOST_TERT_RESUME);
    // printf("CTPTrader4\n");
	m_ptrTraderApi->RegisterFront(const_cast<char*>(m_config.frontTdAddr.c_str()));
    // printf("CTPTrader5\n");
	m_ptrTraderApi->Init();

	m_ptrTraderApi->Join();
	m_ptrTraderApi->Release();
}

int CCTPTrader::newOrder(std::string instrument, Base_SideType side, double orderPrice, int quantity)
{
    CThostFtdcInputOrderField order;
    memset(&order, 0, sizeof(order));

    int local_orderRef = ++m_orderRef;
	strcpy(order.BrokerID, m_config.brokerID.c_str());
	strcpy(order.InvestorID, m_config.userID.c_str());
	strcpy(order.InstrumentID, instrument.c_str());
	strcpy(order.OrderRef, std::to_string(local_orderRef).c_str());
	strcpy(order.UserID, m_config.userID.c_str());
	order.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
    switch(side) //TODO fill other directions
    {
        case Base_SideType_open_long:
            order.Direction = THOST_FTDC_D_Buy;
            order.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
            break;
        case Base_SideType_open_short:
            order.Direction = THOST_FTDC_D_Sell;
            order.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
            break;
        default:
            break;
    }
	// order.Direction = THOST_FTDC_D_Sell;
	// order.Direction = THOST_FTDC_D_Buy;
	// strcpy(order.CombOffsetFlag, "0");
	// strcpy(order.CombHedgeFlag, "1");
	// order.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
	order.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	order.LimitPrice = orderPrice;
	order.VolumeTotalOriginal = quantity;
	order.TimeCondition = THOST_FTDC_TC_GFD;
	// strcpy(order.GTDDate, "");
	order.VolumeCondition = THOST_FTDC_VC_AV;
	order.MinVolume = 1;
	order.ContingentCondition = THOST_FTDC_CC_Immediately;
	order.StopPrice = 0;
	order.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	order.IsAutoSuspend = false;
	strcpy(order.ExchangeID, m_config.exchangeID.c_str()); //TODO

    m_ptrTraderApi->ReqOrderInsert(&order, ++nRequestID);
    return local_orderRef;
}

int CCTPTrader::CxlOrder(Base_ClientToken clientOrderToken)
{
    CThostFtdcInputOrderActionField InputOrderAction;
	strcpy(InputOrderAction.BrokerID, m_config.brokerID.c_str());
	strcpy(InputOrderAction.InvestorID, m_config.userID.c_str());
	strcpy(InputOrderAction.UserID, m_config.userID.c_str());

    strcpy(InputOrderAction.OrderRef, std::to_string(clientOrderToken).c_str());
    InputOrderAction.FrontID = m_FrontID;
    InputOrderAction.SessionID = m_SessionID;

	strcpy(InputOrderAction.ExchangeID, m_config.exchangeID.c_str());
	strcpy(InputOrderAction.OrderSysID, m_orderRefToSysID[clientOrderToken].c_str());
	// InputOrderAction.OrderSysID = ;
	InputOrderAction.ActionFlag = '0';
	return m_ptrTraderApi->ReqOrderAction(&InputOrderAction, ++nRequestID);
}

void CCTPTrader::OnRtnOrder(CThostFtdcOrderField *pOrder) 
{
    printf("OnRtnOrder %s %s %c\n", pOrder->OrderRef, pOrder->OrderSysID, pOrder->OrderStatus);
    m_orderRefToSysID[std::stoi(pOrder->OrderRef)] = pOrder->OrderSysID;
	// cout << "OnRtnOrder..." << endl;
	// cout << "orderSysID:" << pOrder->OrderSysID << endl;
	// cout << "submit status:" << pOrder->OrderSubmitStatus << endl;
	// cout << "order status:" << pOrder->OrderStatus << endl;
}

void CCTPTrader::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
    printf("OnRtnTrade %s %s %d %f\n", pTrade->OrderRef, pTrade->OrderSysID, pTrade->Volume, pTrade->Price);
    Base_OrderExecutionField exec;
    // exec.m_Userid = 0;
    // exec.m_Timestamp = 0; // add later
    exec.m_ClientOrderToken = std::stoi(pTrade->OrderRef);
    exec.m_Quantity = pTrade->Volume;
    exec.m_Price = pTrade->Price;

    (this->m_TraderCallbackQueue)->push(exec);
	// cout << "OnRtnTrade..." << endl;
	// cout << "BrokerID:" << pTrade->BrokerID << endl;
	// cout << "InvestorID:" << pTrade->InvestorID << endl;
	// cout << "OrderRef:" << pTrade->OrderRef << endl;
	// cout << "UserID:" << pTrade->UserID << endl;
	// cout << "ExchangeID:" << pTrade->ExchangeID << endl;
	// cout << "TradeID:" << pTrade->TradeID << endl;
	// cout << "Direction:" << pTrade->Direction << endl;
	// cout << "OrderSysID:" << pTrade->OrderSysID << endl;
	// cout << "ParticipantID:" << pTrade->ParticipantID << endl;
	// cout << "ClientID:" << pTrade->ClientID << endl;
	// cout << "TradingRole:" << pTrade->TradingRole << endl;
	// cout << "OffsetFlag:" << pTrade->OffsetFlag << endl;
	// cout << "HedgeFlag:" << pTrade->HedgeFlag << endl;
	// cout << "Price:" << pTrade->Price << endl;
	// cout << "Volume:" << pTrade->Volume << endl;
	// cout << "TradeDate:" << pTrade->TradeDate << endl;
	// cout << "TradeTime:" << pTrade->TradeTime << endl;
	// cout << "TradeType:" << pTrade->TradeType << endl;
	// cout << "PriceSource:" << pTrade->PriceSource << endl;
	// cout << "TraderID:" << pTrade->TraderID << endl;
	// cout << "OrderLocalID:" << pTrade->OrderLocalID << endl;
	// cout << "ClearingPartID:" << pTrade->ClearingPartID << endl;
	// cout << "BusinessUnit:" << pTrade->BusinessUnit << endl;
	// cout << "SequenceNo:" << pTrade->SequenceNo << endl;
	// cout << "TradingDay:" << pTrade->TradingDay << endl;
	// cout << "SettlementID:" << pTrade->SettlementID << endl;
	// cout << "BrokerOrderSeq:" << pTrade->BrokerOrderSeq << endl;
	// cout << "TradeSource:" << pTrade->TradeSource << endl;
	// cout << "InvestUnitID:" << pTrade->InvestUnitID << endl;
	// cout << "InstrumentID:" << pTrade->InstrumentID << endl;
	// cout << "ExchangeInstID:" << pTrade->ExchangeInstID << endl;
}

void CCTPTrader::OnFrontConnected()
{
    printf("OnFrontConnected\n");
	// login
	CThostFtdcReqAuthenticateField reqAuth;
	strcpy(reqAuth.BrokerID, m_config.brokerID.c_str());
	strcpy(reqAuth.UserID, m_config.userID.c_str());
	// strcpy(reqAuth.UserProductInfo,"");
	strcpy(reqAuth.AuthCode, m_config.authCode.c_str());
	strcpy(reqAuth.AppID, m_config.appID.c_str());

	m_ptrTraderApi->ReqAuthenticate(&reqAuth, ++nRequestID);
}

void CCTPTrader::OnFrontDisConnected(int nReason)
{
	printf("OnFrontDisConnected Reason=%d\n", nReason);
}

void CCTPTrader::OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
    printf("OnRspAuthenticate\n");
	if (pRspInfo->ErrorID == 0)
	{
        CThostFtdcReqUserLoginField reqUserLogin;
        strcpy(reqUserLogin.BrokerID,m_config.brokerID.c_str());
        strcpy(reqUserLogin.UserID,m_config.userID.c_str());
        strcpy(reqUserLogin.Password,m_config.password.c_str());
        m_ptrTraderApi->ReqUserLogin(&reqUserLogin, ++nRequestID);
    } else {
        printf("OnRspAuthenticate Error=%d\n", pRspInfo->ErrorID);
    }
}

void CCTPTrader::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    printf("OnRspUserLogin\n");
    if (pRspInfo->ErrorID == 0)
    {
        m_FrontID = pRspUserLogin->FrontID;
        m_SessionID = pRspUserLogin->SessionID;
        m_orderRef = atoi(pRspUserLogin->MaxOrderRef) + orderRefOffset;

        CThostFtdcSettlementInfoConfirmField settlement;
        strcpy(settlement.BrokerID, m_config.brokerID.c_str());
        strcpy(settlement.InvestorID, m_config.userID.c_str());
        m_ptrTraderApi->ReqSettlementInfoConfirm(&settlement, ++nRequestID);
    }
}

void CCTPTrader::OnRspUserPasswordUpdate(CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{}

void CCTPTrader::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    printf("OnRspOrderInsert, err=%d\n", pRspInfo->ErrorID);
	// cout << "OnRspOrderInsert" << endl;
	// cout << "error id:" << pRspInfo->ErrorID << endl;
	// cout << "error message:" << pRspInfo->ErrorMsg << endl;
}

void CCTPTrader::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    printf("OnRspOrderAction, err=%d\n", pRspInfo->ErrorID);
	// cout << "OnRspOrderAction..." << endl;
}

void CCTPTrader::OnRspQrySettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    printf("OnRspQrySettlementInfoConfirm, err=%d\n", pRspInfo->ErrorID);
}

void CCTPTrader::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    printf("OnRspError, err=%d\n", pRspInfo->ErrorID);
}

void CCTPTrader::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    printf("OnRspQryInstrument, err=%d\n", pRspInfo->ErrorID);
}

void CCTPTrader::OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo)
{
    printf("OnErrRtnOrderInsert, err=%d\n", pRspInfo->ErrorID);
}

void CCTPTrader::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    printf("OnRspSettlementInfoConfirm\n");
}