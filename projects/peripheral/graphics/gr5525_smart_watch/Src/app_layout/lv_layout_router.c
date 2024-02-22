#include "lvgl.h"
#include "lv_layout_router.h"
#include "lv_bt_dev_info.h"

extern lv_obj_t * lv_menulist_layout_create(lv_obj_t * parent_tv_obj);
extern lv_obj_t * lv_ecg_control_layout_create(lv_obj_t * parent_tv_obj);

extern lv_obj_t * lv_watchface_layout_create(lv_obj_t * parent_tv_obj);
extern lv_obj_t * lv_watchface_switch_layout_create(lv_obj_t *parent_tv_obj);

extern lv_obj_t *lv_layout_tv_top_create(lv_obj_t * parent_tv_obj);
extern lv_obj_t *lv_layout_tv_bottom_create(lv_obj_t * parent_tv_obj);
extern lv_obj_t *lv_layout_tv_left_create(lv_obj_t * parent_tv_obj);
extern lv_obj_t *lv_layout_tv_right_create(lv_obj_t * parent_tv_obj);
extern lv_obj_t *lv_layout_tv_mid_create(lv_obj_t * parent_tv_obj);

extern lv_obj_t *lv_activity_layout_create(lv_obj_t * parent_tv_obj);
extern lv_obj_t *lv_status_layout_create(lv_obj_t * parent_tv_obj);
extern lv_obj_t *lv_weather_layout_create(lv_obj_t * parent_tv_obj);
extern lv_obj_t *lv_notification_layout_create(lv_obj_t * parent_tv_obj);
extern lv_obj_t *lv_heartrate_layout_create(lv_obj_t * parent_tv_obj);
extern lv_obj_t *lv_ota_layout_create(lv_obj_t * parent_tv_obj);
extern lv_obj_t *lv_list_entry_layout_create(lv_obj_t * parent_tv_obj);
extern lv_obj_t *lv_heartrate_chart_layout_create(lv_obj_t * parent_tv_obj);
extern lv_obj_t *lv_app_list_layout_create(lv_obj_t * parent_tv_obj);

extern lv_obj_t *lv_bt_device_layout_create(lv_obj_t *parent_tv_obj);
extern lv_obj_t *lv_bt_manage_layout_create(lv_obj_t * parent_tv_obj);
extern lv_obj_t *lv_bt_scan_dev_layout_create(lv_obj_t * parent_tv_obj);
extern lv_obj_t *lv_bt_pair_dev_layout_create(lv_obj_t * parent_tv_obj, void *user_data);
extern lv_obj_t *lv_bt_paired_dev_layout_create(lv_obj_t * parent_tv_obj);
extern lv_obj_t *lv_bt_remove_dev_layout_create(lv_obj_t * parent_tv_obj, void *user_data);
extern lv_obj_t *lv_bt_phonecall_layout_create(lv_obj_t * parent_tv_obj);
extern lv_obj_t *lv_bt_contact_layout_create(lv_obj_t *parent_tv_obj);
extern lv_obj_t *lv_bt_dial_layout_create(lv_obj_t * parent_tv_obj);
extern lv_obj_t *lv_bt_calling_layout_create(lv_obj_t * parent_tv_obj);
extern lv_obj_t *lv_bt_music_layout_create(lv_obj_t *parent_tv_obj);
extern lv_obj_t *lv_bt_ota_layout_create(lv_obj_t *parent_tv_obj);
extern lv_obj_t *lv_bt_discovery_layout_create(lv_obj_t *parent_tv_obj);
extern lv_obj_t *lv_bt_reconn_times_layout_create(lv_obj_t *parent_tv_obj);
extern lv_obj_t *lv_bt_disconnect_layout_create(lv_obj_t *parent_tv_obj, void *user_data);
extern lv_obj_t *lv_bt_reconn_times_confirm_layout_create(lv_obj_t *parent_tv_obj, void *user_data);
extern lv_obj_t *lv_bt_no_entry_layout_create(lv_obj_t *parent_tv_obj);
extern lv_obj_t *lv_bt_local_audio_layout_create(lv_obj_t *parent_tv_obj);
extern lv_obj_t *lv_bt_factory_reset_layout_create(lv_obj_t *parent_tv_obj);

