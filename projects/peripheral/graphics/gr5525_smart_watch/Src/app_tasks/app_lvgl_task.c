/*
 * INCLUDE FILES
 *****************************************************************************************
 */
#include "user_periph_setup.h"
#include "scatter_common.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"
#include "grx_sys.h"
#include "bsp_tp.h"
#include "app_log.h"
#include "app_io.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "platform_sdk.h"
#include "app_rtc.h"
#include "display_crtl_drv.h"
#include "app_key.h"
#include "system_manager.h"
#include "qspi_flash.h"
#include "app_qspi.h"

/*
 * MACRO DEFINITIONS
 *****************************************************************************************
 */
#define LV_MEM_USED_MONITOR (0)

/*
 * Flash Device Setting
 */
#define Q_NOR_FLASH_QSPI_ID APP_QSPI_ID_0
#define Q_NOR_FLASH_CLOCK_PREESCALER 2u
#define Q_NOR_FLASH_PIN_GROUP QSPI0_PIN_GROUP_0

/*
 * GLOBAL PMU FUNCTION DECLARATIONS
 *****************************************************************************************
 */

/*
 * GLOBAL GUI OBJECT DECLARATIONS
 *****************************************************************************************
 */
extern lv_indev_t *indev_touchpad;
extern void lv_home_layout_init(void);

/*
 * LOCAL MACRO DEFINITIONS
 *****************************************************************************************
 */

/*
 * LOCAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */
static bool lv_env_is_inited = false;

extern void lv_layout_startup(void);

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */
static void lvgl_env_init(void)
{
    sys_adjust_dig_core_voltage(1120);

    lv_init();
    lv_port_disp_init();
    lv_port_indev_init();
    lv_env_is_inited = true;

    app_rtc_init(NULL);
    sys_sem_init(&g_semphr.gui_refresh_sem);
}

/*
 * GLOBAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
void gui_schedule_task(void *p_arg)
{
    lvgl_env_init();
    uint32_t delayTime = 0;

    lv_layout_startup();

    while (1)
    {
        delayTime = lv_task_handler();
        // sys_sem_take(g_semphr.gui_refresh_sem, delayTime);
        vTaskDelay(delayTime);
    }
}

extern void handle_uart_ring_buffer_data(void);
void lv_indev_read_task(void *args)
{
    while (1)
    {
        if (lv_env_is_inited)
        {
            touchpad_indev_cache();
        }
        sys_sem_give(g_semphr.gui_refresh_sem);
        {
            vTaskDelay(LV_INDEV_DEF_READ_PERIOD);
        }
    }
}

void lvgl_mem_used_dump(const char *func, int line)
{
#if LV_MEM_USED_MONITOR
    static uint32_t _lv_mem_used_size = 0;
    static uint32_t _lv_mem_max_size = 0;
    _lv_mem_used_size = lv_mem_used_size_get();
    _lv_mem_max_size = LV_MAX(_lv_mem_used_size, _lv_mem_max_size);
    printf("LVGL View: %s, Line: %d, used size: %d, max size: %d\n",
           func, line, _lv_mem_used_size, _lv_mem_max_size);
#endif
}

/**
 *****************************************************************************************
 * @brief To create two task, the one is ble-schedule, another is watcher task
 *****************************************************************************************
 */
void app_create_lvgl_task(void)
{
    xTaskCreate(lv_indev_read_task, "indev_task", TASK_INDEV_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, &g_task_handle.indev_handle);
    xTaskCreate(gui_schedule_task, "gui_task", TASK_GUI_STACK_SIZE, NULL, configMAX_PRIORITIES - 2, &g_task_handle.gui_handle);
}
