#include "lvgl.h"
#include "lv_font.h"
#include "lv_img_dsc_list.h"
#include "lv_imgdigits.h"
#include "lv_layout_router.h"
#include "watchface_manager.h"

#include "app_rtc.h"

#include <stdio.h>

#define WATCHFACE_THUMBNAIL_WIDTH  (214)
#define WATCHFACE_THUMBNAIL_MARGIN (40)

/*
 * STATIC VARS DEFINITIONS
 *****************************************************************************************
 */
static const lv_style_const_prop_t WINDOW_STYLE_PROPS[] = {
    LV_STYLE_CONST_PAD_COLUMN(WATCHFACE_THUMBNAIL_MARGIN),
    LV_STYLE_PROP_INV,
};

static const lv_style_const_prop_t WATCHFACE_CAPTION_STYLE_PROPS[] = {
    LV_STYLE_CONST_TEXT_FONT(&lv_font_montserrat_20),
    LV_STYLE_CONST_TEXT_COLOR(LV_COLOR_MAKE(0xFF, 0xFF, 0xFF)),
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(0, 0, 0)),
    LV_STYLE_PROP_INV,
};

LV_STYLE_CONST_INIT(WINDOW_STYLE, WINDOW_STYLE_PROPS);
LV_STYLE_CONST_INIT(WATCHFACE_CAPTION_STYLE, WATCHFACE_CAPTION_STYLE_PROPS);

/*
 * STATIC METHODS IMPLEMENT
 *****************************************************************************************
 */
static void watchface_select_released_cb(lv_event_t *e)
{
    if (e->current_target->coords.x1 > 0 && e->current_target->coords.x2 < DISP_HOR_RES)
    {
        lv_obj_t *p_target = lv_event_get_target(e);
        // NOTE: WatchFace Switching feature totally depends on watch face layout self destroy
        // So it won't work if watch face layout destroy is disabled
        uintptr_t idx = (uintptr_t)lv_event_get_user_data(e);
        wf_mngr_set_current_watchface((int32_t)idx);
        lv_layout_router_back(p_target);
    }
}

/*
 * GLOBAL METHODS IMPLEMENT
 *****************************************************************************************
 */
lv_obj_t *lv_watchface_switch_layout_create(lv_obj_t *parent)
{
#if LV_GDX_PATCH_DISABLE_STYLE_REFRESH && !LV_GDX_PATCH_USE_FAST_TILEVIEW
    lv_obj_enable_style_refresh(false);
    lv_disp_t * disp = lv_obj_get_disp(parent);
    lv_disp_enable_invalidation(disp, false);
#endif

    // Background
    lv_obj_t *p_window = lv_obj_create(parent);
    // lv_obj_remove_style_all(p_window);
    lv_obj_set_size(p_window, DISP_HOR_RES, DISP_VER_RES);
    lv_obj_add_flag(p_window, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scroll_snap_x(p_window, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_scroll_dir(p_window, LV_DIR_HOR);
    lv_obj_add_flag(p_window, LV_OBJ_FLAG_SCROLL_ONE);
    lv_obj_set_scrollbar_mode(p_window, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_flex_flow(p_window, LV_FLEX_FLOW_ROW);
    lv_obj_add_style(p_window, (lv_style_t *)&WINDOW_STYLE, LV_STATE_DEFAULT);
    lv_obj_center(p_window);

    watchface_info_t *p_watchfaces = wf_mngr_get_watchface_list();
    lv_obj_t *p_former_selected = NULL;
    for (uint8_t i = 0; i < MAX_WATCHFACE_SUPPORTED; i++)
    {
        if (p_watchfaces[i].create_func)
        {
            lv_obj_t *p_wf = lv_obj_create(p_window);
            lv_obj_set_size(p_wf, WATCHFACE_THUMBNAIL_WIDTH, DISP_VER_RES);

            // Caption
            lv_obj_t *p_caption = lv_label_create(p_wf);
            lv_label_set_text(p_caption, p_watchfaces[i].p_caption);
            lv_obj_add_style(p_caption, (lv_style_t *)&WATCHFACE_CAPTION_STYLE, LV_STATE_DEFAULT);
            lv_obj_align(p_caption, LV_ALIGN_TOP_MID, 0, 12);

            // Thumbnail
            lv_obj_t *p_thumbnail;
            if (p_watchfaces[i].thumbnail)
            {
                p_thumbnail = lv_img_create(p_wf);
                lv_img_set_src(p_thumbnail, p_watchfaces[i].thumbnail);
            }
            else
            {
                p_thumbnail = lv_obj_create(p_wf);
            }
            lv_obj_add_flag(p_thumbnail, LV_OBJ_FLAG_CLICKABLE);
            uintptr_t __i = i;
            lv_obj_add_event_cb(p_thumbnail, watchface_select_released_cb, LV_EVENT_SHORT_CLICKED, (void *)__i);
            lv_obj_set_size(p_thumbnail, WATCHFACE_THUMBNAIL_WIDTH, WATCHFACE_THUMBNAIL_WIDTH);
            lv_obj_center(p_thumbnail);

            if (wf_mngr_get_current_watchface() == &p_watchfaces[i])
            {
                p_former_selected = p_wf;
            }
        }
    }

    lv_obj_update_snap(p_window, LV_ANIM_OFF);
    if (p_former_selected)
    {
        lv_obj_scroll_to_view(p_former_selected, LV_ANIM_OFF);
    }

#if LV_GDX_PATCH_DISABLE_STYLE_REFRESH && !LV_GDX_PATCH_USE_FAST_TILEVIEW
    lv_disp_enable_invalidation(disp, true);
    lv_obj_enable_style_refresh(true);
    lv_obj_refresh_style(p_window, LV_PART_ANY, LV_STYLE_PROP_ANY);
#endif

    return p_window;
}