static union lv_router_params_t
{
    bt_dev_info_t bt_dev_info;
    uint8_t reconn_times;
} s_router_params;
  

#if LV_GDX_PATCH_USE_FAST_TILEVIEW
#else
static void lv_layout_tv_load_win(uint8_t row_id, uint8_t col_id, lv_obj_t * tv_obj);
static void lv_layout_tv_scroll_event_handler(lv_event_t * e);
#endif

void lv_layout_tv_init(void);

void lv_layout_startup(void) {
    #if LV_GDX_PATCH_USE_FAST_TILEVIEW
    extern void _init_fast_tileview(void);
    _init_fast_tileview();
    #else
    lv_layout_tv_init();
    #endif
}


#if LV_GDX_PATCH_USE_FAST_TILEVIEW
lv_obj_t * layout_router(lv_obj_t * obj, int map_id, int new_row, int new_col, lv_fast_tileview_transition_effect_t * p_effect)
{
    printf("==> SHOW TILE: ID=%d, ROW=%d, COL=%d\n", map_id, new_row, new_col);
    lv_obj_set_style_bg_color(obj, lv_color_make(0x0, 0x0, 0x0), 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);

    // 弹出的菜单界面地图
    if (map_id == TILEVIEW_MAP_ID_SETTINGS)
    {
        if (new_row == 0) {
            if (new_col == 0) {
                return lv_notification_layout_create(obj);
            }
            if (new_col == 1) {
                // 实现侧滑返回的效果，向左滑动就返回。
                // 此时该函数必须return NULL，否则最终显示的是该函数return的值。
                lv_fast_tileview_pop(obj);
            }
        }
        return NULL;
    }
    if (map_id == TILEVIEW_MAP_ID_APP_LIST)
    {
        if (new_row == 0 && new_col == 0){
            return lv_app_list_layout_create(obj);
        }
        // else {
        //     // 任何方向滑动都退出
        //     lv_fast_tileview_pop(obj);
        // }
        if (new_row == 0 && new_col == 1)
        {
            // 向左滑动就返回。
            lv_fast_tileview_pop(obj);
        }
        return NULL;
    }
    if (map_id == TILEVIEW_MAP_ID_HR_CHART)
    {
        if (new_row == 0 && new_col == 0){
            return lv_heartrate_chart_layout_create(obj);
        }
        if (new_row == 0 && new_col == -1)
        {
            // 向右滑动就返回。
            lv_fast_tileview_pop(obj);
        }
        return NULL;
    }
    if (map_id == TILEVIEW_MAP_ID_OTA)
    {
        if (new_row == 0) {
            if (new_col == 0) {
                return lv_ota_layout_create(obj);
            }
            if (new_col == 1) {
                // 侧滑返回
                lv_fast_tileview_pop(obj);
            }
        }
        return NULL;
    }
    if (map_id == TILEVIEW_MAP_ID_SWITCH_WF)
    {
        if (new_row == 0) {
            if (new_col == 0) {
                return lv_watchface_switch_layout_create(obj);
            }
            if (new_col == 1) {
//                lv_fast_tileview_pop(obj);
            }
        }
        return NULL;
    }
    // if (map_id == TILEVIEW_MAP_ID_BT_SCAN_DEV)
    // {
    //     if (new_row == 0) {
    //         if (new_col == 0) {
    //             return lv_bt_scan_dev_layout_create(obj);
    //         }
    //         if (new_col == 1) {
    //            lv_fast_tileview_pop(obj);
    //         }
    //     }
    //     return NULL;
    // }
    if (map_id == TILEVIEW_MAP_ID_BT_PAIR_DEV)
    {
        if (new_row == 0) {
            if (new_col == 0) {
                return lv_bt_pair_dev_layout_create(obj, (void *)&s_router_params.bt_dev_info);
            }
            if (new_col == 1) {
               lv_fast_tileview_pop(obj);
            }
        }
        return NULL;
    }
    if (map_id == TILEVIEW_MAP_ID_BT_PAIRED_DEV)
    {
        if (new_row == 0) {
            if (new_col == 0) {
                return lv_bt_paired_dev_layout_create(obj);
            }
            if (new_col == 1) {
               lv_fast_tileview_pop(obj);
            }
        }
        return NULL;
    }
    if (map_id == TILEVIEW_MAP_ID_BT_REMOVE_DEV)
    {
        if (new_row == 0) {
            if (new_col == 0) {
                return lv_bt_remove_dev_layout_create(obj, (void *)&s_router_params.bt_dev_info);
            }
            if (new_col == 1) {
               lv_fast_tileview_pop(obj);
            }
        }
        return NULL;
    }
    if (map_id == TILEVIEW_MAP_ID_BT_MANAGE)
    {
        if (new_row == 0) {
            if (new_col == 0) {
                return lv_bt_manage_layout_create(obj);
            }
            if (new_col == 1) {
               lv_fast_tileview_pop(obj);
            }
        }
        return NULL;
    }
    if (map_id == TILEVIEW_MAP_ID_BT_DEVICE)
    {
        if (new_row == 0) {
            if (new_col == 0) {
                return lv_bt_device_layout_create(obj);
            }
            if (new_col == 1) {
               lv_fast_tileview_pop(obj);
            }
        }
        return NULL;
    }
    // if (map_id == TILEVIEW_MAP_ID_BT_PHONECALL)
    // {
    //     if (new_row == 0)
    //     {
    //         if (new_col == 0)
    //         {
    //             return lv_bt_phonecall_layout_create(obj);
    //         }
    //         if (new_col == 1)
    //         {
    //             lv_fast_tileview_pop(obj);
    //         }
    //     }
    //     return NULL;
    // }
    if (map_id == TILEVIEW_MAP_ID_BT_DIAL)
    {
        if (new_row == 0)
        {
            if (new_col == 0)
            {
                return lv_bt_dial_layout_create(obj);
            }
            if (new_col == 1)
            {
                lv_fast_tileview_pop(obj);
            }
        }
        return NULL;
    }
    // if (map_id == TILEVIEW_MAP_ID_BT_CONTACTS)
    // {
    //     if (new_row == 0)
    //     {
    //         if (new_col == 0)
    //         {
    //             return lv_bt_contact_layout_create(obj);
    //         }
    //         if (new_col == 1)
    //         {
    //             lv_fast_tileview_pop(obj);
    //         }
    //     }
    //     return NULL;
    // }
    if (map_id == TILEVIEW_MAP_ID_BT_CALLING)
    {
        if (new_row == 0)
        {
            if (new_col == 0)
            {
                return lv_bt_calling_layout_create(obj);
            }
        }
        return NULL;
    }
    /*
    if (map_id == TILEVIEW_MAP_ID_BT_OTA)
    {
        if (new_row == 0)
        {
            if (new_col == 0)
            {
                return lv_bt_ota_layout_create(obj);
            }
            if (new_col == 1)
            {
                extern bool g_bt_ota_lock;
                if (!g_bt_ota_lock)
                {
                    lv_fast_tileview_pop(obj);
                }
            }
        }
        return NULL;
    }
    */
    if (map_id == TILEVIEW_MAP_ID_BT_MUSIC)
    {
        if (new_row == 0)
        {
            if (new_col == 0)
            {
                return lv_bt_music_layout_create(obj);
            }
            if (new_col == 1)
            {
                lv_fast_tileview_pop(obj);
            }
        }
        return NULL;
    }
    if (map_id == TILEVIEW_MAP_ID_BT_DISCOVERY)
    {
        if (new_row == 0)
        {
            if (new_col == 0)
            {
                return lv_bt_discovery_layout_create(obj);
            }
            if (new_col == 1)
            {
                lv_fast_tileview_pop(obj);
            }
        }
        return NULL;
    }
    // if (map_id == TILEVIEW_MAP_ID_BT_RECONN_TIMES)
    // {
    //     if (new_row == 0)
    //     {
    //         if (new_col == 0)
    //         {
    //             return lv_bt_reconn_times_layout_create(obj);
    //         }
    //         if (new_col == 1)
    //         {
    //             lv_fast_tileview_pop(obj);
    //         }
    //     }
    //     return NULL;
    // }
    // if (map_id == TILEVIEW_MAP_ID_BT_RECONN_TIMES_CONFIRM)
    // {
    //     if (new_row == 0)
    //     {
    //         if (new_col == 0)
    //         {
    //             return lv_bt_reconn_times_confirm_layout_create(obj, (void *)&s_router_params.reconn_times);
    //         }
    //         if (new_col == 1)
    //         {
    //             lv_fast_tileview_pop(obj);
    //         }
    //     }
    //     return NULL;
    // }
    if (map_id == TILEVIEW_MAP_ID_BT_DISCONNECT)
    {
        if (new_row == 0)
        {
            if (new_col == 0)
            {
                return lv_bt_disconnect_layout_create(obj, (void *)&s_router_params.bt_dev_info);
            }
            if (new_col == 1)
            {
                lv_fast_tileview_pop(obj);
            }
        }
        return NULL;
    }
    if (map_id == TILEVIEW_MAP_ID_BT_NO_ENTRY)
    {
        if (new_row == 0)
        {
            if (new_col == 0)
            {
                return lv_bt_no_entry_layout_create(obj);
            }
            if (new_col == 1)
            {
                lv_fast_tileview_pop(obj);
            }
        }
        return NULL;
    }
    if (map_id == TILEVIEW_MAP_ID_BT_LOCAL_AUDIO)
    {
        if (new_row == 0)
        {
            if (new_col == 0)
            {
                return lv_bt_local_audio_layout_create(obj);
            }
            if (new_col == 1)
            {
                extern bool lv_bt_local_audio_is_playing(void);
                if (!lv_bt_local_audio_is_playing())
                {
                    lv_fast_tileview_pop(obj);
                }
            }
        }
        return NULL;
    }
    if (map_id == TILEVIEW_MAP_ID_BT_FACTORY_RESET)
    {
        if (new_row == 0)
        {
            if (new_col == 0)
            {
                return lv_bt_factory_reset_layout_create(obj);
            }
            if (new_col == 1)
            {
                lv_fast_tileview_pop(obj);
            }
        }
        return NULL;
    }

    // 主界面地图
    switch (new_row)
    {
    case -1:
        if (new_col == 0) // 上中
        {
            return lv_status_layout_create(obj);
        }
        break;

    case 0:
        if (new_col == -1) // 中左
        {
            return lv_list_entry_layout_create(obj);
        }
        else if (new_col == 0) // 中中
        {
            return lv_watchface_layout_create(obj);
        }
        else if (new_col == 1) // 中右
        {
            return lv_heartrate_layout_create(obj);
        }
        else if (new_col == 2)
        {
            return lv_watchface_layout_create(obj);
        }
        break;

    case 1:
        if (new_col == 0) // 下中
        {
            return lv_notification_layout_create(obj);
        }
        break;

    default:
        break;
    }
    return NULL;
}

