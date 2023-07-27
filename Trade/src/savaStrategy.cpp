#include "strategy/saveStrategy.h"

saveStrategyProcessor::saveStrategyProcessor(EES_ExchangeID exchangeId):CStrategyProcessor(exchangeId)
{

}

saveStrategyProcessor::~saveStrategyProcessor()
{

}

void saveStrategyProcessor::setDataManager(CMyDataManager* dataManager)
{
    this->dataManager = dataManager;
}
void saveStrategyProcessor::setDataTaskManager(CDataTaskManager* dataTaskManager)
{
    this->dataTaskManager = dataTaskManager;
}

void saveStrategyProcessor::processData(FILE* mFile)
{
    vector<Mydata>* datalist = dataManager->GetDatas();
    if (nullptr != datalist)
    {
        for (auto iter = datalist->begin(); iter != datalist->end(); iter++)
        {
            Mydata data = *iter;
            fwrite(&data.time, sizeof(timespec),
                1, mFile);
                
            fwrite(&data.len, sizeof(int),
                1, mFile);
                
            fwrite(&data.datas, data.len,
                1, mFile);
        }  
            
        delete datalist;
        fflush(mFile);
        }

}