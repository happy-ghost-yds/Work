#include "common/dataTaskManager.h"
#include "common/mydatamanager.h"
#include "common/customListener.h"

#include <iostream>
#include <unistd.h>

#define TASKMANAGER_CPU_ID 1

CDataTaskManager::CDataTaskManager(CMyDataManager* dataManager)
{
    m_dataManager = dataManager;
}

void CDataTaskManager::start()
{
    // bind CPU
    if (false == bind_cpu(TASKMANAGER_CPU_ID, pthread_self()))
    {

        pexit("Error: TM bind cpu error.\n", 0);
    }

    bool isTestForwarding = false;
    exitFlag = false;
    if (isTestForwarding)
    {
        while (1)
        {
            Mydata rData = m_queue.pop();
            //fake processing by sleeping
            usleep(250);
        }
    } else {
        while (!exitFlag)
        {
            Mydata rData = m_queue.pop();
            if (exitFlag) break;
            // logDataID(&rData, "TMpop");
            // std::cout << "calling dataManager SaveData\n";
            m_dataManager->SaveData(std::ref(rData));
        }
    }
    std::cout << "DataTaskManager exited" << std::endl;
}

void CDataTaskManager::push(Mydata& data)
{
    // logDataID(&data, "TMpush");
    m_queue.push(data);
}

void CDataTaskManager::shutdown()
{
    exitFlag = true;
    m_queue.shutdown();
}

