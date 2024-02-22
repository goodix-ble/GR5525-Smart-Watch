#ifndef __BT_CONN_CONTROLLER_H__
#define __BT_CONN_CONTROLLER_H__

#include "app_bt.h"
#include "lv_bt_dev_info.h"

typedef struct
{
    char *p_dev_name;
    int dev_name_len;
    uint8_t *p_mac;
    int8_t rssi;
    uint8_t conn_state;
    uint8_t *connected_dev_mac;
} bt_conn_ctx_t;

typedef void (*bt_conn_ctx_listener_t)(bt_conn_ctx_t *p_ctx, void *user_data);

void bt_conn_controller_set_ctx_listener(bt_conn_ctx_listener_t listener, void *user_data);
bt_conn_ctx_t *bt_conn_controller_get_ctx(void);
void bt_conn_controller_ind_handler(const bt_api_msg_t *p_rx_info);

#endif // __BT_CONN_CONTROLLER_H__
