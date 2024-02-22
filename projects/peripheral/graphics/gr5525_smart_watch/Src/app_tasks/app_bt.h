#ifndef __APP_BT_H__
#define __APP_BT_H__

#include "bt_api.h"

typedef void (*bt_api_ind_callback_t)(bt_api_msg_t *msg);
typedef void (*bt_api_resp_callback_t)(bt_resp_t response);

int8_t bt_api_register_indication_callback(bt_api_ind_callback_t callback, bt_api_ind_type_t ind_type);
void bt_api_deregister_indication_callback(bt_api_ind_callback_t callback);

int8_t bt_api_register_response_callback(bt_api_resp_callback_t callback, bt_api_opcode_t resp_type);
void bt_api_deregister_response_callback(bt_api_resp_callback_t callback);

void app_create_bt_task(void);

#endif // __APP_BT_H__
