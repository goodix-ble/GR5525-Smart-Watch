#include "lvgl.h"
#include "lv_img_dsc_list.h"
#include "lv_font.h"


#include <stdio.h>

static lv_obj_t *create_message_item(lv_obj_t *parent, char *title, char *content, lv_img_dsc_t *icon);
static void delete_btn_scroll_snap(lv_event_t *p_evt);
#if LV_GDX_PATCH_USE_FAST_TILEVIEW
static void delete_all_btn_cb(lv_event_t *p_evt);
#endif

static const lv_style_const_prop_t NTF_WIN_STYLE_PROPS[] = {
    LV_STYLE_CONST_BG_OPA(LV_OPA_COVER),
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(0,0,0)),
    LV_STYLE_CONST_WIDTH(DISP_HOR_RES),
    LV_STYLE_CONST_HEIGHT(DISP_VER_RES),
    LV_STYLE_PROP_INV,
};

static const lv_style_const_prop_t MSG_BG_STYLE_PROPS[] = {
    LV_STYLE_CONST_WIDTH(250),
    LV_STYLE_CONST_HEIGHT(92),
    LV_STYLE_CONST_X(0),
    LV_STYLE_CONST_Y(20),
    LV_STYLE_CONST_BG_OPA(LV_OPA_COVER),
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(0x24,0x24,0x24)),
    LV_STYLE_CONST_RADIUS(12),
    LV_STYLE_PROP_INV,
};

static const lv_style_const_prop_t MSG_TITLE_STYLE_PROPS[] = {
    LV_STYLE_CONST_WIDTH(180),
    LV_STYLE_CONST_HEIGHT(20),
    LV_STYLE_CONST_X(72),
    LV_STYLE_CONST_Y(8),
    LV_STYLE_CONST_TEXT_FONT(&lv_font_montserrat_20),
    LV_STYLE_CONST_TEXT_COLOR(LV_COLOR_MAKE(255,255,255)),
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(0x24,0x24,0x24)),
    LV_STYLE_PROP_INV,
};

static const lv_style_const_prop_t MSG_CONTENT_STYLE_PROPS[] = {
    LV_STYLE_CONST_WIDTH(226),
    LV_STYLE_CONST_HEIGHT(48),
    LV_STYLE_CONST_X(12),
    LV_STYLE_CONST_Y(36),
    LV_STYLE_CONST_TEXT_FONT(&lv_font_montserrat_20),
    LV_STYLE_CONST_TEXT_COLOR(LV_COLOR_MAKE(0x99,0x99,0x99)),
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(0x24,0x24,0x24)),
    LV_STYLE_PROP_INV,
};

static const lv_style_const_prop_t MSG_DELETE_BUTTON_STYLE_PROPS[] = {
    LV_STYLE_CONST_WIDTH(60),
    LV_STYLE_CONST_HEIGHT(92),
    LV_STYLE_CONST_X(258),
    LV_STYLE_CONST_Y(20),
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(0, 0, 0)),
    LV_STYLE_PROP_INV,
};

LV_STYLE_CONST_INIT(NTF_WIN_STYLE, NTF_WIN_STYLE_PROPS);
LV_STYLE_CONST_INIT(MSG_BG_STYLE, MSG_BG_STYLE_PROPS);
LV_STYLE_CONST_INIT(MSG_TITLE_STYLE, MSG_TITLE_STYLE_PROPS);
LV_STYLE_CONST_INIT(MSG_CONTENT_STYLE, MSG_CONTENT_STYLE_PROPS);
LV_STYLE_CONST_INIT(MSG_DELETE_BUTTON_STYLE, MSG_DELETE_BUTTON_STYLE_PROPS);

