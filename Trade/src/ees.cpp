#include "spdlog/spdlog.h"

#include "execution/ees.h"
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

#define CLOCK_GET_MODE CLOCK_REALTIME

TraderEES::TraderEES(void)
{
	m_logonStatus = 0;
	m_reqId = 0;
}

TraderEES::~TraderEES(void)
{
}

void TraderEES::loadConfig(std::string title)
{
    Env& env = Env::getInstance();
	EESConfig _eesConfig = {
        env[title]["tradeServerIp"],
        env[title]["tradeServerPort"],
        env[title]["queryServerIp"],
        env[title]["queryServerPort"],
        env[title]["localIp"],
        env[title]["localUDPPort"],
        env[title]["logonId"], // account ID
        env[title]["logonPwd"], // password
        env[title]["account"], // account ID
        env[title]["localMac"], // auth code
        env[title]["appId"], // appid
        env[title]["exchangeId"] ////
    };
	m_config = &_eesConfig;
	// std::cout << *m_config << std::endl;
}

EES_ExchangeID TraderEES::getExchangeId()
{
    return m_config->exchangeId;
}

void TraderEES::setHoldingStatus(std::string instrument, int volume)
{
	m_currentHoldingStatus[instrument] = {
		volume,
		volume,
		volume,
		volume
	};
}

void TraderEES::updateHoldingStatus(std::string instrument, EES_SideType side, int event, int quantity)
{
	// event 0=onAccepted 1=onExec 2=onCxl
	if (event == 0)
	{
		if (side == EES_SideType_close_today_short)
		{
			m_currentHoldingStatus[instrument].shortCloseable -= quantity;
		}

		if (side == EES_SideType_close_today_long)
		{
			m_currentHoldingStatus[instrument].longCloseable -= quantity;
		}
	}
	if (event == 1)
	{
		if (side == EES_SideType_close_today_short)
		{
			m_currentHoldingStatus[instrument].shortHolding -= quantity;
		}

		if (side == EES_SideType_close_today_long)
		{
			m_currentHoldingStatus[instrument].longHolding -= quantity;
		}

		if (side == EES_SideType_open_long)
		{
			m_currentHoldingStatus[instrument].longHolding += quantity;
			m_currentHoldingStatus[instrument].longCloseable += quantity;
		}

		if (side == EES_SideType_open_short)
		{
			m_currentHoldingStatus[instrument].shortHolding += quantity;
			m_currentHoldingStatus[instrument].shortCloseable += quantity;
		}
	}
	if (event == 2)
	{
		if (side == EES_SideType_close_today_short)
		{
			m_currentHoldingStatus[instrument].shortCloseable += quantity;
		}

		if (side == EES_SideType_close_today_long)
		{
			m_currentHoldingStatus[instrument].longCloseable += quantity;
		}
	}
	printf("currentHoldingStatus: Long %d(%d), Short %d(%d)\n", 
		m_currentHoldingStatus[instrument].longHolding, m_currentHoldingStatus[instrument].longCloseable,
		m_currentHoldingStatus[instrument].shortHolding, m_currentHoldingStatus[instrument].shortCloseable);
}

int TraderEES::newOrder(std::string instrument, EES_SideType side, double orderPrice, int quantity)
{
	EES_ClientToken order_token = 0;
	m_tradeApi->GetMaxToken(&order_token);
	if (order_token < 10) order_token = 10;

	EES_EnterOrderField temp;
	memset(&temp, 0, sizeof(EES_EnterOrderField));
	temp.m_Tif = EES_OrderTif_Day;
	temp.m_HedgeFlag = EES_HedgeFlag_Speculation;

	strcpy(temp.m_Account, m_config->account.c_str());
	strcpy(temp.m_Symbol, instrument.c_str());
	temp.m_Side = side;
	temp.m_Exchange = m_config->exchangeId;
	temp.m_SecType = EES_SecType_fut;
	temp.m_Price = orderPrice;
	temp.m_Qty = quantity;
	temp.m_ClientOrderToken = order_token + 1;

	printf("send order debug Exch=%d Instr=%s\n", temp.m_Exchange, temp.m_Symbol);
	// clock_gettime(CLOCK_GET_MODE, &m_start_time);
	RESULT ret = m_tradeApi->EnterOrder(&temp);
	m_clientTokenToOrderInfoMap[temp.m_ClientOrderToken] = temp;
    // return ret;
	printf("send order id=%d,at=%.4f,dir=%d\n", temp.m_ClientOrderToken, temp.m_Price, temp.m_Side);
	if (ret != NO_ERROR)
	{
		printf("send order failed(%d)\n", ret);
		return -1;
	}
	return temp.m_ClientOrderToken;

	// printf("send order successfully\n");
}

