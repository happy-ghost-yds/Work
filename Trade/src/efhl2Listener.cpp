#include "efhl2/efhl2Listener.h"

#define INSTRUMENT1 "rb2305"
#define INSTRUMENT2 "y2309"

EFHL2marketDatalistener::EFHL2marketDatalistener()
{

    ConfigFilename= "./config_efhl2.json";
}

EFHL2marketDatalistener::~EFHL2marketDatalistener()
{

}



void EFHL2marketDatalistener::OnBest(MDBest* const pQuote)
{
    printf("%c,%u,%u,%u,%llu,%s,%s,%s,%s,%u,%u,%f,%u,%u,%f,%u,%u,%d,%f,%u,%u,%f,%u,%u,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n",
    pQuote->Type,
    pQuote->Length,
    pQuote->Version,
    pQuote->MDGNo,
    pQuote->SeqNo,
    pQuote->Exchange,
    pQuote->Contract,
    pQuote->TradeDate,
    pQuote->GenTime,
    pQuote->ChannelNo,
    pQuote->PackageNo,
    INVALID_PRICE_CHECK_AND_EXCHANGE(pQuote->LastPrice),
    pQuote->LastMatchQty,
    pQuote->MatchTotQty,
    INVALID_PRICE_CHECK_AND_EXCHANGE(pQuote->Turnover),
    pQuote->LastOpenInterest,
    pQuote->OpenInterest,
    pQuote->InterestChg,
    INVALID_PRICE_CHECK_AND_EXCHANGE(pQuote->BuyPriceOne),
    pQuote->BuyQtyOne,
    pQuote->BuyImplyQtyOne,
    INVALID_PRICE_CHECK_AND_EXCHANGE(pQuote->SellPriceOne),
    pQuote->SellQtyOne,
    pQuote->SellImplyQtyOne,
    INVALID_PRICE_CHECK_AND_EXCHANGE(pQuote->AvgPrice),
    INVALID_PRICE_CHECK_AND_EXCHANGE(pQuote->OpenPrice),
    INVALID_PRICE_CHECK_AND_EXCHANGE(pQuote->Close),
    INVALID_PRICE_CHECK_AND_EXCHANGE(pQuote->ClearPrice),
    INVALID_PRICE_CHECK_AND_EXCHANGE(pQuote->HighPrice),
    INVALID_PRICE_CHECK_AND_EXCHANGE(pQuote->LowPrice),
    INVALID_PRICE_CHECK_AND_EXCHANGE(pQuote->LifeLow),
    INVALID_PRICE_CHECK_AND_EXCHANGE(pQuote->LifeHigh),
    INVALID_PRICE_CHECK_AND_EXCHANGE(pQuote->RiseLimit),
    INVALID_PRICE_CHECK_AND_EXCHANGE(pQuote->FallLimit),
    INVALID_PRICE_CHECK_AND_EXCHANGE(pQuote->LastClearPrice),
    INVALID_PRICE_CHECK_AND_EXCHANGE(pQuote->LastClose));
}

void EFHL2marketDatalistener::OnDeep(MDDeep* const pQuote)
{
    printf("%c,%u,%u,%u,%llu,%s,%s,%s,%s,%u,%u,%f,%u,%u,%f,%u,%u,%f,%u,%u,%f,%u,%u,%f,%u,%u,%f,%u,%u,%f,%u,%u,%f,%u,%u,%f,%u,%u\n",
    pQuote->Type,
    pQuote->Length,
    pQuote->Version,
    pQuote->MDGNo,
    pQuote->SeqNo,
    pQuote->Exchange,
    pQuote->Contract,
    pQuote->TradeDate,
    pQuote->GenTime,
    pQuote->ChannelNo,
    pQuote->PackageNo,
    INVALID_PRICE_CHECK_AND_EXCHANGE(pQuote->BuyPriceOne),
    pQuote->BuyQtyOne,
    pQuote->BuyImplyQtyOne,
    INVALID_PRICE_CHECK_AND_EXCHANGE(pQuote->BuyPriceTwo),
    pQuote->BuyQtyTwo,
    pQuote->BuyImplyQtyTwo,
    INVALID_PRICE_CHECK_AND_EXCHANGE(pQuote->BuyPriceThree),
    pQuote->BuyQtyThree,
    pQuote->BuyImplyQtyThree,
    INVALID_PRICE_CHECK_AND_EXCHANGE(pQuote->BuyPriceFour),
    pQuote->BuyQtyFour,
    pQuote->BuyImplyQtyFour,
    INVALID_PRICE_CHECK_AND_EXCHANGE(pQuote->BuyPriceFive),
    pQuote->BuyQtyFive,
    pQuote->BuyImplyQtyFive,
    INVALID_PRICE_CHECK_AND_EXCHANGE(pQuote->SellPriceOne),
    pQuote->SellQtyOne,
    pQuote->SellImplyQtyOne,
    INVALID_PRICE_CHECK_AND_EXCHANGE(pQuote->SellPriceTwo),
    pQuote->SellQtyTwo,
    pQuote->SellImplyQtyTwo,
    INVALID_PRICE_CHECK_AND_EXCHANGE(pQuote->SellPriceThree),
    pQuote->SellQtyThree,
    pQuote->SellImplyQtyThree,
    INVALID_PRICE_CHECK_AND_EXCHANGE(pQuote->SellPriceFour),
    pQuote->SellQtyFour,
    pQuote->SellImplyQtyFour,
    INVALID_PRICE_CHECK_AND_EXCHANGE(pQuote->SellPriceFive),
    pQuote->SellQtyFive,
    pQuote->SellImplyQtyFive);
}

