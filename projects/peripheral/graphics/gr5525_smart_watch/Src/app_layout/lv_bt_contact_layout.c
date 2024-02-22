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

#include "bt_phonecall_controller.h"

#pragma diag_suppress 177

#define WIN_TITLE_POS_Y (30)
#define LIST_POS_Y (80)
#define LIST_ITEM_WIDTH (360)
#define LIST_ITEM_HEIGHT (90)
#define LIST_PADDING_LEFT (60)
#define LIST_ICON_WIDTH (80)
#define LIST_ICON_HEIGHT (80)
#define LIST_CAPTION_HEIGHT (44)
#define LIST_CAPTION_WIDTH (200)
#define LIST_CAPTION_MARGIN_LEFT (10)

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

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
    LV_STYLE_CONST_X(0),
    LV_STYLE_CONST_Y(LIST_POS_Y),
    LV_STYLE_CONST_WIDTH(DISP_HOR_RES),
    LV_STYLE_CONST_HEIGHT((DISP_VER_RES - LIST_POS_Y)),
    LV_STYLE_PROP_INV,
};

static const lv_style_const_prop_t LIST_ITEM_STYLE_PROPS[] = {
    LV_STYLE_CONST_WIDTH(LIST_ITEM_WIDTH),
    LV_STYLE_CONST_HEIGHT(LIST_ITEM_HEIGHT),
    LV_STYLE_PROP_INV,
};

static const lv_style_const_prop_t CAPTION_STYLE_PROPS[] = {
    LV_STYLE_CONST_TEXT_FONT(&lv_font_montserrat_20),
    LV_STYLE_CONST_TEXT_COLOR(LV_COLOR_MAKE(0xFF, 0xFF, 0xFF)),
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(0, 0, 0)),
    // LV_STYLE_CONST_TEXT_ALIGN(LV_TEXT_ALIGN_CENTER),
    LV_STYLE_CONST_X((LIST_PADDING_LEFT + LIST_ICON_WIDTH + LIST_CAPTION_MARGIN_LEFT)),
    LV_STYLE_CONST_Y((LIST_ITEM_HEIGHT - LIST_CAPTION_HEIGHT) / 2),
    LV_STYLE_CONST_WIDTH(LIST_CAPTION_WIDTH),
    LV_STYLE_PROP_INV,
};

static const lv_style_const_prop_t ICON_STYLE_PROPS[] = {
    LV_STYLE_CONST_X(LIST_PADDING_LEFT),
    LV_STYLE_CONST_Y((LIST_ITEM_HEIGHT - LIST_ICON_HEIGHT) / 2),
    LV_STYLE_CONST_WIDTH(LIST_ICON_WIDTH),
    LV_STYLE_CONST_HEIGHT(LIST_ICON_HEIGHT),
    LV_STYLE_PROP_INV,
};

LV_STYLE_CONST_INIT(BT_WINDOW_STYLE, WINDOW_STYLE_PROPS);
LV_STYLE_CONST_INIT(BT_WINDOW_TITLE_STYLE, WINDOW_TITLE_STYLE_PROPS);
LV_STYLE_CONST_INIT(BT_LIST_STYLE, LIST_STYLE_PROPS);
LV_STYLE_CONST_INIT(BT_LIST_ITEM_STYLE, LIST_ITEM_STYLE_PROPS);
LV_STYLE_CONST_INIT(BT_CONTACT_CAPTION_STYLE, CAPTION_STYLE_PROPS);
LV_STYLE_CONST_INIT(BT_CONTACT_ICON_STYLE, ICON_STYLE_PROPS);

static lv_ll_t s_contact_list;
static lv_obj_t *s_list;
static bool s_initialized = false;

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
    lv_obj_t *p_label;
} my_holder_t;

