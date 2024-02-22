#include "lvgl.h"
#include "lv_img_dsc_list.h"
#include "lv_virtual_list.h"
#include "lv_layout_router.h"
#include "lv_bt_dev_info.h"
#include "app_bt.h"
#include "bt_conn_controller.h"
#include "app_log.h"
#include <stdio.h>

#define TITLE_HIGHT (60)
#define LIST_ITEM_WIDTH (240)
#define LIST_ITEM_HEIGHT (70)
#define LIST_BOX_HEIGHT (224)

/*
 * STATIC VARS DEFINITIONS
 *****************************************************************************************
 */
typedef struct
{
    char dev_name[BT_DEV_NAME_MAX_LEN];
    int dev_name_len;
    uint8_t mac[6];
    bool is_connected;
} gx_bt_paired_dev_t;

typedef struct
{
    lv_obj_t *p_dev_name;
    uint8_t mac[6];
    bool is_connected;
} my_holder_t;

static lv_ll_t s_bt_paired_dev_ll;
static lv_obj_t *p_list;

static const lv_style_const_prop_t WINDOW_STYLE_PROPS[] = {
    LV_STYLE_CONST_PAD_TOP(0),
    LV_STYLE_CONST_PAD_BOTTOM(0),
    LV_STYLE_CONST_PAD_LEFT(0),
    LV_STYLE_CONST_PAD_RIGHT(0),
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(0, 0, 0)),
#if LV_GDX_PATCH_REMOVE_BORDER
#else
    LV_STYLE_CONST_BORDER_WIDTH(0),
#endif
    LV_STYLE_PROP_INV,
};

static const lv_style_const_prop_t TITLE_STYLE_PROPS[] = {
    LV_STYLE_CONST_WIDTH(DISP_HOR_RES),
    LV_STYLE_CONST_HEIGHT(TITLE_HIGHT),
    LV_STYLE_CONST_X(0),
    LV_STYLE_CONST_Y((TITLE_HIGHT - 30)), // lv_font_montserrat_28.line_height = 30
    LV_STYLE_CONST_TEXT_FONT(&lv_font_montserrat_28),
    LV_STYLE_CONST_TEXT_COLOR(LV_COLOR_MAKE(0xFF, 0xFF, 0xFF)),
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(0, 0, 0)),
    LV_STYLE_CONST_TEXT_ALIGN(LV_TEXT_ALIGN_CENTER),
    LV_STYLE_PROP_INV,
};

static const lv_style_const_prop_t LINE_STYLE_PROPS[] = {
    LV_STYLE_CONST_LINE_WIDTH(2),
    LV_STYLE_CONST_LINE_COLOR(LV_COLOR_MAKE(0x4B, 0x80, 0xB0)),
    LV_STYLE_PROP_INV,
};

static const lv_style_const_prop_t LIST_BOX_STYLE_PROPS[] = {
    LV_STYLE_CONST_WIDTH(LIST_ITEM_WIDTH),
    LV_STYLE_CONST_HEIGHT(LIST_BOX_HEIGHT),
    LV_STYLE_CONST_PAD_TOP(0),
    LV_STYLE_CONST_PAD_BOTTOM(0),
    LV_STYLE_CONST_PAD_LEFT(0),
    LV_STYLE_CONST_PAD_RIGHT(0),
    LV_STYLE_PROP_INV,
};

static const lv_style_const_prop_t LIST_ITEM_STYLE_PROPS[] = {
    LV_STYLE_CONST_WIDTH(LIST_ITEM_WIDTH),
    LV_STYLE_CONST_HEIGHT(LIST_ITEM_HEIGHT),
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(0x18, 0x18, 0x18)),
    LV_STYLE_CONST_BG_OPA(LV_OPA_COVER),
    LV_STYLE_PROP_INV,
};

static const lv_style_const_prop_t LIST_ITEM_TITLE_STYLE_PROPS[] = {
    LV_STYLE_CONST_WIDTH(LIST_ITEM_WIDTH - 20),
    // LV_STYLE_CONST_HEIGHT(20),
    LV_STYLE_CONST_TEXT_FONT(&lv_font_montserrat_20),
    LV_STYLE_CONST_TEXT_COLOR(LV_COLOR_MAKE(0xFF, 0xFF, 0xFF)),
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(0x18, 0x18, 0x18)),
    LV_STYLE_CONST_TEXT_ALIGN(LV_TEXT_ALIGN_LEFT),
    LV_STYLE_PROP_INV,
};

