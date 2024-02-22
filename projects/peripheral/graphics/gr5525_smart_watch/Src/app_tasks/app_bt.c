#include "app_bt.h"
#include "bt_ifce.h"
#include "lvgl.h"

#include "bt_music_controller.h"
#include "bt_phonecall_controller.h"
#include "bt_conn_controller.h"

#include "FreeRTOS.h"
#include "task.h"

#if CFG_LPCLK_INTERNAL_EN == 0
#error "GR5625 MUST USE INTERNAL OSC AS LPCLK!"
#endif // CFG_LPCLK_INTERNAL_EN == 0

#define ARRAY_SIZE(arr) (sizeof((arr)) / sizeof((arr[0])))

#define BT_TASK_BIT_DATA_AVIALABLE (1 << 0)

typedef struct
{
    bt_api_ind_type_t ind_type;
    bt_api_ind_callback_t cb;
    bt_api_msg_t *param;
} bt_api_ind_cb_ctx_t;

typedef struct
{
    bt_api_opcode_t resp_type;
    bt_api_resp_callback_t cb;
    bt_api_msg_t *param;
} bt_api_resp_cb_ctx_t;

static void ind_callback_async_wrapper(void *param);
static void resp_callback_async_wrapper(void *param);

static bt_api_msg_t *copy_msg(const bt_api_msg_t *orig);
static void free_copied_msg(bt_api_msg_t *copy);

static void app_bt_task(void *args);

static bt_api_resp_cb_ctx_t s_resp_callback[4];
static bt_api_ind_cb_ctx_t s_ind_callback[4];

static TaskHandle_t s_bt_task_handle;

int8_t bt_api_register_indication_callback(bt_api_ind_callback_t callback, bt_api_ind_type_t ind_type)
{
    for (int8_t i = 0; i < ARRAY_SIZE(s_ind_callback); i++)
    {
        if (s_ind_callback[i].cb == callback)
        {
            return i;
        }
    }

    for (int8_t i = 0; i < ARRAY_SIZE(s_ind_callback); i++)
    {
        if (!s_ind_callback[i].cb)
        {
            s_ind_callback[i].cb = callback;
            s_ind_callback[i].ind_type = ind_type;
            s_ind_callback[i].param = NULL;
            return i;
        }
    }

    return -1;
}

void bt_api_deregister_indication_callback(bt_api_ind_callback_t callback)
{
    for (int8_t i = 0; i < ARRAY_SIZE(s_ind_callback); i++)
    {
        if (s_ind_callback[i].cb == callback)
        {
            s_ind_callback[i].cb = NULL;
            return;
        }
    }
}

int8_t bt_api_register_response_callback(bt_api_resp_callback_t callback, bt_api_opcode_t resp_type)
{
    for (int8_t i = 0; i < ARRAY_SIZE(s_resp_callback); i++)
    {
        if (s_resp_callback[i].cb == callback)
        {
            return i;
        }
    }

    for (int8_t i = 0; i < ARRAY_SIZE(s_resp_callback); i++)
    {
        if (!s_resp_callback[i].cb)
        {
            s_resp_callback[i].cb = callback;
            s_resp_callback[i].resp_type = resp_type;
            s_resp_callback[i].param = NULL;
            return i;
        }
    }

    return -1;
}

void bt_api_deregister_response_callback(bt_api_resp_callback_t callback)
{
    for (int8_t i = 0; i < ARRAY_SIZE(s_resp_callback); i++)
    {
        if (s_resp_callback[i].cb == callback)
        {
            s_resp_callback[i].cb = NULL;
            return;
        }
    }
}

void app_create_bt_task(void)
{
    xTaskCreate(app_bt_task, "app_bt_task", 2048, NULL, configMAX_PRIORITIES - 1, &s_bt_task_handle);
}

