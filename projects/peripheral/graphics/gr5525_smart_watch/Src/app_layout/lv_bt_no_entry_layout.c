#include "lv_general_options_page_generator.h"
#include "lv_img_dsc_list.h"
#include "lv_bt_dev_info.h"
#include "lv_layout_router.h"
#include "app_bt.h"
#include "bt_conn_controller.h"
#include "app_log.h"
#include <stdio.h>

/*
 * STATIC VARS DEFINITIONS
 *****************************************************************************************
 */
static lv_obj_t *p_window = NULL;

/*
 * STATIC METHODS IMPLEMENT
 *****************************************************************************************
 */
#if LV_GDX_PATCH_USE_FAST_TILEVIEW


static void bt_pair_dev_event_cb(lv_event_t *e)
{
    if (e->code == LV_EVENT_DELETE)
    {
    }
    else if (e->code == LV_EVENT_READY)
    {
    }
    else if (e->code == LV_EVENT_CANCEL)
    {
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
lv_obj_t *lv_bt_no_entry_layout_create(lv_obj_t *parent_tv_obj, void *user_data)
{
#if LV_GDX_PATCH_DISABLE_STYLE_REFRESH && !LV_GDX_PATCH_USE_FAST_TILEVIEW
    lv_obj_enable_style_refresh(false);
    lv_disp_t *disp = lv_obj_get_disp(parent_tv_obj);
    lv_disp_enable_invalidation(disp, false);
#endif

    general_option_page_dsc_t dsc = {
        .title = NULL,
        .tip_text = NULL,
        .status_text = "Please establish a connection first!",
        .callback = NULL,
    };
    p_window = lv_general_options_page_create(parent_tv_obj, &dsc);
    for (int i = 0; i < p_window->spec_attr->child_cnt; i++)
    {
        if (i == 0)
        {
            lv_obj_clear_flag(p_window->spec_attr->children[i], LV_OBJ_FLAG_HIDDEN);
        }
        else
        {
            lv_obj_add_flag(p_window->spec_attr->children[i], LV_OBJ_FLAG_HIDDEN);
            
        }
    }

#if LV_GDX_PATCH_USE_FAST_TILEVIEW
    lv_obj_add_event_cb(p_window, bt_pair_dev_event_cb, LV_EVENT_ALL, NULL);
#endif

#if LV_GDX_PATCH_DISABLE_STYLE_REFRESH && !LV_GDX_PATCH_USE_FAST_TILEVIEW
    lv_disp_enable_invalidation(disp, true);
    lv_obj_enable_style_refresh(true);
    lv_obj_refresh_style(p_window, LV_PART_ANY, LV_STYLE_PROP_ANY);
#endif

    return p_window;
}
