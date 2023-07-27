#ifndef _UNIFIEDDATA_H_
#define _UNIFIEDDATA_H_

#pragma pack(4)
namespace UnifiedMD
{
    typedef unsigned char     BYTE;
    typedef unsigned short    WORD;
    typedef unsigned long     ULONG;
    typedef long              LONG;
    typedef char              INT1;
    typedef short             INT2;
    typedef int               INT4;
    typedef unsigned int      UINT4;
    typedef float             REAL4;
    typedef double            REAL8;
    typedef int               BOOL;

    typedef struct unifiedMD{
        INT1        Contract[80];                         //合约代码
        INT1        GenTime[13];                          //行情产生时间
        REAL8       BuyPriceOne;
        UINT4       BuyQtyOne;
        REAL8       SellPriceOne;
        UINT4       SellQtyOne;
    }LEVEL2ORDERBOOK;
    #pragma pack()
}

#endif