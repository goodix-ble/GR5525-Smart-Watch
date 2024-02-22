#include "lvgl.h"
#include "lv_font.h"
#include "lv_img_dsc_list.h"
#include "lv_imgdigits.h"
#include "watchface_manager.h"
#include "lv_layout_router.h"
#include "FreeRTOS.h"
#include "task.h"
#include "app_rtc.h"

#include <stdio.h>


#define SWITCH_WATCHFACE_LONG_PRESS_THRESHOLD  (500) // unit: ms

/*
 * STATIC METHODS IMPLEMENT
 *****************************************************************************************
 */
#if LV_GDX_PATCH_USE_FAST_TILEVIEW
static void long_press_event_handler(lv_event_t *e)
{
    if (e->code == LV_EVENT_LONG_PRESSED)
    {
        lv_obj_t *p_target = lv_event_get_target(e);
        lv_layout_router_show_watchface_switch(p_target);
        lv_indev_wait_release(lv_indev_get_act());
    }
}
#endif

/*
 * GLOBAL METHODS IMPLEMENT
 *****************************************************************************************
 */
lv_obj_t *lv_watchface_layout_create(lv_obj_t *parent)
{
#if LV_GDX_PATCH_DISABLE_STYLE_REFRESH && !LV_GDX_PATCH_USE_FAST_TILEVIEW
    lv_obj_enable_style_refresh(false);
    lv_disp_t *disp = lv_obj_get_disp(parent);
    lv_disp_enable_invalidation(disp, false);
#endif

    // Background
    lv_obj_t *p_window = wf_mngr_create_watchface_layout(parent);
#if LV_GDX_PATCH_USE_FAST_TILEVIEW
    lv_obj_add_event_cb(p_window, long_press_event_handler, LV_EVENT_ALL, NULL);
#endif

#if LV_GDX_PATCH_DISABLE_STYLE_REFRESH && !LV_GDX_PATCH_USE_FAST_TILEVIEW
    lv_disp_enable_invalidation(disp, true);
    lv_obj_enable_style_refresh(true);
    lv_obj_refresh_style(p_window, LV_PART_ANY, LV_STYLE_PROP_ANY);
#endif

    return p_window;
}
