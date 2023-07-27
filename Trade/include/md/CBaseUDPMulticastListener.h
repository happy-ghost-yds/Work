#pragma once
#include <string>
#include "md/baseMarketDataListener.h"
#include "common/dataTaskManager.h"
#include "common/mydatamanager.h"
#include "common/customListener.h"


class CBaseUDPMulticastListener : public BaseMarketDataListener {
public:
    CBaseUDPMulticastListener(const std::string& multicast_address, const int multicast_port);
    virtual ~CBaseUDPMulticastListener();

    void setDataManager(CMyDataManager* dataManager);
    void setDataTaskManager(CDataTaskManager* dataTaskManager);
    void setConfig(ConfigUtil* config);
    virtual void Start();

private:
    std::string multicast_address_;
    int multicast_port_;
    int sockfd_;
    
    void exitOnKeyboard();
    void processData(FILE* mFile);
    CMyDataManager* dataManager;
    CDataTaskManager* dataTaskManager;
    ConfigUtil* config;
    mutex m_Mutex;
    bool exitFlag;
    bool aboutToCloseFlag;

};