void _init_fast_tileview(void)
{
    lv_obj_t * scr = lv_scr_act();
    lv_fast_tileview_t* tv = lv_fast_tileview_create(scr, layout_router);
    lv_obj_set_size(&tv->obj, LV_PCT(100), LV_PCT(100));
    tv->effect = LV_FAST_TILEVIEW_EFFECT_TRANSLATION;
}

bool lv_is_tileview_scrolling(void) {
    return false;
}

//override the lv_timer_is_filtered
bool lv_timer_is_filtered(lv_timer_t * timer) {
    return false;
}

void lv_layout_router_back(lv_obj_t * from_obj)
{
    lv_fast_tileview_pop(from_obj);
}

void lv_layout_router_show_settings(lv_obj_t * from_obj)
{
    lv_fast_tileview_push(from_obj, TILEVIEW_MAP_ID_SETTINGS, 0, 0);
}

void lv_layout_router_show_app_list(lv_obj_t * from_obj)
{
    lv_fast_tileview_push(from_obj, TILEVIEW_MAP_ID_APP_LIST, 0, 0);
}

void lv_layout_router_show_heartrate_chart(lv_obj_t * from_obj)
{
    lv_fast_tileview_push(from_obj, TILEVIEW_MAP_ID_HR_CHART, 0, 0);
}

