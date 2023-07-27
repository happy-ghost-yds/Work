#include "spdlog/spdlog.h"

#include "common/mydatamanager.h"
#include "nhmd/dcemarketdata.h"
#include <iostream>
// #include "spdlog/fmt/ostr.h"

// map<string, int> CMyDataManager::

constexpr auto billion = 1000000000;
constexpr auto million = 1000000;
constexpr auto kilo = 1000;
constexpr auto oneday = 86400;
constexpr auto onehour = 3600;
constexpr auto tz_china = 8;

// util functions
int chartoi(char c)
{
    return int(c-'0');
}

int nstoh(uint64_t ns)
{
    return ((ns / billion) % oneday) / onehour + tz_china;
}

int nstom(uint64_t ns)
{
    return ((ns / billion) % onehour) / 60;
}

int nstos(uint64_t ns)
{
    return ((ns / billion) % 60);
}

int stoh(uint32_t s)
{
    return ((s) % oneday) / onehour + tz_china;
}

int stom(uint32_t s)
{
    return ((s) % onehour) / 60;
}

int stos(uint32_t s)
{
    return ((s) % 60);
}

double getField(Mydata& data, int fieldName)
{
    if (data.len == _ftdc_struct_mdl2Size)
    {
        DCEMD::ftdc_struct_mdl2 md = *((DCEMD::ftdc_struct_mdl2*)data.datas);
        switch (fieldName)
        {
        case DATA_FIELD_BUY_PRICE_1:
            return md.ftdc_struct_md_bid1;
        case DATA_FIELD_SELL_PRICE_1:
            return md.ftdc_struct_md_ask1;
        case DATA_FIELD_BUY_VOLUME_1:
            return md.ftdc_struct_md_bid_vol1;
        case DATA_FIELD_SELL_VOLUME_1:
            return md.ftdc_struct_md_ask_vol1;
        case DATA_FIELD_LOWER_LIMIT_PRICE:
            return md.ftdc_struct_md_extra_lowerlimitprice;
        case DATA_FIELD_UPPER_LIMIT_PRICE:
            return md.ftdc_struct_md_extra_upperlimitprice;
        case DATA_FIELD_VOLUME:
            return md.ftdc_struct_md_volume;
        case DATA_FIELD_LAST_TURNOVER:
            return md.ftdc_struct_md_last_turnover;
        case DATA_FIELD_MS:
            return md.ftdc_struct_md_head_ms;
        case DATA_FIELD_TIME_H:
            return chartoi(md.ftdc_struct_md_head_time[0])*10+chartoi(md.ftdc_struct_md_head_time[1]);
        case DATA_FIELD_TIME_M:
            return chartoi(md.ftdc_struct_md_head_time[3])*10+chartoi(md.ftdc_struct_md_head_time[4]);
        case DATA_FIELD_TIME_S:
            return chartoi(md.ftdc_struct_md_head_time[6])*10+chartoi(md.ftdc_struct_md_head_time[7]);
        default:
            break;
        }
    } else if (data.len == _efh3_2_fut_lev2Size) {
        efh3_2_fut_lev2 md = *((efh3_2_fut_lev2*)data.datas);
        switch (fieldName)
        {
        case DATA_FIELD_BUY_PRICE_1:
            return md.m_bid1_px;
        case DATA_FIELD_SELL_PRICE_1:
            return md.m_ask1_px;
        case DATA_FIELD_BUY_VOLUME_1:
            return md.m_bid1_share;
        case DATA_FIELD_SELL_VOLUME_1:
            return md.m_ask1_share;
        case DATA_FIELD_VOLUME:
            return md.m_last_share;
        case DATA_FIELD_LAST_TURNOVER:
            return md.m_total_value;
        case DATA_FIELD_MS:
            return md.m_millisecond;
        case DATA_FIELD_TIME_H:
            return int(md.m_update_time_h);
        case DATA_FIELD_TIME_M:
            return int(md.m_update_time_m);
        case DATA_FIELD_TIME_S:
            return int(md.m_update_time_s);
        default:
            break;
        }
    } else if (data.len == _dcel1_Size){
        DCEL1MD md = *((DCEL1MD*)data.datas);
        switch (fieldName)
        {
        case DATA_FIELD_BUY_PRICE_1:
            return md.mdSnapshot.BidPrice/10000.0;
        case DATA_FIELD_SELL_PRICE_1:
            return md.mdSnapshot.AskPrice/10000.0;
        case DATA_FIELD_BUY_VOLUME_1:
            return md.mdSnapshot.BidVolume;
        case DATA_FIELD_SELL_VOLUME_1:
            return md.mdSnapshot.AskVolume;
        case DATA_FIELD_VOLUME:
            return md.mdSnapshot.TotalQty;
        case DATA_FIELD_LAST_TURNOVER:
            return md.mdSnapshot.Turnover/10000.0;
        case DATA_FIELD_MS:
        //1679628534811066438 -> 1679628534.811066438
            return (md.mdSnapshot.UpdTime % billion) / million;
        case DATA_FIELD_TIME_H:
            return nstoh(md.mdSnapshot.UpdTime);
        case DATA_FIELD_TIME_M:
            return nstom(md.mdSnapshot.UpdTime);
        case DATA_FIELD_TIME_S:
            return nstos(md.mdSnapshot.UpdTime);
        default:
            break;
        }
    } else if (data.len == _ctp_size) {
        CThostFtdcDepthMarketDataField md = *((CThostFtdcDepthMarketDataField*)data.datas);
        switch (fieldName)
        {
        case DATA_FIELD_BUY_PRICE_1:
            return md.BidPrice1;
        case DATA_FIELD_SELL_PRICE_1:
            return md.AskPrice1;
        case DATA_FIELD_BUY_VOLUME_1:
            return md.BidVolume1;
        case DATA_FIELD_SELL_VOLUME_1:
            return md.AskVolume1;
        case DATA_FIELD_VOLUME:
            return md.Volume;
        case DATA_FIELD_LAST_TURNOVER:
            return md.Turnover;
        case DATA_FIELD_MS:
            return md.UpdateMillisec;
        case DATA_FIELD_TIME_H:
            return chartoi(md.UpdateTime[0])*10+chartoi(md.UpdateTime[1]);
        case DATA_FIELD_TIME_M:
            return chartoi(md.UpdateTime[3])*10+chartoi(md.UpdateTime[4]);
        case DATA_FIELD_TIME_S:
            return chartoi(md.UpdateTime[6])*10+chartoi(md.UpdateTime[7]);
        default:
            break;
        }
    } else if (data.len == _npro_size){
        nProMD md = *((nProMD*)data.datas);
        switch (fieldName)
        {
        case DATA_FIELD_BUY_PRICE_1:
            return md.pricetick*md.md_buffer.bid_price[0];
        case DATA_FIELD_SELL_PRICE_1:
            return md.pricetick*md.md_buffer.ask_price[0];
        case DATA_FIELD_BUY_VOLUME_1:
            return md.md_buffer.bid_volume[0];
        case DATA_FIELD_SELL_VOLUME_1:
            return md.md_buffer.ask_volume[0];
        case DATA_FIELD_VOLUME:
            return md.md_buffer.volume;
        case DATA_FIELD_LAST_TURNOVER:
            return md.md_buffer.turnover;
        case DATA_FIELD_MS:
        //1679628534811066438 -> 1679628534.811066438
            return (md.md_buffer.timestamp & 0x03)*250;
        case DATA_FIELD_TIME_H:
            return stoh(((md.md_buffer.timestamp >> 2) & 0x3fffffff) + 0x5e0b7080);
        case DATA_FIELD_TIME_M:
            return stom(((md.md_buffer.timestamp >> 2) & 0x3fffffff) + 0x5e0b7080);
        case DATA_FIELD_TIME_S:
            return stos(((md.md_buffer.timestamp >> 2) & 0x3fffffff) + 0x5e0b7080);
        default:
            break;
        }
    } else if(data.len==_rem_size){
        EESMarketDepthQuoteData md=*((EESMarketDepthQuoteData*)data.datas);
        switch (fieldName){
        case DATA_FIELD_BUY_PRICE_1:
            return md.BidPrice1;
        case DATA_FIELD_SELL_PRICE_1:
            return md.AskPrice1;
        case DATA_FIELD_BUY_VOLUME_1:
            return md.BidVolume1;
        case DATA_FIELD_SELL_VOLUME_1:
            return md.AskVolume1;
        case DATA_FIELD_VOLUME:
            return md.Volume;
        case DATA_FIELD_LAST_TURNOVER:
            return md.Turnover;
        case DATA_FIELD_MS:
            return md.UpdateMillisec;
        case DATA_FIELD_TIME_H:
            return chartoi(md.UpdateTime[0])*10+chartoi(md.UpdateTime[1]);
        case DATA_FIELD_TIME_M:
            return chartoi(md.UpdateTime[3])*10+chartoi(md.UpdateTime[4]);
        case DATA_FIELD_TIME_S:
            return chartoi(md.UpdateTime[6])*10+chartoi(md.UpdateTime[7]);
        default:
            break;
        }
    } else if (data.len == _efhl2_size) {
        DFITC_MARKETDATA_API::MDBest md = *((DFITC_MARKETDATA_API::MDBest*)data.datas);
        switch (fieldName)
        {
        case DATA_FIELD_BUY_PRICE_1:
            return md.BuyPriceOne;
        case DATA_FIELD_SELL_PRICE_1:
            return md.SellPriceOne;
        case DATA_FIELD_BUY_VOLUME_1:
            return md.BuyQtyOne;
        case DATA_FIELD_SELL_VOLUME_1:
            return md.SellQtyOne;
        case DATA_FIELD_VOLUME:
            return md.MatchTotQty;
        case DATA_FIELD_LAST_TURNOVER:
            return md.Turnover;
        case DATA_FIELD_MS:
            return (md.GenTime[9] - '0') * 100 + (md.GenTime[10] - '0') * 10 + (md.GenTime[11] - '0');
        case DATA_FIELD_TIME_H:
            return chartoi(md.GenTime[0]) * 10 + chartoi(md.GenTime[1]);;
        case DATA_FIELD_TIME_M:
            return chartoi(md.GenTime[3]) * 10 + chartoi(md.GenTime[4]); ;
        case DATA_FIELD_TIME_S:
            return chartoi(md.GenTime[6]) * 10 + chartoi(md.GenTime[7]);;
        default:
            break;
        }
    }   else if (data.len == _dfitc_size) {
        mdp::mdp_ln_md md = *((mdp::mdp_ln_md*)data.datas);
        switch (fieldName)
        {
        case DATA_FIELD_BUY_PRICE_1:
            return md.l1_bid_price;
        case DATA_FIELD_SELL_PRICE_1:
            return md.l1_ask_price;
        case DATA_FIELD_BUY_VOLUME_1:
            return md.l1_bid_qty;
        case DATA_FIELD_SELL_VOLUME_1:
            return md.l1_ask_qty;
        case DATA_FIELD_VOLUME:
            return md.last_match_qty;
        case DATA_FIELD_LAST_TURNOVER:
            return md.turnover;
        case DATA_FIELD_MS:
            return (md.send_time[9] - '0') * 100 + (md.send_time[10] - '0') * 10 + (md.send_time[11] - '0');
        case DATA_FIELD_TIME_H:
            return chartoi(md.send_time[0]) * 10 + chartoi(md.send_time[1]);;
        case DATA_FIELD_TIME_M:
            return chartoi(md.send_time[3]) * 10 + chartoi(md.send_time[4]); ;
        case DATA_FIELD_TIME_S:
            return chartoi(md.send_time[6]) * 10 + chartoi(md.send_time[7]);;
        default:
            break;
        } 
    }
    return -1.0;
}

