#include "gr_includes.h"
#include "hal_flash.h"
#include "custom_config.h"
#include "bt_ctl_uart.h"
#include "app_log.h"

#ifdef BLE_BT_OTA_SUPPORT

//创建BT5625通信命令，返回命令的长度
enum{
    BT5625CMD_LinkCheck=0,
    BT5625CMD_ReadReg=3,
    BT5625CMD_WriteReg=1,
    BT5625CMD_SetBaudRate=0x0f,
    BT5625CMD_CheckCRC=0x10,
    BT5625CMD_Reboot=0x0e,
    BT5625CMD_StayRom=0xaa,

    BT5625CMD_StartProg=0xb0,//开始按照flash中的烧录信息进行烧录，相当于烧录器的开始烧录按键
    BT5625CMD_DlpReset=0xb1,//3288 dlp reset,0=mode 0,1=mode 1,2=mode 2;
    BT5625CMD_DlpEnter=0xb2,//3288 dlp reset,get_bus,set BR
    BT5625CMD_SetTarget=0xb3,//设定烧录对象，即后续flash命令的目标

    BT5625CMD_Reset=0xfe,

    BT5625CMD_FlashEraseAll=0x0a,
    BT5625CMD_FlashErase4K=0x0b,
    BT5625CMD_FlashErase=0x0f,
    BT5625CMD_FlashEraseSz=0xfd,
    BT5625CMD_FlashWrite4K=0x07,
    BT5625CMD_FlashWrite=0x06,
    BT5625CMD_FlashReadSR=0x0c,
    BT5625CMD_FlashWriteSR=0x0d,
    BT5625CMD_FlashGetMID=0x0e,
    //OTP扩展命令
    BT5625CMD_OtpEnableWrite=0x70,//OTP写使能
    BT5625CMD_OtpReadDWord=0x71,//OTP读32bits
    BT5625CMD_OtpWriteDWord=0x72,//OTP写32bits

};

typedef struct _t_flashSR_para{
    uint32_t mid;//flash mid
    uint32_t szMem;//flash 容量大小
    int8_t szSR;//SR寄存器数,不大于2
    uint32_t cwUnp;//解保护控制字
    uint32_t cwEnp;//加保护控制字
    uint32_t cwMsk;//保护控制字的屏蔽位
    uint8_t sb;//保护控制字起始位
    uint8_t lb;//保护控制字位长度
    uint8_t cwdRd[4];//各SR控制字读命令字
    uint8_t cwdWr[4];//各SR控制字写命令字,00:写命令字；01:ff=无，其他=写命令字2；02：保留；03：[01]!=0xff时写命令字2的内容
    uint8_t icNam[16];//ic name
    uint8_t manName[16];//制造商名
}_t_flashSR_para;

enum{
    FLASH_ID_XTX_25F08F=0x14400b,//芯天下8M
    FLASH_ID_XTX_25F08B=0x14405e,//芯天下flash,w+v:6.5s,e+w+v:11.5s
    FLASH_ID_MXIC_25V8035F=0x1423c2,//旺宏flash,w+v:8.2s,e+w+v:17.2s
    FLASH_ID_XTX_25F04B=0x13311c,//芯天下flash-4M
    FLASH_ID_GD_25D40=0x134051,//GD flash-4M,w+v:3.1s,e+w+v:5.1s
    FLASH_ID_GD_25D80=0x144051,//GD flash-8M，e+w+v=9.8s
    FLASH_ID_GD_1_25D80=0x1440C8,//GD flash-8M，
    FLASH_ID_Puya_25Q80=0x146085,//puya 8M,w+v:10.4s,e+w+v:11.3s,新版e+w+v：8.3s
    FLASH_ID_Puya_25Q40=0x136085,//puya 4M,e+w+v:6s，新版w+v=4s，e+w+v=4.3s
    FLASH_ID_Puya_25Q16=0x156085,//puya 16M,e+w+v:6s，新版w+v=4s，e+w+v=4.3s
    FLASH_ID_Puya_25Q16SH=0x154285,
    FLASH_ID_Puya_25Q32H=0x166085,//puya 32M******暂时只用于脱机烧录器上7231的外挂flash
    FLASH_ID_GD_25Q16=0x001540c8,//GD 16M******暂时只用于脱机烧录器上7231的外挂flash
    FLASH_ID_GD_25Q16B=0x001540c8,//GD 16M******暂时只用于脱机烧录器上7231的外挂flash
    FLASH_ID_GD_25Q32B=0x001640c8,//GD 16M******暂时只用于脱机烧录器上7231的外挂flash
    FLASH_ID_XTX_25F16B=0x15400b,//XTX 16M
    FLASH_ID_XTX_25F32B =0x0016400b,//xtx 32M******暂时只用于脱机烧录器上7231的外挂flash

    FLASH_ID_MXIC_25V4035F=0x1323c2,//旺宏flash,w+v:8.2s,e+w+v:17.2s
    FLASH_ID_MXIC_25V1635F=0x1523c2,//旺宏flash,w+v:8.2s,e+w+v:17.2s
    FLASH_ID_GD_25Q41B=0x1340c8,//GD flash-4M,w+v:3.1s,e+w+v:5.1s
    FLASH_ID_BY_PN25Q80A=0x1440e0,//GD flash-4M,w+v:3.1s,e+w+v:5.1s
    FLASH_ID_BY_PN25Q40A=0x1340e0,//GD flash-4M,w+v:3.1s,e+w+v:5.1s
    FLASH_ID_UC_25HQ40=0x1360b3,//芯存UC 4M

    FLASH_ID_XTX_25Q64B =0x0017600b,//xtx 64M******暂时只用于脱机烧录器上7231的外挂flash
    FLASH_ID_XTX_25F64B =0x0017400b,//xtx 64M******暂时只用于脱机烧录器上7231的外挂flash
    FLASH_ID_Puya_25Q64H=0x00176085,
    FLASH_ID_GD_25Q64=0x001740c8,
    FLASH_ID_WB_25Q128JV=0x001840ef,
    FLASH_ID_ESMT_25QH16B=0x0015701c,
    FLASH_ID_GD_25WQ64E=0x001765c8,
    FLASH_ID_GD_25WQ32E=0x001665c8,
    FLASH_ID_GD_25WQ16E=0x001565c8,

    FLASH_ID_TH_25Q16HB=0x001560eb,//紫光青藤
    FLASH_ID_TH_25D40HB=0x1360cd,
    FLASH_ID_TH_25D80HB=0x1460cd,
    FLASH_ID_TH_25D20HB=0x001260eb,//紫光青藤-2M

    FLASH_ID_ZB_25D80=0x14325e,//ZBIT

    FLASH_ID_UNKNOWN=-1
//  FLASH_ID_XTX_25F08B=0x14405e,
//  FLASH_ID_MXIC_25V8035F=0x1423c2,
//  FLASH_ID_XTX_25F04B=0x13311c,
//  FLASH_ID_GD_25D40=0x134051,
//  FLASH_ID_GD_25D80=0x144051,
//  FLASH_ID_Puya_25Q80=0x146085,
//  FLASH_ID_Puya_25Q40=0x136085,
//  FLASH_ID_XTX_25F16B=0x15400b,
//  FLASH_ID_GD_25Q16B=0x1540C8,
//  FLASH_ID_XTX_25F32B =0x16400b,//xtx 32M******暂时只用于脱机烧录器上7231的外挂flash
//  FLASH_ID_UNKNOWN=-1
};

typedef int32_t (*CALLBACK_P4)(void * p1,void * p2,void * p3,void * p4);
typedef int32_t (*CALLBACK_P3)(void * p1,void * p2,void * p3);
typedef int32_t (*CALLBACK_P2)(void * p1,void * p2);
typedef int32_t (*CALLBACK_P1)(void * p1);
typedef int32_t (*CALLBACK_P0)(void);

#define BIT(n)      (1<<(n))
#define BFD(v,bs,bl)        ((v&((1<<(bl))-1))<<(bs))
#define GET_ELEMENT_CNT(tbl)    (sizeof(tbl)/sizeof(tbl[0]))

#define Kb          *1024
#define Mb          *1024 Kb

#if (BT_UPGRADE_HIGH_SPEED==true)
#define FLASH_WR_PKG_SIZE   0x1000//4096
#else
#define FLASH_WR_PKG_SIZE   0x100//256
#endif

volatile BT_OTA_STATUS_T bt_ota_state = BT_OTA_IDLE;
volatile BT_OTA_PROTECTFLASH_SUBSTATE_T bt_ota_protectflash_substate = BT_OTA_PROTECTFLASH_IDLE;
volatile BT_OTA_ERASEFLASH_SUBSTATE_T bt_ota_erase_substate = BT_OTA_OPFLASH_IDLE;

uint32_t flash_mid = FLASH_ID_TH_25D80HB;
_t_flashSR_para*flash_psr = NULL;
uint8_t ota_dly=15;

uint8_t chip_keyword[6] = {'B','K','3','2','9','7'};