int TraderEES::smartNewOrder(std::string instrument, int direction, double orderPrice, int quantity)
{
	EES_SideType side;
	if (direction == 1)
	{
		if (m_currentHoldingStatus[instrument].shortCloseable >= quantity)
		{
			side = EES_SideType_close_today_short;
		} else {
			side = EES_SideType_open_long;
		}
	}

	if (direction == -1)
	{
		if (m_currentHoldingStatus[instrument].longCloseable >= quantity)
		{
			side = EES_SideType_close_today_long;
		} else {
			side = EES_SideType_open_short;
		}
	}

	return this->newOrder(instrument, side, orderPrice, quantity);
}

int TraderEES::CxlOrder(EES_ClientToken clientOrderToken)
{
	EES_CancelOrder  temp;
	memset(&temp, 0, sizeof(EES_CancelOrder));

	strcpy(temp.m_Account, m_config->account.c_str());
	temp.m_Quantity = 0;
	temp.m_orderIdType = 0;
	temp.m_MarketOrderToken = m_clientToMarketTokenMap[clientOrderToken];
	// temp.m_ClientOrderToken = clientOrderToken;

	RESULT ret = m_tradeApi->CancelOrder(&temp);
	printf("cancel order id=%d\n", clientOrderToken);
	if (ret != NO_ERROR)
	{
		printf("send cancel failed(%d)\n", ret);
		return -1;
	}
	return clientOrderToken;
}

void TraderEES::logon()
{
	if (!m_tradeApi)
	{
		spdlog::info("INVALID EES trade api object");
		return;
	}

	m_tradeApi->UserLogon(m_config->logonId.c_str(), m_config->logonPwd.c_str(), 
        m_config->appId.c_str(), m_config->localMac.c_str());
	spdlog::info("logon successful!");
}

bool TraderEES::loadEESTrader()
{
    m_handle =  dlopen(EES_TRADER_DLL_NAME, RTLD_LAZY);
    if (!m_handle)
	{
		printf("load library(%s) failed.\n", EES_TRADER_DLL_NAME);
		return false;
	}

	funcCreateEESTraderApi createFun = (funcCreateEESTraderApi)dlsym(m_handle, CREATE_EES_TRADER_API_NAME);
	if (!createFun)
	{
		printf("get function addresss(%s) failed!\n", CREATE_EES_TRADER_API_NAME);
		return false;
	}

	m_distoryFun = (funcDestroyEESTraderApi)dlsym(m_handle, DESTROY_EES_TRADER_API_NAME);
	if (!createFun)
	{
		printf("get function addresss(%s) failed!\n", DESTROY_EES_TRADER_API_NAME);
		return false;
	}

	m_tradeApi = createFun();
	if (!m_tradeApi)
	{
		printf("create trade API object failed!\n");
		return false;
	}
    return true;
}

