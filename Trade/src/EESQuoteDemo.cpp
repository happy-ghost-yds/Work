#include "rem/EESQuoteDemo.h"
#include <iostream>
#include "rem/getconfig.h"

using namespace std;

MyMarketDataListener::MyMarketDataListener()
{
    

}

MyMarketDataListener::~MyMarketDataListener()
{

    //关闭
    Close();
    
}


bool MyMarketDataListener::Init()
{   
    if (!LoadEESQuote())
    {
        return false;
    }


    if(!Loadjson(TcpInfo, LoginParam,IntrumentType, mpSymbols))
    {
        cout<<"加载配置文件失败！"<<endl;
        return false;
    }



    vector<EqsTcpInfo> vecEqsTcpInfo;

    vecEqsTcpInfo.push_back(TcpInfo);

    m_quoteApi->ConnServer(vecEqsTcpInfo,this);
    
    return true;
}

void MyMarketDataListener::Userlogin()
{
    m_quoteApi->LoginToEqs(LoginParam);
}

void MyMarketDataListener::Close()
{
    if (m_quoteApi)
    {
        m_quoteApi->DisConnServer();
    }
    UNloadEESQuote();
}

void MyMarketDataListener::Query()
{
    m_quoteApi->QuerySymbolList();
}

void MyMarketDataListener::Register()
{

    // 查找是否存在需要订阅的合约类型及其对应的合约号
    auto iter = m_symbolsMap.find(IntrumentType);
    if (iter == m_symbolsMap.end())
    {
        cout << "订阅失败：不存在所需订阅的合约类型！" << endl;
        return;
    }
    for(string msymbol:mpSymbols)
    {
    if(std::find(m_symbolsMap[IntrumentType].begin(), m_symbolsMap[IntrumentType].end(), msymbol) != m_symbolsMap[IntrumentType].end())
    {
        // 执行注册操作
        m_quoteApi->RegisterSymbol(IntrumentType, msymbol.c_str());
        cout << "合约号:"<<msymbol <<"注册成功！" << endl;
    }
    else
    {
        cout << "订阅失败：不存在所需订阅的合约号！" << endl;
        return;
    }

    }
    
}


void MyMarketDataListener::UNregister()
{
    // 查找是否存在需要取消订阅的合约类型及其对应的合约号
    auto iter = m_symbolsMap.find(IntrumentType);
    if (iter == m_symbolsMap.end())
    {
        cout << "取消订阅失败：不存在所需取消订阅的合约类型！" << endl;
        return;
    }
    for(string msymbol:mpSymbols)
    {
    if(std::find(m_symbolsMap[IntrumentType].begin(), m_symbolsMap[IntrumentType].end(), msymbol) != m_symbolsMap[IntrumentType].end())
    {
        //执行注销操作
        m_quoteApi->UnregisterSymbol(IntrumentType, msymbol.c_str());
        cout << "合约号:"<<msymbol <<"注销成功！" << endl;
    }
    else
    {
        cout << "取消订阅失败：不存在所需取消订阅的合约号！" << endl;
        return;
    }

    }
}

bool MyMarketDataListener::LoadEESQuote()
{
    
    m_handle =  dlopen(EES_QUOTE_DLL_NAME, RTLD_LAZY);
    if (!m_handle)
	{
		printf("load library(%s) failed.\n", EES_QUOTE_DLL_NAME);
		return false;
	}

    funcCreateEESQuoteApi createFun = (funcCreateEESQuoteApi)dlsym(m_handle, CREATE_EES_QUOTE_API_NAME);
	if (!createFun)
	{
		printf("get function addresss(%s) failed!\n", CREATE_EES_QUOTE_API_NAME);
		return false;
	}

    m_distoryFun = (funcDestroyEESQuoteApi)dlsym(m_handle, DESTROY_EES_QUOTE_API_NAME);
	if (!m_distoryFun)
	{
		printf("get function addresss(%s) failed!\n", DESTROY_EES_QUOTE_API_NAME);
		return false;
	}
    
    m_quoteApi = createFun();
    if (!m_quoteApi)
    {
        std::cout<<"创建行情API失败!\n"<<endl;
        return false;
    }
    cout << "创建行情API成功!\n" << endl;
    return true;

}