uint32_t bt_upgrade_fw_size = 512 Kb;
uint32_t bt_upgrade_fw_flash_addr = FLASH_PROGRAM_START_ADDR;
pwr_mgmt_mode_t ota_pwr_mgmt_mode = PMR_MGMT_SLEEP_MODE;


static uint32_t crc32_table[256];

extern uint32_t WATCH_DEMO_UART_BAUDRATE;
extern volatile uint8_t retry_cnt;
extern volatile bool retry_wr ;

extern void ble_bt_uart_init(void);
extern void app_ota_tim_interrupt(void);
extern void app_ota_prt_tim_interrupt(void);
extern void app_bt_ota_env_deinit(void );
extern void app_ota_tim_stop(void);
extern void app_bt_reset_hw(void);

flashsr_op_t current_rd_flashsr;
flashsr_op_t current_wr_flashsr;
flasherase_op_t current_erase_flash;
flashcrc_op_t current_flash_crc;

const _t_flashSR_para tblFlashInt[]={
    {FLASH_ID_XTX_25F08F,
    8 Mb,1,0x00,0x07,
    BIT(14)|BFD(0x0f,2,4),2,5,{0x05,0x35,0xff,0xff},{0x01,0x31,0xff,0xff},
    "XT25F08F","xtx",
    },
    {FLASH_ID_XTX_25F08B,
    8 Mb,1,0x00,0x07,
    BFD(0x0f,2,4),2,4,{0x05,0xff,0xff,0xff},{0x01,0xff,0xff,0xff},
    "PN25F08B","xtx",
    },
    {FLASH_ID_XTX_25F04B,
    4 Mb,1,0x00,0x07,
    BFD(0x0f,2,4),2,4,{0x05,0xff,0xff,0xff},{0x01,0xff,0xff,0xff},
    "PN25F04B","xtx",
    },
    {FLASH_ID_GD_25D40,
    4 Mb,1,0x00,0x07,
    BFD(0x0f,2,3),2,3,{0x05,0xff,0xff,0xff},{0x01,0xff,0xff,0xff},
    "GD25D40","GD",
    },
    {FLASH_ID_GD_25D80,
    8 Mb,1,0x00,0x07,
    BFD(0x0f,2,3),2,3,{0x05,0xff,0xff,0xff},{0x01,0xff,0xff,0xff},
    "GD25D80","GD",
    },
    {FLASH_ID_GD_1_25D80,
    8 Mb,2,0x00,0x07,
    BIT(14)|BFD(0x1f,2,5),2,5,{0x05,0x35,0xff,0xff},{0x01,0xff,0xff,0xff},
    "GD25D80","GD",
    },
    {FLASH_ID_Puya_25Q80,
    8 Mb,2,0x00,0x07,
    BIT(14)|BFD(0x1f,2,5),2,5,{0x05,0x35,0xff,0xff},{0x01,0xff,0xff,0xff},
    "P25Q80","Puya",
    },
    {FLASH_ID_Puya_25Q40,
    4 Mb,2,0x00,0x07,
    BIT(14)|BFD(0x1f,2,5),2,5,{0x05,0x35,0xff,0xff},{0x01,0xff,0xff,0xff},
    "P25Q40","Puya",
    },
    {FLASH_ID_Puya_25Q16,
    16 Mb,2,0x00,/*0x07*/0x00,
    BIT(14)|BFD(0x1f,2,5),2,5,{0x05,0x35,0xff,0xff},{0x01,/*0xff*/0x31,0xff,0x42},
    "P25Q16H","Puya",
    },
    {FLASH_ID_Puya_25Q16SH,
        16 Mb,2,0x00,/*0x07*/0x00,
        BIT(14)|BFD(0x1f,2,5),2,5,{0x05,0x35,0xff,0xff},{0x01,0x31,0xff,0x42},
    "P25Q16SH","Puya",
    },
    {FLASH_ID_Puya_25Q32H,
        32 Mb,2,0x00,/*0x07*/0x00,
        BIT(14)|BFD(0x1f,2,5),2,5,{0x05,0x35,0xff,0xff},{0x01,/*0xff*/0x31,0xff,0x42},
    "P25Q32H","Puya",
    },
    {FLASH_ID_Puya_25Q64H,
    64 Mb,2,0x00,0x07,
    BIT(14)|BFD(0x1f,2,5),2,5,{0x05,0x35,0xff,0xff},{0x01,0xff,0xff,0xff},
    "P25Q64H","Puya",
    },
    {FLASH_ID_XTX_25F16B,
    16 Mb,2,0x00,0x07,
    BIT(14)|BFD(0x1f,2,5),2,5,{0x05,0x35,0xff,0xff},{0x01,0xff,0xff,0xff},
    "XT25F16B","xtx",
    },
    {FLASH_ID_GD_25Q16B,
    16 Mb,2,0x00,0x07,
    BIT(14)|BFD(0x1f,2,5),2,5,{0x05,0x35,0xff,0xff},{0x01,0xff,0xff,0xff},
    "GD25Q16B","GD",
    },
    {FLASH_ID_GD_25Q32B,
    32 Mb,2,0x00,0x07,
    BIT(14)|BFD(0x1f,2,5),2,5,{0x05,0x35,0xff,0xff},{0x01,0x31,0xff,0x00},
    "GD25Q32B","GD",
    },
    {FLASH_ID_MXIC_25V8035F,
    8 Mb,2,0x00,0x07,
    BIT(12)|BFD(0x1f,2,4),2,5,{0x05,0x15,0xff,0xff},{0x01,0xff,0xff,0xff},
    "MX25V8035F","WH",
    },
    {FLASH_ID_MXIC_25V1635F,
    16 Mb,2,0x00,0x07,
    BIT(12)|BFD(0x1f,2,4),2,5,{0x05,0x15,0xff,0xff},{0x01,0xff,0xff,0xff},
    "MX25V1635F","WH",
    },
    {FLASH_ID_XTX_25F32B,
    32 Mb,2,0x00,0x07,
    BIT(14)|BFD(0x1f,2,5),2,5,{0x05,0x35,0xff,0xff},{0x01,0xff,0xff,0xff},
    "XT25F32B","xtx",
    },
    {FLASH_ID_GD_25Q41B,
    4 Mb,1,0x00,0x07,
    BIT(14)|BFD(0x1f,2,3),2,3,{0x05,0x35,0xff,0xff},{0x01,0xff,0xff,0xff},
    "GD25Q41B","GD",
    },
    {FLASH_ID_BY_PN25Q40A,
    4 Mb,1,0x00,0x07,
    BIT(14)|BFD(0x1f,2,3),2,3,{0x05,0x35,0xff,0xff},{0x01,0xff,0xff,0xff},
    "PN25Q40A","BY",
    },
    {FLASH_ID_BY_PN25Q80A,
    8 Mb,1,0x00,0x07,
    BIT(14)|BFD(0x1f,2,3),2,3,{0x05,0x35,0xff,0xff},{0x01,0xff,0xff,0xff},
    "PN25Q80A","BY",
    },
    {FLASH_ID_UC_25HQ40,
        4 Mb,2,0x00,0x07,
        BIT(14)|BFD(0x1f,2,5),2,5,{0x05,0x35,0xff,0xff},{0x01,0xff,0xff,0xff},
        "UC25HQ40","UC",
    },

    {FLASH_ID_XTX_25F64B,
    64 Mb,2,0x00,0x07,
    BIT(14)|BFD(0x1f,2,5),2,5,{0x05,0x35,0xff,0xff},{0x01,0xff,0xff,0xff},
    "XT25F64B","xtx",
    },
    {FLASH_ID_XTX_25Q64B,
    64 Mb,2,0x00,0x07,
    BIT(14)|BFD(0x1f,2,5),2,5,{0x05,0x35,0xff,0xff},{0x01,0xff,0xff,0xff},
    "XT25Q64B","xtx",
    },
    {FLASH_ID_WB_25Q128JV,
    128 Mb,2,0x00,0x07,
    BIT(14)|BFD(0x1f,2,5),2,5,{0x05,0x35,0xff,0xff},{0x01,0xff,0xff,0xff},
    "WB25Q128JV","WB",
    },
    {FLASH_ID_ESMT_25QH16B,
    16 Mb,1,0x00,0x07,
    BFD(0xf,2,5),2,4,{0x05,0xff,0xff,0xff},{0x01,0xff,0xff,0xff},
    "EN25QH16B","ESMT",
    },

    {FLASH_ID_GD_25WQ16E,
    16 Mb,2,0x00,0x07,
    BIT(14)|BFD(0x1f,2,5),2,5,{0x05,0x35,0xff,0xff},{0x01,0xff,0xff,0xff},
    "GD25WQ16E","GD",
    },
    {FLASH_ID_GD_25WQ32E,
    32 Mb,2,0x00,0x07,
    BIT(14)|BFD(0x1f,2,5),2,5,{0x05,0x35,0xff,0xff},{0x01,0xff,0xff,0xff},
    "GD25WQ32E","GD",
    },
    {FLASH_ID_GD_25WQ64E,
    64 Mb,2,0x00,0x07,
    BIT(14)|BFD(0x1f,2,5),2,5,{0x05,0x35,0xff,0xff},{0x01,0xff,0xff,0xff},
    "GD25WQ64E","GD",
    },
    {FLASH_ID_GD_25Q64,
    64 Mb,2,0x00,0x07,
    BIT(14)|BFD(0x1f,2,5),2,5,{0x05,0x35,0xff,0xff},{0x01,0xff,0xff,0xff},
    "GD25Q64","GD",
    },
    {FLASH_ID_TH_25Q16HB,
    16 Mb,2,0x00,0x07,
    BIT(14)|BFD(0x1f,2,5),2,5,{0x05,0x35,0xff,0xff},{0x01,0xff,0xff,0xff},
    "TH25Q16HB","TH",
    },
    {FLASH_ID_TH_25D20HB,
    2 Mb,2,0x00,/*0x07*/0x07,
    BIT(14)|BFD(0x1f,2,5),2,5,{0x05,0x35,0xff,0xff},{0x01,0xff,0xff,0xff},
    "TH25D40HB","TH",
    },
    {FLASH_ID_TH_25D40HB,
    4 Mb,2,0x00,/*0x07*/0x07,
    BIT(14)|BFD(0x1f,2,5),2,5,{0x05,0x35,0xff,0xff},{0x01,0xff,0xff,0xff},
    "TH25D40HB","TH",
    },
    {FLASH_ID_TH_25D80HB,
    8 Mb,2,0x00,/*0x07*/0x07,
    BIT(14)|BFD(0x1f,2,5),2,5,{0x05,0x35,0xff,0xff},{0x01,0xff,0xff,0xff},
    "TH25Q80HB","TH",
    },
    {FLASH_ID_ZB_25D80,
    8 Mb,1,0x00,0x07,
    BFD(0x1f,2,3),2,3,{0x05,0xff,0xff,0xff},{0x01,0xff,0xff,0xff},
    "ZB25D80","ZB",
    },
};

