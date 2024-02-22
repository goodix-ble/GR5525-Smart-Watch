#include "lv_general_list_generator.h"
#include "lv_img_dsc_list.h"
#include "lv_layout_router.h"
#include "bt_conn_controller.h"

// static bool is_connected = false;

#if LV_GDX_PATCH_USE_FAST_TILEVIEW
static const general_list_item_t BT_DEVICE_LIST_ITEMS[] = {
    {"Phone Call", &wd_img_bt_device_phonecall},
    {"Music", &wd_img_bt_device_music},
    {"BT Manage", &wd_img_bt_device_management},
};

static void list_item_click_event_cb(uint16_t idx, lv_event_t *event)
{
    switch (idx)
    {
    case 0:
        printf("%s\r\n", BT_DEVICE_LIST_ITEMS[0].title);
        if (bt_conn_controller_get_ctx()->conn_state == BT_API_CONN_STATE_CONNECTED)
        {
            lv_layout_router_show_bt_dial(event->current_target);
        }
        else
        {
            lv_layout_router_show_bt_no_entry(event->current_target);
        }
        break;

    case 1:
        printf("%s\r\n", BT_DEVICE_LIST_ITEMS[1].title);
        if (bt_conn_controller_get_ctx()->conn_state == BT_API_CONN_STATE_CONNECTED)
        {
            lv_layout_router_show_bt_music(event->current_target);
        }
        else
        {
            lv_layout_router_show_bt_no_entry(event->current_target);
        }
        break;

    case 2:
        printf("%s\r\n", BT_DEVICE_LIST_ITEMS[2].title);
        lv_layout_router_show_bt_manage(event->current_target);
    }
}

// static void bt_conn_ctx_listener(bt_conn_ctx_t *p_ctx, void *user_data)
// {
//     if (p_ctx)
//     {
//         if (BT_API_CONN_STATE_CONNECTED == p_ctx->conn_state)
//         {
//             is_connected = true;
//         }
//         else
//         {
//             is_connected = false;
//         }
//     }
// }

static void bt_device_event_cb(lv_event_t *e)
{
    if (e->code == LV_EVENT_DELETE)
    {
    }
    else if (e->code == LV_EVENT_READY)
    {
        bt_api_conn_state_get();
        // bt_conn_controller_set_ctx_listener(bt_conn_ctx_listener, NULL);
    }
    else if (e->code == LV_EVENT_CANCEL)
    {
        // bt_conn_controller_set_ctx_listener(NULL, NULL);
    }
}
#endif

lv_obj_t *lv_bt_device_layout_create(lv_obj_t *parent_tv_obj)
{
    general_list_dsc_t dsc = {
        .title = "Bluetooth",
        .items = BT_DEVICE_LIST_ITEMS,
        .item_num = LIST_SIZE(BT_DEVICE_LIST_ITEMS),
        .item_width = 220,
        .suffix = NULL,
        .callback = list_item_click_event_cb,
    };
    lv_obj_t *p_window = lv_general_list_create(parent_tv_obj, &dsc);

#if LV_GDX_PATCH_USE_FAST_TILEVIEW
    lv_obj_add_event_cb(p_window, bt_device_event_cb, LV_EVENT_ALL, NULL);
#endif

    return p_window;
}
