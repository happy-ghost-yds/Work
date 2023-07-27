#ifndef _NHMDLISTENER_H_
#define _NHMDLISTENER_H_

#include <mutex>
#include "common/mydatamanager.h"
#include "common/ConfigUtil.h"
#include "common/customListener.h"
#include "common/dataTaskManager.h"
#include "md/baseMarketDataListener.h"

class NhMdListener : public BaseMarketDataListener
{
public:
    void setDataManager(CMyDataManager* dataManager) {this->dataManager = dataManager;};
    void setDataTaskManager(CDataTaskManager* dataTaskManager) {this->dataTaskManager = dataTaskManager;};
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