void lv_layout_router_show_ota(lv_obj_t * from_obj)
{
    lv_fast_tileview_push(from_obj, TILEVIEW_MAP_ID_OTA, 0, 0);
}

void lv_layout_router_show_watchface_switch(lv_obj_t *from_obj)
{
    lv_fast_tileview_push(from_obj, TILEVIEW_MAP_ID_SWITCH_WF, 0, 0);
}

// void lv_layout_router_show_bt_scan_dev(lv_obj_t *from_obj)
// {
//     lv_fast_tileview_push(from_obj, TILEVIEW_MAP_ID_BT_SCAN_DEV, 0, 0);
// }

void lv_layout_router_show_bt_pair_dev(lv_obj_t *from_obj, void *user_data)
{
    memcpy(&s_router_params.bt_dev_info, (union lv_router_params_t *)user_data, sizeof(union lv_router_params_t));
    lv_fast_tileview_push(from_obj, TILEVIEW_MAP_ID_BT_PAIR_DEV, 0, 0);
}

void lv_layout_router_show_bt_paired_dev(lv_obj_t *from_obj)
{
    lv_fast_tileview_push(from_obj, TILEVIEW_MAP_ID_BT_PAIRED_DEV, 0, 0);
}

void lv_layout_router_show_bt_remove_dev(lv_obj_t *from_obj, void *user_data)
{
    memcpy(&s_router_params.bt_dev_info, (union lv_router_params_t *)user_data, sizeof(union lv_router_params_t));
    lv_fast_tileview_push(from_obj, TILEVIEW_MAP_ID_BT_REMOVE_DEV, 0, 0);
}

