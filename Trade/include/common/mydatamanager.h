#pragma once

#include <mutex>
#include <vector>
#include <map>
#include <string.h>

#include "nhmd/dcemarketdata.h"
#include "efh/guava_quote.h"
#include "ctp/ThostFtdcUserApiStruct.h"

#include "dcel1/dcel1api.h"
#include "md/nPro/include/nProPlus.h"
// #include "common/concurrentQueue.h"
// #include "strategy/meanReversion.h"
// #include "strategy/mdDriven.h"
#include "strategy/baseStrategy.h"
#include "execution/ees.h"
#include "execution/baseTrader.h"
#include "trade/rem/include/EESQuoteDefine.h"
#include "efhl2/DFITCL2_Open_ApiDataType.h"
#include "mdp/types.hpp"
// #include "md/DFITCMarketDataListener.hpp"

//forward declare
class CStrategyProcessor;

using namespace std;

//���ջ�����
#define MAXLINE 1024
#define MARKET_CLOSE_ST 145959
#define MARKET_CLOSE_ET 153059
#define MARKET_CLOSE_OVN_ST 225959
#define MARKET_CLOSE_OVN_ET 230059

const int _bestAndDeepSize = sizeof(DCEMD::MDBestAndDeep);
const int _ftdc_struct_mdl2Size = sizeof(DCEMD::ftdc_struct_mdl2);
const int _efh3_2_fut_lev2Size = sizeof(efh3_2_fut_lev2);
const int _dcel1_Size = sizeof(DCEL1MD);
const int _ctp_size = sizeof(CThostFtdcDepthMarketDataField);
const int _npro_size = sizeof(nProMD);
const int _rem_size = sizeof(EESMarketDepthQuoteData);
const int _efhl2_size = sizeof(DFITC_MARKETDATA_API::MDBest);
const int _dfitc_size = sizeof(mdp::mdp_ln_md);

const int DATA_FIELD_BUY_PRICE_1 = 0;
const int DATA_FIELD_SELL_PRICE_1 = 1;
const int DATA_FIELD_BUY_VOLUME_1 = 10;
const int DATA_FIELD_SELL_VOLUME_1 = 11;
const int DATA_FIELD_LOWER_LIMIT_PRICE = 50;
const int DATA_FIELD_UPPER_LIMIT_PRICE = 51;
const int DATA_FIELD_VOLUME = 52;
const int DATA_FIELD_LAST_TURNOVER = 53;
const int DATA_FIELD_INSTRUMENT_NAME = 21;
const int DATA_FIELD_TIME = 22;
const int DATA_FIELD_MS = 23;
const int DATA_FIELD_TIME_H = 24;
const int DATA_FIELD_TIME_M = 25;
const int DATA_FIELD_TIME_S = 26;

struct Mydata
{
    timespec time;
    int len;
    char datas[MAXLINE];
    map<string, int> referenceIndex; // index in m_Container to find other instrument's last snapshot
    unsigned int seqID;
};

double getField(Mydata& data, int fieldName);
std::string getTextField(Mydata& data, int fieldName);
void logDataStruct(Mydata* data, int index);
void logDataID(Mydata* data, std::string prefix);
double diff_timespec(const struct timespec *time1, const struct timespec *time0);

class CMyDataManager
{
public:
    // CMyDataManager(CConcurrentQueue<int>* mdEventQueue);
    // CMyDataManager(CMeanReversionRatioProcessor* strategyProcessor);
    // CMyDataManager(CMeanReversionRatioProcessor* strategyProcessor, TraderEES* trader);
    CMyDataManager(CStrategyProcessor* strategyProcessor, BaseTrader* trader);
    
    ~CMyDataManager();

    //��������
    void SaveData(Mydata& data);
    // void SaveData(Mydata& data);

    //��ȡ���л������ݣ�û���ݷ��� nullptr������ָ����Ҫ����delete
    vector<Mydata>* GetDatas();
    Mydata* GetDataAtPos(int i);

private:
    vector<Mydata>* m_Container;
    int m_ContainerSize = 0;
    mutex m_Mutex;
    // CConcurrentQueue<int>* mdEventQueue;
    // CMeanReversionRatioProcessor* strategyProcessor;
    CStrategyProcessor* strategyProcessor;
    BaseTrader* trader;
    map<string, int> m_LastSnapshotIndex; // index in m_Container for last received piece for each instrument
    map<string, int> m_AcceptedInstruments;
};
