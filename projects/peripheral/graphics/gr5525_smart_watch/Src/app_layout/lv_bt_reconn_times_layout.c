#include "lv_general_list_generator.h"
#include "lv_img_dsc_list.h"
#include "lv_layout_router.h"
#include "app_bt.h"
#include <stdio.h>

#if 0
/*
 * STATIC VARS DEFINITIONS
 *****************************************************************************************
 */
static const general_list_item_t BT_RECONN_LIST_ITEMS[] = {
    {"Always re-connect", NULL},
    {"Re-connect once", NULL},
    {"Re-connect twice", NULL},
    {"Re-connect three times", NULL},
};

/*
 * STATIC METHODS IMPLEMENT
 *****************************************************************************************
 */
#if LV_GDX_PATCH_USE_FAST_TILEVIEW
static void list_item_click_event_cb(uint16_t idx, lv_event_t *event)
{
    uintptr_t _idx = 3;
    switch (idx)
    {
    case 0:
        _idx = 0;
        break;

    case 1:
        _idx = 3;
        break;

    case 2:
        _idx = 5;
        break;

    case 3:
        _idx = 7;
        break;

    default:
        break;
    }

    lv_layout_router_show_bt_reconn_times_confirm(event->current_target, (void *)&_idx);
}
#else // LV_GDX_PATCH_USE_FAST_TILEVIEW
static void list_item_click_event_cb(uint16_t idx, lv_event_t *event)
{
}
#endif // LV_GDX_PATCH_USE_FAST_TILEVIEW

/*
 * GLOBAL METHODS IMPLEMENT
 *****************************************************************************************
 */
lv_obj_t *lv_bt_reconn_times_layout_create(lv_obj_t *parent_tv_obj)
{
#if LV_GDX_PATCH_DISABLE_STYLE_REFRESH && !LV_GDX_PATCH_USE_FAST_TILEVIEW
    lv_obj_enable_style_refresh(false);
    lv_disp_t *disp = lv_obj_get_disp(parent_tv_obj);
    lv_disp_enable_invalidation(disp, false);
#endif

    general_list_dsc_t dsc = {
        .title = "Re-Con Times",
        .items = BT_RECONN_LIST_ITEMS,
        .item_num = LIST_SIZE(BT_RECONN_LIST_ITEMS),
        .item_width = 260,
        .suffix = &wd_img_chevron_right,
        .callback = list_item_click_event_cb,
    };
    lv_obj_t *p_window = lv_general_list_create(parent_tv_obj, &dsc);

#if LV_GDX_PATCH_DISABLE_STYLE_REFRESH && !LV_GDX_PATCH_USE_FAST_TILEVIEW
    lv_disp_enable_invalidation(disp, true);
    lv_obj_enable_style_refresh(true);
    lv_obj_refresh_style(p_window, LV_PART_ANY, LV_STYLE_PROP_ANY);
#endif

    return p_window;
}
#endif // 0
