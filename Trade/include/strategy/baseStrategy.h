#pragma once

#include <vector>
#include <string.h>
#include <fstream>
#include <iostream>
#include "common/mydatamanager.h"
#include "common/customQueue.h"
#include "execution/ees.h"
#include "execution/baseTrader.h"
//forward declare
struct Mydata; 
class BaseTrader;

struct BaseConfig
{
    EES_ExchangeID exchangeId;
    std::string limitPriceFilename;
};

class CStrategyProcessor
{
private:
    BaseConfig m_config;

protected:
    EES_ExchangeID m_exchangeId; //TODO: not limiting to EES
    customQueue<Base_OrderExecutionField> m_TraderCallbackQueue;

public:
    CStrategyProcessor(EES_ExchangeID exchangeId){
        this->m_exchangeId = exchangeId;
        m_config.limitPriceFilename = "/root/data/limitPrices.dat";
        this->getLimitPrices();
    };
    virtual ~CStrategyProcessor();
    void setTrader(BaseTrader* trader){
        this->trader = trader;
        m_config.exchangeId = trader->getExchangeId();
        trader->m_TraderCallbackQueue = &(this->m_TraderCallbackQueue);
    };
    void getLimitPrices() {
        // format is 3x lines like:
        // a2305
        // 123.5 (lowerlimit)
        // 456.0 (higherlimit)
        std::ifstream inputFile(m_config.limitPriceFilename.c_str());
        std::string instrument;
        double lowerLimitPrice, higherLimitPrice;
        
        while (!inputFile.eof())
        {
            // getline(inputFile, instrument);
            // inputFile >> lowerLimitPrice >> higherLimitPrice;
            inputFile >> instrument >> lowerLimitPrice >> higherLimitPrice;
            instrumentLimitPrices[instrument][EES_SideType_open_long] = higherLimitPrice;
            instrumentLimitPrices[instrument][EES_SideType_close_today_short] = higherLimitPrice;
            instrumentLimitPrices[instrument][EES_SideType_close_ovn_short] = higherLimitPrice;
            instrumentLimitPrices[instrument][EES_SideType_open_short] = lowerLimitPrice;
            instrumentLimitPrices[instrument][EES_SideType_close_today_long] = lowerLimitPrice;
            instrumentLimitPrices[instrument][EES_SideType_close_ovn_long] = lowerLimitPrice;
            std::cout << "Read Limit Prices:" << instrument << " " << lowerLimitPrice << " " << higherLimitPrice << std::endl;
        }
        inputFile.close();
    };
    double getInstrumentLimitPrice(std::string instrument, int side) {
        return instrumentLimitPrices[instrument][(side > 0) ? EES_SideType_open_long : EES_SideType_open_short];
    };

    virtual void processData(std::vector<Mydata>* data, int lastIndex) {};

public:
    BaseTrader* trader;
    std::map<std::string, std::map<EES_SideType, double>> instrumentLimitPrices;
    bool isPaperTrading = false;
};