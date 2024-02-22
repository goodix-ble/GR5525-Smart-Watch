#include "app_key.h"
#include "app_rtc.h"
#include "app_log.h"
#include "lvgl.h"
#include "lv_img_dsc_list.h"
#include "lv_layout_router.h"
#include <stdio.h>


#define ICON_COUNT 7 // fixed number.
#define GAP_HORIZONTAL 30
#define GAP_VERTICALLY 30

/*
 * STATIC VARS DEFINITIONS
 *****************************************************************************************
 */
typedef struct _lv_card_list_entry_layout_item
{
    lv_obj_t *p_obj;
    const lv_img_dsc_t *p_icon;
    // int scene_id;
    // int win_id;
} icon_item_t;

static const lv_style_const_prop_t WINDOW_STYLE_PROPS[] = {
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(0, 0, 0)),
    LV_STYLE_CONST_BG_OPA(LV_OPA_COVER),
    LV_STYLE_CONST_PAD_TOP(0),
    LV_STYLE_CONST_PAD_BOTTOM(0),
    LV_STYLE_CONST_PAD_LEFT(0),
    LV_STYLE_CONST_PAD_RIGHT(0),
    LV_STYLE_PROP_INV,
};

LV_STYLE_CONST_INIT(LIST_ENTRY_WINDOW_STYLE, WINDOW_STYLE_PROPS);

// <NOTICE> DEFINE ICON ITEMS
static icon_item_t g_icon_items[ICON_COUNT] = {
    {
        NULL,
        &wd_img_bt_icon,
    },
    {
        NULL,
        &wd_img_app_01_workout,
    },
    {
        NULL,
        &wd_img_app_02_activity,
    },
    {
        NULL,
        &wd_img_app_03_hr,
    },
    {
        NULL,
        &wd_img_app_04_stopwatch,
    },
    {
        NULL,
        &wd_img_app_05_spo2,
    },
    {
        NULL,
        &wd_img_app_06_events,
    },
};

/*
 * STATIC METHODS DECLARATION
 *****************************************************************************************
 */
static void icon_clicked_event_cb(lv_event_t *e);
static void list_entry_icon_create(lv_obj_t *p_obj, int idx);

/*
 * PUBLIC VARS DEFINITIONS
 *****************************************************************************************
 */
lv_obj_t *lv_status_layout_create(lv_obj_t *parent_tv_obj);

/*
 * STATIC METHODS IMPLEMENT
 *****************************************************************************************
 */
static void icon_clicked_event_cb(lv_event_t *e)
{
    icon_item_t *p_icon_item = (icon_item_t *)lv_event_get_user_data(e);
    //APP_LOG_DEBUG("JUMP LAYOUT:");

#if LV_GDX_PATCH_USE_FAST_TILEVIEW
    if (p_icon_item->p_icon == &wd_img_bt_icon) {
        lv_layout_router_show_bt(p_icon_item->p_obj);
    }
    else if (p_icon_item->p_icon == &wd_img_app_06_events) {
        lv_layout_router_show_app_list(p_icon_item->p_obj);
    }
#endif
}

static void list_entry_icon_create(lv_obj_t *p_obj, int idx)
{
    const lv_img_dsc_t *p_icon = g_icon_items[idx].p_icon;

    lv_obj_t *p_img = lv_img_create(p_obj);
    lv_obj_set_size(p_img, p_icon->header.w, p_icon->header.h);
    lv_img_set_src(p_img, g_icon_items[idx].p_icon);
    lv_obj_add_flag(p_img, LV_OBJ_FLAG_CHECKABLE | LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(p_img, icon_clicked_event_cb, LV_EVENT_CLICKED, &g_icon_items[idx]);

    g_icon_items[idx].p_obj = p_img;
}

/*
 * GLOBAL METHODS IMPLEMENT
 *****************************************************************************************
 */
lv_obj_t *lv_list_entry_layout_create(lv_obj_t *parent_tv_obj)
{
#if LV_GDX_PATCH_DISABLE_STYLE_REFRESH && !LV_GDX_PATCH_USE_FAST_TILEVIEW
    lv_obj_enable_style_refresh(false);
    lv_disp_t *disp = lv_obj_get_disp(parent_tv_obj);
    lv_disp_enable_invalidation(disp, false);
#endif
    lv_obj_t *p_window = lv_obj_create(parent_tv_obj);
    lv_obj_set_size(p_window, DISP_HOR_RES, DISP_VER_RES);
    lv_obj_add_style(p_window, (lv_style_t *)&LIST_ENTRY_WINDOW_STYLE, 0);

    for (int i = 0; i < ICON_COUNT; i++)
    {
        list_entry_icon_create(p_window, i);
    }

    int icon_width = g_icon_items[3].p_icon->header.w;
    // ROW 2
    lv_obj_set_align(g_icon_items[3].p_obj, LV_ALIGN_CENTER);
    lv_obj_align_to(g_icon_items[2].p_obj, g_icon_items[3].p_obj, LV_ALIGN_OUT_LEFT_MID, -GAP_HORIZONTAL, 0);
    lv_obj_align_to(g_icon_items[4].p_obj, g_icon_items[3].p_obj, LV_ALIGN_OUT_RIGHT_MID, GAP_HORIZONTAL, 0);
    // ROW 1
    lv_obj_align_to(g_icon_items[0].p_obj, g_icon_items[3].p_obj, LV_ALIGN_OUT_TOP_MID, -(GAP_HORIZONTAL + icon_width) / 2, -GAP_VERTICALLY);
    lv_obj_align_to(g_icon_items[1].p_obj, g_icon_items[3].p_obj, LV_ALIGN_OUT_TOP_MID, (GAP_HORIZONTAL + icon_width) / 2, -GAP_VERTICALLY);
    // ROW 3
    lv_obj_align_to(g_icon_items[5].p_obj, g_icon_items[3].p_obj, LV_ALIGN_OUT_BOTTOM_MID, -(GAP_HORIZONTAL + icon_width)/ 2, GAP_VERTICALLY);
    lv_obj_align_to(g_icon_items[6].p_obj, g_icon_items[3].p_obj, LV_ALIGN_OUT_BOTTOM_MID, (GAP_HORIZONTAL + icon_width)/ 2, GAP_VERTICALLY);

#if LV_GDX_PATCH_DISABLE_STYLE_REFRESH && !LV_GDX_PATCH_USE_FAST_TILEVIEW
    lv_disp_enable_invalidation(disp, true);
    lv_obj_enable_style_refresh(true);
    lv_obj_refresh_style(p_window, LV_PART_ANY, LV_STYLE_PROP_ANY);
#endif

    return p_window;
}
