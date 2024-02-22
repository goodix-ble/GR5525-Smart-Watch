#include "app_key.h"
#include "app_rtc.h"
#include "lvgl.h"
#include "lv_img_dsc_list.h"
#include "lv_port_gximg.h"
#include "lv_simplified_obj.h"
#include "lv_layout_router.h"
#include <stdio.h>

#include "lv_virtual_list.h"
#include "lv_img_rgb565.h"

#define WIN_TITLE_POS_Y (30)
#define LIST_POS_Y (80)
#define LIST_ITEM_WIDTH (260)
#define LIST_ITEM_HEIGHT (80)
#define LIST_ITEM_GAP (10)
#define LIST_ITEM_ICON_MARGIN_LEFT (16)
#define LIST_ITEM_ICON_WIDTH (20)
#define LIST_ITEM_ICON_HEIGHT (20)
#define LIST_ITEM_NAME_MARGIN_LEFT (16)
#define LIST_ITEM_NAME_HEIGHT (20)
#define LIST_ITEM_NAME_WIDTH (100)
#define LIST_ITEM_STATUS_MARGIN_TOP (20)
#define LIST_ITEM_STATUS_MARGIN_RIGHT (16)
#define LIST_ITEM_STATUS_HEIGHT (22)
#define LIST_ITEM_STATUS_WIDTH (100)
#define LIST_ITEM_TIME_INFO_MARGIN_TOP (6)
#define LIST_ITEM_TIME_INFO_MARGIN_RIGHT (16)
#define LIST_ITEM_TIME_INFO_HEIGHT (22)
#define LIST_ITEM_TIME_INFO_WIDTH (100)

/*
 * STATIC VARS DEFINITIONS
 *****************************************************************************************
 */

static const lv_style_const_prop_t WINDOW_STYLE_PROPS[] = {
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(0, 0, 0)),
    LV_STYLE_CONST_BG_OPA(LV_OPA_COVER),
    LV_STYLE_CONST_WIDTH(DISP_HOR_RES),
    LV_STYLE_CONST_HEIGHT(DISP_VER_RES),
    LV_STYLE_CONST_PAD_TOP(0),
    LV_STYLE_CONST_PAD_BOTTOM(0),
    LV_STYLE_CONST_PAD_LEFT(0),
    LV_STYLE_CONST_PAD_RIGHT(0),
    LV_STYLE_PROP_INV,
};

static const lv_style_const_prop_t WINDOW_TITLE_STYLE_PROPS[] = {
    LV_STYLE_CONST_TEXT_FONT(&lv_font_montserrat_40),
    LV_STYLE_CONST_TEXT_COLOR(LV_COLOR_MAKE(0xFF, 0xFF, 0xFF)),
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(0, 0, 0)),
    LV_STYLE_CONST_TEXT_ALIGN(LV_TEXT_ALIGN_CENTER),
    LV_STYLE_CONST_X(0),
    LV_STYLE_CONST_Y(WIN_TITLE_POS_Y),
    LV_STYLE_CONST_WIDTH(DISP_HOR_RES),
    LV_STYLE_PROP_INV,
};

static const lv_style_const_prop_t LIST_STYLE_PROPS[] = {
    LV_STYLE_CONST_X((DISP_HOR_RES - LIST_ITEM_WIDTH) / 2),
    LV_STYLE_CONST_Y(LIST_POS_Y),
    LV_STYLE_CONST_WIDTH(LIST_ITEM_WIDTH),
    LV_STYLE_CONST_HEIGHT((DISP_VER_RES - LIST_POS_Y)),
    LV_STYLE_PROP_INV,
};

static const lv_style_const_prop_t LIST_ITEM_STYLE_PROPS[] = {
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(24, 24, 24)),
    LV_STYLE_CONST_BG_OPA(LV_OPA_COVER),
    LV_STYLE_CONST_WIDTH(LIST_ITEM_WIDTH),
    LV_STYLE_CONST_HEIGHT(LIST_ITEM_HEIGHT),
    LV_STYLE_PROP_INV,
};

static const lv_style_const_prop_t LIST_ITEM_ICON_STYLE_PROPS[] = {
    LV_STYLE_CONST_X(LIST_ITEM_ICON_MARGIN_LEFT),
    LV_STYLE_CONST_Y((LIST_ITEM_HEIGHT - LIST_ITEM_ICON_HEIGHT) / 2),
    LV_STYLE_CONST_WIDTH(LIST_ITEM_ICON_WIDTH),
    LV_STYLE_CONST_HEIGHT(LIST_ITEM_ICON_HEIGHT),
    LV_STYLE_PROP_INV,
};