int TraderEES::start()
{
    bool ret = loadEESTrader();
	if (!ret)
	{
		return TRADE_API_CREATE_FAILED;
	}

	// printf("about to get info %d %d %d\n", sizeof(EES_TradeSvrInfo), sizeof(char), sizeof(unsigned short));
    EES_TradeSvrInfo serverInfoUDP __attribute__((aligned (1)));
    serverInfoUDP.m_LocalTradeUDPPort = m_config->localUDPPort;

	// printf("about to get info3 %d\n", serverInfoUDP->m_remoteTradeTCPPort);
    serverInfoUDP.m_remoteTradeTCPPort = (m_config->tradeServerPort)+1; // don't use TCP
	// printf("about to get info4 %d\n", serverInfoUDP->m_remoteTradeUDPPort);
    serverInfoUDP.m_remoteTradeUDPPort = m_config->tradeServerPort;
	// printf("about to get info6 %d\n", serverInfoUDP->m_remoteQueryTCPPort);
    serverInfoUDP.m_remoteQueryTCPPort = m_config->queryServerPort;
	// printf("about to get info5 %s\n", m_config->queryServerIp.c_str());
    strcpy(serverInfoUDP.m_remoteQueryIp, m_config->queryServerIp.c_str());
	// printf("about to get info2 %s\n", m_config->tradeServerIp.c_str());
    strcpy(serverInfoUDP.m_remoteTradeIp, m_config->tradeServerIp.c_str());

	// printf("about to get info7 %s\n", m_config->localIp.c_str());
    strcpy(serverInfoUDP.m_LocalTradeIp, m_config->localIp.c_str());
	
	// printf("about to conn\n");
    RESULT ret_err = m_tradeApi->ConnServer(serverInfoUDP, this);
	// printf("conned\n");

    if (ret_err != NO_ERROR)
	{
		printf("connect to REM server failed!\n");
		return TRADE_API_CREATE_FAILED;
	}

    int waitTime = 0;
	while (m_logonStatus != 2 && m_logonStatus != 3)
	{
		usleep(1000*100);
		waitTime++;
		if (waitTime >= 50)
		{
			printf("wait for logon response timeout!\n");
			return TRADE_API_CREATE_FAILED;
		}
	}
	
	return 0;
}

void TraderEES::OnConnection(ERR_NO errNo, const char* pErrStr)
{
	if (errNo != NO_ERROR)
	{
		printf("connect to rem server failed(%d), %s!\n", errNo, pErrStr);
		return;
	}
	m_logonStatus = 1;
	logon();
}

void TraderEES::OnDisConnection(ERR_NO errNo, const char* pErrStr)
{
	printf("disconnect from rem server(%d), %s!\n", errNo,pErrStr);
	m_logonStatus = 3;
}

void TraderEES::OnUserLogon(EES_LogonResponse* pLogon)
{	

	if (pLogon->m_Result != NO_ERROR)
	{
		m_logonStatus = 3;
		printf("logon failed, result=%d\n", pLogon->m_Result);
		return;
	}
	m_logonStatus = 2;
	printf("logon successfully, trading date(%u), max token(%d)\n", pLogon->m_TradingDate, pLogon->m_MaxToken);

	// query positions
	m_tradeApi->QueryAccountPosition(m_config->logonId.c_str(), ++m_reqId);
}

void TraderEES::OnQueryAccountPosition(const char* pAccount, EES_AccountPosition* pAccountPosition, int nReqId, bool bFinish)
{
	printf("---------------------------------------------------------\n");
	printf("OnQueryAccountPosition\n");
	printf("Contract ID: %s\n", pAccountPosition->m_Symbol);
	printf("---------------------------------------------------------\n");

	std::string instrument(pAccountPosition->m_Symbol);
	if (pAccountPosition->m_PosiDirection == EES_PosiDirection_long)
	{
		m_currentHoldingStatus[instrument].longHolding = (pAccountPosition->m_OvnQty + pAccountPosition->m_TodayQty);
		m_currentHoldingStatus[instrument].longCloseable = m_currentHoldingStatus[instrument].longHolding - (pAccountPosition->m_FrozenOvnQty + pAccountPosition->m_FrozenTodayQty);
	} else {
		m_currentHoldingStatus[instrument].shortHolding = (pAccountPosition->m_OvnQty + pAccountPosition->m_TodayQty);
		m_currentHoldingStatus[instrument].shortCloseable = m_currentHoldingStatus[instrument].longHolding - (pAccountPosition->m_FrozenOvnQty + pAccountPosition->m_FrozenTodayQty);
	}

	if (!bFinish)
	{
		printf("Contract ID: %s\n", pAccountPosition->m_Symbol);
		printf("currentHoldingStatus: Long %d(%d), Short %d(%d)\n", 
		m_currentHoldingStatus[instrument].longHolding, m_currentHoldingStatus[instrument].longCloseable,
		m_currentHoldingStatus[instrument].shortHolding, m_currentHoldingStatus[instrument].shortCloseable);
	}
}

void TraderEES::OnOrderAccept(EES_OrderAcceptField* pAccept)
{
	m_clientToMarketTokenMap[pAccept->m_ClientOrderToken] = pAccept->m_MarketOrderToken;

	printf("---------------------------------------------------------\n");
	printf("OnOrderAccept\n");
	printf("---------------------------------------------------------\n");
	printf("ClientOrderToken:     %d\n", pAccept->m_ClientOrderToken);
	printf("REM Order ID    :     %lld\n", pAccept->m_MarketOrderToken);
	printf("Contract ID     :     %s\n", pAccept->m_Symbol);
	printf("price           :     %.4f\n", pAccept->m_Price);
	printf("quantity        :     %d\n", pAccept->m_Qty);
	printf("\n");
}