int32_t BuildBT5625Cmd_Reset(uint8_t *buf)
{
    int32_t len=1+(4);
    buf[0]=0x01;
    buf[1]=0xe0;
    buf[2]=0xfc;
    buf[3]=len;
    buf[4]=BT5625CMD_Reset;
    buf[5]=0x95;
    buf[6]=0x27;
    buf[7]=0x95;
    buf[8]=0x27;
    return(len+4);
}

int32_t BuildBT5625Cmd_StayRom(uint8_t *buf)
{
    int32_t len=1+(1);
    buf[0]=0x01;
    buf[1]=0xe0;
    buf[2]=0xfc;
    buf[3]=len;
    buf[4]=BT5625CMD_StayRom;
    buf[5]=0x55;
    return(len+4);
}

int32_t BuildBT5625Cmd_LinkCheck(uint8_t *buf)
{
    int32_t len=1;
    buf[0]=0x01;
    buf[1]=0xe0;
    buf[2]=0xfc;
    buf[3]=len;
    buf[4]=BT5625CMD_LinkCheck;
    return(len+4);
}

int32_t BuildBT5625Cmd_FlashGetMID(uint8_t *buf,uint8_t regAddr)
{
    int32_t len=(1+4);
    buf[0]=0x01;
    buf[1]=0xe0;
    buf[2]=0xfc;
    buf[3]=0xff;
    buf[4]=0xf4;
    buf[5]=(len&0xff);
    buf[6]=((len>>8)&0xff);
    buf[7]=BT5625CMD_FlashGetMID;
    buf[8]=(regAddr&0xff);
    buf[9]=0;
    buf[10]=0;
    buf[11]=0;
    return(len+7);
}

int32_t BuildBT5625Cmd_FlashWrite4K(uint8_t *buf,uint32_t addr,uint8_t *dat)
{
    int32_t len=1+(4+4*1024);
    buf[0]=0x01;
    buf[1]=0xe0;
    buf[2]=0xfc;
    buf[3]=0xff;
    buf[4]=0xf4;
    buf[5]=(len&0xff);
    buf[6]=((len>>8)&0xff);
    buf[7]=BT5625CMD_FlashWrite4K;
    buf[8]=(addr&0xff);
    buf[9]=((addr>>8)&0xff);
    buf[10]=((addr>>16)&0xff);
    buf[11]=((addr>>24)&0xff);
    memcpy(&buf[12],dat,4*1024);
    return(len+7);
}

int BuildBT5625Cmd_FlashWrite(uint8_t *buf,uint32_t addr,uint8_t *dat,int sz)
{
    int len=1+(4+sz);
    buf[0]=0x01;
    buf[1]=0xe0;
    buf[2]=0xfc;
    buf[3]=0xff;
    buf[4]=0xf4;
    buf[5]=(len&0xff);
    buf[6]=((len>>8)&0xff);
    buf[7]=BT5625CMD_FlashWrite;
    buf[8]=(addr&0xff);
    buf[9]=((addr>>8)&0xff);
    buf[10]=((addr>>16)&0xff);
    buf[11]=((addr>>24)&0xff);
    memcpy(&buf[12],dat,sz);
    return(len+7);
}

int32_t BuildBT5625Cmd_FlashErase(uint8_t *buf,uint32_t addr,uint8_t szCmd)
{
    int32_t len=1+(4+1);
    buf[0]=0x01;
    buf[1]=0xe0;
    buf[2]=0xfc;
    buf[3]=0xff;
    buf[4]=0xf4;
    buf[5]=(len&0xff);
    buf[6]=((len>>8)&0xff);
    buf[7]=BT5625CMD_FlashErase;
    buf[8]=szCmd;
    buf[9]=(addr&0xff);
    buf[10]=((addr>>8)&0xff);
    buf[11]=((addr>>16)&0xff);
    buf[12]=((addr>>24)&0xff);
    return(len+7);
}

int32_t BuildBT5625Cmd_CheckCRC(uint8_t *buf,uint32_t startAddr,uint32_t endAddr)
{
    int32_t len=1+(4+4);
    buf[0]=0x01;
    buf[1]=0xe0;
    buf[2]=0xfc;
    buf[3]=len;
    buf[4]=BT5625CMD_CheckCRC;
    buf[5]=(startAddr&0xff);
    buf[6]=((startAddr>>8)&0xff);
    buf[7]=((startAddr>>16)&0xff);
    buf[8]=((startAddr>>24)&0xff);
    buf[9]=(endAddr&0xff);
    buf[10]=((endAddr>>8)&0xff);
    buf[11]=((endAddr>>16)&0xff);
    buf[12]=((endAddr>>24)&0xff);
    return(len+4);
}

int32_t BuildBT5625Cmd_Reboot(uint8_t *buf)
{
    int32_t len=1+(1);
    buf[0]=0x01;
    buf[1]=0xe0;
    buf[2]=0xfc;
    buf[3]=len;
    buf[4]=BT5625CMD_Reboot;
    buf[5]=0xa5;
    return(len+4);
}

int32_t BuildBT5625Cmd_FlashReadSR(uint8_t *buf,uint8_t regAddr)
{
    int32_t len=1+(1+0);
    buf[0]=0x01;
    buf[1]=0xe0;
    buf[2]=0xfc;
    buf[3]=0xff;
    buf[4]=0xf4;
    buf[5]=(len&0xff);
    buf[6]=((len>>8)&0xff);
    buf[7]=BT5625CMD_FlashReadSR;
    buf[8]=(regAddr&0xff);
    return(len+7);
}

int32_t BuildBT5625Cmd_FlashWriteSR(uint8_t *buf,uint8_t regAddr,uint8_t val)
{
    int32_t len=1+(1+1);
    buf[0]=0x01;
    buf[1]=0xe0;
    buf[2]=0xfc;
    buf[3]=0xff;
    buf[4]=0xf4;
    buf[5]=(len&0xff);
    buf[6]=((len>>8)&0xff);
    buf[7]=BT5625CMD_FlashWriteSR;
    buf[8]=(regAddr&0xff);
    buf[9]=((val)&0xff);
    return(len+7);
}

int32_t BuildBT5625Cmd_FlashWriteSR2(uint8_t *buf,uint8_t regAddr,uint16_t val)
{
    int32_t len=1+(1+2);
    buf[0]=0x01;
    buf[1]=0xe0;
    buf[2]=0xfc;
    buf[3]=0xff;
    buf[4]=0xf4;
    buf[5]=(len&0xff);
    buf[6]=((len>>8)&0xff);
    buf[7]=BT5625CMD_FlashWriteSR;
    buf[8]=(regAddr&0xff);
    buf[9]=((val)&0xff);
    buf[10]=((val>>8)&0xff);
    return(len+7);
}

