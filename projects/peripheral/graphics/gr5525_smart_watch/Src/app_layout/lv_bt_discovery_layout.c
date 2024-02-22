#include "lv_general_options_page_generator.h"
#include "lv_img_dsc_list.h"
#include "lv_bt_dev_info.h"
#include "lv_layout_router.h"
#include "app_bt.h"
#include "bt_conn_controller.h"
#include <stdio.h>

/*
 * STATIC VARS DEFINITIONS
 *****************************************************************************************
 */
static lv_obj_t *p_window = NULL;
static bool inquiry_scanning = false;

/*
 * STATIC METHODS IMPLEMENT
 *****************************************************************************************
 */
#if LV_GDX_PATCH_USE_FAST_TILEVIEW
static void bt_conn_ctx_listener(bt_conn_ctx_t *p_ctx, void *user_data)
{
    if (p_ctx)
    {
        if (BT_API_CONN_STATE_CONNECTED == p_ctx->conn_state)
        {
            lv_general_options_page_status_text_set(p_window, "Pair Success!");
        }
        else
        {
            lv_general_options_page_status_text_set(p_window, "Pair Failed!");
        }
        inquiry_scanning = false;
    }
}

static void bt_discovery_state_cb(bt_resp_t result)
{
    if (RESP_BT_NO_ERR == result)
    {
        lv_general_options_page_status_text_set(p_window, "Waiting for connection...");
        bt_conn_controller_set_ctx_listener(bt_conn_ctx_listener, NULL);
    }
    else
    {
        lv_general_options_page_status_text_set(p_window, "Failed to enter discoverable mode!");
    }
}

static void btn_click_event_cb(lv_event_t *event)
{
    uintptr_t idx = (uintptr_t)lv_event_get_user_data(event);

    if ((int32_t)idx == 1)
    {
        lv_general_options_page_show_status(p_window);
        bt_api_device_inquiry_scan_start();
        inquiry_scanning = true;
        bt_api_register_response_callback(bt_discovery_state_cb, OPCODE_BT_INQUIRY_SCAN);
    }
    else
    {
        lv_layout_router_back(p_window);
    }
}

static void bt_discovery_event_cb(lv_event_t *e)
{
    if (e->code == LV_EVENT_DELETE)
    {
    }
    else if (e->code == LV_EVENT_READY)
    {
    }
    else if (e->code == LV_EVENT_CANCEL)
    {
        bt_api_deregister_response_callback(bt_discovery_state_cb);
        bt_conn_controller_set_ctx_listener(NULL, NULL);
        if (inquiry_scanning)
        {
            bt_api_device_inquiry_scan_stop();
            inquiry_scanning = false;
        }
    }
}
#else // LV_GDX_PATCH_USE_FAST_TILEVIEW
static void btn_click_event_cb(lv_event_t *evt)
{
}
#endif // LV_GDX_PATCH_USE_FAST_TILEVIEW

/*
 * GLOBAL METHODS IMPLEMENT
 *****************************************************************************************
 */
lv_obj_t *lv_bt_discovery_layout_create(lv_obj_t *parent_tv_obj)
{
#if LV_GDX_PATCH_DISABLE_STYLE_REFRESH && !LV_GDX_PATCH_USE_FAST_TILEVIEW
    lv_obj_enable_style_refresh(false);
    lv_disp_t *disp = lv_obj_get_disp(parent_tv_obj);
    lv_disp_enable_invalidation(disp, false);
#endif

    general_option_page_dsc_t dsc = {
        .title = "Discovery",
        .tip_text = "Whether to enter discoverable mode?",
        .status_text = "Waiting for connection...",
        .callback = btn_click_event_cb,
    };
    p_window = lv_general_options_page_create(parent_tv_obj, &dsc);

#if LV_GDX_PATCH_USE_FAST_TILEVIEW
    lv_obj_add_event_cb(p_window, bt_discovery_event_cb, LV_EVENT_ALL, NULL);
#endif

#if LV_GDX_PATCH_DISABLE_STYLE_REFRESH && !LV_GDX_PATCH_USE_FAST_TILEVIEW
    lv_disp_enable_invalidation(disp, true);
    lv_obj_enable_style_refresh(true);
    lv_obj_refresh_style(p_window, LV_PART_ANY, LV_STYLE_PROP_ANY);
#endif

    return p_window;
}
