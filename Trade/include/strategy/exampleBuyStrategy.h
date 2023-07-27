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
#include "execution/ees.h"
#include "strategy/baseStrategy.h"
#include "common/Env.h"

//forward declare
struct Mydata; 
class TraderEES;

struct ExampleBuyStrategyConfig
{
    std::string instrument;
    unsigned int targetInventory;
    double minPriceChange;
    double multiplier;
};

struct OrderInfo
{
    EES_ClientToken openOrderClientToken;
    int direction;
    unsigned int openVolume;
    unsigned int pendingVolume;
    unsigned int price; // actual price divided by minPriceChange
};

class CExampleBuyStrategyProcessor: public CStrategyProcessor
{
private:
    ExampleBuyStrategyConfig m_exampleBuyConfig;
    unsigned int currentHolding = 0;

public:
    CExampleBuyStrategyProcessor(Base_ExchangeID exchangeId);
    ~CExampleBuyStrategyProcessor();
    void setTrader(BaseTrader* trader);
    void startTraderCallbackWorker();
    void processData(std::vector<Mydata>* data, int lastIndex);
    unsigned int getInstrumentLimitPrice(std::string instrument, int side);

private:
    bool isPaperTrading = false;
    bool isTraderCallbackStarted = false;
    std::thread m_callbackThread;
    std::vector<int> localMdIndex;
    std::map<int, OrderInfo> m_openOrders;
    std::set<unsigned int> m_activePriceLevels;
    std::set<unsigned int> m_toRemovePriceLevels;
    void _traderCallbackWorker();
    void _processTraderCallback(Base_OrderExecutionField* pExec);
};