static const lv_style_const_prop_t LIST_ITEM_NAME_STYLE_PROPS[] = {
    LV_STYLE_CONST_TEXT_FONT(&lv_font_montserrat_20),
    LV_STYLE_CONST_TEXT_COLOR(LV_COLOR_MAKE(0xFF, 0xFF, 0xFF)),
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(24, 24, 24)),
    // LV_STYLE_CONST_TEXT_ALIGN(LV_TEXT_ALIGN_CENTER),
    LV_STYLE_CONST_X((LIST_ITEM_ICON_MARGIN_LEFT + LIST_ITEM_ICON_WIDTH + LIST_ITEM_NAME_MARGIN_LEFT)),
    LV_STYLE_CONST_Y((LIST_ITEM_HEIGHT - LIST_ITEM_NAME_HEIGHT) / 2),
    LV_STYLE_CONST_WIDTH(LIST_ITEM_NAME_WIDTH),
    LV_STYLE_CONST_HEIGHT(LIST_ITEM_NAME_HEIGHT),
    LV_STYLE_PROP_INV,
};

static const lv_style_const_prop_t LIST_ITEM_STATUS_STYLE_PROPS[] = {
    LV_STYLE_CONST_TEXT_FONT(&lv_font_montserrat_20),
    LV_STYLE_CONST_TEXT_COLOR(LV_COLOR_MAKE(0xFF, 0xFF, 0xFF)),
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(24, 24, 24)),
    LV_STYLE_CONST_TEXT_ALIGN(LV_TEXT_ALIGN_RIGHT),
    LV_STYLE_CONST_X((LIST_ITEM_WIDTH - LIST_ITEM_STATUS_MARGIN_RIGHT - LIST_ITEM_STATUS_WIDTH)),
    LV_STYLE_CONST_Y(LIST_ITEM_STATUS_MARGIN_TOP),
    LV_STYLE_CONST_WIDTH(LIST_ITEM_STATUS_WIDTH),
    LV_STYLE_CONST_HEIGHT(LIST_ITEM_STATUS_HEIGHT),
    LV_STYLE_PROP_INV,
};

static const lv_style_const_prop_t LIST_ITEM_TIME_INFO_STYLE_PROPS[] = {
    LV_STYLE_CONST_TEXT_FONT(&lv_font_montserrat_20),
    LV_STYLE_CONST_TEXT_COLOR(LV_COLOR_MAKE(0xFF, 0xFF, 0xFF)),
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(24, 24, 24)),
    LV_STYLE_CONST_TEXT_ALIGN(LV_TEXT_ALIGN_RIGHT),
    LV_STYLE_CONST_X((LIST_ITEM_WIDTH - LIST_ITEM_TIME_INFO_MARGIN_RIGHT - LIST_ITEM_TIME_INFO_WIDTH)),
    LV_STYLE_CONST_Y(LIST_ITEM_STATUS_MARGIN_TOP + LIST_ITEM_STATUS_HEIGHT + LIST_ITEM_TIME_INFO_MARGIN_TOP),
    LV_STYLE_CONST_WIDTH(LIST_ITEM_TIME_INFO_WIDTH),
    LV_STYLE_CONST_HEIGHT(LIST_ITEM_TIME_INFO_HEIGHT),
    LV_STYLE_PROP_INV,
};

static LV_STYLE_CONST_INIT(BT_WINDOW_STYLE, WINDOW_STYLE_PROPS);
static LV_STYLE_CONST_INIT(BT_WINDOW_TITLE_STYLE, WINDOW_TITLE_STYLE_PROPS);
static LV_STYLE_CONST_INIT(BT_LIST_STYLE, LIST_STYLE_PROPS);
static LV_STYLE_CONST_INIT(BT_LIST_ITEM_STYLE, LIST_ITEM_STYLE_PROPS);
static LV_STYLE_CONST_INIT(BT_LIST_ITEM_ICON_STYLE, LIST_ITEM_ICON_STYLE_PROPS);
static LV_STYLE_CONST_INIT(BT_LIST_ITEM_NAME_STYLE, LIST_ITEM_NAME_STYLE_PROPS);
static LV_STYLE_CONST_INIT(BT_LIST_ITEM_STATUS_STYLE, LIST_ITEM_STATUS_STYLE_PROPS);
static LV_STYLE_CONST_INIT(BT_LIST_ITEM_TIME_INFO_STYLE, LIST_ITEM_TIME_INFO_STYLE_PROPS);
/*
 * STATIC METHODS DECLARATION
 *****************************************************************************************
 */

/*
 * PUBLIC VARS DEFINITIONS
 *****************************************************************************************
 */

/*
 * STATIC METHODS IMPLEMENT
 *****************************************************************************************
 */

/*
 * GLOBAL METHODS IMPLEMENT
 *****************************************************************************************
 */
typedef struct
{
    lv_obj_t *p_icon;
    lv_obj_t *p_name_label;
    lv_obj_t *p_status_label;
    lv_obj_t *p_time_info_label;
} my_holder_t;

