#ifndef __LV_LAYOUT_ROUTER_H__
#define __LV_LAYOUT_ROUTER_H__

#include "lvgl.h"

enum {
    TILEVIEW_MAP_ID_MAIN_SCREEN = 0,
    TILEVIEW_MAP_ID_RESERVED_01,
    TILEVIEW_MAP_ID_SETTINGS,
    TILEVIEW_MAP_ID_APP_LIST,
    TILEVIEW_MAP_ID_HR_CHART,
    TILEVIEW_MAP_ID_OTA,
    TILEVIEW_MAP_ID_SWITCH_WF,
    // TILEVIEW_MAP_ID_BT_SCAN_DEV,
    TILEVIEW_MAP_ID_BT_PAIR_DEV,
    TILEVIEW_MAP_ID_BT_PAIRED_DEV,
    TILEVIEW_MAP_ID_BT_REMOVE_DEV,
    TILEVIEW_MAP_ID_BT_MANAGE,
    TILEVIEW_MAP_ID_BT_DEVICE,
    // TILEVIEW_MAP_ID_BT_PHONECALL,
    TILEVIEW_MAP_ID_BT_DIAL,
    // TILEVIEW_MAP_ID_BT_CONTACTS,
    TILEVIEW_MAP_ID_BT_CALLING,
    TILEVIEW_MAP_ID_BT_MUSIC,
    TILEVIEW_MAP_ID_BT_OTA,
    TILEVIEW_MAP_ID_BT_DISCOVERY,
    // TILEVIEW_MAP_ID_BT_RECONN_TIMES,
    // TILEVIEW_MAP_ID_BT_RECONN_TIMES_CONFIRM,
    TILEVIEW_MAP_ID_BT_DISCONNECT,
    TILEVIEW_MAP_ID_BT_NO_ENTRY,
    TILEVIEW_MAP_ID_BT_LOCAL_AUDIO,
    TILEVIEW_MAP_ID_BT_FACTORY_RESET,
    TILEVIEW_MAP_ID_MAX,
};

/** @brief Return to previous page.
*/
void lv_layout_router_back(lv_obj_t * from_obj);

void lv_layout_router_show_settings(lv_obj_t * from_obj);

void lv_layout_router_show_app_list(lv_obj_t * from_obj);

void lv_layout_router_show_heartrate_chart(lv_obj_t * from_obj);

void lv_layout_router_show_ota(lv_obj_t * from_obj);

void lv_layout_router_show_watchface_switch(lv_obj_t *from_obj);

void lv_layout_router_show_bt_scan_dev(lv_obj_t *from_obj);

void lv_layout_router_show_bt_pair_dev(lv_obj_t *from_obj, void *user_data);

void lv_layout_router_show_bt_paired_dev(lv_obj_t *from_obj);

void lv_layout_router_show_bt_remove_dev(lv_obj_t *from_obj, void *user_data);

void lv_layout_router_show_bt_manage(lv_obj_t *from_obj);

void lv_layout_router_show_bt(lv_obj_t *from_obj);

void lv_layout_router_show_bt_dial(lv_obj_t *from_obj);

// void lv_layout_router_show_bt_contacts(lv_obj_t *from_obj);

// void lv_layout_router_show_bt_phonecall(lv_obj_t *from_obj);

void lv_layout_router_show_bt_ota(lv_obj_t *from_obj);

void lv_layout_router_show_bt_music(lv_obj_t *from_obj);

void lv_layout_router_show_calling_layout(lv_obj_t *from_obj);

void lv_layout_router_show_bt_discovery(lv_obj_t *from_obj);

void lv_layout_router_show_bt_reconn_times(lv_obj_t *from_obj);

void lv_layout_router_show_bt_reconn_times_confirm(lv_obj_t *from_obj, void *user_data);

void lv_layout_router_show_bt_disconnect(lv_obj_t *from_obj, void *user_data);

void lv_layout_router_show_bt_no_entry(lv_obj_t *from_obj);

void lv_layout_router_show_bt_local_audio(lv_obj_t *from_obj);

void lv_layout_router_show_factory_reset(lv_obj_t *from_obj);

#endif // __LV_LAYOUT_ROUTER_H__

