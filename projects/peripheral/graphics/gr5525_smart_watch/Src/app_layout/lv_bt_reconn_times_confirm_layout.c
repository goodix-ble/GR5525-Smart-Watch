#include "lv_img_dsc_list.h"
#include "lv_general_options_page_generator.h"
#include "lv_layout_router.h"
#include "lv_bt_dev_info.h"
#include "bt_conn_controller.h"
#include "app_log.h"
#include <stdio.h>

/*
 * STATIC VARS DEFINITIONS
 *****************************************************************************************
 */
static uint8_t *p_reconn_times = NULL;
static lv_obj_t *p_window = NULL;

/*
 * STATIC METHODS IMPLEMENT
 *****************************************************************************************
 */
#if LV_GDX_PATCH_USE_FAST_TILEVIEW
static void bt_reconn_times_resp_cb(bt_resp_t result)
{
    if (RESP_BT_NO_ERR == result)
    {
        lv_general_options_page_status_text_set(p_window, "Setting succeeded!");
    }
    else
    {
        lv_general_options_page_status_text_set(p_window, "Setting failed!");
    }
}

static void btn_click_event_cb(lv_event_t *event)
{
    uintptr_t idx = (uintptr_t)lv_event_get_user_data(event);

    if ((int32_t)idx == 1)
    {
        lv_general_options_page_show_status(p_window);

        // clean the pairing information.
        // Deprecated: No longer able to set reconnect times
        // bt_api_quick_connect_times_set(p_reconn_times[0]);
        bt_api_register_response_callback(bt_reconn_times_resp_cb, OPCODE_BT_QUICK_CON_TIMES);
    }
    else
    {
        lv_layout_router_back(p_window);
    }
}

static void bt_reconn_times_confirm_event_cb(lv_event_t *e)
{
    if (e->code == LV_EVENT_DELETE)
    {
    }
    else if (e->code == LV_EVENT_READY)
    {
    }
    else if (e->code == LV_EVENT_CANCEL)
    {
        bt_api_deregister_response_callback(bt_reconn_times_resp_cb);
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
lv_obj_t *lv_bt_reconn_times_confirm_layout_create(lv_obj_t *parent_tv_obj, void *user_data)
{
#if LV_GDX_PATCH_DISABLE_STYLE_REFRESH && !LV_GDX_PATCH_USE_FAST_TILEVIEW
    lv_obj_enable_style_refresh(false);
    lv_disp_t *disp = lv_obj_get_disp(parent_tv_obj);
    lv_disp_enable_invalidation(disp, false);
#endif

    // p_bt_dev_info = (bt_dev_info_t *)user_data;
    p_reconn_times = (uint8_t *)user_data;
    char text[1 + 36] = "";
    sprintf(text, "Set the times of reconnections to %d?", p_reconn_times[0]);

    general_option_page_dsc_t dsc = {
        .title = "Re-Conn Times",
        .tip_text = text,
        .status_text = "Setting succeeded!",
        .callback = btn_click_event_cb,
    };
    p_window = lv_general_options_page_create(parent_tv_obj, &dsc);

#if LV_GDX_PATCH_USE_FAST_TILEVIEW
    lv_obj_add_event_cb(p_window, bt_reconn_times_confirm_event_cb, LV_EVENT_ALL, NULL);
#endif

#if LV_GDX_PATCH_DISABLE_STYLE_REFRESH && !LV_GDX_PATCH_USE_FAST_TILEVIEW
    lv_disp_enable_invalidation(disp, true);
    lv_obj_enable_style_refresh(true);
    lv_obj_refresh_style(p_window, LV_PART_ANY, LV_STYLE_PROP_ANY);
#endif

    return p_window;
}