std::string getTextField(Mydata& data, int fieldName)
{
    if (data.len == _ftdc_struct_mdl2Size)
    {
        DCEMD::ftdc_struct_mdl2 md = *((DCEMD::ftdc_struct_mdl2*)data.datas);
        switch (fieldName)
        {
        case DATA_FIELD_INSTRUMENT_NAME:
            return md.ftdc_struct_md_head_id;
        case DATA_FIELD_TIME:
            return md.ftdc_struct_md_head_time;
        default:
            break;
        }
    } else if (data.len == _efh3_2_fut_lev2Size) {
        efh3_2_fut_lev2 md = *((efh3_2_fut_lev2*)data.datas);
        std::string s = "";
        switch (fieldName)
        {
        case DATA_FIELD_INSTRUMENT_NAME:
            return md.m_symbol;
        case DATA_FIELD_TIME:
            return std::to_string(int(md.m_update_time_h))+":"+std::to_string(int(md.m_update_time_m))+":"+std::to_string(int(md.m_update_time_s));
        default:
            break;
        }
    } else if (data.len == _dcel1_Size) {
        DCEL1MD md = *((DCEL1MD*)data.datas);
        std::string s = "";
        switch (fieldName)
        {
        case DATA_FIELD_INSTRUMENT_NAME:
            return md.ContractID;
        case DATA_FIELD_TIME:
            return std::to_string(nstoh(md.mdSnapshot.UpdTime))+":"+std::to_string(nstom(md.mdSnapshot.UpdTime))+":"+std::to_string(nstos(md.mdSnapshot.UpdTime));
        default:
            break;
        }
    } else if (data.len == _ctp_size) {
        CThostFtdcDepthMarketDataField md = *((CThostFtdcDepthMarketDataField*)data.datas);
        std::string s = "";
        switch (fieldName)
        {
        case DATA_FIELD_INSTRUMENT_NAME:
            return md.InstrumentID;
        case DATA_FIELD_TIME:
            return md.UpdateTime;
        default:
            break;
        }
    } else if (data.len == _npro_size){
        nProMD md = *((nProMD*)data.datas);
        uint64_t _s = ((md.md_buffer.timestamp >> 2) & 0x3fffffff) + 0x5e0b7080;
        switch (fieldName)
        {
        case DATA_FIELD_INSTRUMENT_NAME:
            return md.instr;
        case DATA_FIELD_TIME:
            return std::to_string(stoh(_s))+":"+std::to_string(stom(_s))+":"+std::to_string(stos(_s));
        default:
            break;
        }
    } else if (data.len == _rem_size) {
        EESMarketDepthQuoteData md = *((EESMarketDepthQuoteData*)data.datas);
        std::string s = "";
        switch (fieldName)
        {
        case DATA_FIELD_INSTRUMENT_NAME:
            return md.InstrumentID;
        case DATA_FIELD_TIME:
            return md.UpdateTime;
        default:
            break;
        }
    } if (data.len == _efhl2_size)
    {
        DFITC_MARKETDATA_API::MDBest md = *((DFITC_MARKETDATA_API::MDBest*)data.datas);

        std::string s = "";
        switch (fieldName)
        {
        case DATA_FIELD_INSTRUMENT_NAME:
            return md.Contract;
        case DATA_FIELD_TIME:
            return md.GenTime;
        default:
            break;
        }
    } else if (data.len == _dfitc_size)
    {
        mdp::mdp_ln_md md = *((mdp::mdp_ln_md*)data.datas);

        switch (fieldName)
        {
        case DATA_FIELD_INSTRUMENT_NAME:
            return md.contract_name;
        case DATA_FIELD_TIME:
            return md.send_time;
        default:
            break;
        }
    } 
    return "";
}