int32_t CheckRespond_StayRom(uint8_t *buf)
{
    uint8_t cBuf[256]={0x04,0x0e,0x05,0x01,0xe0,0xfc,BT5625CMD_StayRom};
    cBuf[2]=3+1+1;
    if(memcmp(cBuf,buf,7)==0){
        if(buf[7]==0x55)return(1);
    }
    return(0);
}

int32_t CheckRespond_SetBaudRate(uint8_t *buf,uint32_t baudrate,uint8_t dly_ms)
{
    uint8_t cBuf[256]={0x04,0x0e,0x05,0x01,0xe0,0xfc,BT5625CMD_SetBaudRate};
    cBuf[2]=3+1+4+1;
    cBuf[7]=(baudrate&0xff);
    cBuf[8]=((baudrate>>8)&0xff);
    cBuf[9]=((baudrate>>16)&0xff);
    cBuf[10]=((baudrate>>24)&0xff);
    cBuf[11]=(dly_ms&0xff);

    if(memcmp(cBuf,buf,12)==0){
        return(1);
    }
    return(0);
}

int32_t CheckRespond_LinkCheck(uint8_t *buf)
{
    const uint8_t cBuf[]={0x04,0x0e,0x05,0x01,0xe0,0xfc,BT5625CMD_LinkCheck+1,0x00};
    if(memcmp(cBuf,buf,8)==0)return(1);
    return(0);
}

int32_t CheckRespond_FlashGetMID(uint8_t *buf,uint32_t*val,uint8_t*status)
{
    uint8_t cBuf[256]={
        0x04,0x0e,0xff,0x01,
        0xe0,0xfc,0xf4,(1+4)&0xff,
        ((1+4)>>8)&0xff,BT5625CMD_FlashGetMID};
        //uint32_t t;
        if((memcmp(cBuf,buf,10)==0)){
            if(status){
                *status=buf[10];
            }
            uint32_t*p32=(uint32_t*)&buf[11];
            if(val)*val=*p32;
            return(1);
        }else {
            cBuf[7]++;
            if((memcmp(cBuf,buf,10)==0)){
                if(status){
                    *status=buf[10];
                }
                uint32_t*p32=(uint32_t*)&buf[11];
                if(val)*val=*p32;
                return(1);
            }
        }
        return(0);
}

int32_t CheckRespond_FlashWriteSR(uint8_t *buf,uint8_t regAddr,uint8_t val,uint8_t*status)
{
    uint8_t cBuf[256]={
        0x04,0x0e,0xff,0x01,
        0xe0,0xfc,0xf4,(1+1+(1+1))&0xff,
        ((1+1+(1+1))>>8)&0xff,BT5625CMD_FlashWriteSR};
        if((val==buf[12])&&(regAddr==buf[11])&&(memcmp(cBuf,buf,10)==0)){
//          uint32_t t;
            if(status){
                *status=buf[10];
            }
            return(1);
        }
        return(0);
}

int32_t CheckRespond_FlashReadSR(uint8_t *buf,uint8_t regAddr,uint8_t*status,uint32_t*sr)
{
    uint8_t cBuf[256]={
        0x04,0x0e,0xff,0x01,
        0xe0,0xfc,0xf4,(1+1+(1+1))&0xff,
        ((1+1+(1+1))>>8)&0xff,BT5625CMD_FlashReadSR};
        //      uint8_t tbuf[32];
        //      memcpy(tbuf,buf,13);
        if((regAddr==buf[11])&&(memcmp(cBuf,buf,10)==0)){
//          uint32_t t;
            if(status){
                *status=buf[10];
            }
            if(sr)*sr=buf[12];
            return(1);
        }
        return(0);
}

int32_t CheckRespond_FlashWriteSR2(uint8_t *buf,uint8_t regAddr,uint16_t val,uint8_t*status)
{
    uint8_t cBuf[256]={
        0x04,0x0e,0xff,0x01,
        0xe0,0xfc,0xf4,(1+1+(1+2))&0xff,
        ((1+1+(1+2))>>8)&0xff,BT5625CMD_FlashWriteSR};
        if(((val&0xff)==buf[12])&&(((val>>8)&0xff)==buf[13])&&(regAddr==buf[11])&&(memcmp(cBuf,buf,10)==0))
        {
//          uint32_t t;
            if(status)
            {
                *status=buf[10];
            }
            return(1);
        }
        return(0);
}

int32_t CheckRespond_FlashErase(uint8_t *buf,uint32_t addr,uint32_t szCmd,uint8_t*status)
{
    uint8_t cBuf[256]={
        0x04,0x0e,0xff,0x01,
        0xe0,0xfc,0xf4,1+1+(1+4),
        0x00,BT5625CMD_FlashErase};
        if((buf[11]==szCmd)&&(memcmp(&buf[12],&addr,4)==0)&&(memcmp(cBuf,buf,10)==0))
        {
//          uint32_t t;
            if(status)
            {
                *status=buf[10];
            }
            return(1);
        }
        return(0);
}

int32_t CheckRespond_FlashWrite4K(uint8_t *buf,uint32_t addr,uint8_t*status)
{
    uint8_t cBuf[256]={
        0x04,0x0e,0xff,0x01,
        0xe0,0xfc,0xf4,1+1+(4),
        0x00,BT5625CMD_FlashWrite4K};
        if((memcmp(&buf[11],&addr,4)==0)&&(memcmp(cBuf,buf,10)==0))
        {
//          uint32_t t;
            if(status)
            {
                *status=buf[10];
            }
            return(1);
        }
        return(0);
}

int CheckRespond_FlashWrite(uint8_t *buf,uint32_t addr,uint8_t *status,uint32_t *sz)
{
    uint8_t cBuf[256]={
        0x04,0x0e,0xff,0x01,
        0xe0,0xfc,0xf4,(1+1+(4+1))&0xff,
        ((1+1+(4+1))>>8)&0xff,BT5625CMD_FlashWrite};

        if((memcmp(&buf[11],&addr,4)==0)&&(memcmp(cBuf,buf,10)==0))
        {
            if(status)
            {
                *status=buf[10];
            }
            if(sz)*sz=(uint32_t)buf[15];
            return(1);
        }
        return(0);
}

int32_t CheckRespond_CheckCRC(uint8_t *buf,uint32_t startAddr,uint32_t endAddr,uint32_t*val)
{
    uint8_t cBuf[256]={0x04,0x0e,0x05,0x01,0xe0,0xfc,BT5625CMD_CheckCRC};
    cBuf[2]=3+1+4;
    if(memcmp(cBuf,buf,7)==0)
    {
        if(val)
        {
            uint32_t t;
            t=buf[10];
            t=(t<<8)+buf[9];
            t=(t<<8)+buf[8];
            t=(t<<8)+buf[7];
            *val=t;
        }
        return(1);
    }
    return(0);
}

void *GetFlashListPtr(void)
{
    return((void*)tblFlashInt);
}

int32_t GetFlashListCount(void)
{
    return(GET_ELEMENT_CNT(tblFlashInt));
}

void*GetFlashInfo(uint32_t mid)
{
    _t_flashSR_para*psr=(_t_flashSR_para*)GetFlashListPtr();
    int32_t i,j=GetFlashListCount();//sizeof(tblFlashInt)/sizeof(_t_flashSR_para);
    for(i=0;i<j;i++)
    {
        if(psr->mid==mid)
            return((_t_flashSR_para*)psr);
        psr++;
    }
    return(NULL);
}

void ReadFlashSR(int32_t regCmd)
{
    uint8_t txbuf[32];
    int32_t txl;

    current_rd_flashsr.regAddr = regCmd;
    current_rd_flashsr.val = 0;
    current_rd_flashsr.status = 0;

    txl=BuildBT5625Cmd_FlashReadSR((uint8_t*)txbuf,regCmd);

    bt_ota_protectflash_substate = BT_OTA_PROTECTFLASH_RDSR1;
    watch_uart_data_tx_send(txbuf,txl);
     while(bt_ota_protectflash_substate == BT_OTA_PROTECTFLASH_RDSR1);
}

void WriteFlashSR(int32_t sz, int32_t regCmd, uint32_t val)
{
    uint8_t txbuf[32];
    int32_t txl;

    current_wr_flashsr.regAddr = regCmd;
    current_wr_flashsr.val = val;
    current_wr_flashsr.status = 0;

    if(sz==1)
     {
        bt_ota_protectflash_substate = BT_OTA_PROTECTFLASH_WRSR1;
        txl=BuildBT5625Cmd_FlashWriteSR((uint8_t*)txbuf,regCmd,val);
    }
    else
    {
        bt_ota_protectflash_substate = BT_OTA_PROTECTFLASH_WRSR2;
        txl=BuildBT5625Cmd_FlashWriteSR2((uint8_t*)txbuf,regCmd,val);
    }

    watch_uart_data_tx_send(txbuf,txl);
    while((bt_ota_protectflash_substate == BT_OTA_PROTECTFLASH_WRSR1) ||(bt_ota_protectflash_substate == BT_OTA_PROTECTFLASH_WRSR2));
}

