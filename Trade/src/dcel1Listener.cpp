#include "dcel1/dcel1Listener.h"

#include <thread>
#include <time.h>

#define INSTRUMENT1 "rb2305"
#define INSTRUMENT2 "i2309"

bool InitApi(CDCEL1Api* api, const ApiConfig& conf)
{
    if (!api->BaseInfoInit(conf.BaseInfoIP.c_str(), conf.BaseInfoPort))
    {
        std::cout << "init baseinfo " << std::endl;
        return false;
    }
    for (const auto& c : conf.MCGroups)
    {
        if (!api->RegisterMultiCastRecv(c.MulticastIP.c_str(), c.BindAddr.c_str()))
        {
            std::cout << "init multicast " << std::endl;
            return false;
        }
    }
    if (!api->RegisterSnapshotRecv(conf.SnapshotPort, "0.0.0.0"))
    {
        std::cout << "snapshot port " << std::endl;
        return false;
    }
    if (!api->RegisterL1RecvPromisc(conf.L1Port, conf.L1NIC.c_str()))
    {
        std::cout << "level 1 port raw" << std::endl;
        return false;
    }
    return true;
}

void DCEL1Listener::setDataManager(CMyDataManager* dataManager)
{
    this->dataManager = dataManager;
}

void DCEL1Listener::setDataTaskManager(CDataTaskManager* dataTaskManager)
{
    this->dataTaskManager = dataTaskManager;
}

void DCEL1Listener::OnMarketData(const char* contractID, const MarketDataSnapshot* snapshot)
{
    Mydata mydata = { 0 };
    unsigned int n = sizeof(DCEL1MD);
    memcpy(&mydata.datas, contractID, ContractIdLen+7);
    memcpy(&mydata.datas[ContractIdLen+7], snapshot, sizeof(MarketDataSnapshot));
    memcpy(&mydata.datas[n], (char*)&seq, sizeof(unsigned int)); 
    clock_gettime(CLOCK_REALTIME, &mydata.time);
    mydata.len = n;
    if ((getTextField(mydata, DATA_FIELD_INSTRUMENT_NAME) != INSTRUMENT1) && (getTextField(mydata, DATA_FIELD_INSTRUMENT_NAME) != INSTRUMENT2))
    {
        // std::cout << "symbol not match " << getTextField(mydata, DATA_FIELD_INSTRUMENT_NAME) << std::endl;
        return;
    }
    // std::cout << "upd time=" << snapshot->UpdTime << std::endl;
    dataTaskManager->push(mydata);
    ++seq;
    // m_CSV.OnData(contractID, 0, *snapshot);
}

void DCEL1Listener::start()
{
    auto* api = CDCEL1Api::CreateApi();
    
    Env& env = Env::getInstance();
    ApiConfig conf{};
    conf.BaseInfoIP = env["mdDCEL1"]["BaseInfoIP"]; //"172.42.13.230";
    conf.BaseInfoPort = env["mdDCEL1"]["BaseInfoPort"];
    conf.MCGroups.clear();
    conf.SnapshotPort = env["mdDCEL1"]["SnapshotPort"];
    conf.L1Port = env["mdDCEL1"]["L1Port"];
    conf.L1NIC = env["mdDCEL1"]["L1NIC"];

    MultiCastConfig mcConf{env["mdDCEL1"]["MCGroups"]["MulticastIP"], env["mdDCEL1"]["MCGroups"]["BindAddr"]};
    conf.MCGroups.push_back(mcConf);
    // std::cout << conf.MCGroups[0].MulticastIP << std::endl;
    // std::cout << conf.MCGroups[0].BindAddr << std::endl;

    if (!InitApi(api, conf))
    {
        std::cout << "Init api failed" << std::endl;
        return;
    }

    // DemoSpi spi(MakeClockName(), "demo");
    api->RegisterSpi(this);
    api->Init();

    std::cout << "Init api done. Looping..." << std::endl;
    for(;;)
    {
        std::this_thread::yield();
    }
}