bool MyMarketDataListener::UNloadEESQuote()
{
    if (m_quoteApi)
    {
        m_distoryFun(m_quoteApi);
    }
    return false;
}

void MyMarketDataListener::OnEqsConnected()
{
    cout << "EES行情服务器连接成功!" << endl;
    //登录
    Userlogin();
}

void MyMarketDataListener::OnEqsDisconnected()
{
    cout << "EES行情服务器断开成功!" << endl;
}

void MyMarketDataListener::OnLoginResponse(bool bSuccess, const char* pReason)
{
    
    if (bSuccess) {
        cout << "EES登录成功" << endl;
        //查询合约
        Query();

    }
    else {

        cout << "EES登录失败! Reason: " << pReason << endl;
    }
}


void MyMarketDataListener::OnQuoteUpdated(EesEqsIntrumentType chInstrumentType, EESMarketDepthQuoteData* pDepthQuoteData)
{
// 处理行情更新
cout << "行情接收: " << "合约代码 - " << pDepthQuoteData->InstrumentID <<
"，最新价 - " << pDepthQuoteData->LastPrice <<
"，买价一 - " << pDepthQuoteData->BidPrice1 <<
"，卖价一 - " << pDepthQuoteData->AskPrice1 <<
"，买量一 - " << pDepthQuoteData->BidVolume1 <<
"，卖量一 - " << pDepthQuoteData->AskVolume1 << endl;
}

void MyMarketDataListener::OnWriteTextLog(EesEqsLogLevel nlevel, const char* pLogText, int nLogLen)
{
// 输出日志信息
cout << "收到日志消息: " << "日志级别 - " << nlevel <<
"，日志内容长度 - " << nLogLen <<
"，日志内容 - " << pLogText << endl;
}

void MyMarketDataListener::OnSymbolRegisterResponse(EesEqsIntrumentType chInstrumentType, const char* pSymbol, bool bSuccess)
{
if (bSuccess) {
cout << "注册合约成功: " << "合约类型 - " << chInstrumentType << "，合约名称 - " << pSymbol << endl;
}
else {
cout << "注册合约失败: " << "合约类型 - " << chInstrumentType << "，合约名称 - " << pSymbol << endl;
}
}

void MyMarketDataListener::OnSymbolUnregisterResponse(EesEqsIntrumentType chInstrumentType, const char* pSymbol, bool bSuccess)
{
if (bSuccess) {
cout << "注销合约成功: " << "合约类型 - " << chInstrumentType << "，合约名称 - " << pSymbol << endl;
}
else {
cout << "注销合约失败: " << "合约类型 - " << chInstrumentType << "，合约名称 - " << pSymbol << endl;
}
}

void MyMarketDataListener::OnSymbolListResponse(EesEqsIntrumentType chInstrumentType, const char* pSymbol, bool bLast)
{    
    if (!bLast) {
    //保存合约列表
    cout <<"合约类型: "<< chInstrumentType << "收到合约列表: " << pSymbol << endl;
    m_symbolsMap[chInstrumentType].push_back(pSymbol);     
    }
    else
    {
        cout << "所有合约列表接收完毕！" << endl;
        //注册合约
        Register();
    }
}

void MyMarketDataListener::start()
{
    //初始化
    Init();  
}

void MyMarketDataListener::_pushMarketData()
{
    while (true)
    {
        EESMarketDepthQuoteData depthMarketData = m_MdQueue.pop();
        Mydata mydata = { 0 };
        unsigned int n = sizeof(EESMarketDepthQuoteData);
        memcpy(&mydata.datas, &depthMarketData, sizeof(EESMarketDepthQuoteData));
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

void MyMarketDataListener::dumpMarketData(EESMarketDepthQuoteData* pDepthMarketData)
{
    m_MdQueue.push(*pDepthMarketData);
}