lv_obj_t *lv_notification_layout_create(lv_obj_t *parent_tv_obj)
{
#if LV_GDX_PATCH_DISABLE_STYLE_REFRESH && !LV_GDX_PATCH_USE_FAST_TILEVIEW
    lv_obj_enable_style_refresh(false);
    lv_disp_t * disp = lv_obj_get_disp(parent_tv_obj);
    lv_disp_enable_invalidation(disp, false);
#endif
    lv_obj_t *p_window = lv_obj_create(parent_tv_obj);
    lv_obj_add_style(p_window, (lv_style_t *)&NTF_WIN_STYLE, 0);
    lv_obj_set_style_pad_all(p_window, 0, 0);
    lv_obj_clear_flag(p_window, LV_OBJ_FLAG_SCROLLABLE);

    // Notification Title
    lv_obj_t *p_title = lv_img_create(p_window);
    lv_img_set_src(p_title, &wd_img_notification_title);
    lv_obj_align(p_title, LV_ALIGN_TOP_MID, 0, 20);

    // Messages
    lv_obj_t *msg_box = lv_obj_create(p_window);
    lv_obj_remove_style_all(msg_box);
    lv_obj_add_flag(msg_box, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scroll_dir(msg_box, LV_DIR_VER);
    lv_obj_set_size(msg_box, DISP_HOR_RES, 250);
    // lv_obj_set_pos(msg_box, 0, 40);
    lv_obj_center(msg_box);
    for (uint8_t i = 0; i < 5; i++)
    {
        lv_obj_t *msg = create_message_item(msg_box, "Wechat", "Hello there, this is GR5525 LVGL Demo.", (lv_img_dsc_t *)&wd_img_notification_msg_wechat);
        lv_obj_set_pos(msg, (DISP_HOR_RES - 250) / 2, i * (112 + 16));
    }

    // Delete All Button
    lv_obj_t *delete_all = lv_img_create(p_window);
    lv_img_set_src(delete_all, &wd_img_notification_del_all);

    lv_obj_align(delete_all, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(delete_all, lv_color_make(0xcc, 0x3c, 0x3c), 0);
    lv_obj_set_style_bg_opa(delete_all, LV_OPA_COVER, 0);
    #if LV_GDX_PATCH_USE_FAST_TILEVIEW
    lv_obj_add_flag(delete_all, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(delete_all, delete_all_btn_cb, LV_EVENT_CLICKED, 0);
    #endif

#if LV_GDX_PATCH_DISABLE_STYLE_REFRESH && !LV_GDX_PATCH_USE_FAST_TILEVIEW
    lv_disp_enable_invalidation(disp, true);
    lv_obj_enable_style_refresh(true);
    lv_obj_refresh_style(p_window, LV_PART_ANY, LV_STYLE_PROP_ANY);
#endif


    return p_window;
}

static lv_obj_t *create_message_item(lv_obj_t *parent, char *title, char *content, lv_img_dsc_t *icon)
{
    lv_obj_t *item = lv_obj_create(parent);
    lv_obj_remove_style_all(item);
    lv_obj_add_flag(item, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(item, 250, 112);
    lv_obj_set_scrollbar_mode(item, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(item, LV_DIR_HOR);

    // Background
    lv_obj_t *background = lv_obj_create(item);
    lv_obj_remove_style_all(background);
    lv_obj_add_style(background, (lv_style_t *)&MSG_BG_STYLE, 0);
    lv_obj_clear_flag(background, LV_OBJ_FLAG_SCROLLABLE);

    // Icon
    lv_obj_t *p_icon = lv_img_create(item);
    lv_obj_set_pos(p_icon, 12, 0);
    lv_img_set_src(p_icon, icon);

    // Title
    lv_obj_t *p_title = lv_label_create(background);
    lv_obj_add_style(p_title, (lv_style_t *)&MSG_TITLE_STYLE, 0);
    lv_label_set_long_mode(p_title, LV_LABEL_LONG_DOT);
    lv_label_set_text(p_title, title);

    // Content
    lv_obj_t *p_content = lv_label_create(background);
    lv_obj_add_style(p_content, (lv_style_t *)&MSG_CONTENT_STYLE, 0);
    lv_label_set_long_mode(p_content, LV_LABEL_LONG_WRAP);
    lv_label_set_text(p_content, content);

    // Delete Button
    lv_obj_t *delete_button = lv_img_create(item);
    lv_img_set_src(delete_button, &wd_img_notification_del_msg_btn);
    lv_obj_add_style(delete_button, (lv_style_t *)&MSG_DELETE_BUTTON_STYLE, 0);

    lv_obj_add_event_cb(item, delete_btn_scroll_snap, LV_EVENT_SCROLL_END, NULL);

    return item;
}

static void delete_btn_scroll_snap(lv_event_t *p_evt)
{
    // Hardcorded to check if half ot delete button has been shown
    if (!lv_event_get_param(p_evt))
    {
        // Scroll is caused by animation, not what we expected
        return;
    }

    lv_obj_t *obj = lv_event_get_target(p_evt);
    lv_coord_t sx = lv_obj_get_scroll_x(obj);

    if (sx <= 0 || sx >= 68)
    {
        return;
    }

    if (sx > 34)
    {
        // Scroll to delete bar
        lv_obj_scroll_to(obj, 68, 0, LV_ANIM_ON);
    }
    else
    {
        // Scroll back
        lv_obj_scroll_to(obj, 0, 0, LV_ANIM_ON);
    }
}

#if LV_GDX_PATCH_USE_FAST_TILEVIEW
#include "lv_layout_router.h"
static void delete_all_btn_cb(lv_event_t *p_evt)
{
    lv_obj_t *obj = lv_event_get_target(p_evt);
    lv_layout_router_back(obj);
}
#endif
