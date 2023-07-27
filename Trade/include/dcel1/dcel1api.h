#pragma once
#include <cstdint>
#include "marketdata.h"

#define MDAPI __attribute((visibility("default")))

class CDCEL1Spi
{
public:
    virtual void OnMarketData(const char* contractID, const MarketDataSnapshot* snapshot)
    {

    }
    virtual void OnContractSeqBroken(const char* contractID, uint32_t contractNo, int64_t lastValidSeqNo, int64_t comingSeqNo)
    {

    }
    virtual void OnContractSeqReady(const char* contractID, uint32_t contractNo, int64_t contractSeqNo)
    {
        
    }
    virtual ~CDCEL1Spi() = default;
};



class MDAPI CDCEL1Api 
{
public:
    static CDCEL1Api* CreateApi();
    static const char* GetApiVersion();
    virtual void Release() = 0;
    virtual void RegisterSpi(CDCEL1Spi* spi) = 0;
    virtual bool BaseInfoInit(const char* ip, uint16_t port) = 0;
    virtual bool RegisterMultiCastRecv(const char* mcip, const char* mcbindaddr) = 0;
    virtual bool RegisterSnapshotRecv(uint16_t port, const char* host) = 0;
    virtual bool RegisterL1Recv(uint16_t port, const char* host) = 0;
    virtual bool RegisterL1RecvPromisc(uint16_t port, const char* nic) = 0;
    virtual void Init() = 0;
    virtual void Stop() = 0;
protected:
    ~CDCEL1Api() = default;
};