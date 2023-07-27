#pragma once

#include "md/baseMarketDataListener.h"
#include "ctp/ThostFtdcMdApi.h"
#include "common/customQueue.h"
#include "common/Env.h"


struct CTPMdConfig
{
    std::string frontMdAddr;
    std::string brokerID;
    std::string userID;
    std::vector<std::string> instrumentIDs;
};

class CCTPMarketDataListener : public BaseMarketDataListener
{
public:
    CCTPMarketDataListener() {};
    virtual ~CCTPMarketDataListener() {};
    void setDataManager(CMyDataManager* dataManager) {this->dataManager = dataManager;};
    void setDataTaskManager(CDataTaskManager* dataTaskManager) {this->dataTaskManager = dataTaskManager;};
    void start();
    void dumpMarketData(CThostFtdcDepthMarketDataField* pDepthMarketData);

protected:
    bool exitFlag;
    bool aboutToCloseFlag;

private:
    CMyDataManager* dataManager;
    CDataTaskManager* dataTaskManager;
    CTPMdConfig m_config;
    unsigned int seq = 0;
    std::thread m_pushMdThread;
    void _pushMarketData();
    std::vector<CThostFtdcDepthMarketDataField> mdStorage;
    customQueue<CThostFtdcDepthMarketDataField> m_MdQueue;
    int pDataIndex = 0;
    int mdStorageSize = 0;
};

class CSimpleMdHandler : public CThostFtdcMdSpi
{
public:
	// ���캯������Ҫһ����Ч��ָ��CThostFtdcMduserApiʵ����ָ��
	CSimpleMdHandler(CThostFtdcMdApi *pUserApi, CCTPMarketDataListener* pMdListener) : m_pUserMdApi(pUserApi), m_pMdListener(pMdListener){}
	~CSimpleMdHandler() {}
	// ���ͻ����뽻���й�ϵͳ������ͨ�����ӣ��ͻ�����Ҫ���е�¼
	void deletemyself()
	{
		delete this;
	}

	virtual void OnFrontConnected()
	{
		// strcpy_s(g_chBrokerID, getConfig("config", "BrokerID").c_str());
		// strcpy_s(g_chUserID, getConfig("config", "UserID").c_str());
		// strcpy_s(g_chPassword, getConfig("config", "Password").c_str());
		//SetEvent(xinhao);
		// LOG("<OnFrontConnected>\n");
		// LOG("</OnFrontConnected>\n");
		// SetEvent(xinhao);
		//ReqUserLogin();
	}

	void RegisterFensUserInfo()
	{
		// strcpy_s(g_chBrokerID, getConfig("config", "BrokerID").c_str());
		// strcpy_s(g_chUserID, getConfig("config", "UserID").c_str());
		// CThostFtdcFensUserInfoField pFens = { 0 };
		// strcpy_s(pFens.BrokerID, g_chBrokerID);
		// strcpy_s(pFens.UserID, g_chUserID);
		// pFens.LoginMode = THOST_FTDC_LM_Trade;
		// m_pUserMdApi->RegisterFensUserInfo(&pFens);
	}

	void ReqUserLogin(CTPMdConfig config)
	{
		CThostFtdcReqUserLoginField reqUserLogin = { 0 };
		strcpy(reqUserLogin.BrokerID, config.brokerID.c_str());
		//strcpy_s(reqUserLogin.UserID, g_chUserID);
		//strcpy_s(reqUserLogin.UserID, g_chPassword);
		int num = m_pUserMdApi->ReqUserLogin(&reqUserLogin, 2);
		printf("\t[CTPMd] login num = %d\n", num);
	}

	void ReqUserLogout(CTPMdConfig config)
	{
		CThostFtdcUserLogoutField a = { 0 };
		strcpy(a.BrokerID, config.brokerID.c_str());
		strcpy(a.UserID, config.userID.c_str());
		int num = m_pUserMdApi->ReqUserLogout(&a, 1);
		printf("\t[CTPMd]logout num = %d\n", num);
	}

	virtual void OnHeartBeatWarning(int nTimeLapse)
	{
		printf("<OnHeartBeatWarning>\n");
		printf("\tnTimeLapse [%d]\n", nTimeLapse);
		printf("</OnHeartBeatWarning>\n");
	}

	// ���ͻ����뽻���й�ϵͳͨ�����ӶϿ�ʱ���÷���������
	virtual void OnFrontDisconnected(int nReason)
	{
		// ��������������API���Զ��������ӣ��ͻ��˿ɲ�������
		printf("<OnFrontDisconnected>\n");
		printf("\tnReason= = [%d]", nReason);
		printf("</OnFrontDisconnected>\n");
	}

