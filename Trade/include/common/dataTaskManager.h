#pragma once

#include "common/customQueue.h"
#include "common/mydatamanager.h"

class CDataTaskManager
{
private:
    customQueue<Mydata> m_queue;
    CMyDataManager* m_dataManager;
    bool exitFlag;
public:
    CDataTaskManager(CMyDataManager* dataManager);
    // ~CDataTaskManager();
    void push(Mydata& data);
    void start();
    void shutdown();
};