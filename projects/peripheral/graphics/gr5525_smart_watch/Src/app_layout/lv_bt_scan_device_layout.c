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
#define LIST_BOX_HEIGHT (150)

/*
 * STATIC VARS DEFINITIONS
 *****************************************************************************************
 */
/** Description of a bt scan device linked list*/
typedef struct
{
    char dev_name[BT_DEV_NAME_MAX_LEN];
    int dev_name_len;
    uint8_t mac[6];
    uint32_t mac_u32;
    int rssi;
} gx_bt_scan_dev_t;

typedef struct
{
    lv_obj_t *p_dev_name;
    lv_obj_t *p_rssi;
    uint8_t mac_addr[6];
} my_holder_t;

static lv_obj_t *s_dev_name = NULL;
static lv_ll_t s_bt_scan_dev_ll;
static lv_obj_t *p_list;
static bool is_scanning = false;

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

static const lv_style_const_prop_t LABEL_STYLE_PROPS[] = {
    LV_STYLE_CONST_TEXT_FONT(&lv_font_montserrat_20),
    LV_STYLE_CONST_TEXT_COLOR(LV_COLOR_MAKE(0xD7, 0xD7, 0xD7)),
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(0, 0, 0)),
    LV_STYLE_CONST_TEXT_ALIGN(LV_TEXT_ALIGN_CENTER),
    LV_STYLE_PROP_INV,
};

static const lv_style_const_prop_t LIST_BOX_STYLE_PROPS[] = {
    LV_STYLE_CONST_WIDTH(LIST_ITEM_WIDTH),
    LV_STYLE_CONST_HEIGHT(LIST_BOX_HEIGHT),
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
    LV_STYLE_CONST_TEXT_FONT(&lv_font_montserrat_20),
    LV_STYLE_CONST_TEXT_COLOR(LV_COLOR_MAKE(0xC4, 0xC4, 0xC4)),
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(0x18, 0x18, 0x18)),
    LV_STYLE_CONST_TEXT_ALIGN(LV_TEXT_ALIGN_LEFT),
    LV_STYLE_PROP_INV,
};

LV_STYLE_CONST_INIT(BT_SCAN_WINDOW_STYLE, WINDOW_STYLE_PROPS);
LV_STYLE_CONST_INIT(BT_SCAN_TITLE_STYLE, TITLE_STYLE_PROPS);
LV_STYLE_CONST_INIT(BT_SCAN_LABEL_STYLE, LABEL_STYLE_PROPS);
LV_STYLE_CONST_INIT(BT_SCAN_LIST_BOX_STYLE, LIST_BOX_STYLE_PROPS);
LV_STYLE_CONST_INIT(BT_SCAN_LIST_ITEM_STYLE, LIST_ITEM_STYLE_PROPS);
LV_STYLE_CONST_INIT(BT_SCAN_LIST_ITEM_TITLE_STYLE, LIST_ITEM_TITLE_STYLE_PROPS);

/*
 * STATIC METHODS DECLARATION
 *****************************************************************************************
 */

/*
 * STATIC METHODS IMPLEMENT
 *****************************************************************************************
 */
static uint32_t addr2int32(uint8_t *p_addr)
{
    uint32_t mac;
    mac = ((p_addr[5] & 0xF0) << 20) +
          ((((p_addr[5] & 0x0F) ^ ((p_addr[4] & 0xF0) >> 4))) << 20) +
          ((((p_addr[4] & 0x0F) ^ ((p_addr[3] & 0xF0) >> 4))) << 16) +
          ((((p_addr[3] & 0x0F) ^ ((p_addr[2] & 0xF0) >> 4))) << 12) +
          ((((p_addr[2] & 0x0F) ^ ((p_addr[1] & 0xF0) >> 4))) << 8) +
          ((((p_addr[1] & 0x0F) ^ ((p_addr[0] & 0xF0) >> 4))) << 4) +
          (p_addr[0] & 0x0F);
    return mac;
}

static void bt_scan_dev_ll_init(void)
{
    _lv_ll_clear(&s_bt_scan_dev_ll);
    _lv_ll_init(&s_bt_scan_dev_ll, sizeof(gx_bt_scan_dev_t));
}