void lv_layout_router_show_bt_manage(lv_obj_t *from_obj)
{
    lv_fast_tileview_push(from_obj, TILEVIEW_MAP_ID_BT_MANAGE, 0, 0);
}

void lv_layout_router_show_bt(lv_obj_t *from_obj)
{
    lv_fast_tileview_push(from_obj, TILEVIEW_MAP_ID_BT_DEVICE, 0, 0);
}

void lv_layout_router_show_bt_dial(lv_obj_t *from_obj)
{
    lv_fast_tileview_push(from_obj, TILEVIEW_MAP_ID_BT_DIAL, 0, 0);
}

// void lv_layout_router_show_bt_contacts(lv_obj_t *from_obj)
// {
//     lv_fast_tileview_push(from_obj, TILEVIEW_MAP_ID_BT_CONTACTS, 0, 0);
// }

// void lv_layout_router_show_bt_phonecall(lv_obj_t *from_obj)
// {
//     lv_fast_tileview_push(from_obj, TILEVIEW_MAP_ID_BT_PHONECALL, 0, 0);
// }

void lv_layout_router_show_bt_ota(lv_obj_t *from_obj)
{
    lv_fast_tileview_push(from_obj, TILEVIEW_MAP_ID_BT_OTA, 0, 0);
}

void lv_layout_router_show_bt_music(lv_obj_t *from_obj)
{
    lv_fast_tileview_push(from_obj, TILEVIEW_MAP_ID_BT_MUSIC, 0, 0);
}

void lv_layout_router_show_calling_layout(lv_obj_t *from_obj)
{
    lv_fast_tileview_push(from_obj, TILEVIEW_MAP_ID_BT_CALLING, 0, 0);
}

void lv_layout_router_show_bt_discovery(lv_obj_t *from_obj)
{
    lv_fast_tileview_push(from_obj, TILEVIEW_MAP_ID_BT_DISCOVERY, 0, 0);
}

// void lv_layout_router_show_bt_reconn_times(lv_obj_t *from_obj)
// {
//     lv_fast_tileview_push(from_obj, TILEVIEW_MAP_ID_BT_RECONN_TIMES, 0, 0);
// }

// void lv_layout_router_show_bt_reconn_times_confirm(lv_obj_t *from_obj, void *user_data)
// {
//     memcpy(&s_router_params.reconn_times, (union lv_router_params_t *)user_data, sizeof(union lv_router_params_t));
//     lv_fast_tileview_push(from_obj, TILEVIEW_MAP_ID_BT_RECONN_TIMES_CONFIRM, 0, 0);
// }

void lv_layout_router_show_bt_disconnect(lv_obj_t *from_obj, void *user_data)
{
    memcpy(&s_router_params.bt_dev_info, (union lv_router_params_t *)user_data, sizeof(union lv_router_params_t));
    lv_fast_tileview_push(from_obj, TILEVIEW_MAP_ID_BT_DISCONNECT, 0, 0);
}

