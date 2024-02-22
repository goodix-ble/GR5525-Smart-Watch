#ifndef __LV_BT_DEV_INFO_H__
#define __LV_BT_DEV_INFO_H__

#include <stdint.h>

#define BT_SCAN_DEV_MAX_NUM (10)
#define BT_DEV_NAME_MAX_LEN (32)

typedef struct 
{
    char dev_name[BT_DEV_NAME_MAX_LEN];
    uint8_t mac[6];
    int rssi;
} bt_dev_info_t;

#endif