	// ���ͻ��˷�����¼����֮�󣬸÷����ᱻ���ã�֪ͨ�ͻ��˵�¼�Ƿ�ɹ�
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		printf("<OnRspUserLogin>\n");
		if (pRspUserLogin)
		{
			printf("\tTradingDay [%s]\n", pRspUserLogin->TradingDay);
			printf("\tLoginTime [%s]\n", pRspUserLogin->LoginTime);
			printf("\tBrokerID [%s]\n", pRspUserLogin->BrokerID);
			printf("\tUserID [%s]\n", pRspUserLogin->UserID);
			printf("\tSystemName [%s]\n", pRspUserLogin->SystemName);
			printf("\tMaxOrderRef [%s]\n", pRspUserLogin->MaxOrderRef);
			printf("\tSHFETime [%s]\n", pRspUserLogin->SHFETime);
			printf("\tDCETime [%s]\n", pRspUserLogin->DCETime);
			printf("\tCZCETime [%s]\n", pRspUserLogin->CZCETime);
			printf("\tFFEXTime [%s]\n", pRspUserLogin->FFEXTime);
			printf("\tINETime [%s]\n", pRspUserLogin->INETime);
			printf("\tFrontID [%d]\n", pRspUserLogin->FrontID);
			printf("\tSessionID [%d]\n", pRspUserLogin->SessionID);
		}
		if (pRspInfo)
		{
			printf("\tErrorMsg [%s]\n", pRspInfo->ErrorMsg);
			printf("\tErrorID [%d]\n", pRspInfo->ErrorID);
		}
		printf("\tnRequestID [%d]\n", nRequestID);
		printf("\tbIsLast [%d]\n", bIsLast);
		printf("</OnRspUserLogin>\n");
		if (pRspInfo->ErrorID != 0) {
			// �˵�ʧ�ܣ��ͻ�������д�����
			printf("\tFailed to login, errorcode=%d errormsg=%s requestid=%d chain = %d",
				pRspInfo->ErrorID, pRspInfo->ErrorMsg, nRequestID, bIsLast);
			cin.get();
			cin.ignore();
			exit(-1);
		}
	}

	///�ǳ�������Ӧ
	virtual void OnRspUserLogout(CThostFtdcUserLogoutField* pUserLogout, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
	{
		printf("<OnRspUserLogout>\n");
		if (pUserLogout)
		{
			printf("\tBrokerID [%s]\n", pUserLogout->BrokerID);
			printf("\tUserID [%s]\n", pUserLogout->UserID);
		}
		if (pRspInfo)
		{
			printf("\tErrorMsg [%s]\n", pRspInfo->ErrorMsg);
			printf("\tErrorID [%d]\n", pRspInfo->ErrorID);
		}
		printf("\tnRequestID [%d]\n", nRequestID);
		printf("\tbIsLast [%d]\n", bIsLast);
		printf("</OnRspUserLogout>\n");
		//pUserApi->Release();
	}

	void SubscribeMarketData(std::vector<std::string> instruments)
	{
		int md_num = 0;
		char **ppInstrumentID = new char*[5000];
		for (int count1 = 0; count1 <= instruments.size() / 500; count1++)
		{
			if (count1 < instruments.size() / 500)
			{
				int a = 0;
				for (a; a < 500; a++)
				{
					ppInstrumentID[a] = const_cast<char *>(instruments.at(md_num).c_str());
					md_num++;
				}
				int result = m_pUserMdApi->SubscribeMarketData(ppInstrumentID, a);
				printf((result == 0) ? "Subscribe success\n" : "Subscribe failed with error=[%d]\n", result);
			}
			else if (count1 == instruments.size() / 500)
			{
				int count2 = 0;
				for (count2; count2 < instruments.size() % 500; count2++)
				{
					ppInstrumentID[count2] = const_cast<char *>(instruments.at(md_num).c_str());
					md_num++;
				}
				int result = m_pUserMdApi->SubscribeMarketData(ppInstrumentID, count2);
				printf((result == 0) ? "Subscribe success\n" : "Subscribe failed with error=[%d]\n", result);
			}
		}
	}

	virtual void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		printf("<OnRspUnSubMarketData>\n");
		if (pSpecificInstrument)
		{
			printf("\tInstrumentID [%s]\n", pSpecificInstrument->InstrumentID);
		}
		if (pRspInfo)
		{
			printf("\tErrorMsg [%s]\n", pRspInfo->ErrorMsg);
			printf("\tErrorID [%d]\n", pRspInfo->ErrorID);
		}
		printf("\tnRequestID [%d]\n", nRequestID);
		printf("\tbIsLast [%d]\n", bIsLast);
		printf("</OnRspUnSubMarketData>\n");
		// if (bIsLast)
		// {
		// 	SetEvent(xinhao);
		// }
	};

	void UnSubscribeMarketData()//������
	{
		// int md_num = 0;
		// char **ppInstrumentID = new char*[5000];
		// for (int count1 = 0; count1 <= md_InstrumentID.size() / 500; count1++)
		// {
		// 	if (count1 < md_InstrumentID.size() / 500)
		// 	{
		// 		int a = 0;
		// 		for (a; a < 500; a++)
		// 		{
		// 			ppInstrumentID[a] = const_cast<char *>(md_InstrumentID.at(md_num).c_str());
		// 			md_num++;
		// 		}
		// 		int result = m_pUserMdApi->SubscribeMarketData(ppInstrumentID, a);
		// 		LOG((result == 0) ? "������������1......���ͳɹ�\n" : "������������1......����ʧ�ܣ��������=[%d]\n", result);
		// 	}
		// 	else if (count1 == md_InstrumentID.size() / 500)
		// 	{
		// 		int count2 = 0;
		// 		for (count2; count2 < md_InstrumentID.size() % 500; count2++)
		// 		{
		// 			ppInstrumentID[count2] = const_cast<char *>(md_InstrumentID.at(md_num).c_str());
		// 			md_num++;
		// 		}
		// 		int result = m_pUserMdApi->SubscribeMarketData(ppInstrumentID, count2);
		// 		LOG((result == 0) ? "������������2......���ͳɹ�\n" : "������������2......����ʧ�ܣ��������=[%d]\n", result);
		// 	}
		// }
	}

	virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		printf("<OnRspSubMarketData>\n");
		if (pSpecificInstrument)
		{
			printf("\tInstrumentID [%s]\n", pSpecificInstrument->InstrumentID);
		}
		if (pRspInfo)
		{
			printf("\tErrorMsg [%s]\n", pRspInfo->ErrorMsg);
			printf("\tErrorID [%d]\n", pRspInfo->ErrorID);
		}
		printf("\tnRequestID [%d]\n", nRequestID);
		printf("\tbIsLast [%d]\n", bIsLast);
		printf("</OnRspSubMarketData>\n");
		// if (bIsLast)
		// {
		// 	SetEvent(xinhao);
		// }
	};

	///�������֪ͨ
	virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
	{
		//��ȡϵͳʱ��
		// SYSTEMTIME sys;
		// GetLocalTime(&sys);
		// LOG("%02d:%02d:%02d.%03d\t",
		// 	sys.wHour,
		// 	sys.wMinute,
		// 	sys.wSecond,
		// 	sys.wMilliseconds);
		printf("<OnRtnDepthMarketData>");
		if (pDepthMarketData)
		{
            // printf("\tInstrumentID [%s]\n", pDepthMarketData->InstrumentID);
            // printf("\tUpdateTime [%s]\n", pDepthMarketData->UpdateTime);
            // m_pMdListener->m_MdQueue.push(*pDepthMarketData);
            m_pMdListener->dumpMarketData(pDepthMarketData);

			// LOG("\tTradingDay [%s]\n", pDepthMarketData->TradingDay);
			// LOG("\tInstrumentID [%s]", pDepthMarketData->InstrumentID);
			// LOG("\tExchangeID [%s]", pDepthMarketData->ExchangeID);
			// LOG("\tExchangeInstID [%s]\n", pDepthMarketData->ExchangeInstID);
			// LOG("\tUpdateTime [%s]", pDepthMarketData->UpdateTime);
			// LOG("\tActionDay [%s]\n", pDepthMarketData->ActionDay);
			// LOG("\tVolume [%d]\n", pDepthMarketData->Volume);
			// LOG("\tUpdateMillisec [%d]", pDepthMarketData->UpdateMillisec);
			// LOG("\tBidVolume1 [%d]\n", pDepthMarketData->BidVolume1);
			// LOG("\tAskVolume1 [%d]\n", pDepthMarketData->AskVolume1);
			// LOG("\tBidVolume2 [%d]\n", pDepthMarketData->BidVolume2);
			// LOG("\tAskVolume2 [%d]\n", pDepthMarketData->AskVolume2);
			// LOG("\tBidVolume3 [%d]\n", pDepthMarketData->BidVolume3);
			// LOG("\tAskVolume3 [%d]\n", pDepthMarketData->AskVolume3);
			// LOG("\tBidVolume4 [%d]\n", pDepthMarketData->BidVolume4);
			// LOG("\tAskVolume4 [%d]\n", pDepthMarketData->AskVolume4);
			// LOG("\tBidVolume5 [%d]\n", pDepthMarketData->BidVolume5);
			// LOG("\tAskVolume5 [%d]\n", pDepthMarketData->AskVolume5);
			// LOG("\tLastPrice [%.8lf]\n", (pDepthMarketData->LastPrice > 10000000) ? 0 : pDepthMarketData->LastPrice);
			// LOG("\tPreSettlementPrice [%.8lf]\n", (pDepthMarketData->PreSettlementPrice > 10000000) ? 0 : pDepthMarketData->PreSettlementPrice);
			// LOG("\tPreClosePrice [%.8lf]\n", (pDepthMarketData->PreClosePrice > 10000000) ? 0 : pDepthMarketData->PreClosePrice);
			// LOG("\tPreOpenInterest [%.8lf]\n", (pDepthMarketData->PreOpenInterest > 10000000) ? 0 : pDepthMarketData->PreOpenInterest);
			// LOG("\tOpenPrice [%.8lf]\n", (pDepthMarketData->OpenPrice > 10000000) ? 0 : pDepthMarketData->OpenPrice);
			// LOG("\tHighestPrice [%.8lf]\n", (pDepthMarketData->HighestPrice > 10000000) ? 0 : pDepthMarketData->HighestPrice);
			// LOG("\tLowestPrice [%.8lf]\n", (pDepthMarketData->LowestPrice > 10000000) ? 0 : pDepthMarketData->LowestPrice);
			// LOG("\tTurnover [%.8lf]\n", (pDepthMarketData->Turnover > 10000000) ? 0 : pDepthMarketData->Turnover);
			// LOG("\tOpenInterest [%.8lf]\n", (pDepthMarketData->OpenInterest > 10000000) ? 0 : pDepthMarketData->OpenInterest);
			// LOG("\tClosePrice [%.8lf]\n", (pDepthMarketData->ClosePrice > 10000000) ? 0 : pDepthMarketData->ClosePrice);
			// LOG("\tSettlementPrice [%.8lf]\n", (pDepthMarketData->SettlementPrice > 10000000) ? 0 : pDepthMarketData->SettlementPrice);
			// LOG("\tUpperLimitPrice [%.8lf]\n", (pDepthMarketData->UpperLimitPrice > 10000000) ? 0 : pDepthMarketData->UpperLimitPrice);
			// LOG("\tLowerLimitPrice [%.8lf]\n", (pDepthMarketData->LowerLimitPrice > 10000000) ? 0 : pDepthMarketData->LowerLimitPrice);
			// LOG("\tPreDelta [%.8lf]\n", (pDepthMarketData->PreDelta > 10000000) ? 0 : pDepthMarketData->PreDelta);
			// LOG("\tCurrDelta [%.8lf]\n", (pDepthMarketData->CurrDelta > 10000000) ? 0 : pDepthMarketData->CurrDelta);
			// LOG("\tBidPrice1 [%.8lf]\n", (pDepthMarketData->BidPrice1 > 10000000) ? 0 : pDepthMarketData->BidPrice1);
			// LOG("\tAskPrice1 [%.8lf]\n", (pDepthMarketData->AskPrice1 > 10000000) ? 0 : pDepthMarketData->AskPrice1);
			// LOG("\tBidPrice2 [%.8lf]\n", (pDepthMarketData->BidPrice2 > 10000000) ? 0 : pDepthMarketData->BidPrice2);
			// LOG("\tAskPrice2 [%.8lf]\n", (pDepthMarketData->AskPrice2 > 10000000) ? 0 : pDepthMarketData->AskPrice2);
			// LOG("\tBidPrice3 [%.8lf]\n", (pDepthMarketData->BidPrice3 > 10000000) ? 0 : pDepthMarketData->BidPrice3);
			// LOG("\tAskPrice3 [%.8lf]\n", (pDepthMarketData->AskPrice3 > 10000000) ? 0 : pDepthMarketData->AskPrice3);
			// LOG("\tBidPrice4 [%.8lf]\n", (pDepthMarketData->BidPrice4 > 10000000) ? 0 : pDepthMarketData->BidPrice4);
			// LOG("\tAskPrice4 [%.8lf]\n", (pDepthMarketData->AskPrice4 > 10000000) ? 0 : pDepthMarketData->AskPrice4);
			// LOG("\tBidPrice5 [%.8lf]\n", (pDepthMarketData->BidPrice5 > 10000000) ? 0 : pDepthMarketData->BidPrice5);
			// LOG("\tAskPrice5 [%.8lf]\n", (pDepthMarketData->AskPrice5 > 10000000) ? 0 : pDepthMarketData->AskPrice5);
			// LOG("\tAveragePrice [%.8lf]\n", (pDepthMarketData->AveragePrice > 10000000) ? 0 : pDepthMarketData->AveragePrice);
		}
		printf("</OnRtnDepthMarketData>\n");
	};

	virtual void OnRspSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{};

	virtual void OnRspUnSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{};

	///ѯ��֪ͨ
	virtual void OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp)
	{};

	virtual void OnRspQryMulticastInstrument(CThostFtdcMulticastInstrumentField *pMulticastInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{};

private:
	// ָ��CThostFtdcMduserApiʵ����ָ��
	CThostFtdcMdApi *m_pUserMdApi;
    CCTPMarketDataListener* m_pMdListener;
};