LV_STYLE_CONST_INIT(BT_PAIRED_WINDOW_STYLE, WINDOW_STYLE_PROPS);
LV_STYLE_CONST_INIT(BT_PAIRED_TITLE_STYLE, TITLE_STYLE_PROPS);
LV_STYLE_CONST_INIT(BT_PAIRED_LINE_STYLE, LINE_STYLE_PROPS);
LV_STYLE_CONST_INIT(BT_PAIRED_LIST_BOX_STYLE, LIST_BOX_STYLE_PROPS);
LV_STYLE_CONST_INIT(BT_PAIRED_LIST_ITEM_STYLE, LIST_ITEM_STYLE_PROPS);
LV_STYLE_CONST_INIT(BT_PAIRED_LIST_ITEM_TITLE_STYLE, LIST_ITEM_TITLE_STYLE_PROPS);

/*
 * STATIC METHODS IMPLEMENT
 *****************************************************************************************
 */
static void bt_paird_dev_ll_init(void)
{
    _lv_ll_clear(&s_bt_paired_dev_ll);
    _lv_ll_init(&s_bt_paired_dev_ll, sizeof(gx_bt_paired_dev_t));
}

static void bt_paired_dev_ll_node_insert(char *p_dev_name, int dev_name_length, uint8_t *p_mac, bool is_connected)
{
    if (dev_name_length > BT_DEV_NAME_MAX_LEN)
    {
        dev_name_length = BT_DEV_NAME_MAX_LEN;
    }

    gx_bt_paired_dev_t *p_paired_dev = _lv_ll_ins_tail(&s_bt_paired_dev_ll);
    memcpy(p_paired_dev->dev_name, p_dev_name, dev_name_length);
    memcpy(p_paired_dev->mac, p_mac, 6);
    p_paired_dev->dev_name_len = dev_name_length;
    p_paired_dev->is_connected = is_connected;
}

static void bt_paired_dev_ll_delete(void)
{
    _lv_ll_clear(&s_bt_paired_dev_ll);
}

static lv_obj_t *_create_item_view(lv_obj_t *obj, void *user_data, void *p_item_holder)
{
    my_holder_t *my_holder = (my_holder_t *)p_item_holder;

    lv_obj_t *p_item_view = lv_obj_create(obj);
    lv_obj_add_style(p_item_view, (lv_style_t *)&BT_PAIRED_LIST_ITEM_STYLE, 0);
    lv_obj_add_flag(p_item_view, LV_OBJ_FLAG_EVENT_BUBBLE);

    my_holder->p_dev_name = lv_label_create(p_item_view);
    lv_obj_add_style(my_holder->p_dev_name, (lv_style_t *)&BT_PAIRED_LIST_ITEM_TITLE_STYLE, 0);
    lv_obj_align(my_holder->p_dev_name, LV_ALIGN_LEFT_MID, 10, 0);

    return p_item_view;
}

static void _bind_item_view(lv_obj_t *obj, void *user_data, void *p_item_holder, uint32_t index, lv_obj_t *p_resued_item_obj, bool bindOrUnbind)
{
    my_holder_t *my_holder = (my_holder_t *)p_item_holder;

    gx_bt_paired_dev_t *p_paired_dev;
    int ll_idx = 0;
    _LV_LL_READ(&s_bt_paired_dev_ll, p_paired_dev)
    {
        if (ll_idx == index)
        {
            lv_label_set_text_fmt(my_holder->p_dev_name, "%s", p_paired_dev->dev_name);
            memcpy(my_holder->mac, p_paired_dev->mac, 6);
            my_holder->is_connected = p_paired_dev->is_connected;
            break;
        }
        ll_idx++;
    }
}

#if LV_GDX_PATCH_USE_FAST_TILEVIEW
static void bt_conn_ctx_listener(bt_conn_ctx_t *p_ctx, void *user_data)
{
    if (p_ctx)
    {
        if (p_ctx->connected_dev_mac && (0 == memcmp(p_ctx->p_mac, p_ctx->connected_dev_mac, 6)))
        {
            bt_paired_dev_ll_node_insert(p_ctx->p_dev_name, p_ctx->dev_name_len, p_ctx->p_mac, true);
        }
        else
        {
            bt_paired_dev_ll_node_insert(p_ctx->p_dev_name, p_ctx->dev_name_len, p_ctx->p_mac, false);
        }

        lv_virtual_list_update_data(p_list, NULL, _lv_ll_get_len(&s_bt_paired_dev_ll));
    }
}