int32_t WaitForWIP(int32_t regCmd)
{
    ReadFlashSR(regCmd);
    if(bt_ota_protectflash_substate == BT_OTA_PROTECTFLASH_RDSR1_RSP_E)
    {
        return(-1);
    }

    if(current_rd_flashsr.val&BIT(0))
        {
            return(0);
    }

    return(1);
}


int32_t BuildBT5625Cmd_SetBaudRate(uint8_t *buf,uint32_t baudrate,uint8_t dly_ms)
{
    int32_t len=1+(4+1);
    buf[0]=0x01;
    buf[1]=0xe0;
    buf[2]=0xfc;
    buf[3]=len;
    buf[4]=BT5625CMD_SetBaudRate;
    buf[5]=(baudrate&0xff);
    buf[6]=((baudrate>>8)&0xff);
    buf[7]=((baudrate>>16)&0xff);
    buf[8]=((baudrate>>24)&0xff);
    buf[9]=(dly_ms&0xff);
    return(len+4);
}


int32_t bt_ota_AppReset(void)
{
    uint8_t txbuf[32];
    int32_t txl;

    WATCH_DEMO_UART_BAUDRATE = WATCH_DEMO_UART_BAUDRATE_ORI;
    ble_bt_uart_init();

    txl=BuildBT5625Cmd_Reset((uint8_t*)txbuf);
    watch_uart_data_tx_sync_send(txbuf,txl);

    return 1;
}

void bt_ota_StayRom(void)
{
    uint8_t txbuf[32];
    int32_t txl;

    txl=BuildBT5625Cmd_StayRom((uint8_t*)txbuf);
    watch_uart_data_tx_sync_send(txbuf,txl);
}

void bt_ota_SetBR(int32_t br, uint8_t dly)
{
    uint8_t txbuf[32];
    int32_t txl;

    bt_ota_state = BT_OTA_SETBR;
    txl=BuildBT5625Cmd_SetBaudRate((uint8_t*)txbuf,br,dly);
    watch_uart_data_tx_sync_send(txbuf,txl);

    //reset local baudrate
    WATCH_DEMO_UART_BAUDRATE = br;
    ble_bt_uart_init();

    while(bt_ota_state == BT_OTA_SETBR);
}

void bt_ota_LinkCheck(void)
{
    uint8_t txbuf[32];
    int32_t txl;

    bt_ota_state = BT_OTA_LINKCHECK;
    txl=BuildBT5625Cmd_LinkCheck((uint8_t*)txbuf);
    watch_uart_data_tx_send(txbuf,txl);
    while(bt_ota_state == BT_OTA_LINKCHECK);
}

void bt_ota_GetFlashMID(void)
{
    uint8_t txbuf[32];
    int32_t txl;

    bt_ota_state = BT_OTA_GETMID;
    txl=BuildBT5625Cmd_FlashGetMID((uint8_t*)txbuf,0x9f);
    watch_uart_data_tx_send(txbuf,txl);
    while(bt_ota_state == BT_OTA_GETMID);
}

int32_t bt_ota_WriteSector(uint32_t addr, uint8_t* buf)
{
    uint8_t txbuf[5*1024];
    int32_t txl;

    bt_ota_erase_substate = BT_OTA_OPFLASH_WRITE1SEC;
    current_erase_flash.addr = addr;

    txl=BuildBT5625Cmd_FlashWrite4K((uint8_t*)txbuf,addr,buf);
    watch_uart_data_tx_sync_send(txbuf,txl);

    while(bt_ota_erase_substate == BT_OTA_OPFLASH_WRITE1SEC)
    {
        if (retry_wr)
        {
            retry_wr = false;
            break;
        }
    }

    if(bt_ota_erase_substate == BT_OTA_OPFLASH_WRITE1SEC_RSP)
    {
        return(1);
    }
    return(-2);

}

int32_t bt_ota_Write(uint32_t addr, uint8_t* buf)
{
    uint8_t txbuf[FLASH_WR_PKG_SIZE + 20];
    int32_t txl;

    bt_ota_erase_substate = BT_OTA_OPFLASH_WRITE1SEC;
    current_erase_flash.addr = addr;

    txl=BuildBT5625Cmd_FlashWrite((uint8_t*)txbuf,addr,buf, FLASH_WR_PKG_SIZE);
    watch_uart_data_tx_sync_send(txbuf,txl);

    while(bt_ota_erase_substate == BT_OTA_OPFLASH_WRITE1SEC)
    {
        if (retry_wr)
        {
            retry_wr = false;
            break;
        }
    }

    if(bt_ota_erase_substate == BT_OTA_OPFLASH_WRITE1SEC_RSP)
    {
        return(1);
    }

    return(-2);

}

int32_t bt_ota_EraseBlock(uint8_t cmd, uint32_t addr)
{
    uint8_t txbuf[32];
    int32_t txl;

    bt_ota_erase_substate = BT_OTA_OPFLASH_EREASE;

    current_erase_flash.addr = addr;
    current_erase_flash.szCmd = cmd;

    txl=BuildBT5625Cmd_FlashErase((uint8_t*)txbuf,addr,cmd);
    watch_uart_data_tx_send(txbuf,txl);

    while(bt_ota_erase_substate == BT_OTA_OPFLASH_EREASE);

    if(bt_ota_erase_substate == BT_OTA_OPFLASH_EREASE_RSP)
    {
        return(1);
    }

    return -2;
}

int bt_ota_WriteFlashBytes(uint32_t addr, uint8_t* dat, uint32_t sz)
{
    uint8_t txbuf[512];
    int32_t txl;

    bt_ota_state = BT_OTA_WRITE_CHIP_NAME;

    current_erase_flash.addr = addr;
    current_erase_flash.szCmd = sz;
    current_erase_flash.status = 0;

    txl=BuildBT5625Cmd_FlashWrite((uint8_t*)txbuf,addr,dat,sz);
    watch_uart_data_tx_send(txbuf,txl);

    while(bt_ota_state == BT_OTA_WRITE_CHIP_NAME);

    if(bt_ota_state == BT_OTA_WRITE_CHIP_NAME_RSP)
    {
        return(1);
    }

    return(-2);
}

int32_t bt_ota_ReadCRC(uint32_t sAdr, uint32_t sz)
{
    uint8_t txbuf[32];
    int32_t txl;

    bt_ota_state = BT_OTA_READCRC;
    current_flash_crc.saddr = sAdr;
    current_flash_crc.sz = sz;
    current_flash_crc.crc = 0;

    txl=BuildBT5625Cmd_CheckCRC((uint8_t*)txbuf,sAdr,sAdr+sz-1);
    watch_uart_data_tx_send(txbuf,txl);

    while(bt_ota_state == BT_OTA_READCRC);

    if(bt_ota_state == BT_OTA_READCRC_RSP)
    {
        return(1);
    }

    return(-2);
}

int32_t bt_ota_Reboot(void)
{
    uint8_t txbuf[32];
    int32_t txl;
    int32_t i;
    for(i=0;i<1;i++)
    {
        txl=BuildBT5625Cmd_Reboot((uint8_t*)txbuf);
        watch_uart_data_tx_send(txbuf,txl);
        delay_ms(10);
    }
    return 1;
}

static uint32_t make_crc32_table(void)
{
    static int32_t bFlag=0;
    if(bFlag==1)return(0);
    uint32_t c;
    int32_t i = 0;
    int32_t bit = 0;

    for(i = 0;i < 256;i++)
    {
        c = (uint32_t)i;

        for(bit = 0;bit < 8;bit++)
        {
            if(c&1)
            {
                c = (c>>1)^(0xEDB88320);
            }
            else
            {
                c = c >> 1;
            }
        }

        crc32_table[i] = c;
        //      APP_LOG_DEBUG("crc32_table[%d] = %08x\r\n",i,crc32_table[i]);
    }
    bFlag=1;
    return 0;
}

uint32_t crc32_ver2(uint32_t crc, const uint8_t *buf, int32_t len)
{
    if (crc == 0xffffffff)
    {
        make_crc32_table();
    }

    while(len--)
    {
        crc = (crc >> 8)^(crc32_table[(crc^*buf++)&0xff]);

    }

    return crc;
}

int32_t _uart_erase_cmd(void *p1,void *p2)
{
    int32_t cmd=(int32_t)p2;
    uint32_t addr=(uint32_t)p1;

    if(cmd==0)return bt_ota_EraseBlock(0x20,addr&0xfffff000);
    if(cmd==1)return bt_ota_EraseBlock(0xd8,addr&0xfffff000);
    if(cmd==2)return bt_ota_EraseBlock(0x52,addr&0xfffff000);
    return -1;
}

int32_t _uart_write1sec(void*p1,void*p2)
{
    return bt_ota_WriteSector((uint32_t)p2,(uint8_t*)p1);
}

