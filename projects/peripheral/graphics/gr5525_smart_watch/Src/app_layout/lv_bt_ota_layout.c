#include "lv_general_options_page_generator.h"
#include "lv_img_dsc_list.h"
#include "lv_layout_router.h"
#include "app_bt.h"
#include <stdio.h>


/*
 * PUBLIC VARS DEFINITIONS
 *****************************************************************************************
 */
bool g_bt_ota_lock = false;
static float s_update_per;
static lv_timer_t *bt_ota_status_update_timer;
extern void ota_start_task(void);
extern void ota_abort_task(void);
extern float ble_ota_progress_get(void);

/*
 * STATIC METHODS IMPLEMENT
 *****************************************************************************************
 */
static void bt_ota_update_status_timer(lv_timer_t *p_timer)
{
    lv_obj_t *p_window = (lv_obj_t *)p_timer->user_data;
    s_update_per = ble_ota_progress_get();
    if (s_update_per == 100.f)
    {
        lv_label_set_text_fmt(p_window->spec_attr->children[2], "100.00%%\nUpgrade Complete");
        lv_timer_del(p_timer);
    }
    else
    {
        lv_label_set_text_fmt(p_window->spec_attr->children[2], "%.2f%%", s_update_per);
    }
}

#if LV_GDX_PATCH_USE_FAST_TILEVIEW
static void bt_ota_event_cb(lv_event_t *e)
{
    if (e->code == LV_EVENT_DELETE)
    {
        if (bt_ota_status_update_timer)
        {
            lv_timer_del(bt_ota_status_update_timer);
            bt_ota_status_update_timer = NULL;
        }
    }
    else if (e->code == LV_EVENT_READY)
    {
        ota_start_task();
    }
    else if (e->code == LV_EVENT_CANCEL)
    {
        if (bt_ota_status_update_timer)
        {
            lv_timer_pause(bt_ota_status_update_timer);
        }
        ota_abort_task();
    }
}

static void btn_click_event_cb(lv_event_t *event)
{
    lv_obj_t *p_window = event->target->parent;
    uintptr_t click_yes = (uintptr_t)lv_event_get_user_data(event);

    if ((bool)click_yes == true)
    {
        lv_general_options_page_show_status(p_window);
        bt_ota_status_update_timer = lv_timer_create(bt_ota_update_status_timer, 50, event->current_target->parent);
        // bt_api_start_ota(0x00240000, 512*1024);
        s_update_per = 0;
        g_bt_ota_lock = true;
    }
    else
    {
        lv_layout_router_back(p_window);
    }
}
#else
static void btn_click_event_cb(lv_event_t *evt)
{

}
#endif

/*
 * GLOBAL METHODS IMPLEMENT
 *****************************************************************************************
 */
lv_obj_t *lv_bt_ota_layout_create(lv_obj_t *parent_tv_obj)
{
#if LV_GDX_PATCH_DISABLE_STYLE_REFRESH && !LV_GDX_PATCH_USE_FAST_TILEVIEW
    lv_obj_enable_style_refresh(false);
    lv_disp_t *disp = lv_obj_get_disp(parent_tv_obj);
    lv_disp_enable_invalidation(disp, false);
#endif

    general_option_page_dsc_t dsc = {
        .title = "OTA",
        .tip_text = "New version firmware found, do you want to upgrade now?",
        .status_text = "0%%",
        .callback = btn_click_event_cb,
    };
    lv_obj_t *p_window = lv_general_options_page_create(parent_tv_obj, &dsc);

#if LV_GDX_PATCH_USE_FAST_TILEVIEW
    lv_obj_add_event_cb(p_window, bt_ota_event_cb, LV_EVENT_ALL, NULL);
#endif

#if LV_GDX_PATCH_DISABLE_STYLE_REFRESH && !LV_GDX_PATCH_USE_FAST_TILEVIEW
    lv_disp_enable_invalidation(disp, true);
    lv_obj_enable_style_refresh(true);
    lv_obj_refresh_style(p_window, LV_PART_ANY, LV_STYLE_PROP_ANY);
#endif

    return p_window;
}

void app_bt_opa_on_write_proc_update(uint32_t current, uint32_t total)
{
    s_update_per = 100.f * current / total;
    if (current == total)
    {
        s_update_per = 100.f;
        g_bt_ota_lock = false;
    }
}
