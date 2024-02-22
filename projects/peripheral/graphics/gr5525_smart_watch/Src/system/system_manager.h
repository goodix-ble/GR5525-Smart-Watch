#ifndef __SYSTEM_MANAGER_H__
#define __SYSTEM_MANAGER_H__

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

typedef struct {
    TaskHandle_t gui_handle;
    TaskHandle_t indev_handle;
    TaskHandle_t gsensor_handle;
    TaskHandle_t gomore_data_handle;
    TaskHandle_t health_data_send_handle;
    TaskHandle_t gomore_ui_handle;
    TaskHandle_t battery_handle;
    TaskHandle_t ble_bt_ifce_handle;
} sys_task_handle_t;



typedef struct {
    SemaphoreHandle_t gui_refresh_sem;
    SemaphoreHandle_t display_sync_sem ;    /* used to sync the display-refresh */
    SemaphoreHandle_t dma_mem_xfer_sem;
} sys_semphr_handle_t;


typedef struct {
    bool                is_allow_hif_sleep;
} sys_param_t;

extern sys_task_handle_t        g_task_handle;
extern sys_semphr_handle_t      g_semphr;
extern volatile sys_param_t     g_sys_psram;


#define TASK_GUI_STACK_SIZE         (1024 * 2)
#define TASK_INDEV_STACK_SIZE       (512)
#define TASK_BLE_BT_IFCE_STACK_SIZE (1024 * 2)
#define TASK_BLE_BT_OTA_STACK_SIZE  (1024 * 4)


void sys_adjust_dig_core_voltage(uint32_t mV);

void sys_sem_init(SemaphoreHandle_t * sem);
void sys_sem_take(SemaphoreHandle_t sem, TickType_t xTicksToWait);
void sys_sem_give(SemaphoreHandle_t sem);

#endif /* __SYSTEM_MANAGER_H__ */