void logDataStruct(Mydata* data, int index)
{
    spdlog::info("[Index={0}] instrument={7} time={4} ms={3} B1={1} B1V={5} A1={2} A1V={6} VOL={8} TO={9}", index, 
        getField(*data, DATA_FIELD_BUY_PRICE_1), 
        getField(*data, DATA_FIELD_SELL_PRICE_1),
        getField(*data, DATA_FIELD_MS),
        getTextField(*data, DATA_FIELD_TIME),
        getField(*data, DATA_FIELD_BUY_VOLUME_1), 
        getField(*data, DATA_FIELD_SELL_VOLUME_1),
        getTextField(*data, DATA_FIELD_INSTRUMENT_NAME),
        getField(*data, DATA_FIELD_VOLUME),
        getField(*data, DATA_FIELD_LAST_TURNOVER)
    );
}

void logDataID(Mydata* data, std::string prefix)
{
    spdlog::info("[{0}]instrument={1} time={2} ms={3}", prefix,
        getTextField(*data, DATA_FIELD_INSTRUMENT_NAME), 
        getTextField(*data, DATA_FIELD_TIME),
        getField(*data, DATA_FIELD_MS)
    );
}

double diff_timespec(const struct timespec *time1, const struct timespec *time0) {
  return (time1->tv_sec - time0->tv_sec)
      + (time1->tv_nsec - time0->tv_nsec) / 1e9;
}