void EFHL2marketDatalistener::OnOptParam(MDOptParam* const pQuote)
{
    printf("%c,%u,%u,%u,%llu,%s,%s,%s,%s,%u,%u,%f,%f,%f,%f,%f\n",
           pQuote->Type,
           pQuote->Length,
           pQuote->Version,
           pQuote->MDGNo,
           pQuote->SeqNo,
           pQuote->Exchange,
           pQuote->Contract,
           pQuote->TradeDate,
           pQuote->GenTime,
           pQuote->ChannelNo,
           pQuote->PackageNo,
           INVALID_PRICE_CHECK_AND_EXCHANGE(pQuote->Delta),
           INVALID_PRICE_CHECK_AND_EXCHANGE(pQuote->Gamma),
           INVALID_PRICE_CHECK_AND_EXCHANGE(pQuote->Rho),
           INVALID_PRICE_CHECK_AND_EXCHANGE(pQuote->Theta),
           INVALID_PRICE_CHECK_AND_EXCHANGE(pQuote->Vega));
}

void EFHL2marketDatalistener::OnArbiBest(ArbiMDBest* const pQuote)
{
    printf("%c,%u,%u,%u,%llu,%s,%s,%s,%s,%u,%u,%f,%u,%f,%u,%f,%u,%f,%f,%f,%f,%f,%f\n",
    pQuote->Type,
    pQuote->Length,
    pQuote->Version,
    pQuote->MDGNo,
    pQuote->SeqNo,
    pQuote->Exchange,
    pQuote->Contract,
    pQuote->TradeDate,
    pQuote->GenTime,
    pQuote->ChannelNo,
    pQuote->PackageNo,
    INVALID_PRICE_CHECK_AND_EXCHANGE(pQuote->LastPrice),
    pQuote->LastMatchQty,
    INVALID_PRICE_CHECK_AND_EXCHANGE(pQuote->BuyPriceOne),
    pQuote->BuyQtyOne,
    INVALID_PRICE_CHECK_AND_EXCHANGE(pQuote->SellPriceOne),
    pQuote->SellQtyOne,
    INVALID_PRICE_CHECK_AND_EXCHANGE(pQuote->HighPrice),
    INVALID_PRICE_CHECK_AND_EXCHANGE(pQuote->LowPrice),
    INVALID_PRICE_CHECK_AND_EXCHANGE(pQuote->LifeLow),
    INVALID_PRICE_CHECK_AND_EXCHANGE(pQuote->LifeHigh),
    INVALID_PRICE_CHECK_AND_EXCHANGE(pQuote->RiseLimit),
    INVALID_PRICE_CHECK_AND_EXCHANGE(pQuote->FallLimit));
}

void EFHL2marketDatalistener::OnTenEntrust(MDTenEntrust* const pQuote)
{
    printf("%c,%u,%u,%u,%llu,%s,%s,%s,%s,%u,%u,%f,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%f,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u\n",
    pQuote->Type,
    pQuote->Length,
    pQuote->Version,
    pQuote->MDGNo,
    pQuote->SeqNo,
    pQuote->Exchange,
    pQuote->Contract,
    pQuote->TradeDate,
    pQuote->GenTime,
    pQuote->ChannelNo,
    pQuote->PackageNo,
    INVALID_PRICE_CHECK_AND_EXCHANGE(pQuote->BestBuyOrderPrice),
    pQuote->BestBuyOrderQtyOne,
    pQuote->BestBuyOrderQtyTwo,
    pQuote->BestBuyOrderQtyThree,
    pQuote->BestBuyOrderQtyFour,
    pQuote->BestBuyOrderQtyFive,
    pQuote->BestBuyOrderQtySix,
    pQuote->BestBuyOrderQtySeven,
    pQuote->BestBuyOrderQtyEight,
    pQuote->BestBuyOrderQtyNine,
    pQuote->BestBuyOrderQtyTen,
    INVALID_PRICE_CHECK_AND_EXCHANGE(pQuote->BestSellOrderPrice),
    pQuote->BestSellOrderQtyOne,
    pQuote->BestSellOrderQtyTwo,
    pQuote->BestSellOrderQtyThree,
    pQuote->BestSellOrderQtyFour,
    pQuote->BestSellOrderQtyFive,
    pQuote->BestSellOrderQtySix,
    pQuote->BestSellOrderQtySeven,
    pQuote->BestSellOrderQtyEight,
    pQuote->BestSellOrderQtyNine,
    pQuote->BestSellOrderQtyTen);
}

