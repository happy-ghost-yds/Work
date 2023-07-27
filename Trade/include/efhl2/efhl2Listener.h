#pragma once
#include <stdio.h>
#include "efhl2/DFITCL2_Open_ApiDataType.h"
#include "efhl2/i_sl_dce_api.h"
#include "efhl2/ring_buffer.h"
#include <thread>
#include <unistd.h>
#include "efhl2/getconfig.h"
#include "md/baseMarketDataListener.h"
#include "common/customQueue.h"
#include <iostream>

using namespace std;

const unsigned long long InValid_Uint64_Value = 0x7FEFFFFFFFFFFFFF;
const double             InValid_double_value = 9999999999999999.0;

//无效价格检查及替换
#define INVALID_PRICE_CHECK_AND_EXCHANGE(a) ((*(unsigned long long*)&a) == InValid_Uint64_Value ? InValid_double_value:a)

class EFHL2marketDatalistener : public i_sl_dce_api_event,public BaseMarketDataListener
{
public:
    EFHL2marketDatalistener();
    virtual ~EFHL2marketDatalistener();
    void setDataManager(CMyDataManager* dataManager) {this->dataManager = dataManager;};
    void setDataTaskManager(CDataTaskManager* dataTaskManager) {this->dataTaskManager = dataTaskManager;};
    void start();
    void dumpMarketData(MDBest* pDepthMarketData);
public:
    bool init();

    virtual void OnBest(MDBest* const pQuote);

    virtual void OnDeep(MDDeep* const pQuote);

    virtual void OnArbiBest(ArbiMDBest* const pQuote);

    virtual void OnOptParam(MDOptParam* const pQuote);

    virtual void OnTenEntrust(MDTenEntrust* const pQuote);

    virtual void OnOrderStatistic(MDOrderStatistic* const pQuote);

    virtual void OnMatchPriceQty(MDMatchPriceQty* const pQuote);

    virtual void OnErrorMsg(const char* msg);

    virtual void OnDebugMsg(const char* msg);

    virtual void OnInfoMsg(const char* msg);



    void close();

    

private:
    void _pushMarketData();

private:
    string          ConfigFilename;
    std::thread m_pushMdThread;
    unsigned int seq = 0;

    customQueue<DFITC_MARKETDATA_API::MDBest> m_MdQueue;


    CMyDataManager* dataManager;
    CDataTaskManager* dataTaskManager;

};