int32_t _uart_write_define_size(void*p1,void*p2)
{
    int32_t ret = -2;
    do
    {
        ret = bt_ota_Write((uint32_t)p2,(uint8_t*)p1);
        if ( ret == 1)
        {
            break;
        }
        APP_LOG_DEBUG("retry write 0x%08x, %d times \r\n", p2, 3-retry_cnt);
    }while((bt_ota_state != BT_OTA_IDLE) && (retry_cnt--));

    retry_cnt = 3;

    return ret;
}

int32_t _uart_dbgmsg(void*p1)
{
    //APP_LOG_DEBUG("cur addr=%.8x\r\n",(uint32_t)p1);
    return 1;
}

int32_t _erase_flash_range_(int32_t startAddr,int32_t len,void*ersCbk,void*rd1sCbk,void*wr1sCbk,void*othCbk,void*fbStop,void*curAddr)
{
    CALLBACK_P2 eraseFun=(CALLBACK_P2)ersCbk;
    CALLBACK_P2 readSec=(CALLBACK_P2)rd1sCbk;
    CALLBACK_P2 writeSec=(CALLBACK_P2)wr1sCbk;
    CALLBACK_P1 apdCbk=(CALLBACK_P1)othCbk;
    int32_t*pCurAddr=(int32_t*)curAddr;

    int32_t*ifbStop=(int32_t*)fbStop;
    int32_t i;

    uint8_t buf[0x1000];
    uint32_t addr;
    int32_t j,k;
    for(i=startAddr;i<(startAddr+len);)
    {
        if(ifbStop)
        {
            if(*ifbStop)
                return(-10);
        }

        addr=i;
        if(pCurAddr)
            *pCurAddr=i;
        if(((i&0xffff)==0)&&((startAddr+len-i)>=0x10000))
        {
            if(eraseFun((void *)i, (void *)1)<1)
                return(-1);
            i+=0x10000;
        }
        else if(((i&0x7fff)==0)&&((startAddr+len-i)>=0x8000))
        {
            if(eraseFun((void *)i, (void *)2)<1)
                return(-2);;
            i+=0x8000;
        }
        else if(((i&0xfff)==0))
        {
            if((startAddr+len-i)>=0x1000)
            {
                if(eraseFun( (void *)i, (void *)0)<1)
                    return(-3);
                i+=0x1000;
            }
            else
            {//尾部处理
                if(readSec)
                    if(readSec( (void *)buf, (void *)(i&0xfffff000))<1)
                        return(-6);;
                if(eraseFun( (void *)(i&0xfffff000), (void *)0)<1)
                    return(-4);
                memset(buf,0xff,startAddr+len-i);
                if(readSec&&writeSec)
                    if(writeSec( (void *)buf, (void *)(i&0xfffff000))<1)
                        return(-7);
                i+=(startAddr+len-i);
            }
        }
        else
        {
            j=(i+0xfff)&0xfffff000;
            if((startAddr+len)>j)
            {//头部处理
                k=j-i;
            }
            else
            {
                k=(startAddr+len)-i;
            }
            if(readSec)
                if(readSec( (void *)buf, (void *)(i&0xfffff000))<1)
                    return(-8);
            if(eraseFun( (void *)(i&0xfffff000), (void *)0)<1)
                return(-5);;
            memset(&buf[i&0xfff],0xff,k);
            if(readSec&&writeSec)
                if(writeSec( (void *)buf, (void *)(i&0xfffff000))<1)
                    return(-9);
            i+=k;
        }

        if(apdCbk)
        {
            if(apdCbk( (void *)addr)<1)
                return -11;
        }
    }
    return(1);
}

int32_t spi_flash_read_fw(uint32_t fw_qspi_addr, uint8_t *fw_4K_addr)
{
    uint8_t flash_read_idx = 0;
#if (FLASH_WR_PKG_SIZE==0x100)
    uint8_t flash_read_max = 1;
    uint16_t flash_read_block = FLASH_WR_PKG_SIZE;
#else
    uint8_t flash_read_max = 4;
    uint16_t flash_read_block = 1024;
#endif

    if (fw_4K_addr == NULL)
    {
        return -1;
    }

    for (flash_read_idx=0; flash_read_idx<flash_read_max; flash_read_idx++)
    {
        hal_exflash_read(fw_qspi_addr + flash_read_idx * flash_read_block, &fw_4K_addr[flash_read_idx * flash_read_block], flash_read_block);
    }

    current_flash_crc.crc_me = crc32_ver2(current_flash_crc.crc_me,(uint8_t*)fw_4K_addr, flash_read_max*flash_read_block);

    return 0;
}

int32_t _write_flash_data_(int32_t startAddr, uint32_t flash_addr, int32_t len,void*rd1sCbk,void*wr1sCbk,void*othCbk,void*fbStop,void*curAddr){
    CALLBACK_P2 readSec=(CALLBACK_P2)rd1sCbk;
    CALLBACK_P2 writeSec=(CALLBACK_P2)wr1sCbk;
    CALLBACK_P1 apdCbk=(CALLBACK_P1)othCbk;

    int32_t*ifbStop=(int32_t*)fbStop;
    int32_t i;
    //uint8_t*buf1=(uint8_t*)fbuf;

    uint8_t buf[FLASH_WR_PKG_SIZE] = {0};;
    int32_t j,k;
    int32_t*pCurAddr=(int32_t*)curAddr;
    uint32_t addr;

    for(i=startAddr;i<(startAddr+len);)
    {
        addr=i;
        if(pCurAddr)
            *pCurAddr=i;

        if(ifbStop)
        {
            if(*ifbStop)
                return(-10);
        }

        if(((i&(FLASH_WR_PKG_SIZE-1))==0))
        {
            if((startAddr+len-i)>=FLASH_WR_PKG_SIZE)
            {
                spi_flash_read_fw(flash_addr + i-startAddr, buf);
                if(writeSec( (void *)buf, (void *)i)<1)
                    return(-2);
                i+=FLASH_WR_PKG_SIZE;
            }
            else
            {//尾部处理
                if (len < FLASH_WR_PKG_SIZE)
                {
                    if(readSec( (void *)buf, (void *)i)<1)
                        return(-4);

                    k=startAddr+len-i;
                    //memcpy(buf,&buf1[i-startAddr],k);

                    if(writeSec( (void *)buf, (void *)i)<1)
                        return(-5);

                    i+=(startAddr+len-i);
                }
                else
                {
                    i = startAddr + len - FLASH_WR_PKG_SIZE;
                    spi_flash_read_fw(flash_addr + i-startAddr, buf);
                    if(writeSec( (void *)buf, (void *)i)<1)
                        return(-2);
                }

            }
        }
        else
        {
            j=(i+0xfff)&0xfffff000;
            if((startAddr+len)>j)
            {//头部处理
                k=j-i;
            }
            else
            {
                k=(startAddr+len)-i;
            }
            if(readSec( (void *)buf, (void *)(i&0xfffff000))<1)
                return(-7);

            //memcpy(&buf[i&0xfff],buf1,k);

            if(writeSec( (void *)buf, (void *)(i&0xfffff000))<1)
                return(-8);
            i+=k;
        }

        if(apdCbk)
        {
            if(apdCbk( (void *)addr)<1)
                return(-9);
        }
    }
    return(1);
}