static void bt_scan_dev_ll_node_insert(char *p_dev_name, int dev_name_length, uint8_t *p_mac, int rssi)
{
    if (dev_name_length > BT_DEV_NAME_MAX_LEN)
    {
        dev_name_length = BT_DEV_NAME_MAX_LEN;
    }

    gx_bt_scan_dev_t *p_cur_dev;
    gx_bt_scan_dev_t *p_new_dev;
    uint32_t mac_u32 = addr2int32(p_mac);

    // 基于MAC地址去重；若RSSI有改变，则移除该节点，基于新的RSSI值调整顺序后重新插入链表
    _LV_LL_READ(&s_bt_scan_dev_ll, p_cur_dev)
    {
        if (p_cur_dev->mac_u32 == mac_u32)
        {
            if (p_cur_dev->rssi == rssi)
            {
                memcpy(p_cur_dev->dev_name, p_dev_name, dev_name_length);
                p_cur_dev->dev_name_len = dev_name_length;
                return;
            }
            else
            {
                _lv_ll_remove(&s_bt_scan_dev_ll, p_cur_dev);
            }
            break;
        }
    }

    // 基于rssi插入当前链表中
    if (_lv_ll_is_empty(&s_bt_scan_dev_ll))
    {
        p_new_dev = _lv_ll_ins_head(&s_bt_scan_dev_ll);
    }
    else
    {
        _LV_LL_READ(&s_bt_scan_dev_ll, p_cur_dev)
        {
            if (p_cur_dev->rssi <= rssi)
            {
                p_new_dev = _lv_ll_ins_prev(&s_bt_scan_dev_ll, p_cur_dev);
                break;
            }

            gx_bt_scan_dev_t *p_next_dev = _lv_ll_get_next(&s_bt_scan_dev_ll, p_cur_dev);
            if (p_next_dev == NULL)
            {
                if (p_cur_dev->rssi > rssi)
                {
                    p_new_dev = _lv_ll_ins_tail(&s_bt_scan_dev_ll);
                }
                else
                {
                    p_new_dev = _lv_ll_ins_prev(&s_bt_scan_dev_ll, p_cur_dev);
                }
                break;
            }
            else
            {
                if (p_cur_dev->rssi > rssi && (p_next_dev->rssi <= rssi))
                {
                    p_new_dev = _lv_ll_ins_prev(&s_bt_scan_dev_ll, p_next_dev);
                    break;
                }
            }
        }
    }

    p_new_dev->mac_u32 = mac_u32;
    p_new_dev->rssi = rssi;
    memcpy(p_new_dev->mac, p_mac, 6);
    memcpy(p_new_dev->dev_name, p_dev_name, dev_name_length);
    p_new_dev->dev_name_len = dev_name_length;

    int dev_num = _lv_ll_get_len(&s_bt_scan_dev_ll);
    if (dev_num > BT_SCAN_DEV_MAX_NUM)
    {
        gx_bt_scan_dev_t *p_tail = _lv_ll_get_tail(&s_bt_scan_dev_ll);
        _lv_ll_remove(&s_bt_scan_dev_ll, p_tail);
    }
}

static void bt_scan_dev_ll_delete(void)
{
    _lv_ll_clear(&s_bt_scan_dev_ll);
}

static lv_obj_t *_create_item_view(lv_obj_t *obj, void *user_data, void *p_item_holder)
{
    my_holder_t *my_holder = (my_holder_t *)p_item_holder;
    lv_obj_t *p_item_view = lv_obj_create(obj);
    lv_obj_add_style(p_item_view, (lv_style_t *)&BT_SCAN_LIST_ITEM_STYLE, 0);
    lv_obj_add_flag(p_item_view, LV_OBJ_FLAG_EVENT_BUBBLE);

    my_holder->p_dev_name = lv_label_create(p_item_view);
    lv_obj_set_width(my_holder->p_dev_name, LIST_ITEM_WIDTH - 40);
    lv_obj_add_style(my_holder->p_dev_name, (lv_style_t *)&BT_SCAN_LIST_ITEM_TITLE_STYLE, 0);
    lv_obj_align(my_holder->p_dev_name, LV_ALIGN_LEFT_MID, 10, 0);

    my_holder->p_rssi = lv_label_create(p_item_view);
    lv_obj_set_width(my_holder->p_rssi, 40);
    lv_obj_add_style(my_holder->p_rssi, (lv_style_t *)&BT_SCAN_LIST_ITEM_TITLE_STYLE, 0);
    lv_obj_align(my_holder->p_rssi, LV_ALIGN_RIGHT_MID, -10, 0);

    return p_item_view;
}

