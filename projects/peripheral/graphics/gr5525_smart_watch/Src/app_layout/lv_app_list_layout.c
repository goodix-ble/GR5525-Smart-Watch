#include "lvgl.h"
#include "lv_font.h"
#include "lv_img_dsc_list.h"
#include "lv_simplified_obj.h"
#include "lv_arc_scrollbar.h"

#include <stdio.h>
#include <stdint.h>

#define LIST_ITEM_WIDTH (360)
#define LIST_ITEM_HEIGHT (100)
#define LIST_ITEM_SPACING (30)

#define LIST_RADIUS (240)

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof((arr)[0]))

#define LIST_ENABLE_ARC_SCROLLBAR 0

static const struct
{
    const char *title;
    const lv_img_dsc_t *icon;
} APP_LIST_ITEMS[] = {
    {"Workout", &wd_img_app_01_workout},
    {"Activity", &wd_img_app_02_activity},
    {"Heart Rate", &wd_img_app_03_hr},
    {"Stop Watch", &wd_img_app_04_stopwatch},
    {"SPO2", &wd_img_app_05_spo2},
    {"Events", &wd_img_app_06_events},
    {"Settings", &wd_img_app_07_settings},
};

static const lv_style_const_prop_t LIST_ITEM_STYLE_PROPS[] = {
    LV_STYLE_CONST_WIDTH(LIST_ITEM_WIDTH),
    LV_STYLE_CONST_HEIGHT(LIST_ITEM_HEIGHT),
    LV_STYLE_PROP_INV,
};

static const lv_style_const_prop_t LIST_ITEM_TITLE_STYLE_PROPS[] = {
    LV_STYLE_CONST_TEXT_FONT(&lv_font_montserrat_40),
    LV_STYLE_CONST_TEXT_COLOR(LV_COLOR_MAKE(0xFF, 0xFF, 0xFF)),
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(0, 0, 0)),
    LV_STYLE_PROP_INV,
};

LV_STYLE_CONST_INIT(LIST_ITEM_STYLE, LIST_ITEM_STYLE_PROPS);
LV_STYLE_CONST_INIT(LIST_ITEM_TITLE_STYLE, LIST_ITEM_TITLE_STYLE_PROPS);

static void list_window_scroll_event_cb(lv_event_t *evt)
{
    lv_obj_t *obj = evt->target;

    uint8_t child_cnt = obj->spec_attr->child_cnt;
    for (uint8_t i = 0;i<child_cnt;i++)
    {
        lv_obj_t *child = obj->spec_attr->children[i];
        if (child->coords.y2 <= 0)
        {
            continue;
        }
        else if (child->coords.y1 >= (lv_coord_t)DISP_HOR_RES)
        {
            break;
        }

        lv_coord_t y_center = child->coords.y1 + LIST_ITEM_HEIGHT / 2;
        lv_coord_t y_diff = y_center - DISP_VER_RES / 2;
        y_diff = LV_ABS(y_diff);
        lv_coord_t x_trans;

        if (y_diff >= LIST_RADIUS)
        {
            x_trans = LIST_RADIUS;
        }
        else
        {
            uint32_t x_sqr = LIST_RADIUS * LIST_RADIUS - y_diff * y_diff;
            lv_sqrt_res_t res;
            lv_sqrt(x_sqr, &res, 0x8000);
            x_trans = LIST_RADIUS - res.i;
        }
        // Risky but fastest way to set LV_STYLE_X value and update layout
        lv_style_value_t *value = (lv_style_value_t *)child->styles->style->v_p.values_and_props;
        value->num = x_trans;
        lv_obj_mark_layout_as_dirty(child);
    }
    lv_obj_mark_layout_as_dirty(obj);
}

#if LV_GDX_PATCH_USE_FAST_TILEVIEW
#include "lv_layout_router.h"
static void list_item_click_event_cb(lv_event_t *evt)
{
    lv_obj_t *obj = lv_event_get_target(evt);
    void *dat = lv_event_get_user_data(evt);
    if (dat == (&APP_LIST_ITEMS[6]))
    {
        lv_layout_router_show_ota(obj);
    }
}
#else
static void list_item_click_event_cb(lv_event_t *evt)
{
}
#endif

lv_obj_t *lv_app_list_layout_create(lv_obj_t *parent)
{
#if LV_GDX_PATCH_DISABLE_STYLE_REFRESH && !LV_GDX_PATCH_USE_FAST_TILEVIEW
    lv_obj_enable_style_refresh(false);
    lv_disp_t * disp = lv_obj_get_disp(parent);
    lv_disp_enable_invalidation(disp, false);
#endif

    lv_obj_t *p_window = lv_obj_create(parent);
    lv_obj_set_size(p_window, DISP_HOR_RES, DISP_VER_RES);

#if LIST_ENABLE_ARC_SCROLLBAR
    lv_obj_t *p_app_list = lv_simplified_obj_create(p_window);
#else
    lv_obj_t *p_app_list = p_window;
#endif // LIST_ENABLE_ARC_SCROLLBAR
    lv_obj_set_size(p_app_list, DISP_HOR_RES, DISP_VER_RES);
    lv_obj_add_flag(p_app_list, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(p_app_list, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(p_app_list, LV_DIR_VER);
    lv_obj_set_scroll_snap_y(p_app_list, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_style_pad_ver(p_app_list, DISP_VER_RES / 2, 0);

    lv_obj_add_event_cb(p_app_list, list_window_scroll_event_cb, LV_EVENT_SCROLL, NULL);

    for (uint8_t i = 0; i < ARRAY_SIZE(APP_LIST_ITEMS); i++)
    {
        lv_obj_t *item = lv_simplified_obj_create(p_app_list);
        lv_obj_add_style(item, (lv_style_t *)&LIST_ITEM_STYLE, 0);
        lv_obj_set_pos(item, 0, (LIST_ITEM_HEIGHT + LIST_ITEM_SPACING) * i);
        lv_obj_add_flag(item, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(item, list_item_click_event_cb, LV_EVENT_CLICKED, (void*)&APP_LIST_ITEMS[i]);

        lv_obj_t *icon = lv_img_create(item);
        lv_img_set_src(icon, APP_LIST_ITEMS[i].icon);
        lv_obj_set_pos(icon, 0, 10);

        lv_obj_t *label = lv_label_create(item);
        lv_obj_add_style(label, (lv_style_t *)&LIST_ITEM_TITLE_STYLE, 0);
        lv_label_set_text_static(label, APP_LIST_ITEMS[i].title);
        lv_obj_align(label, LV_ALIGN_LEFT_MID, 100, 0);
    }

#if LIST_ENABLE_ARC_SCROLLBAR
    lv_obj_t *bar = lv_arc_scrollbar_create(p_window);
    lv_arc_scrollbar_attach(bar, p_app_list);
#endif // LIST_ENABLE_ARC_SCROLLBAR

    lv_obj_scroll_to_view(lv_obj_get_child(p_app_list, 0), LV_ANIM_OFF);

#if LV_GDX_PATCH_DISABLE_STYLE_REFRESH && !LV_GDX_PATCH_USE_FAST_TILEVIEW
    lv_disp_enable_invalidation(disp, true);
    lv_obj_enable_style_refresh(true);
    lv_obj_refresh_style(p_app_list, LV_PART_ANY, LV_STYLE_PROP_ANY);
#endif

    lv_event_send(p_app_list, LV_EVENT_SCROLL, NULL);

    return p_window;
}