/*
CMyDataManager::CMyDataManager(CMeanReversionRatioProcessor* strategyProcessor, TraderEES* trader)
{
    std::cout << "efh3_2_fut_lev2Size: " << _efh3_2_fut_lev2Size << "\n";
    std::cout << "ftdc_struct_mdl2Size: " << _ftdc_struct_mdl2Size << "\n";
    m_Container = new vector<Mydata>();
    for (const auto &pair : m_AcceptedInstruments)
    {
        m_LastSnapshotIndex[pair.first] = -1;
    }
    this->strategyProcessor = strategyProcessor;
    this->m_AcceptedInstruments = {
        {(strategyProcessor->getConfig())->firstInstrument,1},
        {(strategyProcessor->getConfig())->secondInstrument,2},
    };
    this->trader = trader;
    strategyProcessor->setTrader(trader);
}
*/

CMyDataManager::CMyDataManager(CStrategyProcessor* strategyProcessor, BaseTrader* trader)
{
    std::cout << "efh3_2_fut_lev2Size: " << _efh3_2_fut_lev2Size << "\n";
    std::cout << "ftdc_struct_mdl2Size: " << _ftdc_struct_mdl2Size << "\n";
    std::cout << "dcel1_Size: " << _dcel1_Size << "\n";
    std::cout << "ctp_Size: " << _ctp_size << "\n";
    std::cout << "nPro_Size: " << _npro_size << "\n";
    std::cout << "rem_Size: " <<_rem_size <<"\n";
    std::cout << "efhl2_Size: " <<_efhl2_size <<"\n";
    std::cout << "dfitc_Size: " <<_dfitc_size <<"\n";
    
    m_Container = new vector<Mydata>();
    for (const auto &pair : m_AcceptedInstruments)
    {
        m_LastSnapshotIndex[pair.first] = -1;
    }
    this->strategyProcessor = strategyProcessor;
    this->m_AcceptedInstruments = {};

    int i = 0;
    for (const auto &pair : strategyProcessor->instrumentLimitPrices)
    {
        i++;
        this->m_AcceptedInstruments[pair.first] = i;
    }
    //     {(strategyProcessor->getConfig())->firstInstrument,1},
    //     {(strategyProcessor->getConfig())->secondInstrument,2},
    // };
    this->trader = trader;
    strategyProcessor->setTrader(trader);
}

