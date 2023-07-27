#pragma once
#include "dcel1/dcel1api.h"
#include "common/mydatamanager.h"
#include "common/dataTaskManager.h"
#include "common/Env.h"
#include "md/baseMarketDataListener.h"

struct MultiCastConfig
{
    std::string MulticastIP;
    std::string BindAddr;
};

struct ApiConfig
{
    // 基本信息服务地址
    std::string BaseInfoIP;
    uint16_t BaseInfoPort;
    std::vector<MultiCastConfig> MCGroups;
    uint16_t SnapshotPort;
    // 目前使用的两路组播数据到达的是同一个端口
    uint16_t L1Port;
    std::string L1NIC;
};

class DCEL1Listener : public CDCEL1Spi, public BaseMarketDataListener
{
public:
    void setDataManager(CMyDataManager* dataManager);
    void setDataTaskManager(CDataTaskManager* dataTaskManager);
    void start();
    virtual void OnMarketData(const char* contractID, const MarketDataSnapshot* snapshot);

    void OnContractSeqBroken(const char* contractID, uint32_t contractNo, int64_t lastValidSeqNo, int64_t comingSeqNo) override
    {
        // m_Logger.warn("contract = {} encounter seq broken, lastseq = {}, coming = {} ", contractID, lastValidSeqNo, comingSeqNo);
    }
    void OnContractSeqReady(const char* contractID, uint32_t contractNo, int64_t contractSeqNo) override
    {
        // printf("contract = %s init ready /recover sequence, seq = %d\n", contractID, contractSeqNo);
        // m_Logger.info("contract = {} init ready /recover sequence, seq = {}", contractID, contractSeqNo);
    }

private:
    CMyDataManager* dataManager;
    CDataTaskManager* dataTaskManager;
    unsigned int seq;
};