#include "bt_conn_controller.h"
#include "lvgl.h"
#include "FreeRTOS.h"
#include "app_log.h"

static bt_conn_ctx_t s_bt_conn_ctx = {
    .p_dev_name = NULL,
    .p_mac = NULL,
    .rssi = 0,
    .conn_state = 0,
    .dev_name_len = 0,
    .connected_dev_mac = NULL,
};

static bt_conn_ctx_listener_t s_bt_conn_ctx_listener = NULL;
static void *s_conn_user_data = NULL;

static void copy_dev_name(char *p_dest, int max_len, bt_api_device_name_t *p_dev_name, int dev_name_len)
{
    max_len--;
    char *p_dev = p_dev_name;
    for (int i = 0; i < dev_name_len; i++)
    {
        if (i > max_len)
        {
            break;
        }
        *(p_dest++) = p_dev[i];
    }
    *p_dest = '\0';
}

static void listener_async_call_wrapper(void *user_data)
{
    LV_UNUSED(user_data);
    if (s_bt_conn_ctx_listener)
    {
        s_bt_conn_ctx_listener(&s_bt_conn_ctx, s_conn_user_data);
    }
}

void bt_conn_controller_set_ctx_listener(bt_conn_ctx_listener_t listener, void *user_data)
{
    s_bt_conn_ctx_listener = listener;
    s_conn_user_data = user_data;
}

bt_conn_ctx_t *bt_conn_controller_get_ctx(void)
{
    return &s_bt_conn_ctx;
}

