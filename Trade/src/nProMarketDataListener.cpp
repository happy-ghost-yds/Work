#include <unistd.h>
#include <thread>
#include <cstring>

#include "md/nProMarketDataListener.h"

#define INSTRUMENT1 "rb2305"
#define INSTRUMENT2 "rb2310"

static inline uint64_t GetLocalTime()
{
    struct timespec tp = {0};
    clock_gettime(CLOCK_REALTIME, &tp);
    return ((uint64_t)tp.tv_sec) * 1000000000 + tp.tv_nsec;
}

void CnProMarketDataListener::start()
{
    m_pushMdThread = std::thread(&CnProMarketDataListener::_pushMarketData, this);
    
    Env& env = Env::getInstance();
    m_config = {};
    m_config.nic_interface = env["mdnPro"]["nic_interface"];
    m_config.exchange_type = env["mdnPro"]["exchange_type"];
    m_config.serverIP = env["mdnPro"]["serverIP"];
    m_config.if_restart_fpga = env["mdnPro"]["if_restart_fpga"];
    m_config.md_bind_cpu = env["mdnPro"]["md_bind_cpu"];
    m_config.license_filepath = env["mdnPro"]["license_filepath"];
    std::vector<std::string> _instrumentIDs = env["mdnPro"]["instrumentIDs"];
    m_config.instrumentIDs = _instrumentIDs;

    printf("[nPro] using nProPlusApi, version: %s\n", nProPlus_get_version());
    nProPlusHandle_t* handle_1 = nProPlus_acquire_handle();

    int re = -100;
    re = nProPlus_init(handle_1, m_config.license_filepath.c_str(), m_config.nic_interface.c_str(), 
        m_config.exchange_type, m_config.serverIP.c_str());

    if (re!=0)
    {
        nProPlus_release(handle_1);
        printf("[nPro] nProPlus_init failed, re = %d\n", re);
        return;
    }

    uint32_t  count = nProPlus_get_base_info_count(handle_1);

    for (uint32_t i = 0 ; i< count; i++)
    {
        const InsrItem*  ptr = nProPlus_get_base_info(handle_1, i, 1);
        if (ptr)
        {
            printf("[nPro] log base info  max_count: %d, i %d: %s, %d\n", count, i, ptr->instr, ptr->inst_no);
        }
    }

    if (m_config.if_restart_fpga)
    {
        int cnt = m_config.instrumentIDs.size();
        char * instrument [cnt];
        for (int i = 0; i < cnt; i++)
        {
            instrument[i] = new char[64];
            std::strncpy(instrument[i], m_config.instrumentIDs[i].c_str(), 64);
        }

        printf("[nPro] begin nProPlus_fpga_start\n");
        /// 此处传入待订阅合约列表和合约个数,  若需全量订阅， 请合约列表填NULL, 合约个数填0。
        ///  注意， nProPlus_fpga_start 仅需盘前调用一次， 盘中接收程序重启则无需调用

        re = nProPlus_fpga_start(handle_1, (const char**)instrument, m_config.instrumentIDs.size(), 
            "/root/nProPlus");

        if (re != 0)
        {
            printf("[nPro] nProPlus_fpga_start failed, re = %d\n", re);
            nProPlus_release(handle_1);
            return;
        }
        else
        {
            printf("[nPro] fpga start success...\n");
        }
    }

    const InsrItem * insr = NULL;
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(m_config.md_bind_cpu, &mask);
    int result = sched_setaffinity(0, sizeof(mask), &mask);
    printf("[nPro] res of set cpu affinity %d is %d\n", m_config.md_bind_cpu, result);

    printf("[nPro] start recv data...\n");

    while (!exitFlag)
    {
        // marketData& item = m_mdArray[m_arrHeader % ARRLEN];
        /// insr 为行情数据对应合约的基础信息的指针， 可保存用于后续计算， 在nProPlus_release函数调用前均有效， 请勿修改其内容
        Mydata mydata = { 0 };
        insr = nProPlus_fetch_market_data(handle_1, mydata.datas + INST_CHAR_SIZE + sizeof(double), MAXLINE);

        if (insr)
        {
            unsigned int n = sizeof(nProMD);
            memcpy(&mydata.datas, &insr->instr, INST_CHAR_SIZE);
            memcpy(&mydata.datas[INST_CHAR_SIZE], (char*)&insr->pricetick, sizeof(double));
            // memcpy(&mydata.datas[INST_CHAR_SIZE], &item.md_buffer, sizeof(nProPlusmd));
            memcpy(&mydata.datas[n], (char*)&m_arrHeader, sizeof(unsigned int)); 
            clock_gettime(CLOCK_REALTIME, &mydata.time);
            mydata.len = n;
            if ((getTextField(mydata, DATA_FIELD_INSTRUMENT_NAME) != INSTRUMENT1) && (getTextField(mydata, DATA_FIELD_INSTRUMENT_NAME) != INSTRUMENT2))
            {
                std::cout << "symbol not match " << getTextField(mydata, DATA_FIELD_INSTRUMENT_NAME) << std::endl;
                return;
            }
            dataTaskManager->push(mydata);

            // item.timestamp = GetLocalTime();
            // item.insr = (InsrItem* )insr;
            ++m_arrHeader;
        }
    }

    nProPlus_release(handle_1);
}

void CnProMarketDataListener::_pushMarketData()
{
    while (true)
    {
        CThostFtdcDepthMarketDataField depthMarketData = m_MdQueue.pop();
        Mydata mydata = { 0 };
        unsigned int n = sizeof(CThostFtdcDepthMarketDataField);
        memcpy(&mydata.datas, &depthMarketData, sizeof(CThostFtdcDepthMarketDataField));
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
        // ++pDataIndex;
    }
}

void CnProMarketDataListener::dumpMarketData(CThostFtdcDepthMarketDataField* pDepthMarketData)
{
    // printf("\t[CL] InstrumentID [%s]\n", pDepthMarketData->InstrumentID);
    // printf("\t[CL] UpdateTime [%s]\n", pDepthMarketData->UpdateTime);
    m_MdQueue.push(*pDepthMarketData);
    // mdStorage.push_back(*pDepthMarketData);
    // mdStorageSize++;
}