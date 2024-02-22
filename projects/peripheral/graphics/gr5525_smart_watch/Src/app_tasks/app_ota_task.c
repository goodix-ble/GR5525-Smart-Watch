#include "FreeRTOS.h"
#include "task.h"
#include "dfu_port.h"
#include "flash_driver.h"
#include "bt_api.h"
#include "app_log.h"

#define DFU_DATA_SAVE_ADDR (0x00240000)

static float ble_ota_progress = 0.0f;
static bool s_dfu_scheduler_running = false;

static void dfu_program_start_callback(void);
static void dfu_programing_callback(uint8_t pro);
static void dfu_program_end_callback(uint8_t status);

static dfu_pro_callback_t dfu_pro_call =
{
    .dfu_program_start_callback = dfu_program_start_callback,
    .dfu_programing_callback = dfu_programing_callback,
    .dfu_program_end_callback = dfu_program_end_callback,
};

static void dfu_program_start_callback(void)
{
    APP_LOG_DEBUG("Dfu start program");
}

static void dfu_programing_callback(uint8_t pro)
{
    APP_LOG_DEBUG("Dfu programing---%d%%", pro);
    ble_ota_progress = pro;
}

static void dfu_program_end_callback(uint8_t status)
{
    APP_LOG_DEBUG("Dfu program end");
    if (0x01 == status)
    {
        APP_LOG_DEBUG("status: successful");
        // bt_api_start_ota(DFU_DATA_SAVE_ADDR, 512*1024);
    }
    else
    {
        APP_LOG_DEBUG("status: error");
    }
}

static void dfu_task(void *param)
{
    while (s_dfu_scheduler_running)
    {
        dfu_schedule();

        vTaskDelay(100); // 如果注释掉这一句，就会导致栈溢出。
    }

    APP_LOG_INFO("!!!!!!!!!!!! exit dfu !!!!!!!!!!!!\n");
    vTaskDelete(NULL);
}

void ota_start_task(void)
{
    APP_LOG_DEBUG("ota_start_task");
    bool exit = false;

    portENTER_CRITICAL();
    if (s_dfu_scheduler_running)
    {
        exit = true;
    }
    else
    {
        s_dfu_scheduler_running = true;
    }
    portEXIT_CRITICAL();

    if (exit)
    {
        return;
    }

    dfu_buffer_malloc(&dfu_pro_call);
    xTaskCreate(dfu_task, "dfu_task", 512, NULL, 2, NULL);
}

void ota_abort_task(void)
{
    APP_LOG_DEBUG("ota_abort_task");
    portENTER_CRITICAL();
    s_dfu_scheduler_running = false;
    portEXIT_CRITICAL();

    dfu_buffer_free(&dfu_pro_call);
    gx_flash_init();
}

dfu_pro_callback_t *dfu_pro_cb_get(void)
{
    return &dfu_pro_call;
}

float ble_ota_progress_get(void)
{
    return ble_ota_progress;
}



