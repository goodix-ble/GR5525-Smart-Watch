#ifndef __BT_PHONECALL_CONTROLLER_H__
#define __BT_PHONECALL_CONTROLLER_H__

#include "app_bt.h"
#include "lv_ll.h"

#include <stdint.h>

typedef enum
{
    CALL_STATE_IDLE = 0,
    CALL_STATE_CALLING,
    CALL_STATE_INCOMING,
    CALL_STATE_ACCEPTED,
    CALL_STATE_HOLD,
    CALL_STATE_END,
    CALL_STATE_MAX,
} bt_phonecall_state_t;

typedef struct
{
    char phone_number[32];
    bt_phonecall_state_t call_state;
    lv_ll_t contacts_list;
    int8_t volume;
} bt_phonecall_ctx_t;

typedef void (*bt_phonecall_controller_contacts_updated_cb)(lv_ll_t* contact_list);
typedef void (*bt_phonecall_controller_call_state_updated_cb)(bt_phonecall_state_t call_state);

void bt_phonecall_controller_ind_handler(const bt_api_msg_t *rx_info);

void bt_phonecall_controller_make_call(char* phone_number);

bt_phonecall_ctx_t *bt_phonecall_controller_get_context(void);

void bt_phonecall_controller_request_contacts_update(bool force);

void bt_phonecall_controller_set_contacts_updated_cb(bt_phonecall_controller_contacts_updated_cb cb);

void bt_phonecall_controller_set_call_state_updated_cb(bt_phonecall_controller_call_state_updated_cb cb);

void bt_phonecall_controller_on_incoming_call(void *param);

#endif // __BT_PHONECALL_CONTROLLER_H__
