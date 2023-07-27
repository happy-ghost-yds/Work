#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <unistd.h>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

#include "common/ConfigUtil.h"
#include "nhmd/NhMdListener.h"
#include "common/customListener.h"
#include "dcel1/dcel1Listener.h"
#include "common/mydatamanager.h"
#include "common/dataTaskManager.h"
#include "strategy/exampleBuyStrategy.h"
#include "execution/ees.h"
#include "common/Env.h"

#include "md/CTPMarketDataListener.h"
#include "md/nProMarketDataListener.h"
#include "md/DFITCMarketDataListener.hpp"
#include "execution/CTPTrader.h"
#include "md/EFHL2MarketDataListener.h"
#include "rem/EESQuoteDemo.h"


int main()
{   
    Env& env = Env::getInstance();
    env.loadFromFile("src/EnvConfig.json");

    // get time
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer,sizeof(buffer),"%d-%m-%Y-%H-%M-%S",timeinfo);
    std::string logfilename(buffer);
    // set logger
    auto new_logger = spdlog::basic_logger_mt("new_default_logger", "logs/log-" + logfilename + ".txt", true);
    spdlog::set_default_logger(new_logger);
    spdlog::info("hello world");
    // // read account and config info

    std::string outputFileName = "coutlog.txt";
    std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
    std::ofstream out(outputFileName.c_str());
    std::cout.rdbuf(out.rdbuf());

    // initialize marketdata thread
    std::string title_dce1 = "mdDCE1"; //DCE local multicast
    std::string title_dce2 = "mdDCE2"; //DCE receive forwarded message from SHFE
    std::string title_shfe1 = "mdSHFE1"; //SHFE local multicast
    std::string title_shfe2 = "mdSHFE2"; //SHFE receive forwarded message from SHFE
    
    ConfigUtil mdConfigDCE1(title_dce1);
    ConfigUtil mdConfigDCE2(title_dce2);
    ConfigUtil mdConfigSHFE1(title_shfe1);
    ConfigUtil mdConfigSHFE2(title_shfe2);
    mdConfigDCE1.loadConfig();
    // mdConfigDCE1.printConfig();
    mdConfigDCE2.loadConfig();
    // mdConfigDCE2.printConfig();
    mdConfigSHFE1.loadConfig();
    // mdConfigSHFE1.printConfig();
    mdConfigSHFE2.loadConfig();
    // mdConfigSHFE2.printConfig();

    char hostname[100];
    gethostname(hostname, 100);
    std::cout << "Hostname:" << hostname << " " << strlen(hostname) << "\n";
    bool isSHFE = (strlen(hostname) < 7);

    // strategy engine
    std::string buyStrategyType = env["buyStrategyType"];

    CExampleBuyStrategyProcessor strategyProcessor(buyStrategyType == "SHFE" ? EES_ExchangeID_shfe : EES_ExchangeID_dce);
    // if (buyStrategyType == "SHFE") {
    //     CExampleBuyStrategyProcessor strategyProcessor(EES_ExchangeID_shfe);
    // }
    // else if (buyStrategyType == "DCE") {
    //     CExampleBuyStrategyProcessor strategyProcessor(EES_ExchangeID_dce);
    // }
    // else {
    //     std::cout << "invalid buyStrategyType\n";
    // }

    std::cout << "about to start trader" << std::endl;
    std::string traderType = env["traderType"];
    BaseTrader* p_trader;
    if (traderType == "CTP") {
        p_trader = new CCTPTrader();
    }
    else if (traderType == "EES") {
        p_trader = new TraderEES();
    }
    else {
        std::cout << "invalid traderType\n";
    }
    std::thread ttrader([&p_trader]() {
        p_trader->start();
    });
    std::cout << "trader started" << std::endl;

    CMyDataManager mdDataManager(&strategyProcessor, p_trader);
    CDataTaskManager mdDataTaskManager(&mdDataManager);
    std::thread tDataTaskManager(&CDataTaskManager::start, &mdDataTaskManager);

    std::string mdType = env["mdType"];
    BaseMarketDataListener* p_mdListener;

    if (mdType == "CTP") {
        p_mdListener = new CCTPMarketDataListener();
    }
    else if (mdType == "NhMd") {
        p_mdListener = new NhMdListener();
    }
    else if (mdType == "DCEL1") {
        // define DCEL1 and link library of DCEL1 in Makefile before using DCEL1Listener()
        // p_mdListener = new DCEL1Listener();
    }
    else if (mdType == "nPro") {
        // define NPRO and link library of NPRO in Makefile before using CnProMarketDataListener()
        // p_mdListener = new CnProMarketDataListener();
    }
    else if (mdType == "DFITC") {
        // define DFITC and link library of DFITC in Makefile before using DFITCMarketDataListener()
        // p_mdListener = new DFITCMarketDataListener();
    }
    else if (mdType == "EES") {
        p_mdListener = new MyMarketDataListener();
    }
    else if (mdType == "EFHL2") {
        // define EFHL2 and link library of EFHL2 in Makefile before using EFHL2MarketDatalistener()
        // p_mdListener = new EFHL2MarketDatalistener();
    }
    else {
        std::cout << "invalid mdType\n";
    }

    p_mdListener->setDataManager(&mdDataManager);
    p_mdListener->setDataTaskManager(&mdDataTaskManager);

    std::thread tmdListener([&p_mdListener]() {
        p_mdListener->start();
    });

    ttrader.join();
    tDataTaskManager.join();
    tmdListener.join();
    // t2.join();
    std::cout << "Main shutdown!\n";

    std::cout.rdbuf(coutbuf);
    return 0;
}