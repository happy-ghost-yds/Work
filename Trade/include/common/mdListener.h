#ifndef _MDLISTENER_H_
#define _MDLISTENER_H_

#include <mutex>
#include "mydatamanager.h"

class MDListener
{
private:
    CMyDataManager* dataManager;
    mutex m_Mutex;
    void exitOnKeyboard();
    bool exitFlag;

public:
    void setDataManager(CMyDataManager* dataManager);
    void start();
};

#endif