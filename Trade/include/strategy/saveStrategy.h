#pragma once

#include <map>
#include <set>
#include <vector>
#include <deque>
#include <thread>
#include <mutex>
#include <string.h>
#include "common/mydatamanager.h"
#include "common/customQueue.h"
#include "strategy/baseStrategy.h"
#include "common/dataTaskManager.h"

class saveStrategyProcessor : public CStrategyProcessor
{
public:
    saveStrategyProcessor(Base_ExchangeID exchangeId);
    virtual ~saveStrategyProcessor();
    void setDataManager(CMyDataManager* dataManager);
    void setDataTaskManager(CDataTaskManager* dataTaskManager);

private:
    void processData(FILE* mFile);

    CMyDataManager* dataManager;
    CDataTaskManager* dataTaskManager;

};