void EFHL2marketDatalistener::OnOrderStatistic(MDOrderStatistic* const pQuote)
{
    printf("%c,%u,%u,%u,%llu,%s,%s,%s,%s,%u,%u,%u,%u,%f,%f\n",
    pQuote->Type,
    pQuote->Length,
    pQuote->Version,
    pQuote->MDGNo,
    pQuote->SeqNo,
    pQuote->Exchange,
    pQuote->Contract,
    pQuote->TradeDate,
    pQuote->GenTime,
    pQuote->ChannelNo,
    pQuote->PackageNo,
    pQuote->TotalBuyOrderNum,
    pQuote->TotalSellOrderNum,
    INVALID_PRICE_CHECK_AND_EXCHANGE(pQuote->WeightedAverageBuyOrderPrice),
    INVALID_PRICE_CHECK_AND_EXCHANGE(pQuote->WeightedAverageSellOrderPrice));
}

void EFHL2marketDatalistener::OnMatchPriceQty(MDMatchPriceQty* const pQuote)
{
    printf("%c,%u,%u,%u,%llu,%s,%s,%s,%s,%u,%u,%f,%u,%u,%u,%u,%f,%u,%u,%u,%u,%f,%u,%u,%u,%u,%f,%u,%u,%u,%u,%f,%u,%u,%u,%u\n",
    pQuote->Type,
    pQuote->Length,
    pQuote->Version,
    pQuote->MDGNo,
    pQuote->SeqNo,
    pQuote->Exchange,
    pQuote->Contract,
    pQuote->TradeDate,
    pQuote->GenTime,
    pQuote->ChannelNo,
    pQuote->PackageNo,
    INVALID_PRICE_CHECK_AND_EXCHANGE(pQuote->PriceOne),
    pQuote->PriceOneBOQty,
    pQuote->PriceOneBEQty,
    pQuote->PriceOneSOQty,
    pQuote->PriceOneSEQty,
    INVALID_PRICE_CHECK_AND_EXCHANGE(pQuote->PriceTwo),
    pQuote->PriceTwoBOQty,
    pQuote->PriceTwoBEQty,
    pQuote->PriceTwoSOQty,
    pQuote->PriceTwoSEQty,
    INVALID_PRICE_CHECK_AND_EXCHANGE(pQuote->PriceThree),
    pQuote->PriceThreeBOQty,
    pQuote->PriceThreeBEQty,
    pQuote->PriceThreeSOQty,
    pQuote->PriceThreeSEQty,
    INVALID_PRICE_CHECK_AND_EXCHANGE(pQuote->PriceFour),
    pQuote->PriceFourBOQty,
    pQuote->PriceFourBEQty,
    pQuote->PriceFourSOQty,
    pQuote->PriceFourSEQty,
    INVALID_PRICE_CHECK_AND_EXCHANGE(pQuote->PriceFive),
    pQuote->PriceFiveBOQty,
    pQuote->PriceFiveBEQty,
    pQuote->PriceFiveSOQty,
    pQuote->PriceFiveSEQty);
}

void EFHL2marketDatalistener::OnErrorMsg(const char* msg)
{
    fprintf(stderr,"%s",msg);
}

void EFHL2marketDatalistener::OnDebugMsg(const char* msg)
{
    fprintf(stderr,"%s",msg);
}

void EFHL2marketDatalistener::OnInfoMsg(const char* msg)
{
    fprintf(stderr,"%s",msg);
}




  

void EFHL2marketDatalistener::start()
{   
    m_pushMdThread = std::thread(&EFHL2marketDatalistener::_pushMarketData, this);
    dce_api_param_t param;
    parseJson(param);

    i_sl_dce_api* pApi = i_sl_dce_api::CreateApiObject();  

    if(NULL == pApi)
    {
        fprintf(stderr,"create efhl2 api failed.\n");
        return ;
    }

    EFHL2marketDatalistener api_event;
 

    if(!pApi->init(&param,&api_event))
    {   
        printf("api init failed.\n");
        return ;
    }

    pApi->start();

    return ;

}


void EFHL2marketDatalistener::_pushMarketData()
{
    while (true)
    {
        MDBest depthMarketData = m_MdQueue.pop();
        Mydata mydata = { 0 };
        unsigned int n = sizeof(MDBest);
        memcpy(&mydata.datas, &depthMarketData, sizeof(MDBest));
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

void EFHL2marketDatalistener::dumpMarketData(MDBest* pDepthMarketData)
{
    m_MdQueue.push(*pDepthMarketData);
}
