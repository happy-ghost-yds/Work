#pragma once

#include "md/baseMarketDataListener.h"
#include "md/nPro/include/nProPlus.h"
#include "common/customQueue.h"
#include "common/Env.h"

#define RAW_DATA_LEN 112
#define ARRLEN 2048

struct nProMdConfig
{
    std::string nic_interface;
    int exchange_type;
    std::string serverIP;
    bool if_restart_fpga;
	int md_bind_cpu;
	std::string license_filepath;
	std::vector<std::string> instrumentIDs;
};

struct marketData
{
    uint64_t timestamp;
    char md_buffer[256];
    InsrItem* insr;
};

class CnProMarketDataListener : public BaseMarketDataListener
{
public:
    CnProMarketDataListener() {};
    virtual ~CnProMarketDataListener() {};
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
    nProMdConfig m_config;
	uint32_t volatile m_arrHeader = 0;
	marketData m_mdArray[ARRLEN];
    unsigned int seq = 0;
    std::thread m_pushMdThread;
    void _pushMarketData();
    std::vector<CThostFtdcDepthMarketDataField> mdStorage;
    customQueue<CThostFtdcDepthMarketDataField> m_MdQueue;
    int pDataIndex = 0;
    int mdStorageSize = 0;
};