static void bt_paired_dev_event_cb(lv_event_t *e)
{
    if (e->code == LV_EVENT_DELETE)
    {
    }
    else if (e->code == LV_EVENT_READY)
    {
        bt_conn_controller_set_ctx_listener(bt_conn_ctx_listener, NULL);
        bt_api_pair_list_get();
    }
    else if (e->code == LV_EVENT_CANCEL)
    {
        bt_conn_controller_set_ctx_listener(NULL, NULL);
        bt_paired_dev_ll_delete();
    }
}

static void _item_clicked_cb(lv_obj_t *obj, void *user_data, void *p_widget_holder, uint32_t index, lv_event_t *evt)
{
    bt_dev_info_t bt_dev_info;
    my_holder_t *my_holder = (my_holder_t *)p_widget_holder;
    char *text = ((lv_label_t *)my_holder->p_dev_name)->text;
    memcpy(bt_dev_info.dev_name, text, strlen(text) + 1);
    memcpy(&bt_dev_info.mac, my_holder->mac, 6);

    if (evt->code == LV_EVENT_CLICKED)
    {
        if (my_holder->is_connected)
        {
            lv_layout_router_show_bt_disconnect(obj->parent, (void *)&bt_dev_info);
        }
        else
        {
            lv_layout_router_show_bt_pair_dev(obj->parent, (void *)&bt_dev_info);
        }
    }
    else if (evt->code == LV_EVENT_LONG_PRESSED)
    {
        lv_layout_router_show_bt_remove_dev(obj->parent, (void *)&bt_dev_info);
    }
}
#else // LV_GDX_PATCH_USE_FAST_TILEVIEW
static void _item_clicked_cb(lv_obj_t *obj, void *user_data, void *p_widget_holder, uint32_t index, lv_event_t *evt)
{
}
#endif // LV_GDX_PATCH_USE_FAST_TILEVIEW

/*
 * GLOBAL METHODS IMPLEMENT
 *****************************************************************************************
 */
lv_obj_t *lv_bt_paired_dev_layout_create(lv_obj_t *parent_tv_obj)
{
#if LV_GDX_PATCH_DISABLE_STYLE_REFRESH && !LV_GDX_PATCH_USE_FAST_TILEVIEW
    lv_obj_enable_style_refresh(false);
    lv_disp_t *disp = lv_obj_get_disp(parent_tv_obj);
    lv_disp_enable_invalidation(disp, false);
#endif

    lv_obj_t *p_window = lv_obj_create(parent_tv_obj);
    lv_obj_set_size(p_window, DISP_HOR_RES, DISP_VER_RES);
    lv_obj_add_style(p_window, (lv_style_t *)&BT_PAIRED_WINDOW_STYLE, 0);

    // Title
    lv_obj_t *p_title = lv_label_create(p_window);
    lv_obj_set_size(p_title, DISP_HOR_RES, 40);
    lv_obj_add_style(p_title, (lv_style_t *)&BT_PAIRED_TITLE_STYLE, 0);
    lv_label_set_text_fmt(p_title, "%s", "Paired Device");
    lv_obj_set_pos(p_title, 0, 34);

    bt_paird_dev_ll_init();

    // Paired device list
    p_list = lv_virtual_list_create(p_window,
                                    LIST_ITEM_HEIGHT,
                                    sizeof(my_holder_t),
                                    _create_item_view,
                                    _bind_item_view);
    lv_obj_add_style(p_list, (lv_style_t *)&BT_PAIRED_LIST_BOX_STYLE, 0);
    lv_obj_align(p_list, LV_ALIGN_CENTER, 0, 10);
    lv_virtual_list_set_padding_space(p_list, 0, 100, 10);
    lv_virtual_list_set_item_clicked_cb(p_list, _item_clicked_cb);

#if LV_GDX_PATCH_USE_FAST_TILEVIEW
    lv_obj_add_event_cb(p_window, bt_paired_dev_event_cb, LV_EVENT_ALL, NULL);
#endif

#if LV_GDX_PATCH_DISABLE_STYLE_REFRESH && !LV_GDX_PATCH_USE_FAST_TILEVIEW
    lv_disp_enable_invalidation(disp, true);
    lv_obj_enable_style_refresh(true);
    lv_obj_refresh_style(p_window, LV_PART_ANY, LV_STYLE_PROP_ANY);
#endif

    return p_window;
}