int32_t bt_ota_ProtectFlash(void *ptr, int32_t mid, int32_t en_dis)
{
    int32_t i,/*j,*/f;
    _t_flashSR_para*psr=(_t_flashSR_para*)ptr;

    if(psr==NULL)return(-1);
    uint32_t srt,sr;
    uint32_t cw;
    if(en_dis)
        cw=psr->cwEnp;//0x07
    else
        cw=psr->cwUnp;

    bt_ota_state = BT_OTA_PROTECTFLASH;
#if 0
    while(1){
        sr=0;
        for(i=0;i<psr->szSR;i++){
            f=ReadFlashSR(psr->cwdRd[i],&srt);
            if(f>0){
                sr|=(srt<<(8*i));
            }
        }
        if((sr&psr->cwMsk)==BFD(cw,psr->sb,psr->lb))return(1);
        if(mt.GetTimePast()>1)return(-2);
        srt=sr&(psr->cwMsk^0xffffffff);
        srt|=BFD(cw,psr->sb,psr->lb);
        f=WriteFlashSR(psr->szSR,psr->cwdWr[0],srt&0xffff);
        Sleep(10);
    }
#endif
    int32_t r;
    while(1)
    {
        sr=0;
        for(i=0;i<psr->szSR;i++)
        {
            ReadFlashSR(psr->cwdRd[i]);

            if(bt_ota_protectflash_substate == BT_OTA_PROTECTFLASH_RDSR1_RSP)
            {
                sr|=(current_rd_flashsr.val<<(8*i));
            }
        }
        if((sr&psr->cwMsk)==BFD(cw,psr->sb,psr->lb))
            return(1);
                // TODO : timeout
        //if(mt.GetTimePast()>1)return(-2);
        //      if(mid!=FLASH_ID_Puya_25Q16SH){
        if(0xff==psr->cwdWr[1])
        {
            srt=sr&(psr->cwMsk^0xffffffff);
            srt|=BFD(cw,psr->sb,psr->lb);
                        //bt_ota_protectflash_substate = BT_OTA_PROTECTFLASH_WRSR1;
            WriteFlashSR(psr->szSR,psr->cwdWr[0],srt&0xffff);

            delay_ms(1*1000);
        }
        else
        {
            srt=sr&(psr->cwMsk^0xffffffff);
            srt|=BFD(cw,psr->sb,psr->lb);
            //if(en_dis)srt|=BIT(9);
            if(en_dis)
            {
                srt|=BFD(psr->cwdWr[3],8,8);
            }
            else
            {
                srt=0;
            }

            r=0;
            for(i=0;i<10;i++)
            {
                delay_ms(1*1000);
                WriteFlashSR(1,psr->cwdWr[0],srt&0xff);

                if ((bt_ota_protectflash_substate == BT_OTA_PROTECTFLASH_WRSR1_RSP_E)
                        ||(bt_ota_protectflash_substate == BT_OTA_PROTECTFLASH_WRSR2_RSP_E))
                {
                    continue;
                }

                r=0;
                for(int32_t j=0;j<10;j++)
                {
                    delay_ms(1*1000);
                    f=WaitForWIP(psr->cwdRd[0]);
                    if(f>0)
                    {
                        r=1;
                        break;
                    }
                }

                if(r<1)
                    return(-4);

                ReadFlashSR(psr->cwdRd[0]);
                if(bt_ota_protectflash_substate == BT_OTA_PROTECTFLASH_RDSR1_RSP)
                {
                    if(current_rd_flashsr.val ==((srt&0xff)))
                    {
                        r=1;
                        break;
                    }
                }
            }

            for(i=0;i<10;i++)
            {
                delay_ms(1*1000);
                WriteFlashSR(1,psr->cwdWr[1],(srt>>8)&0xff);
                if ((bt_ota_protectflash_substate == BT_OTA_PROTECTFLASH_WRSR1_RSP_E)
                        ||(bt_ota_protectflash_substate == BT_OTA_PROTECTFLASH_WRSR2_RSP_E))
                {
                    continue;
                }

                r=0;
                for(int32_t j=0;j<10;j++)
                {
                    delay_ms(1*1000);
                    f=WaitForWIP(psr->cwdRd[0]);
                    if(f>0)
                    {
                        r=1;
                        break;
                    }
                }
                if(r<1)
                    return(-4);

                delay_ms(1*1000);
                ReadFlashSR(psr->cwdRd[1]);
                if(bt_ota_protectflash_substate == BT_OTA_PROTECTFLASH_RDSR1_RSP)
                {
                    if(current_rd_flashsr.val ==((srt>>8)&0xff))
                    {
                        return(1);
                    }
                }
            }

            if(r<1)
                return(-3);
        }
    }

    //return(1);
}

void app_bt_ota_reponse_handle(uint8_t *buf, uint16_t len)
{
    uint8_t status = 0;
    uint8_t i = 0;

    switch(bt_ota_state)
    {
        case BT_OTA_RESET:
            APP_LOG_DEBUG("ota begin, reset bt core !!!");
            break;
        case BT_OTA_STAYROM:
            if (CheckRespond_StayRom(buf) == 1)
            {
                APP_LOG_DEBUG("ota BT_OTA_STAYROM RSP!!!");
                bt_ota_state = BT_OTA_STAYROM_RSP;
            }
            else
            {
                APP_LOG_DEBUG("ota BT_OTA_STAYROM abort!!!");
            }
            break;
        case BT_OTA_SETBR:
            if (CheckRespond_SetBaudRate(buf, WATCH_DEMO_UART_BAUDRATE_OTA, ota_dly))
            {
                APP_LOG_DEBUG("BT baud rate set ok !!");
            }
            else
            {
                APP_LOG_DEBUG("BT baud rate set fail !!");
                while(len > i)
                {
                    APP_LOG_RAW_INFO(" %02x ", buf[i]);
                    i++;
                }
                APP_LOG_RAW_INFO("\n");
            }
            bt_ota_state = BT_OTA_LINKCHECK;
            break;
        case BT_OTA_LINKCHECK:
            if (CheckRespond_LinkCheck(buf))
            {
                bt_ota_state = BT_OTA_LINKCHECK_RSP;
                APP_LOG_DEBUG("link check OK!!");
            }
            else
            {
                APP_LOG_DEBUG("link check fail!!");
            }
            bt_ota_state = BT_OTA_GETMID;
            break;
        case BT_OTA_GETMID:
            if(CheckRespond_FlashGetMID(buf, &flash_mid,&status)>0)
            {
                flash_mid >>=8;
                APP_LOG_DEBUG("get mid success 0x%08x!!", flash_mid);
                bt_ota_state = BT_OTA_GETMID_RSP;

                flash_psr = (_t_flashSR_para*)GetFlashInfo(flash_mid);
                if (flash_psr != NULL)
                {
                    if (flash_psr->szMem/8 < bt_upgrade_fw_size)
                    {
                        APP_LOG_DEBUG("program file too large , abort upgrade");
                        bt_ota_state = BT_OTA_IDLE;
                    }
                    else
                    {
                        APP_LOG_DEBUG("get flash mid ok, begin to check !!");
                        bt_ota_state = BT_OTA_PROTECTFLASH;
                    }
                }
                else
                {
                    bt_ota_state = BT_OTA_IDLE;
                    APP_LOG_DEBUG("do not find the flash id !!!! upgrade abort!!!");
                }
            }
            break;
        case BT_OTA_PROTECTFLASH:
            switch(bt_ota_protectflash_substate)
            {
                case BT_OTA_PROTECTFLASH_RDSR1:
                    if(CheckRespond_FlashReadSR((uint8_t*)buf,current_rd_flashsr.regAddr, &current_rd_flashsr.status,&current_rd_flashsr.val)>0)
                    {
                        bt_ota_protectflash_substate = BT_OTA_PROTECTFLASH_RDSR1_RSP;
                        APP_LOG_DEBUG("protect flash rd 1");
                    }
                    else
                    {
                        bt_ota_protectflash_substate = BT_OTA_PROTECTFLASH_RDSR1_RSP_E;
                        APP_LOG_DEBUG("protect flash rd 1 err");
                    }
                    break;
                case BT_OTA_PROTECTFLASH_WRSR1:
                    if(CheckRespond_FlashWriteSR((uint8_t*)buf,current_wr_flashsr.regAddr, current_wr_flashsr.val&0xff,&current_wr_flashsr.status)>0)
                    {
                        bt_ota_protectflash_substate = BT_OTA_PROTECTFLASH_WRSR1_RSP;
                        APP_LOG_DEBUG("protect flash wr 1");
                    }
                    else
                    {
                        bt_ota_protectflash_substate = BT_OTA_PROTECTFLASH_WRSR1_RSP_E;
                        APP_LOG_DEBUG("protect flash wr 1 err");
                    }
                    break;
                case BT_OTA_PROTECTFLASH_WRSR2:
                    if(CheckRespond_FlashWriteSR2((uint8_t*)buf,current_wr_flashsr.regAddr, current_wr_flashsr.val&0xffff,&current_wr_flashsr.status)>0)
                    {
                        bt_ota_protectflash_substate = BT_OTA_PROTECTFLASH_WRSR2_RSP;
                        APP_LOG_DEBUG("protect flash wr 2");
                    }
                    else
                    {
                        bt_ota_protectflash_substate = BT_OTA_PROTECTFLASH_WRSR2_RSP_E;
                        APP_LOG_DEBUG("protect flash wr 2 err");
                    }
                    break;
                default:
                    APP_LOG_DEBUG("bt_ota_protectflash_substate is %d", bt_ota_protectflash_substate);
                    break;
            }
            break;
        case BT_OTA_OP_ERASEFLASH:
        case BT_OTA_OP_WRITEFLASH:
            switch(bt_ota_erase_substate)
            {
                case BT_OTA_OPFLASH_EREASE:
                    if(CheckRespond_FlashErase((uint8_t*)buf, current_erase_flash.addr, current_erase_flash.szCmd, &current_erase_flash.status)>0)
                    {
                        bt_ota_erase_substate = BT_OTA_OPFLASH_EREASE_RSP;
                        //APP_LOG_DEBUG("erease flash 1");
                    }
                    else
                    {
                        bt_ota_erase_substate = BT_OTA_OPFLASH_EREASE_RSP_E;
                        APP_LOG_DEBUG("cur addr=0x%08x\r\n",current_erase_flash.addr);
                        APP_LOG_DEBUG("erease flash 1 err");
                    }
                    break;
                case BT_OTA_OPFLASH_WRITE1SEC:
#if  (FLASH_WR_PKG_SIZE==0x100)
                    if(CheckRespond_FlashWrite((uint8_t*)buf, current_erase_flash.addr,&current_erase_flash.status, NULL)>0)
#else
                    if(CheckRespond_FlashWrite4K((uint8_t*)buf, current_erase_flash.addr,&current_erase_flash.status)>0)
#endif
                    {
                        bt_ota_erase_substate = BT_OTA_OPFLASH_WRITE1SEC_RSP;
                    }
                    else
                    {
                        bt_ota_erase_substate = BT_OTA_OPFLASH_WRITE1SEC_RSP_E;
                        APP_LOG_DEBUG("cur addr=0x%08x\r\n", current_erase_flash.addr);
                        APP_LOG_DEBUG("write flash 1 sec err");
                    }
                    break;
                default:
                    APP_LOG_DEBUG("bt_ota_erase_substate is %d", bt_ota_erase_substate);
                    break;
            }
            break;
        case BT_OTA_WRITE_CHIP_NAME:
            if (CheckRespond_FlashWrite((uint8_t*)buf, current_erase_flash.addr, &current_erase_flash.status, NULL))
            {
                bt_ota_state = BT_OTA_WRITE_CHIP_NAME_RSP;
                APP_LOG_DEBUG("write chip name success");
            }
            else
            {
                bt_ota_state = BT_OTA_WRITE_CHIP_NAME_RSP_E;
                APP_LOG_DEBUG("write chip name fail");
            }
            break;
        case BT_OTA_READCRC:
            if(CheckRespond_CheckCRC((uint8_t*)buf,current_flash_crc.saddr, current_flash_crc.saddr+current_flash_crc.sz-1, &current_flash_crc.crc)>0)
            {
                bt_ota_state = BT_OTA_READCRC_RSP;
                APP_LOG_DEBUG("rd crc ok");
            }
            else
            {
                bt_ota_state = BT_OTA_READCRC_RSP_E;
                APP_LOG_DEBUG("rd crc err");
            }
            break;
        default:
            APP_LOG_DEBUG("bt_ota_state is %d", bt_ota_state);
            break;
    }

}

