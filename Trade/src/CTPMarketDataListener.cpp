#include <unistd.h>
#include <thread>

#include "md/CTPMarketDataListener.h"


#define INSTRUMENT1 "rb2305"
#define INSTRUMENT2 "y2309"

void CCTPMarketDataListener::start()
{
    m_pushMdThread = std::thread(&CCTPMarketDataListener::_pushMarketData, this);
    
    Env& env = Env::getInstance();
    m_config = {};
    m_config.frontMdAddr = env["mdCTP"]["frontMdAddr"]; // "tcp://192.88.10.18:61616"; //"tcp://27.115.78.100:21616";
    m_config.brokerID = env["mdCTP"]["brokerID"];
    m_config.userID = env["mdCTP"]["userID"];
    std::vector<std::string> _instrumentIDs = env["mdCTP"]["instrumentIDs"];
    m_config.instrumentIDs = _instrumentIDs;

    CThostFtdcMdApi *pUserMdApi = CThostFtdcMdApi::CreateFtdcMdApi();
    CSimpleMdHandler mdHandler(pUserMdApi, this);
    pUserMdApi->RegisterSpi(&mdHandler);
    pUserMdApi->RegisterFront(const_cast<char *>(m_config.frontMdAddr.c_str()));
    pUserMdApi->Init();
    sleep(5);
    // WaitForSingleObject(xinhao, INFINITE);
    mdHandler.ReqUserLogin(m_config);
    sleep(5);
    // WaitForSingleObject(xinhao, INFINITE);
    //��ʼ��ȡ��Լ
    mdHandler.SubscribeMarketData(m_config.instrumentIDs);
    // for(;;)
    // {
    //     std::this_thread::yield();
    // }
    pUserMdApi->Join();
    pUserMdApi->Release();
}

void CCTPMarketDataListener::_pushMarketData()
{
    while (true)
    {
        CThostFtdcDepthMarketDataField depthMarketData = m_MdQueue.pop();
        Mydata mydata = { 0 };
        unsigned int n = sizeof(CThostFtdcDepthMarketDataField);
        memcpy(&mydata.datas, &depthMarketData, sizeof(CThostFtdcDepthMarketDataField));
        memcpy(&mydata.datas[n], (char*)&seq, sizeof(unsigned int)); 
        clock_gettime(CLOCK_REALTIME, &mydata.time);
        mydata.len = n;
        if ((getTextField(mydata, DATA_FIELD_INSTRUMENT_NAME) != INSTRUMENT1) && (getTextField(mydata, DATA_FIELD_INSTRUMENT_NAME) != INSTRUMENT2))
        {
            // std::cout << "symbol not match " << getTextField(mydata, DATA_FIELD_INSTRUMENT_NAME) << std::endl;
            return;
        }
        // std::cout << "upd time=" << snapshot->UpdTime << std::endl;
        dataTaskManager->push(mydata);
        ++seq;
        // ++pDataIndex;
    }
}

void CCTPMarketDataListener::dumpMarketData(CThostFtdcDepthMarketDataField* pDepthMarketData)
{
    // printf("\t[CL] InstrumentID [%s]\n", pDepthMarketData->InstrumentID);
    // printf("\t[CL] UpdateTime [%s]\n", pDepthMarketData->UpdateTime);
    m_MdQueue.push(*pDepthMarketData);
    // mdStorage.push_back(*pDepthMarketData);
    // mdStorageSize++;
}