static void _bind_item_view(lv_obj_t *obj, void *user_data, void *p_item_holder, uint32_t index, lv_obj_t *p_resued_item_obj, bool bindOrUnbind)
{
    my_holder_t *my_holder = (my_holder_t *)p_item_holder;

    gx_bt_scan_dev_t *dev;
    int ll_idx = 0;
    _LV_LL_READ(&s_bt_scan_dev_ll, dev)
    {
        if (ll_idx == index)
        {
            lv_label_set_text_fmt(my_holder->p_dev_name, "%s", dev->dev_name);
            lv_label_set_text_fmt(my_holder->p_rssi, "%d", dev->rssi);
            memcpy(my_holder->mac_addr, &dev->mac, 6);
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
        bt_scan_dev_ll_node_insert(p_ctx->p_dev_name, p_ctx->dev_name_len, p_ctx->p_mac, p_ctx->rssi);
        int item_num = _lv_ll_get_len(&s_bt_scan_dev_ll);
        lv_virtual_list_update_data(p_list, NULL, item_num);
    }
}

static void _item_clicked_cb(lv_obj_t *obj, void *user_data, void *p_widget_holder, uint32_t index, lv_event_t *evt)
{
    bt_dev_info_t bt_dev_info;
    my_holder_t *my_holder = (my_holder_t *)p_widget_holder;
    char *text = ((lv_label_t *)my_holder->p_dev_name)->text;

    memcpy(bt_dev_info.dev_name, text, strlen(text) + 1);
    memcpy(&bt_dev_info.mac, my_holder->mac_addr, 6);

    if (evt->code == LV_EVENT_CLICKED)
    {
        lv_layout_router_show_bt_pair_dev(obj->parent, (void *)&bt_dev_info);
    }
}

static void bt_scan_dev_event_cb(lv_event_t *e)
{
    if (e->code == LV_EVENT_DELETE)
    {
    }
    else if (e->code == LV_EVENT_READY)
    {
        bt_conn_controller_set_ctx_listener(bt_conn_ctx_listener, NULL);
        bt_api_device_inquiry_start();
        is_scanning = true;
    }
    else if (e->code == LV_EVENT_CANCEL)
    {
        bt_conn_controller_set_ctx_listener(NULL, NULL);
        if (is_scanning)
        {
            bt_api_device_inquiry_stop();
            is_scanning = false;
        }
        bt_scan_dev_ll_delete();
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
lv_obj_t *lv_bt_scan_dev_layout_create(lv_obj_t *parent_tv_obj)
{
#if LV_GDX_PATCH_DISABLE_STYLE_REFRESH && !LV_GDX_PATCH_USE_FAST_TILEVIEW
    lv_obj_enable_style_refresh(false);
    lv_disp_t *disp = lv_obj_get_disp(parent_tv_obj);
    lv_disp_enable_invalidation(disp, false);
#endif

    lv_obj_t *p_window = lv_obj_create(parent_tv_obj);
    lv_obj_set_size(p_window, DISP_HOR_RES, DISP_VER_RES);
    lv_obj_add_style(p_window, (lv_style_t *)&BT_SCAN_WINDOW_STYLE, 0);

    // Title
    lv_obj_t *p_title = lv_label_create(p_window);
    lv_obj_add_style(p_title, (lv_style_t *)&BT_SCAN_TITLE_STYLE, 0);
    lv_label_set_text_fmt(p_title, "%s", "Scan Device");

    // BT device name label
    lv_obj_t *p_bt_name_label = lv_label_create(p_window);
    lv_obj_set_size(p_bt_name_label, 100, 20);
    lv_obj_add_style(p_bt_name_label, (lv_style_t *)&BT_SCAN_LABEL_STYLE, 0);
    lv_label_set_text(p_bt_name_label, "BT Name:");
    lv_obj_set_pos(p_bt_name_label, 56, 90);

    s_dev_name = lv_label_create(p_window);
    lv_obj_set_size(s_dev_name, 120, 20);
    lv_obj_add_style(s_dev_name, (lv_style_t *)&BT_SCAN_LABEL_STYLE, 0);
    lv_label_set_text(s_dev_name, "xxxxxxxx");
    lv_obj_align_to(s_dev_name, p_bt_name_label, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

    // Available device label
    lv_obj_t *p_scan_label = lv_label_create(p_window);
    lv_obj_set_size(p_scan_label, 170, 20);
    lv_obj_add_style(p_scan_label, (lv_style_t *)&BT_SCAN_LABEL_STYLE, 0);
    lv_label_set_text(p_scan_label, "Available device:");
    lv_obj_set_pos(p_scan_label, 56, 120);

    bt_scan_dev_ll_init();

    // Scan device list
    p_list = lv_virtual_list_create(p_window,
                                    LIST_ITEM_HEIGHT,
                                    sizeof(my_holder_t),
                                    _create_item_view,
                                    _bind_item_view);
    lv_obj_add_style(p_list, (lv_style_t *)&BT_SCAN_LIST_BOX_STYLE, 0);
    lv_obj_align(p_list, LV_ALIGN_CENTER, 0, 40);
    lv_virtual_list_set_padding_space(p_list, 0, 0, 10);
    lv_virtual_list_set_item_clicked_cb(p_list, _item_clicked_cb);

#if LV_GDX_PATCH_USE_FAST_TILEVIEW
    lv_obj_add_event_cb(p_window, bt_scan_dev_event_cb, LV_EVENT_ALL, NULL);
#endif

#if LV_GDX_PATCH_DISABLE_STYLE_REFRESH && !LV_GDX_PATCH_USE_FAST_TILEVIEW
    lv_disp_enable_invalidation(disp, true);
    lv_obj_enable_style_refresh(true);
    lv_obj_refresh_style(p_window, LV_PART_ANY, LV_STYLE_PROP_ANY);
#endif

    return p_window;
}
