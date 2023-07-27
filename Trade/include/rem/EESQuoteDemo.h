#pragma once

// #ifdef WIN32
// /// add by zhou.hu review 2014/4/22 Windwos平台锟斤拷锟矫碉拷头锟侥硷拷
// #include <Windows.h>

// typedef HMODULE		T_DLL_HANDLE;

// #else
/// add by zhou.hu review 2014/4/22 linux平台锟斤拷锟矫碉拷头锟侥硷拷
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <string.h>

typedef void* T_DLL_HANDLE;

// #endifs
#include <fstream>
#include <string>
#include <mutex>
#include <map>
#include <vector>
#include <algorithm>
#include "trade/rem/include/EESQuoteApi.h"
#include "json/json.hpp"
#include "md/baseMarketDataListener.h"
#include "common/customQueue.h"
#include "common/Env.h"

using namespace std;
using nlohmann::json;

#define INSTRUMENT1 "rb2305"
#define INSTRUMENT2 "y2309"

class MyMarketDataListener : public EESQuoteEvent,public BaseMarketDataListener
{
public:
    MyMarketDataListener();
    virtual ~MyMarketDataListener();
    void setDataManager(CMyDataManager* dataManager) {this->dataManager = dataManager;};
    void setDataTaskManager(CDataTaskManager* dataTaskManager) {this->dataTaskManager = dataTaskManager;};
    void start();
    void dumpMarketData(EESMarketDepthQuoteData* pDepthMarketData);
    

private:
    // EES锟斤拷锟斤拷API锟接匡拷
    EESQuoteApi* m_quoteApi ;
    EESQuoteEvent* myevent;
    //行情服务器登录信息
    EqsLoginParam LoginParam;
    EqsTcpInfo TcpInfo;

    T_DLL_HANDLE				m_handle;				///< EES锟斤拷锟斤拷API锟斤拷锟�
	funcDestroyEESQuoteApi		m_distoryFun;
   
    //合约类型
    EesEqsIntrumentType  IntrumentType;
    //合约号
    vector<string> mpSymbols;
    //查询结果
    map<EesEqsIntrumentType,vector<string>> m_symbolsMap;



    customQueue<EESMarketDepthQuoteData> m_MdQueue ;

    CMyDataManager* dataManager;
    CDataTaskManager* dataTaskManager;
    unsigned int seq = 0;

    


    
 

private:
    //行情服务器初始化
    bool Init();
    //用户登录
    void Userlogin();
    //
    void Close();
    //查询合约列表
    void Query();
    //注册合约列表
    void Register();
    //注销合约列表
    void UNregister();

    //加载动态库
    bool LoadEESQuote();
    //卸载动态库
    bool UNloadEESQuote();

    void _pushMarketData();


private:
    //锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟铰硷拷
    virtual void OnEqsConnected();
    //锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟较匡拷
    virtual void OnEqsDisconnected();
    //锟斤拷录锟斤拷息锟侥回碉拷
    virtual void OnLoginResponse(bool bSuccess, const char* pReason);
    //锟秸碉拷锟斤拷锟斤拷幕氐锟�
    virtual void OnQuoteUpdated(EesEqsIntrumentType chInstrumentType, EESMarketDepthQuoteData* pDepthQuoteData);
    // 锟斤拷志
    virtual void OnWriteTextLog(EesEqsLogLevel nlevel, const char* pLogText, int nLogLen);
    // 注锟斤拷symbol锟斤拷应锟斤拷息
    virtual void OnSymbolRegisterResponse(EesEqsIntrumentType chInstrumentType, const char* pSymbol, bool bSuccess);
    //注锟斤拷symbol锟斤拷应锟斤拷息锟斤拷时锟斤拷锟斤拷
    virtual void OnSymbolUnregisterResponse(EesEqsIntrumentType chInstrumentType, const char* pSymbol, bool bSuccess);
    //锟斤拷询symbol锟叫憋拷锟斤拷应锟斤拷息锟斤拷时锟斤拷锟斤拷
    virtual void OnSymbolListResponse(EesEqsIntrumentType chInstrumentType, const char* pSymbol, bool bLast);







   
};