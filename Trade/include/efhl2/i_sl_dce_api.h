#ifndef _I_SL_DCE_API_H_
#define _I_SL_DCE_API_H_
#include "DFITCL2_Open_ApiDataType.h"
using namespace DFITC_MARKETDATA_API;

#pragma pack(push,1)

struct session_t
{	
    char src_ip[16];             //源地址
    char dst_ip[16];             //目的地址
    int  src_port;               //源端口
    int  dst_port;               //目的端口
};

struct dce_api_param_t
{
    dce_api_param_t(){
        session_nums  = 0;
        cpu_nums      = 0;
    }

    int          session_nums;   //会话组个数
    session_t    sessions[16];   //会话组
    char         eth_name[256];  //网卡名称
    int          cpu_arrays[32]; //cpu核数组
    int          cpu_nums;       //cpu核个数
};

#pragma pack(pop)

class i_sl_dce_api_event
{
public:
    virtual ~i_sl_dce_api_event(){}

    /*
    名称  : OnBest
    功能  : 最优行情
    入参  : pQuote 最优行情对象
    出参  : 无
    返回值: 无
    */
    virtual void OnBest(MDBest* const pQuote){}

    /*
    名称  : OnDeep
    功能  : 深度行情
    入参  : pQuote 深度行情对象
    出参  : 无
    返回值: 无
    */
    virtual void OnDeep(MDDeep* const pQuote){}

    /*
    名称  : OnArbiBest
    功能  : 套利行情
    入参  : pQuote 套利行情对象
    出参  : 无
    返回值: 无
    */
    virtual void OnArbiBest(ArbiMDBest* const pQuote){}

    /*
    名称  : OnOptParam
    功能  : 期权行情
    入参  : pQuote 期权行情对象
    出参  : 无
    返回值: 无
    */
    virtual void OnOptParam(MDOptParam* const pQuote){}

    /*
    名称  : OnTenEntrust
    功能  : 最优十笔委托行情
    入参  : pQuote 最优十笔委托行情对象
    出参  : 无
    返回值: 无
    */
    virtual void OnTenEntrust(MDTenEntrust* const pQuote){}

    /*
    名称  : OnOrderStatistic
    功能  : 委托统计行情
    入参  : pQuote 委托统计行情对象
    出参  : 无
    返回值: 无
    */
    virtual void OnOrderStatistic(MDOrderStatistic* const pQuote){}

    /*
    名称  : OnMatchPriceQty
    功能  : 分价位成交行情
    入参  : pQuote 分价位成交行情对象
    出参  : 无
    返回值: 无
    */
    virtual void OnMatchPriceQty(MDMatchPriceQty* const pQuote){}

    /*
    名称  : OnErrorMsg
    功能  : 输出错误日志信息
    入参  : msg  错误日志地址
    出参  : 无
    返回值: 无
    */
    virtual void OnErrorMsg(const char* msg){}

    /*
    名称  : on_debug_msg
    功能  : 输出调试日志信息
    入参  : msg  调试日志地址
    出参  : 无
    返回值: 无
    */
    virtual void OnDebugMsg(const char* msg){}

    /*
    名称  : OnInfoMsg
    功能  : 输出一般日志信息
    入参  : msg  一般日志地址
    出参  : 无
    返回值: 无
    */
    virtual void OnInfoMsg(const char* msg){}
};

class i_sl_dce_api
{
public:
    virtual ~i_sl_dce_api(){}

    /*
    名称  : init
    功能  : 初始化API
    入参  : param 初始化参数,包括
            ev    行情输出回调对象指针
    出参  : 无
    返回值: true 成功,false 失败
    */
    virtual bool init(const dce_api_param_t* param,i_sl_dce_api_event* ev) = 0;

    /*
    名称  : start
    功能  : 启动服务
    入参  : 无
    出参  : 无
    返回值: true 成功,false 失败
    */
    virtual bool start() = 0;

    /*
    名称  : get_api_version
    功能  : 获取API版本号
    入参  : 无
    出参  : 无
    返回值: 版本号
    */
    virtual int  get_api_version() = 0;

    /*
    名称  : stop
    功能  : 停止服务
    入参  : 无
    出参  : 无
    返回值: 无
    */
    virtual void stop() = 0;

    /*
    名称  : CreateApiObject
    功能  : 创建API对象指针
    入参  : 无
    出参  : 无
    返回值: API对象指针
    */
    static i_sl_dce_api* CreateApiObject();

    /*
    名称  : ReleaseApiObject
    功能  : 销毁API对象指针
    入参  : API对象指针
    出参  : 无
    返回值: 无
    */
    static void ReleaseApiObject(i_sl_dce_api* obj);
};

#endif