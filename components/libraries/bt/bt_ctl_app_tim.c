/*
 * INCLUDE FILES
 *****************************************************************************************
 */
#include <stdio.h>
#include <string.h>
#include "app_log.h"
#include "board_SK.h"
#include "app_timer.h"
#include "app_error.h"
#include "bt_ctl_comm_interface.h"

#ifdef BLE_BT_OTA_SUPPORT
/*
 * DEFINES
 *****************************************************************************************
 */
static app_timer_id_t    ota_timer_id;
static app_timer_id_t    ota_dae_timer_id;
/*
 * GLOBAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */
extern volatile BT_OTA_STATUS_T bt_ota_state;
extern volatile BT_OTA_PROTECTFLASH_SUBSTATE_T bt_ota_protectflash_substate;
extern volatile BT_OTA_ERASEFLASH_SUBSTATE_T bt_ota_erase_substate;
extern uint32_t WATCH_DEMO_UART_BAUDRATE;
extern flashsr_op_t current_rd_flashsr;
extern flashsr_op_t current_wr_flashsr;
extern flasherase_op_t current_erase_flash;
extern flashcrc_op_t current_flash_crc;
extern uint32_t bt_upgrade_fw_size;
extern uint32_t bt_upgrade_fw_flash_addr;


volatile BT_OTA_STATUS_T last_bt_ota_state = BT_OTA_IDLE;
volatile uint32_t last_ota_addr = 0;
volatile bool retry_wr = false;
volatile uint8_t retry_cnt = 3;
volatile uint8_t retry_upgrade = 3;
extern void app_bt_ota_env_deinit(void );

/*
 * GLOBAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */
static void app_ota_tim_event_handler(void* p_arg)
{
    app_bt_ota_env_deinit();
    app_timer_stop(ota_timer_id);
    app_timer_stop(ota_dae_timer_id);

    APP_LOG_DEBUG(" BT FW upgrade timeout, reset device !!!!!!!!");
}

static void app_ota_daemon_event_handler(void* p_arg)
{
    APP_LOG_DEBUG("bt_ota_state=%d, erase_substate=%d, 0x%08x ",
                                bt_ota_state,
                                bt_ota_erase_substate,
                                current_erase_flash.addr);

    if (bt_ota_state == BT_OTA_OP_WRITEFLASH)
    {
        if (last_ota_addr == current_erase_flash.addr)//maybe block here, re-write this addr
        {
            if (bt_ota_erase_substate == BT_OTA_OPFLASH_WRITE1SEC)
            {
                retry_wr = true;
            }
        }
        last_ota_addr = current_erase_flash.addr;
    }
    else if ((last_bt_ota_state != BT_OTA_IDLE)
                    && (last_bt_ota_state == bt_ota_state)
                    && (retry_upgrade > 0))
    {
        retry_upgrade --;
        app_bt_ota_env_deinit();
        app_bt_ota_procedure_begin(bt_upgrade_fw_flash_addr, bt_upgrade_fw_size);
    }
    last_bt_ota_state = bt_ota_state;
}

void app_ota_prt_tim_interrupt(void)
{
    sdk_err_t error_code;
    error_code = app_timer_start(ota_dae_timer_id, 2000, NULL);
    if (BLE_SUCCESS != error_code)
    {
        APP_LOG_DEBUG("error_code = 0x%02x", error_code);
    }
}

void app_ota_tim_interrupt(void)
{
    sdk_err_t error_code;
    error_code = app_timer_start(ota_timer_id, WATCH_DEMO_OTA_TIMEOUT, NULL);
    if (BLE_SUCCESS != error_code)
    {
        APP_LOG_DEBUG("error_code = 0x%02x", error_code);
    }
}

void app_ota_tim_init(void)
{
    sdk_err_t error_code;

    error_code = app_timer_create(&ota_timer_id, ATIMER_ONE_SHOT, app_ota_tim_event_handler);
    if (BLE_SUCCESS != error_code)
    {
        APP_LOG_DEBUG("error_code = 0x%02x", error_code);
    }
    
    error_code = app_timer_create(&ota_dae_timer_id, ATIMER_REPEAT, app_ota_daemon_event_handler);
    if (BLE_SUCCESS != error_code)
    {
        APP_LOG_DEBUG("error_code = 0x%02x", error_code);
    }
}

void app_ota_tim_stop(void)
{
    app_timer_stop(ota_timer_id);
    app_timer_stop(ota_dae_timer_id);
}
#endif