void bt_conn_controller_ind_handler(const bt_api_msg_t *p_rx_info)
{
    uint8_t opcode = p_rx_info->opcode;

    switch (opcode)
    {
    case IND_INQUIRY_INFO:
    {
        // device name
        int dev_name_len = p_rx_info->length - 10;

        if (s_bt_conn_ctx.p_dev_name)
        {
            vPortFree(s_bt_conn_ctx.p_dev_name);
            s_bt_conn_ctx.p_dev_name = NULL;
        }

        s_bt_conn_ctx.p_dev_name = pvPortMalloc(BT_DEV_NAME_MAX_LEN);
        if (dev_name_len == 0)
        {
            snprintf(s_bt_conn_ctx.p_dev_name, BT_DEV_NAME_MAX_LEN, "N/A\n%02X:%02X:%02X:%02X:%02X:%02X",
                     p_rx_info->data.indication->inquiry_info.bd_addr.addr[5],
                     p_rx_info->data.indication->inquiry_info.bd_addr.addr[4],
                     p_rx_info->data.indication->inquiry_info.bd_addr.addr[3],
                     p_rx_info->data.indication->inquiry_info.bd_addr.addr[2],
                     p_rx_info->data.indication->inquiry_info.bd_addr.addr[1],
                     p_rx_info->data.indication->inquiry_info.bd_addr.addr[0]);
        }
        else
        {
            copy_dev_name(s_bt_conn_ctx.p_dev_name, BT_DEV_NAME_MAX_LEN, &p_rx_info->data.indication->inquiry_info.device_name, dev_name_len);
        }
        s_bt_conn_ctx.dev_name_len = strlen(s_bt_conn_ctx.p_dev_name) + 1;

        // mac
        if (s_bt_conn_ctx.p_mac)
        {
            vPortFree(s_bt_conn_ctx.p_mac);
            s_bt_conn_ctx.p_mac = NULL;
        }
        s_bt_conn_ctx.p_mac = pvPortMalloc(6);
        memcpy(s_bt_conn_ctx.p_mac, p_rx_info->data.indication->inquiry_info.bd_addr.addr, 6);

        // rssi
        s_bt_conn_ctx.rssi = p_rx_info->data.indication->inquiry_info.rssi;

        if (s_bt_conn_ctx_listener)
        {
            lv_async_call(listener_async_call_wrapper, NULL);
        }
    }
    break;

    case IND_PAIR_DEV_INFO:
    {
        int dev_name_len = p_rx_info->length - 10;
        char *cc = &p_rx_info->data.indication->pair_dev_info.dev_name;
        cc[dev_name_len] = 0;
        APP_LOG_DEBUG("dev name: %s", cc);

        // device name
        if (s_bt_conn_ctx.p_dev_name)
        {
            vPortFree(s_bt_conn_ctx.p_dev_name);
            s_bt_conn_ctx.p_dev_name = NULL;
        }
        s_bt_conn_ctx.p_dev_name = pvPortMalloc(BT_DEV_NAME_MAX_LEN);

        if (dev_name_len == 0)
        {
            snprintf(s_bt_conn_ctx.p_dev_name, BT_DEV_NAME_MAX_LEN, "N/A\n%02X:%02X:%02X:%02X:%02X:%02X",
                     p_rx_info->data.indication->pair_dev_info.dev_addr.addr[5],
                     p_rx_info->data.indication->pair_dev_info.dev_addr.addr[4],
                     p_rx_info->data.indication->pair_dev_info.dev_addr.addr[3],
                     p_rx_info->data.indication->pair_dev_info.dev_addr.addr[2],
                     p_rx_info->data.indication->pair_dev_info.dev_addr.addr[1],
                     p_rx_info->data.indication->pair_dev_info.dev_addr.addr[0]);
        }
        else
        {
            copy_dev_name(s_bt_conn_ctx.p_dev_name, BT_DEV_NAME_MAX_LEN, &p_rx_info->data.indication->pair_dev_info.dev_name, dev_name_len);
        }
        s_bt_conn_ctx.dev_name_len = strlen(s_bt_conn_ctx.p_dev_name) + 1;

        // mac
        if (s_bt_conn_ctx.p_mac)
        {
            vPortFree(s_bt_conn_ctx.p_mac);
            s_bt_conn_ctx.p_mac = NULL;
        }
        s_bt_conn_ctx.p_mac = pvPortMalloc(6);
        memcpy(s_bt_conn_ctx.p_mac, p_rx_info->data.indication->pair_dev_info.dev_addr.addr, 6);

        if (s_bt_conn_ctx_listener)
        {
            lv_async_call(listener_async_call_wrapper, NULL);
        }
    }
    break;

    case IND_CONNECTION_STATE:
    {
        bt_api_ind_connection_state_t *p_state = &p_rx_info->data.indication->connection_state;
        // connection status
        s_bt_conn_ctx.conn_state = p_state->status;

        // connected dev mac
        if (BT_API_CONN_STATE_CONNECTED == s_bt_conn_ctx.conn_state)
        {
            if (s_bt_conn_ctx.connected_dev_mac)
            {
                vPortFree(s_bt_conn_ctx.connected_dev_mac);
                s_bt_conn_ctx.connected_dev_mac = NULL;
            }
            s_bt_conn_ctx.connected_dev_mac = pvPortMalloc(6);
            memcpy(s_bt_conn_ctx.connected_dev_mac, p_state->bd_addr.addr, 6);
        }
        else if (BT_API_CONN_STATE_DISCONNECTED == s_bt_conn_ctx.conn_state)
        {
            vPortFree(s_bt_conn_ctx.connected_dev_mac);
            s_bt_conn_ctx.connected_dev_mac = NULL;
        }

        // mac
        if (s_bt_conn_ctx.p_mac)
        {
            vPortFree(s_bt_conn_ctx.p_mac);
            s_bt_conn_ctx.p_mac = NULL;
        }
        s_bt_conn_ctx.p_mac = pvPortMalloc(6);
        memcpy(s_bt_conn_ctx.p_mac, p_rx_info->data.indication->connection_state.bd_addr.addr, 6);

        if (s_bt_conn_ctx_listener)
        {
            lv_async_call(listener_async_call_wrapper, NULL);
        }
    }
    break;

    default:
        break;
    }
}

