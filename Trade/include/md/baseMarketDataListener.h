#pragma once

#include "common/mydatamanager.h"
#include "common/dataTaskManager.h"

//fwd dclr
struct Mydata; 

class BaseMarketDataListener
{
public:
    BaseMarketDataListener() {};
    virtual ~BaseMarketDataListener() {};
    void setDataManager(CMyDataManager* dataManager) {this->dataManager = dataManager;};
    void setDataTaskManager(CDataTaskManager* dataTaskManager) {this->dataTaskManager = dataTaskManager;};
    virtual void start() {};

protected:
    bool exitFlag;
    bool aboutToCloseFlag;

private:
    CMyDataManager* dataManager;
    CDataTaskManager* dataTaskManager;
};