void TraderEES::OnOrderMarketAccept(EES_OrderMarketAcceptField* pAccept)
{
	EES_EnterOrderField orderField = m_clientTokenToOrderInfoMap[pAccept->m_ClientOrderToken];
	std::string instrument(orderField.m_Symbol);
	this->updateHoldingStatus(instrument, orderField.m_Side, 0, orderField.m_Qty);
}

void TraderEES::OnOrderReject(EES_OrderRejectField* pReject)
{
	printf("---------------------------------------------------------\n");
	printf("OnOrderReject\n");
	printf("---------------------------------------------------------\n");
	printf("ClientOrderToken:     %d\n", pReject->m_ClientOrderToken);	
	printf("ReasonCode      :     %d\n", int(pReject->m_ReasonCode));
	printf("GrammerText     :     %s\n", pReject->m_GrammerText);
	printf("RiskText        :     %s\n", pReject->m_RiskText);
	printf("\n");
}

void TraderEES::OnOrderMarketReject(EES_OrderMarketRejectField* pReject)
{
	printf("---------------------------------------------------------\n");
	printf("OnOrderMarketReject\n");
	printf("---------------------------------------------------------\n");
	printf("Exchange OrderSysID  :     %lld\n", pReject->m_MarketOrderToken);
	printf("Reason Text          :     %.100s\n", pReject->m_ReasonText);
	printf("\n");
}

void TraderEES::OnOrderExecution(EES_OrderExecutionField* pExec)
{
	// TODO: handle more general case
	// Now only handle mdDriven cases to provide open price
	printf("---------------------------------------------------------\n");
	printf("OnOrderExecution\n");
	printf("---------------------------------------------------------\n");
	printf("ClientOrderToken:     %d\n", pExec->m_ClientOrderToken);
	printf("REM Order ID    :     %lld\n", pExec->m_MarketOrderToken);
	printf("price           :     %.4f\n", pExec->m_Price);
	printf("quantity        :     %d\n", pExec->m_Quantity);
	printf("\n");

	EES_EnterOrderField orderField = m_clientTokenToOrderInfoMap[pExec->m_ClientOrderToken];
	std::string instrument(orderField.m_Symbol);
	this->updateHoldingStatus(instrument, orderField.m_Side, 1, orderField.m_Qty);

	Base_OrderExecutionField exec;
    // exec.m_Userid = 0;
    // exec.m_Timestamp = 0; // add later
    exec.m_ClientOrderToken = pExec->m_ClientOrderToken;
    exec.m_Quantity = pExec->m_Quantity;
    exec.m_Price = pExec->m_Price;

    (this->m_TraderCallbackQueue)->push(exec);
	// (this->m_TraderCallbackQueue)->push(*pExec);
}

void TraderEES::OnOrderCxled(EES_OrderCxled* pCxled)
{
	EES_EnterOrderField orderField = m_clientTokenToOrderInfoMap[pCxled->m_ClientOrderToken];
	std::string instrument(orderField.m_Symbol);
	this->updateHoldingStatus(instrument, orderField.m_Side, 2, orderField.m_Qty);
	printf("---------------------------------------------------------\n");
	printf("OnOrderCxled\n");
	printf("---------------------------------------------------------\n");
	printf("ClientOrderToken:     %d\n",   pCxled->m_ClientOrderToken);
	printf("MarketOrderToken:     %lld\n", pCxled->m_MarketOrderToken);
	printf("\n");
}

void TraderEES::OnCxlOrderReject(EES_CxlOrderRej* pReject)
{
	printf("---------------------------------------------------------\n");
	printf("OnCxlOrderReject\n");
	printf("---------------------------------------------------------\n");
	printf("m_MarketOrderToken:     %lld\n", pReject->m_MarketOrderToken);
	printf("m_ReasonCode      :     %u\n", pReject->m_ReasonCode);
	printf("m_ReasonText    :     %s\n", pReject->m_ReasonText);
	printf("\n");
}