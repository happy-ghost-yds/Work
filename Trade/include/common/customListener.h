#ifndef _CUSTOMLISTENER_H_
#define _CUSTOMLISTENER_H_

#include <mutex>
#include "common/mydatamanager.h"
#include "common/ConfigUtil.h"
#include "common/dataTaskManager.h"

bool bind_cpu(int cpu_id, pthread_t thd_id);
void pexit(const char* msg, int sockfd);

class CustomListener
{
public:
    void setDataManager(CMyDataManager* dataManager);
    void setDataTaskManager(CDataTaskManager* dataTaskManager);
    void setConfig(ConfigUtil* config);
    void start();

private:
    void exitOnKeyboard();
    void processData(FILE* mFile);
    CMyDataManager* dataManager;
    CDataTaskManager* dataTaskManager;
    ConfigUtil* config;
    mutex m_Mutex;
    bool exitFlag;
    bool aboutToCloseFlag;
};

#endif