int32_t app_bt_ota_procedure(void)
{
    int32_t r;
    uint16_t re_try = 0;
    uint16_t re_try_reset = 0;
    int8_t ret = 0;

    if (bt_ota_state == BT_OTA_START)
    {
        while(1)
        {
            if (bt_ota_state == BT_OTA_IDLE)
            {
                break;
            }

            //reset local baudrate
            WATCH_DEMO_UART_BAUDRATE = WATCH_DEMO_UART_BAUDRATE_STAY_ROM;
            ble_bt_uart_init();
            bt_ota_state = BT_OTA_STAYROM;
            while(1)
            {
                if (bt_ota_state == BT_OTA_IDLE)
                {
                    break;
                }

                if (bt_ota_state != BT_OTA_STAYROM_RSP)
                {
                    re_try_reset ++;
                    app_bt_reset_hw();
                }

                if (re_try_reset == 200)
                {
                    bt_ota_state = BT_OTA_IDLE;
                    break;
                }



                while(1)
                {
                    if (bt_ota_state == BT_OTA_STAYROM_RSP)
                    {
                        break;
                    }

                    //bt_ota_state = BT_OTA_STAYROM;
                    bt_ota_StayRom();
                    re_try ++;
                    //if ((re_try/5) == 0)
                    {
                        delay_ms(2);
                    }
                    if (re_try == 200000)
                    {
                        re_try = 0;
                        break;

                    }
                }

                if (bt_ota_state == BT_OTA_STAYROM_RSP)
                {
                    break;
                }
            }

            if (bt_ota_state == BT_OTA_IDLE)
            {
                break;
            }

            if (bt_ota_state == BT_OTA_RESET)
            {
                continue;
            }

            //reset to stay rom mode, baudrate change to 115200
            if (WATCH_DEMO_UART_BAUDRATE_OTA != WATCH_DEMO_UART_BAUDRATE_STAY_ROM)
            {
                bt_ota_SetBR(WATCH_DEMO_UART_BAUDRATE_OTA, ota_dly);
            }

            bt_ota_LinkCheck();

            bt_ota_GetFlashMID();

            if (bt_ota_state != BT_OTA_PROTECTFLASH)
            {
                continue;
            }

            r = bt_ota_ProtectFlash(flash_psr, flash_mid, 0);
            if(r<1)
            {
                bt_ota_state = BT_OTA_IDLE;
                APP_LOG_DEBUG("programming FAILED,error code=%d",-3);
                break;
            }

//            int32_t j,k;
//            j=bt_upgrade_fw_size/0x1000;
            int32_t addr = 0;

            addr&=0xfffff000;
            int32_t curPtr=addr;

            APP_LOG_DEBUG("begin erasing.....\r\n");
            bt_ota_state = BT_OTA_OP_ERASEFLASH;
#if (FLASH_WR_PKG_SIZE==0x100)
            r=_erase_flash_range_(addr,bt_upgrade_fw_size,(void*)_uart_erase_cmd,(void*)NULL,(void*)_uart_write_define_size,(void*)_uart_dbgmsg,(void*)NULL,(void*)&curPtr);
#else
            r=_erase_flash_range_(addr,bt_upgrade_fw_size,(void*)_uart_erase_cmd,(void*)NULL,(void*)_uart_write1sec,(void*)_uart_dbgmsg,(void*)NULL,(void*)&curPtr);
#endif
            if(r<1)
            {
                ret=-4;
                break;
            }

            APP_LOG_DEBUG("begin writing.....\r\n");
            bt_ota_state = BT_OTA_OP_WRITEFLASH;
            curPtr=addr;

            current_flash_crc.crc_me = 0xffffffff;
#if (FLASH_WR_PKG_SIZE==0x100)
            r=_write_flash_data_(addr, bt_upgrade_fw_flash_addr, bt_upgrade_fw_size,(void*)NULL,(void*)_uart_write_define_size,(void*)_uart_dbgmsg,(void*)NULL,(void*)&curPtr);
#else
            r=_write_flash_data_(addr, bt_upgrade_fw_flash_addr, bt_upgrade_fw_size,(void*)NULL,(void*)_uart_write1sec,(void*)_uart_dbgmsg,(void*)NULL,(void*)&curPtr);
#endif
            if(r<1)
            {
                ret=(-5);
                break;
            }

            APP_LOG_DEBUG("begin writing chip keyword ...\r\n");
            bt_ota_WriteFlashBytes(CHIP_KEYWORD_ADDR, chip_keyword, sizeof(chip_keyword));
            //校验
            uint32_t crc_rem,crc_me;
            APP_LOG_DEBUG("begin verifying.....\r\n");
            r=bt_ota_ReadCRC(addr, bt_upgrade_fw_size);
            APP_LOG_DEBUG("verifying 0x%08x == 0x%08x\r\n", current_flash_crc.crc, current_flash_crc.crc_me);
            crc_rem = current_flash_crc.crc;
            crc_me= current_flash_crc.crc_me;

            current_flash_crc.crc = 0xffffffff;
            current_flash_crc.crc_me = 0xffffffff;
            if(r<1)
            {
                ret= -6;
                break;
            }

            if(crc_me!=crc_rem)
            {
                ret= -7;
                break;
            }

            bt_ota_ProtectFlash(flash_psr, flash_mid, 1);
            if(r<1)
            {
                ret= -8;
                break;
            }

            ret= 1;
            break;
        }
        app_bt_ota_env_deinit();

    }

    return ret;
}

void app_bt_ota_env_deinit(void )
{
    app_bt_reset_hw();

    app_ota_tim_stop();

    bt_ota_state = BT_OTA_IDLE;
    bt_ota_protectflash_substate = BT_OTA_PROTECTFLASH_IDLE;
    bt_ota_erase_substate = BT_OTA_OPFLASH_IDLE;

    WATCH_DEMO_UART_BAUDRATE = WATCH_DEMO_UART_BAUDRATE_ORI;
    ble_bt_uart_init();
    pwr_mgmt_mode_set(ota_pwr_mgmt_mode);
}

void app_bt_ota_procedure_begin(uint32_t flash_qspi_addr, uint32_t flash_fw_size)
{
    if (bt_ota_state != BT_OTA_IDLE)
    {
        APP_LOG_DEBUG("bt ota fw upgrade has been begin !!!");
    }
    else
    {
        ota_pwr_mgmt_mode = pwr_mgmt_mode_get();
        pwr_mgmt_mode_set(PMR_MGMT_ACTIVE_MODE);
        bt_upgrade_fw_flash_addr = flash_qspi_addr;
        bt_upgrade_fw_size = flash_fw_size;
        bt_ota_state = BT_OTA_START;

        app_ota_tim_interrupt();
        app_ota_prt_tim_interrupt();

        APP_LOG_DEBUG("bt ota fw upgrade begin !!!");
    }
}
#else
void app_bt_ota_procedure_begin(uint32_t flash_qspi_addr, uint32_t flash_fw_size)
{
    APP_LOG_ERROR("bt ota not supported !!!");
}
#endif //OTA support


