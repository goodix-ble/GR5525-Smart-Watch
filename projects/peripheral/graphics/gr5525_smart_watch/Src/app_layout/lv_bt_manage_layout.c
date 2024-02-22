#include "lv_general_list_generator.h"
#include "lv_img_dsc_list.h"
#include "lv_layout_router.h"
#include "bt_conn_controller.h"
#include <stdio.h>

/*
 * STATIC VARS DEFINITIONS
 *****************************************************************************************
 */
// static bool is_connected = false;

static const general_list_item_t BT_MANAGE_LIST_ITEMS[] = {
    // {"Scan Device", &wd_img_1_scan_device},
    {"Paired Device", &wd_img_2_bond_device},
    {"Discovery", &wd_img_1_scan_device},
    {"Local Audio", &wd_img_7_version},
    // {"Re-Con Times", &wd_img_4_reconnecton_times},
    // {"Enter Sniff", &wd_img_5_enter_aniff},
    // {"Sniff Period", &wd_img_6_aniff_period},
    // {"Version", &wd_img_7_version},
    {"Factory Reset", &wd_img_8_restore_setting},
    // {"OTA", &wd_img_9_ota},
    {"Power Off", &wd_img_10_power_off},
};

enum {
    // BT_SCAN_DEVICE = 0,
    BT_PAIRED_DEVICE,
    BT_DISCOVERY,
    BT_LOCAL_AUDIO,
    // BT_RE_CON_TIMES,
    // BT_ENTER_SNIFF,
    // BT_SNIFF_PERIOD,
    // BT_VERSION,
    BT_FACTORY_RESET,
    // BT_OTA,
    BT_POWER_OFF,
};

/*
 * STATIC METHODS IMPLEMENT
 *****************************************************************************************
 */
#if LV_GDX_PATCH_USE_FAST_TILEVIEW
// static void bt_conn_ctx_listener(bt_conn_ctx_t *p_ctx, void *user_data)
// {
//     if (p_ctx)
//     {
//         if (BT_API_CONN_STATE_CONNECTED == p_ctx->conn_state)
//         {
//             is_connected = true;
//         }
//         else
//         {
//             is_connected = false;
//         }
//     }
// }

static void bt_manage_event_cb(lv_event_t *e)
{
    if (e->code == LV_EVENT_DELETE)
    {
    }
    else if (e->code == LV_EVENT_READY)
    {
        bt_api_conn_state_get();
        // bt_conn_controller_set_ctx_listener(bt_conn_ctx_listener, NULL);
    }
    // else if (e->code == LV_EVENT_CANCEL)
    // {
    //     bt_conn_controller_set_ctx_listener(NULL, NULL);
    // }
}

static void list_item_click_event_cb(uint16_t idx, lv_event_t *event)
{
    switch (idx)
    {
    // case BT_SCAN_DEVICE:
    //     lv_layout_router_show_bt_scan_dev(event->current_target);
    //     break;

    case BT_PAIRED_DEVICE:
        lv_layout_router_show_bt_paired_dev(event->current_target);
        break;

    case BT_DISCOVERY:
        lv_layout_router_show_bt_discovery(event->current_target);
        break;

    case BT_LOCAL_AUDIO:
        lv_layout_router_show_bt_local_audio(event->current_target);
        break;

    case BT_FACTORY_RESET:
        lv_layout_router_show_factory_reset(event->current_target);
        break;

    // case BT_RE_CON_TIMES:
    //     if (is_connected)
    //     {
    //         lv_layout_router_show_bt_reconn_times(event->current_target);
    //     }
    //     else
    //     {
    //         lv_layout_router_show_bt_no_entry(event->current_target);
    //     }
    //     break;

    // case BT_OTA:
    //     if (is_connected)
    //     {
    //         lv_layout_router_show_bt_ota(event->current_target);
    //     }
    //     else
    //     {
    //         lv_layout_router_show_bt_no_entry(event->current_target);
    //     }
    //     break;

    default:
        break;
    }
}
#else
static void list_item_click_event_cb(uint16_t idx, lv_event_t *event)
{
}
#endif

/*
 * GLOBAL METHODS IMPLEMENT
 *****************************************************************************************
 */
lv_obj_t *lv_bt_manage_layout_create(lv_obj_t *parent_tv_obj)
{
#if LV_GDX_PATCH_DISABLE_STYLE_REFRESH && !LV_GDX_PATCH_USE_FAST_TILEVIEW
    lv_obj_enable_style_refresh(false);
    lv_disp_t *disp = lv_obj_get_disp(parent_tv_obj);
    lv_disp_enable_invalidation(disp, false);
#endif

    general_list_dsc_t dsc = {
        .title = "BT Manage",
        .items = BT_MANAGE_LIST_ITEMS,
        .item_num = LIST_SIZE(BT_MANAGE_LIST_ITEMS),
        .item_width = 220,
        .suffix = &wd_img_chevron_right,
        .callback = list_item_click_event_cb,
    };
    lv_obj_t *p_window = lv_general_list_create(parent_tv_obj, &dsc);

#if LV_GDX_PATCH_USE_FAST_TILEVIEW
    lv_obj_add_event_cb(p_window, bt_manage_event_cb, LV_EVENT_ALL, NULL);
#endif

#if LV_GDX_PATCH_DISABLE_STYLE_REFRESH && !LV_GDX_PATCH_USE_FAST_TILEVIEW
    lv_disp_enable_invalidation(disp, true);
    lv_obj_enable_style_refresh(true);
    lv_obj_refresh_style(p_window, LV_PART_ANY, LV_STYLE_PROP_ANY);
#endif

    return p_window;
}
