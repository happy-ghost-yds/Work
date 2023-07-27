/**
*  @filename: nProPlus.h
*  @brief: 定义了用于nProPlus行情接收的相关接口
*  @author lifeng023872@gtjas.com
*  @copyright Copyright (c) 2021
*/

#ifndef NPROPlUS_MARKET_H
#define NPROPlUS_MARKET_H

#define EXPORT_NPROPLUS_API __attribute__((visibility("default")))


#include <stdint.h>

struct nProPlusHandle_t;

#define NPP_EXCH_BAD -1
#define NPP_EXCH_SHFE 0 
#define NPP_EXCH_INE 1

#define INST_CHAR_SIZE 36

#pragma pack(push, 1)
struct nProPlusmd
{
    uint8_t version;            //协议版本,目前为5, 低四位为版本， 高四位表示交易所， SHFE 0  INE 1
    uint8_t len;                //总体长度
    uint16_t inst_no;           //合约编号
    uint32_t timestamp;         //时间戳，高位30bits表示秒数相对 0x5e0b7080(该时间表示2020年1月1日0时0分0秒)的正偏移量，最低2bit表示是毫秒数  0, 250, 500, 750
    int32_t last_price;         //最新价, 需要乘以pricetick
    int32_t volume;             //成交量
    int64_t turnover;            //成交额, 需要乘以pricetick
    int64_t open_interest;      //持仓量
    int32_t bid_volume[5];      //五档买手数
    int32_t ask_volume[5];      //五档卖手数
    int32_t bid_price[5];       //五档买价格, 需要乘以pricetick
    int32_t ask_price[5];       //五档卖价格, 需要乘以pricetick
};

struct InsrItem
{   
    uint16_t exch_type;      // 交易所类型， SHFE 0  INE 1
    uint16_t multi;          // 合约乘数
    uint32_t inst_no;        // 交易所合约序号
    double pricetick;        // 合约最小变动价位
    double base_price;       // 合约基准价， 即昨结算价
    char instr[36];          // 合约名称
};

struct nProMD
{
	char instr[INST_CHAR_SIZE];
    double pricetick;
	nProPlusmd md_buffer;
};
#pragma pack(pop)

/** 
 * @brief nProPlus 获取版本信息
 * @return 返回值为字符数组， 形如  v1.0.1_20220330
 */
extern "C" EXPORT_NPROPLUS_API const char* nProPlus_get_version();


/** 
 * @brief nProPlus 获取nProPlus handle
 * @return 返回值为 fpga行情使用的handle
 */
extern "C" EXPORT_NPROPLUS_API nProPlusHandle_t* nProPlus_acquire_handle();



/** 
 * @brief nProPlus 初始化
 * @param handle fpga行情handle
 * @param license_file 授权文件的路径（带文件名）, 一般为/root/nProPlus/license/nProPlus.license， 多板卡需要区分授权文件路径
 * @param nic_name 待接收数据的网口名
 * @param exchange_type 交易所类型
 * @param instr_qry_address 交易所合约序号映射查询服务的地址， 格式形如 tcp://127.0.0.1:12345
 * @return 返回值为0表示初始化成功， 返回值为负表示失败
 */
extern "C" EXPORT_NPROPLUS_API int nProPlus_init(nProPlusHandle_t* handle, const char * license_file, const char * nic_name, int exchange_type, const char* instr_qry_address);


/** 
 * @brief 重置订阅并重新启动nProPlus本地板卡， 网络版无需调用
 * @param handle fpga行情handle
 * @param nic_name 待接收数据的网口名
 * @param instruments 待订阅的合约列表
 * @param cnt 待订阅的合约个数， 若cnt为0， 则表示全部订阅
 * @param local_install_path 本地安装路径
 * @return 返回值为0表示初始化成功， 返回值为负表示失败
 * @remark 该板卡启动函数， 盘中请勿调用，若盘中异常程序重启，重新初始化后， 仍能接收上次订阅的数据
 */
extern "C" EXPORT_NPROPLUS_API int nProPlus_fpga_start(nProPlusHandle_t* handle, const char* instruments[], int cnt, const char* local_install_path = "/root/nProPlus");


/** 
 * @brief 获取nProPlus行情数据， 该函数非线程安全
 * @param handle fpga行情handle
 * @param buffer 存放行情数据的指针
 * @param buffer_size 大小应不小于256
 * @return struct InsrItem*, 若指针为空， 则接收数据为空或失败， 非空则为该条数据对应的合约基础数据
 */
extern "C" EXPORT_NPROPLUS_API const struct InsrItem* nProPlus_fetch_market_data(nProPlusHandle_t* handle, char* buffer, int buffer_size);


/** 
 * @brief 释放资源
 * @param handle fpga行情handle
 */
extern "C" EXPORT_NPROPLUS_API int nProPlus_release(nProPlusHandle_t *handle);

/** 
 * @brief 获取合约基础信息
 * @param handle fpga行情handle
 * @param inst_no 合约序号
 * @param exchange_type 交易所类型，  shfe: 0, ine: 1
 * @return data 获取到的合约基础信息指针
 * @remark 该指针指向内部预分配的内存， 存续期为nProPlus_init之后到nProPlus_release之前， 该内容只读， 请勿修改
 */
extern "C" EXPORT_NPROPLUS_API const struct InsrItem* nProPlus_get_base_info(nProPlusHandle_t* handle, uint32_t inst_no, int exchange_type);

/** 
 * @brief 获取合约总数
 * @param handle fpga行情handle
 * @return data  uint32_t  该板卡对应交易所合约总数
 */
extern "C" EXPORT_NPROPLUS_API uint32_t nProPlus_get_base_info_count(nProPlusHandle_t* handle);



/** 
 * @brief 获取初始化时传入的交易所类型
 * @param handle fpga行情handle
 * @return int 交易所类型， 未初始化: -1, shfe: 0, ine: 1
 */
extern "C" EXPORT_NPROPLUS_API int nProPlus_get_exchange_type(nProPlusHandle_t* handle);


#endif