static lv_obj_t *_create_item_view(lv_obj_t *obj, void *user_data, void *p_item_holder)
{
    my_holder_t *my_holder = (my_holder_t *)p_item_holder;
    lv_obj_t *p_item_view = lv_obj_create(obj);
    lv_obj_add_style(p_item_view, (lv_style_t *)&BT_LIST_ITEM_STYLE, 0);
    lv_obj_add_flag(p_item_view, LV_OBJ_FLAG_EVENT_BUBBLE);

    my_holder->p_label = lv_label_create(p_item_view);
    lv_obj_add_style(my_holder->p_label, (lv_style_t *)&BT_CONTACT_CAPTION_STYLE, 0);

    my_holder->p_icon = lv_img_rgb565_create(p_item_view);
    lv_img_rgb565_set_src(my_holder->p_icon, &wd_img_app_02_activity);
    lv_obj_add_style(my_holder->p_icon, (lv_style_t *)&BT_CONTACT_ICON_STYLE, 0);
    lv_obj_add_flag(my_holder->p_icon, LV_OBJ_FLAG_EVENT_BUBBLE);

    return p_item_view;
}

// const char *NAMES[10] = {
//     "Tom",
//     "Jerry",
//     "fo祖保佑",
//     "永无BUG",
//     "Christopher",
//     "Edison",
//     "Jhon von Neumann",
//     "Alan Mathison Turing",
//     "Claude Elwood Shannon",
//     "Dennis M.Ritchie",
// };

static void _bind_item_view(lv_obj_t *obj, void *user_data, void *p_item_holder, uint32_t index, lv_obj_t *p_resued_item_obj, bool bindOrUnbind)
{
    my_holder_t *my_holder = (my_holder_t *)p_item_holder;
    // if (index < 10)
    // {
    //     lv_label_set_text(my_holder->p_label, NAMES[index]);
    // }
    // else
    // {
    //     lv_label_set_text_fmt(my_holder->p_label, "IDX: %d", index);
    // }
    char** dev_name;
    uint32_t idx = 0;
    lv_ll_t *p_contacts_list = (lv_ll_t *)user_data;
    _LV_LL_READ(p_contacts_list, dev_name)
    {
        if (idx == index)
        {
            lv_label_set_text_static(my_holder->p_label, *dev_name);
            break;
        }
        idx++;
    }
}

static void _item_clicked_cb(lv_obj_t *obj, void *user_data, void *p_item_holder, uint32_t index, lv_event_t *evt)
{
    printf("ITEM: %d clicked\n", index);
    // if (evt->code == LV_EVENT_CLICKED)
    // {
    //     if (index < 10)
    //     {
    //         lv_virtual_list_update_data(obj, NULL, 50);
    //     }
    //     else
    //     {
    //         lv_virtual_list_update_data(obj, NULL, 10);
    //     }
    // }
}



static void bt_contact_on_contacts_updated(lv_ll_t *p_contacts_list)
{
    if (s_list)
    {
        lv_virtual_list_update_data(s_list, (void *)p_contacts_list, _lv_ll_get_len(p_contacts_list));
    }
}

static void contact_layout_lifecycle(lv_event_t *event)
{
    if (event->code == LV_EVENT_READY)
    {
        bt_phonecall_controller_set_contacts_updated_cb(bt_contact_on_contacts_updated);
        bt_phonecall_controller_request_contacts_update(false);
    }
    else if (event->code == LV_EVENT_DELETE)
    {
        bt_phonecall_controller_set_contacts_updated_cb(NULL);
        s_list = NULL;
    }
}

lv_obj_t *lv_bt_contact_layout_create(lv_obj_t *parent_tv_obj)
{
    lv_obj_t *p_window = lv_obj_create(parent_tv_obj);

    lv_obj_add_style(p_window, (lv_style_t *)&BT_WINDOW_STYLE, 0);

    lv_obj_t *p_title = lv_label_create(p_window);
    lv_obj_add_style(p_title, (lv_style_t *)&BT_WINDOW_TITLE_STYLE, 0);
    lv_label_set_text(p_title, "CONTACT");

    lv_obj_t *p_list = lv_virtual_list_create(p_window,
                                              LIST_ITEM_HEIGHT,
                                              sizeof(my_holder_t),
                                              _create_item_view,
                                              _bind_item_view);
    lv_obj_add_style(p_list, (lv_style_t *)&BT_LIST_STYLE, 0);
    lv_virtual_list_set_padding_space(p_list, 50, 100, 0);
    lv_virtual_list_set_item_clicked_cb(p_list, _item_clicked_cb);

    s_list = p_list;

    lv_obj_add_event_cb(p_window, contact_layout_lifecycle, LV_EVENT_ALL, NULL);

    return p_window;
}