void bt_api_on_msg_arrived(const bt_api_msg_t *msg)
{
    if (msg->cmd == BT_DATA_HEAD_ACK)
    {
        // Response
        bt_api_opcode_t resp_type = (bt_opcode_t)msg->opcode;
        for (int8_t i = 0; i < ARRAY_SIZE(s_resp_callback); i++)
        {
            if (s_resp_callback[i].cb)
            {
                if (s_resp_callback[i].resp_type == resp_type || s_resp_callback[i].resp_type == IND_ALL)
                {
                    bt_api_msg_t *msg_copy = copy_msg(msg);
                    lv_res_t res = lv_async_call(resp_callback_async_wrapper, (void *)msg_copy);
                }
            }
        }
    }
    else if (msg->cmd == BT_DATA_HEAD_IND)
    {
        // Indication
        bt_ind_type_t ind_type = (bt_ind_type_t)msg->opcode;
        for (int8_t i = 0; i < ARRAY_SIZE(s_ind_callback); i++)
        {
            if (s_ind_callback[i].cb)
            {
                if (s_ind_callback[i].ind_type == ind_type || s_ind_callback[i].ind_type == IND_ALL)
                {
                    bt_api_msg_t *msg_copy = copy_msg(msg);
                    lv_res_t res = lv_async_call(ind_callback_async_wrapper, (void *)msg_copy);
                }
            }
        }

        // Internal Indication Handler
        bt_music_controller_ind_handler(msg);
        bt_phonecall_controller_ind_handler(msg);
        bt_conn_controller_ind_handler(msg);
    }
}

void bt_ifce_data_available(void)
{
    if (__get_IPSR())
    {
        BaseType_t xHigherPriorityTaskWoken;
        xTaskNotifyFromISR(s_bt_task_handle, BT_TASK_BIT_DATA_AVIALABLE, eSetBits, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
    else
    {
        xTaskNotify(s_bt_task_handle, BT_TASK_BIT_DATA_AVIALABLE, eSetBits);
    }
}


static void ind_callback_async_wrapper(void *param)
{
    bt_api_msg_t *msg = (bt_api_msg_t *)param;
    bt_api_ind_type_t ind_type = (bt_api_ind_type_t)msg->opcode;
    for (int8_t i = 0; i < ARRAY_SIZE(s_ind_callback); i++)
    {
        if (s_ind_callback[i].cb)
        {
            if (s_ind_callback[i].ind_type == ind_type || s_ind_callback[i].ind_type == IND_ALL)
            {
                s_ind_callback[i].cb(msg);
                free_copied_msg(msg);
            }
        }
    }
}

static void resp_callback_async_wrapper(void *param)
{
    bt_api_msg_t *msg = (bt_api_msg_t *)param;
    bt_api_opcode_t resp_type = (bt_api_opcode_t)msg->opcode;
    for (int8_t i = 0; i < ARRAY_SIZE(s_resp_callback); i++)
    {
        if (s_resp_callback[i].cb)
        {
            if (s_resp_callback[i].resp_type == resp_type || s_resp_callback[i].resp_type == RESP_ALL)
            {
                s_resp_callback[i].cb((bt_resp_t)*msg->data.response);
                free_copied_msg(msg);
            }
        }
    }
}

static bt_api_msg_t *copy_msg(const bt_api_msg_t *orig)
{
    bt_api_msg_t *copy = pvPortMalloc(sizeof(bt_api_msg_t) + orig->length);
    memcpy(copy, orig, sizeof(bt_api_msg_t));
    copy->data.data = (uint8_t *)&copy[1];
    memcpy(copy->data.data, orig->data.data, orig->length);
    return copy;
}

static void free_copied_msg(bt_api_msg_t *copy)
{
    vPortFree(copy);
}

static void app_bt_task(void *args)
{
    (void)args;
    uint32_t flags = 0;
    while(1)
    {
        bt_api_msg_handler();
        xTaskNotifyWait(0, 0xFFFFFFFF, &flags, portMAX_DELAY);
    }
}

