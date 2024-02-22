#include "system_manager.h"
#include "app_drv_config.h"

#if (APP_DRIVER_CHIP_TYPE == APP_DRIVER_GR5526X) 
typedef struct
{
    uint16_t    dcdc_mv;     /**< dcdc vout mv. */
    uint16_t    dcore_mv;    /**< dcore vout mv. */
    uint16_t    dcdc_vout;   /**< dcdc vout code. */
    uint16_t    dcore_vout;  /**< dcore vout code. */
    bool        dcore_bypass_en; /**< dcore bypass enable. */
}dcdc_dcore_info_t;


extern void     sys_pmu_dcdc_set(uint32_t dcdc_96m_mv, uint32_t dcdc_64m_mv);
extern void     sys_pmu_digcore_set(uint32_t dcore_96m_mv, uint32_t dcore_64m_mv);
extern void     pmu_dcdc_dcore_info_record(dcdc_dcore_info_t* p_dcdc_dcore);
extern          dcdc_dcore_info_t g_dcdc_dcore_set_for_gpu;
#endif

sys_task_handle_t g_task_handle = {
    .gui_handle           = NULL,
    .indev_handle         = NULL,
    .gsensor_handle       = NULL,
    .gomore_data_handle   = NULL,
    .gomore_ui_handle     = NULL,
    .battery_handle       = NULL,
    .ble_bt_ifce_handle   = NULL,
};

sys_semphr_handle_t      g_semphr = {
    .gui_refresh_sem      = NULL,
    .display_sync_sem     = NULL,
    .dma_mem_xfer_sem     = NULL,
};

volatile sys_param_t g_sys_psram ;


void sys_adjust_dig_core_voltage(uint32_t mV) {
#if (APP_DRIVER_CHIP_TYPE == APP_DRIVER_GR5526X) 
    sys_pmu_dcdc_set(mV /* 96MHz */, mV /* 64MHz */);
    sys_pmu_digcore_set(mV/* 96MHz */, mV /* 64MHz */);
    pmu_dcdc_dcore_info_record(&g_dcdc_dcore_set_for_gpu);
#endif
}


void sys_sem_init(SemaphoreHandle_t * sem) {
    * sem = xSemaphoreCreateBinary();
}

void sys_sem_take(SemaphoreHandle_t sem, TickType_t xTicksToWait) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if(sem != NULL) {
        if (__get_IPSR())
        {
            xSemaphoreTakeFromISR(sem, &xHigherPriorityTaskWoken);
        } else {
            xSemaphoreTake(sem, xTicksToWait);
        }
    }
}

void sys_sem_give(SemaphoreHandle_t sem) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if(sem != NULL) {
        if (__get_IPSR())
        {
            xSemaphoreGiveFromISR(sem, &xHigherPriorityTaskWoken);
        } else {
            xSemaphoreGive(sem);
        }
    }
}