static lv_obj_t *_create_item_view(lv_obj_t *obj, void *user_data, void *p_item_holder)
{
    my_holder_t *my_holder = (my_holder_t *)p_item_holder;

    lv_obj_t *p_item_view = lv_obj_create(obj);
    lv_obj_add_style(p_item_view, (lv_style_t *)&BT_LIST_ITEM_STYLE, 0);
    lv_obj_add_flag(p_item_view, LV_OBJ_FLAG_EVENT_BUBBLE);

    my_holder->p_icon = lv_img_rgb565_create(p_item_view);
    lv_img_rgb565_set_src(my_holder->p_icon, &wd_img_weather_moonset_icon);
    lv_obj_add_style(my_holder->p_icon, (lv_style_t *)&BT_LIST_ITEM_ICON_STYLE, 0);
    lv_obj_add_flag(my_holder->p_icon, LV_OBJ_FLAG_EVENT_BUBBLE);

    my_holder->p_name_label = lv_label_create(p_item_view);
    lv_obj_add_style(my_holder->p_name_label, (lv_style_t *)&BT_LIST_ITEM_NAME_STYLE, 0);

    my_holder->p_status_label = lv_label_create(p_item_view);
    lv_obj_add_style(my_holder->p_status_label, (lv_style_t *)&BT_LIST_ITEM_STATUS_STYLE, 0);

    my_holder->p_time_info_label = lv_label_create(p_item_view);
    lv_obj_add_style(my_holder->p_time_info_label, (lv_style_t *)&BT_LIST_ITEM_TIME_INFO_STYLE, 0);

    return p_item_view;
}

static const char *NAMES[10] = {
    "Tom",
    "Jerry",
    "fo祖保佑",
    "永无BUG",
    "Christopher",
    "Edison",
    "Jhon von Neumann",
    "Alan Mathison Turing",
    "Claude Elwood Shannon",
    "Dennis M.Ritchie",
};

static void _bind_item_view(lv_obj_t *obj, void *user_data, void *p_item_holder, uint32_t index, lv_obj_t *p_resued_item_obj, bool bindOrUnbind)
{
    my_holder_t *my_holder = (my_holder_t *)p_item_holder;
    // icon
    if ((index & 1) == 0)
    {
        lv_img_rgb565_set_src(my_holder->p_icon, &wd_img_weather_sunrise_icon);
        printf("sunrise+++++%d\n", index);
    }
    else
    {
        lv_img_rgb565_set_src(my_holder->p_icon, &wd_img_weather_moon_icon);
        printf("sunrise#####%d\n", index);
    }
    // name
    if (index < 10)
    {
        lv_label_set_text(my_holder->p_name_label, NAMES[index]);
    }
    else
    {
        lv_label_set_text_fmt(my_holder->p_name_label, "IDX: %d", index);
    }
    // time
    lv_label_set_text_fmt(my_holder->p_time_info_label, "%02d:%02d", index, index);
    // status
    if ((index & 1) == 0)
    {
        lv_label_set_text(my_holder->p_status_label, "Income");
    }
    else
    {
        lv_label_set_text(my_holder->p_status_label, "Rejected");
    }
}

static void _item_clicked_cb(lv_obj_t *obj, void *user_data, void *p_item_holder, uint32_t index, lv_event_t *evt)
{
    printf("ITEM: %d clicked, code: %d\n", index, evt->code);
    if (evt->code == LV_EVENT_LONG_PRESSED)
    {
        if (index < 10)
        {
            lv_virtual_list_update_data(obj, NULL, 50);
        }
        else
        {
            lv_virtual_list_update_data(obj, NULL, 10);
        }
    }
}

lv_obj_t *lv_bt_contact_recent_layout_create(lv_obj_t *parent_tv_obj)
{
    lv_obj_t *p_window = lv_obj_create(parent_tv_obj);

    lv_obj_add_style(p_window, (lv_style_t *)&BT_WINDOW_STYLE, 0);

    lv_obj_t *p_title = lv_label_create(p_window);
    lv_obj_add_style(p_title, (lv_style_t *)&BT_WINDOW_TITLE_STYLE, 0);
    lv_label_set_text(p_title, "RECENT");

    lv_obj_t *p_list = lv_virtual_list_create(p_window,
                                              LIST_ITEM_HEIGHT,
                                              sizeof(my_holder_t),
                                              _create_item_view,
                                              _bind_item_view);
    lv_obj_add_style(p_list, (lv_style_t *)&BT_LIST_STYLE, 0);
    lv_virtual_list_update_data(p_list, NULL, 100);
    lv_virtual_list_set_padding_space(p_list, 50, 100, 10);
    lv_virtual_list_set_item_clicked_cb(p_list, _item_clicked_cb);

    return p_window;
}