void lv_layout_router_show_bt_no_entry(lv_obj_t *from_obj)
{
    lv_fast_tileview_push(from_obj, TILEVIEW_MAP_ID_BT_NO_ENTRY, 0, 0);
}

void lv_layout_router_show_bt_local_audio(lv_obj_t *from_obj)
{
    lv_fast_tileview_push(from_obj, TILEVIEW_MAP_ID_BT_LOCAL_AUDIO, 0, 0);
}

void lv_layout_router_show_factory_reset(lv_obj_t *from_obj)
{
    lv_fast_tileview_push(from_obj, TILEVIEW_MAP_ID_BT_FACTORY_RESET, 0, 0);
}

#else
void lv_layout_tv_init(void) {

    s_tv._tv_root = lv_tileview_create(lv_scr_act());

    lv_obj_set_scrollbar_mode(s_tv._tv_root, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(s_tv._tv_root, LV_OBJ_FLAG_SCROLLABLE);

    /**** Laylout
            ------
            |    |
            |    |
       ----------------
       |    |    |    |
       |    |    |    |
       ----------------
            |    |
            |    |
            ------

     ***/

    s_tv._tv_01 = lv_tileview_add_tile2(s_tv._tv_root, 0, 1, LV_DIR_BOTTOM);
    s_tv._tv_10 = lv_tileview_add_tile2(s_tv._tv_root, 1, 0, LV_DIR_RIGHT);
    s_tv._tv_11 = lv_tileview_add_tile2(s_tv._tv_root, 1, 1, LV_DIR_ALL);
    s_tv._tv_12 = lv_tileview_add_tile2(s_tv._tv_root, 1, 2, LV_DIR_LEFT);
    s_tv._tv_21 = lv_tileview_add_tile2(s_tv._tv_root, 2, 1, LV_DIR_TOP);

    lv_layout_tv_load_win(0, 1, s_tv._tv_01);
    lv_layout_tv_load_win(1, 0, s_tv._tv_10);
    lv_layout_tv_load_win(1, 1, s_tv._tv_11);
    lv_layout_tv_load_win(1, 2, s_tv._tv_12);
    lv_layout_tv_load_win(2, 1, s_tv._tv_21);

    lv_obj_set_tile_id(s_tv._tv_root, 2, 1, LV_ANIM_OFF);
    lv_obj_add_event_cb(s_tv._tv_root, lv_layout_tv_scroll_event_handler, LV_EVENT_ALL, NULL);
}


static void lv_layout_tv_load_win(uint8_t row_id, uint8_t col_id, lv_obj_t * tv_obj) {

    switch(_TV_RowCol_ID(row_id, col_id)) {
        case _TV_RowCol_ID(0, 1):
        {
            lv_status_layout_create(tv_obj);
        }
        break;

        case _TV_RowCol_ID(1, 0):
        {
            lv_list_entry_layout_create(tv_obj);
        }
        break;

        case _TV_RowCol_ID(1, 1):
        {
            lv_watchface_layout_create(tv_obj);
        }
        break;

        case _TV_RowCol_ID(1, 2):
        {
            lv_heartrate_chart_layout_create(tv_obj);
        }
        break;

        case _TV_RowCol_ID(2, 1):
        {
            lv_notification_layout_create(tv_obj);
        }
        break;
    }
}

void lv_layout_tv_unload_win(uint8_t row_id, uint8_t col_id, lv_obj_t * tv_obj) {

}


volatile bool is_lvgl_tileview_scrolling = false;

static void lv_layout_tv_scroll_event_handler(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    switch(code) {
        case LV_EVENT_SCROLL_BEGIN:
        {
            is_lvgl_tileview_scrolling = true;
        }
        break;

        case LV_EVENT_SCROLL:
        {
        }
        break;

        case LV_EVENT_SCROLL_END:
        {
            is_lvgl_tileview_scrolling = false;
        }
        break;

        default:break;
    }
}

bool lv_is_tileview_scrolling(void) {
    return is_lvgl_tileview_scrolling;
}

//override the lv_timer_is_filtered
bool lv_timer_is_filtered(lv_timer_t * timer) {
#if LV_GDX_PATCH_TIMER_PRIO > 0u
    if(is_lvgl_tileview_scrolling && (timer->sys_prio == 0)) {
        return true;
    }
#endif
    return false;
}
#endif