CMyDataManager::~CMyDataManager()
{
    lock_guard<mutex> lock(m_Mutex);
    m_Container->clear();
    delete m_Container;
}

void CMyDataManager::SaveData(Mydata& data)
{
    bool isNewDataReceived = false; // filter based on instrument
    std::string instrument;
    unsigned int seq = *((unsigned int*)(data.datas + data.len));
    data.seqID = seq;
    if (data.len == _ftdc_struct_mdl2Size)
    {
        DCEMD::ftdc_struct_mdl2 md = *((DCEMD::ftdc_struct_mdl2*)data.datas);
        instrument = md.ftdc_struct_md_head_id;
        isNewDataReceived |= (0 != m_AcceptedInstruments[instrument]);
    } else if (data.len == _dcel1_Size) {
        DCEL1MD md = *((DCEL1MD*)data.datas);
        instrument = md.ContractID;
        isNewDataReceived |= (0 != m_AcceptedInstruments[instrument]);
    } else if (data.len == _npro_size) {
        nProMD md = *((nProMD*)data.datas);
        instrument = md.instr;
        isNewDataReceived |= (0 != m_AcceptedInstruments[instrument]);
        std::cout << md.pricetick << " " << md.md_buffer.bid_price[0] << " " << md.md_buffer.timestamp << std::endl;
    } else if (data.len == _ctp_size) {
        CThostFtdcDepthMarketDataField md = *((CThostFtdcDepthMarketDataField*)data.datas);
        instrument = md.InstrumentID;
        isNewDataReceived |= (0 != m_AcceptedInstruments[instrument]);
    } else if (0 == (data.len % _efh3_2_fut_lev2Size)) {
        unsigned int proc_len = 0;
        while(proc_len < data.len && (!isNewDataReceived))
		{
			efh3_2_fut_lev2 md = *((efh3_2_fut_lev2*)(data.datas + proc_len));
			instrument = md.m_symbol;
            isNewDataReceived |= (0 != m_AcceptedInstruments[instrument]);
			proc_len += _efh3_2_fut_lev2Size;
		}
        if (isNewDataReceived)
        {
            // std::cout << "Found data for " << instrument << " at " << proc_len << "\n";
            // rebuild data
            data.len = _efh3_2_fut_lev2Size;
            char tmp[MAXLINE];
            memcpy(tmp, data.datas+proc_len-_efh3_2_fut_lev2Size, _efh3_2_fut_lev2Size);
            memcpy(data.datas, tmp, _efh3_2_fut_lev2Size);
            data.datas[_efh3_2_fut_lev2Size] = '\0';
            efh3_2_fut_lev2 md = *((efh3_2_fut_lev2*)(data.datas));
            // std::cout << "now data looks like " << md.m_symbol << "\n";
        }
    } else if(data.len == _rem_size) {
        EESMarketDepthQuoteData md = *((EESMarketDepthQuoteData*)data.datas);
        instrument = md.InstrumentID;
        isNewDataReceived |= (0 != m_AcceptedInstruments[instrument]);
    } else if(data.len == _efhl2_size) {
        DFITC_MARKETDATA_API::MDBest md = *((DFITC_MARKETDATA_API::MDBest*)data.datas);
        instrument = md.Contract;
        isNewDataReceived |= (0 != m_AcceptedInstruments[instrument]);
    } else if(data.len == _dfitc_size) {
        mdp::mdp_best_quot md = *((mdp::mdp_best_quot*)data.datas);
        instrument = md.contract_id;
        isNewDataReceived |= (0 != m_AcceptedInstruments[instrument]);
    } 
    // add other market data types here in the future

    if (isNewDataReceived)
    {
        std::cout << "[DM]"<< getTextField(data, DATA_FIELD_INSTRUMENT_NAME) << " " << getTextField(data, DATA_FIELD_TIME) << ":" << getField(data, DATA_FIELD_MS) << std::endl;
        logDataStruct(&data, m_ContainerSize); //this isn't in the lock so we can see if there is potential racing
        lock_guard<mutex> lock(m_Mutex);
        m_LastSnapshotIndex[instrument] = m_ContainerSize;
        data.referenceIndex = m_LastSnapshotIndex;

        m_Container->push_back(data);
        // std::cout << "new data looks like " << instrument << "\n";
        // spdlog::info("[Index={0}] New MD received for contract {1} at {2}.{3}", m_ContainerSize, instrument, (long long)data.time.tv_sec, data.time.tv_nsec);
        // std::cout << "spdlog ran. data feeding for " << instrument << "\n";
        strategyProcessor->processData(m_Container, m_ContainerSize);
        ++m_ContainerSize;
        // mdEventQueue->push(m_ContainerSize++); //pushed value in queue is always the index of the tail
    } else {
        std::cout << "[" << (long long)data.time.tv_sec << "." << data.time.tv_nsec << "] " << "data didnt pass check, len=" << data.len << "\n";
    }
}

vector<Mydata>* CMyDataManager::GetDatas()
{
    lock_guard<mutex> lock(m_Mutex);
    if (!m_Container->empty())
    {
        vector<Mydata>* ret = m_Container;
        m_Container = new vector<Mydata>();
        return ret;
    }
    else
    {
        return nullptr;
    }
}

Mydata* CMyDataManager::GetDataAtPos(int i)
{
    return &(*m_Container)[i];
}
