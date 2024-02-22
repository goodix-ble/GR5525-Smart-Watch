/**
 ****************************************************************************************
 *
 * @file bt_ctl_comm_interface.h
 *
 * @brief Header file - bluetooth BR/EDR control interface
 *
 ****************************************************************************************
 * @attention
  #####Copyright (c) 2019 GOODIX
  All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
  * Neither the name of GOODIX nor the names of its contributors may be used
    to endorse or promote products derived from this software without
    specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************************
 */

 #ifndef _BT_CTL_COMM_INTERFACE_H_
#define _BT_CTL_COMM_INTERFACE_H_

#include "gr_includes.h"

#define SUM_CALCULATE       1

#define BLE_BT_OTA_SUPPORT

/*
* if define true , will use 4K bytes per packet to wirte BT flash when upgrade BT FW.
* if define false, will use 256 bytes per packet to wirte BT flash when upgrade BT FW. This way will save the stack size.
*/
#define BT_UPGRADE_HIGH_SPEED   true

/* command is from ble to control bt
         format : CMD_HEAD + OPCODE + LEN  + CHECKSUM
*/
/* indicate is from bt to bt, notify command response.
         format : CMD_ACK_HEAD + OPCODE  + LEN + CHECKSUM
*/
/* indicate is from bt to bt, notify command response.
         format : IND_HEAD + OPCODE  + LEN + CHECKSUM
*/
#define BT_CTL_CMD_HEAD            0xABBA
#define BT_CTL_CMD_ACK_HEAD        0xAABB
#define BT_CTL_IND_HEAD            0xCDDC

#define BT_CTL_HEAD_LEN            2
#define BT_CTL_OPCODE_LEN          1
#define BT_CTL_DATA_LEN            2

#if SUM_CALCULATE
#define BT_CTL_CHECKSUM_LEN        1
#else
#define BT_CTL_CHECKSUM_LEN        0
#endif

#define BT_CTL_DATA_INDEX          (BT_CTL_HEAD_LEN+BT_CTL_OPCODE_LEN + BT_CTL_DATA_LEN)
#define BT_CTL_DATA_LEN_MIN        (BT_CTL_HEAD_LEN+BT_CTL_OPCODE_LEN + BT_CTL_DATA_LEN + BT_CTL_CHECKSUM_LEN)

#define BT_MAX_RX_LEN   200

#define WATCH_DEMO_OTA_TIMEOUT           (150 * 1000) /**< ota timeout (in unit of 1 ms). */


#define CHIP_KEYWORD_ADDR 0x00000110
#define FLASH_PROGRAM_START_ADDR            (0x00250000ul)

#define BLE_BT_TEST_MODE_L_LEN    0x64
#define BLE_BT_TEST_MODE_S_LEN    0x1E

// BT PIN is GPIO7<-->P11
#define BLE_WAKEUP_GPIO_TYPE    APP_IO_TYPE_AON
#define BLE_WAKEUP_GPIO_PIN     APP_IO_PIN_7

// BT PIN is GPIO14<-->P21
#define BT_WAKEUP_GPIO_TYPE    APP_IO_TYPE_GPIOB
#define BT_WAKEUP_GPIO_PIN     APP_IO_PIN_11

// Wakeup test PIN is GPIO14<-->P21
#define WAKEUP_OUT_GPIO_TYPE    APP_IO_TYPE_GPIOA
#define WAKEUP_OUT_GPIO_PIN     APP_IO_PIN_12

#define BT_RESET_GPIO_TYPE    APP_IO_TYPE_GPIOB
#define BT_RESET_GPIO_PIN     APP_IO_PIN_12

/*
 * ENUMERATIONS
 *****************************************************************************************
 */
typedef enum {
    RX_HEADER = 0,
    RX_OPCODE,
    RX_DATA_LENTH,
    RX_DATA,
#if SUM_CALCULATE
    RX_SUM,
#endif
    RX_CMP,
}RX_STATUS_T;

typedef enum {
    /* Audio play control opcode */
    OPCODE_A2DP_STATUS_SET,
    OPCODE_PLAY_STATUS_SET,
    OPCODE_PLAY_NEXT,
    OPCODE_PLAY_PREV,
    OPCODE_PLAY_REWIND,
    OPCODE_PLAY_FORWARD,
    OPCODE_AUDIO_VOL_SET,
    OPCODE_AUDIO_VOL_GET,
    OPCODE_VOL_GAIN_SET,
    OPCODE_VOL_GAIN_GET,
    OPCODE_AUDIO_INFO_GET,
    OPCODE_AUDIO_WORDS_GET ,//0x0B

    /* phone call control opcode */
    OPCODE_CALL_ADDR_BOOK_GET,
    OPCODE_CALL_ACCEPT,
    OPCODE_CALL_DTMF_SET,
    OPCODE_CALL_END,
    OPCODE_CALL_REJECT,
    OPCODE_CALL_REDIAL,
    OPCODE_CALL_VOL_SET,
    OPCODE_CALL_VOL_GET,
    OPCODE_CALL_VOL_GAIN_SET,
    OPCODE_CALL_VOL_GAIN_GET,
    OPCODE_CALL_VOL_SWITCH,
    OPCODE_CALL_HOLD_FIR_ACCEPT_SEC,
    OPCODE_CALL_END_FIR_ACCEPT_SEC,
    OPCODE_CALL_HOLD_FIR_REJECT_SEC,
    OPCODE_CALL_LINE_SWITCH,
    OPCODE_CALL_NUMBER_GET,
    OPCODE_CALL_DIAL_NUMBER,
    OPCODE_CALL_RING_STATE_SET,
    OPCODE_CALL_RING_STATE_GET,//0x1e

    /* bt common control opcode */
    OPCODE_BT_RESET,//0x1f
    OPCODE_BT_INQUIRY,
    OPCODE_BT_INQUIRY_SCAN,
    OPCODE_BT_CONNECT,
    OPCODE_BT_CONNECT_ABORT,
    OPCODE_BT_PAGE_SCAN,
    OPCODE_BT_PAIRING_BEGIN,
    OPCODE_BT_QUICK_CONNECT,
    OPCODE_BT_DISCONNECT,
    OPCODE_BT_SWITCH_PHONE,
    OPCODE_BT_PAIRING_INFO_STORE,
    OPCODE_BT_PAIRING_INFO_CLEAN,
    OPCODE_BT_ACTIVE_TIMEOUT_SET,
    OPCODE_BT_SNIFF_INTERVAL_SET,
    OPCODE_BT_QUICK_CON_TIMES,
    OPCODE_BT_QUICK_CON_MODE_SET,
    OPCODE_BT_BD_ADDR_GET,//0x2F
    OPCODE_BT_DEVICE_NAME_SET,//0x30
    OPCODE_BT_DEVICE_NAME_GET,

    OPCODE_BT_THRPUT_BEGIN,
    OPCODE_BT_THRPUT_DATA,
    OPCODE_BT_THRPUT_END,

    OPCODE_BT_FW_VERSION_GET,//0x35
    OPCODE_BT_OTA_BEGIN,

    OPCODE_BT_SIRI_WAKE,//0x37
    OPCODE_BT_SWITCH_NOTICE,
    OPCODE_BT_MIC_MUTE,
    OPCODE_VOL_P,
    OPCODE_VOL_M,
    OPCODE_VOL_MUTE,
    OPCODE_BT_MIC_GAIN_SET,
    OPCODE_BT_MIC_GAIN_GET,
    OPCODE_BT_MIC_TEST_START,
    OPCODE_BT_MIC_TEST_STOP,//0x40
    OPCODE_BT_MIC_AUTO_TEST,
    OPCODE_BT_CUSTOM_NOTICE_PLAY,
    OPCODE_BT_CUSTOM_NOTICE_PAUSE,

    OPCODE_BT_UART_MAX_LENGTH,
    OPCODE_BT_FACTORY_RESET,//0x45
    OPCODE_BT_POWERDOWN,
    OPCODE_BT_WAKEUP,
    OPCODE_BT_PAIR_DEV_LIST_GET,//0x48
    OPCODE_BT_CALL_OFFSET_SET,//0x49
    OPCODE_BT_CALL_OFFSET_GET,//0x4a
    OPCODE_BT_LOG_CTRL,
    OPCODE_BT_BD_ADDR_SET,
    OPCODE_BT_INQ_PAGE_GET,
    OPCODE_BT_CONN_STATE_GET,//0x4e

    OPCODE_BT_BATT_LV_SYNC,//0x4f

    OPCODE_CUR_VOL_GET,
    OPCODE_CUR_GAIN_GET,
    OPCODE_VOL_MUTE_GET = 0x52,
    OPCODE_MIC_MUTE_GET,

    OPCODE_DRAIN_VIBRA_START,//0x54
    OPCODE_DRAIN_VIBRA_STOP,//0x55
    OPCODE_SEARCH_DEV_START,//0x56
    OPCODE_SEARCH_DEV_STOP,//0x57

    OPCODE_BT_SIRI_SHUTDOWN,//0x58
    OPCODE_BT_SIRI_STATE_GET,//0x59

    OPCODE_BT_NOTICE_LVL_SET,//0x5A
    OPCODE_BT_NOTICE_LVL_GET,//0x5B

    OPCODE_BT_MAX,

    OPCODE_BT_TEST_CMD = 0xFF,//0xFF
}BT_CTL_OPCODE_T;

typedef enum {
    IND_PAIRING_INFO,
    IND_PROFILE_STATE,
    IND_INQUIRY_INFO,
    IND_CONNECTION_STATE,
    IND_BT_BD_ADDR,
    IND_BT_DEV_NAME,
    IND_THROUGHPUT_CMP,
    IND_FW_VERSION_INFO,
    IND_VOL_LEVEL_INFO,
    IND_MIC_GAIN_INFO,
    IND_ADDR_BOOK_INFO,//0x0a
    IND_CALL_LINE_STATE,
    IND_CALL_RING_STATE,
    IND_VOL_SITE_INFO,
    IND_AUDIO_SONG_INFO,
    IND_AUDIO_SONG_WORD_INFO,
    IND_AUDIO_STATE,//0x10
    IND_POWER_ON_STATE,
    IND_PAIR_DEV_INFO,
    IND_BT_CALL_OFFSET,
    IND_BT_LOG_STATE,
    IND_BT_INQ_PAGE_STATE,//inquiry(1 byte)+inquiry scan(1 byte)+page(1 byte)+page scan(1 byte)
    IND_BT_SNIFF_STATE,

    IND_HFP_VOL_INFO,//0x17  1 byte
    IND_AUDIO_VOL_INFO,//0x18  1 byte
    IND_HFP_GAIN_INFO,//0x19 1 byte
    IND_AUDIO_GAIN_INFO,//0x1A  1 byte
    IND_VOL_MUTE_INFO,//0x1B  1 byte
    IND_MIC_MUTE_INFO,//0x1C  1 byte

    IND_LOCAL_VOICE_STATE,//0x1D  1 byte
    IND_SIRI_STATE,//0x1E

    IND_NOTICE_LVL_STATE,//0x1F

    IND_MESSAGE_MAX,

    IND_BT_TEST_MSG = 0xFF,//0xFF
}BT_CTL_STATUS_T;

typedef enum {
    OPCODE_BT_NO_ERR,
    OPCODE_BT_INVALID_PARAM,
    OPCODE_BT_A2DP_NO_CONN_STATE,
    OPCODE_BT_HFP_NO_CONN_STATE,
    OPCODE_BT_NO_CALL_LINE,
    OPCODE_BT_NO_CONN_STATE,
    OPCODE_BT_PHONE_BOOK_ACCESS_DENIED,
    OPCODE_BT_ADDRESS_INEXISTENCE,
    OPCODE_BT_OP_CONDITION_NOT_MET,
    OPCODE_BT_IS_CONNECTTED,
    OPCODE_BT_NO_PAIR_RECORD,
    OPCODE_BT_EXIST_CALL_LINE,

    OPCODE_BT_ERR_CPDE_MAX,
}BT_CTL_OPCODE_ERR_CODE_T;

typedef enum
{
    BT_OTA_START,
    BT_OTA_RESET,
    BT_OTA_STAYROM,
    BT_OTA_STAYROM_RSP,
    BT_OTA_SETBR,
    BT_OTA_LINKCHECK,
    BT_OTA_LINKCHECK_RSP,
    BT_OTA_GETMID,
    BT_OTA_GETMID_RSP,
    BT_OTA_PROTECTFLASH,

    BT_OTA_OP_ERASEFLASH,
    BT_OTA_OP_WRITEFLASH,

    BT_OTA_WRITE_CHIP_NAME,
    BT_OTA_WRITE_CHIP_NAME_RSP,
    BT_OTA_WRITE_CHIP_NAME_RSP_E,

    BT_OTA_READCRC,
    BT_OTA_READCRC_RSP,
    BT_OTA_READCRC_RSP_E,

    BT_OTA_IDLE,
}BT_OTA_STATUS_T;

typedef enum
{
    BT_OTA_PROTECTFLASH_RDSR1,
    BT_OTA_PROTECTFLASH_RDSR1_RSP,
    BT_OTA_PROTECTFLASH_RDSR1_RSP_E,
    BT_OTA_PROTECTFLASH_WRSR1,
    BT_OTA_PROTECTFLASH_WRSR1_RSP,
    BT_OTA_PROTECTFLASH_WRSR1_RSP_E,
    BT_OTA_PROTECTFLASH_WRSR2,
    BT_OTA_PROTECTFLASH_WRSR2_RSP,
    BT_OTA_PROTECTFLASH_WRSR2_RSP_E,

    BT_OTA_PROTECTFLASH_IDLE,
}BT_OTA_PROTECTFLASH_SUBSTATE_T;

typedef enum
{
    BT_OTA_OPFLASH_EREASE,
    BT_OTA_OPFLASH_EREASE_RSP,
    BT_OTA_OPFLASH_EREASE_RSP_E,
    BT_OTA_OPFLASH_WRITE1SEC,
    BT_OTA_OPFLASH_WRITE1SEC_RSP,
    BT_OTA_OPFLASH_WRITE1SEC_RSP_E,
    BT_OTA_OPFLASH_IDLE,
}BT_OTA_ERASEFLASH_SUBSTATE_T;


typedef struct flashsr_op__t
{
    uint8_t regAddr;
    uint32_t val;
    uint8_t status;
}flashsr_op_t;

typedef struct flasherase_op__t
{
    uint32_t addr;
    uint32_t szCmd;
    uint8_t status;
}flasherase_op_t;

typedef struct flashcrc_op__t
{
    uint32_t saddr;
    uint32_t sz;
    uint32_t crc;
    uint32_t crc_me;
}flashcrc_op_t;

typedef struct
{
    uint16_t cmd;
    uint8_t opcode;
    uint16_t length;
    uint8_t *data;
} bt_rx_info;

typedef void (*app_bt_handler_func_t)(bt_rx_info *rx_info);

typedef struct
{
    app_bt_handler_func_t func_cb;
    uint16_t max_len;
 #if APP_LOG_ENABLE
    char * log_info;
 #endif
}app_bt_evt_handler_st;

typedef struct
{
    app_bt_handler_func_t func_cb;
 #if APP_LOG_ENABLE
    char *log_info[OPCODE_BT_MAX];
 #endif
}app_bt_ack_handler_st;

/*
 * GLOBAL FUNCTION DECLARATION
 *****************************************************************************************
 */

/**
 *****************************************************************************************
 * @brief Function to hand bt model ack message
 *****************************************************************************************
 */
 extern void app_bt_ack_handle(bt_rx_info *g_bt_rx_info_p);

/**
 *****************************************************************************************
 * @brief Function for reset bt device
 *****************************************************************************************
 */
extern void app_bt_device_reset(void);

/**
 *****************************************************************************************
 * @brief Function for get inquiry/inquiry scan/page/page scan status
 *****************************************************************************************
 */
extern void app_bt_device_inquiry_page_get(void);

/**
 *****************************************************************************************
 * @brief Function for start inquiry scan.
 *****************************************************************************************
 */
extern void app_bt_device_inquiry_scan_start(void);

/**
 *****************************************************************************************
 * @brief Function for stop inquiry scan.
 *****************************************************************************************
 */
extern void app_bt_device_inquiry_scan_stop(void);

/**
 *****************************************************************************************
 * @brief Function for get current connection state
 *
 ****************************************************************************************
  */
extern void app_bt_conn_state_get(void);


/**
 *****************************************************************************************
 * @brief Function for abort current connection. 
 *
 ****************************************************************************************
  */
extern void app_bt_connect_abort(void);

/**
 *****************************************************************************************
 * @brief Function for start page scan
 *****************************************************************************************
 */
extern void app_bt_device_page_scan_start(void);

/**
 *****************************************************************************************
 * @brief Function for stop page scan
 *****************************************************************************************
 */
extern void app_bt_device_page_scan_stop(void);


/**
 *****************************************************************************************
 * @brief Function for re-connect the last pairing device or last store link key device
 *****************************************************************************************
 */
extern void app_bt_quick_reconnect(void);

/**
 *****************************************************************************************
 * @brief Function for disconnect current connection device.
 *****************************************************************************************
 */
extern void app_bt_disconnect(void);

/**
 *****************************************************************************************
 * @brief Function for store remote addr and link key
 *
 * @param[in] remote_addr: remote addr to store
 * @param[in] link_key: remote pairing link key.
 *
 *****************************************************************************************
 */
extern void app_bt_pairing_info_store(uint8_t remote_addr[6], uint8_t link_key[16]);

/**
 *****************************************************************************************
 * @brief Function for clean pairing bonding info.
 *
 * @param[in] remote_addr: remote addr to store
 *
 *****************************************************************************************
 */
extern void app_bt_pairing_info_clean(uint8_t remote_addr[6]);

/**
 *****************************************************************************************
 * @brief Function for control re-connect procedure.
 *
 * @param[in] enable: true is open re-connect, false is close re-connect.
 *
 *****************************************************************************************
 */
extern void app_bt_enable_quick_connect(bool enable);

/**
 *****************************************************************************************
 * @brief Function for get bt address, will report by addr ind message
 *****************************************************************************************
 */
extern void app_bt_addr_get(void);

/**
 *****************************************************************************************
 * @brief Function for set bt address, valid after restart
 *
 * @param[in] bdaddr: BT address buffer ptr.
 *
 *****************************************************************************************
 */
extern void app_bt_addr_set(uint8_t bdaddr[6]);

/**
 *****************************************************************************************
 * @brief Function for set bt device name
 *
 * @param[in] device_name: device name buffer ptr.
 * @param[in] len: device name length,shall be less than 32 bytes.
 *
 *****************************************************************************************
 */
extern void app_bt_device_name_set(uint8_t *device_name, uint8_t lenth);

/**
 *****************************************************************************************
 * @brief Function for get bt device name
 *****************************************************************************************
 */
extern void app_bt_device_name_get(void);

/**
 *****************************************************************************************
 * @brief Function for get software version
 *****************************************************************************************
 */
extern void app_bt_software_version_get(void);

/**
 *****************************************************************************************
 * @brief Function for begin ota
 *
 * @param[in] flash_qspi_addr: ota FW flash start address.
 * @param[in] flash_fw_size: ota FW size.
 *
 *****************************************************************************************
 */
extern void app_bt_ota_procedure_begin(uint32_t flash_qspi_addr, uint32_t flash_fw_size);

/**
 *****************************************************************************************
 * @brief Function for siri wake up.
 *****************************************************************************************
 */
extern void app_bt_siri_wake_up(void);

/**
 *****************************************************************************************
 * @brief Function for siri exit.
 *****************************************************************************************
 */
extern void app_bt_siri_exit(void);

/**
 *****************************************************************************************
 * @brief Function to get siri state
 *****************************************************************************************
 */
extern void app_bt_siri_state_get(void);

/**
 *****************************************************************************************
 * @brief Function for mute the mic
 *
 * @param[mute] mute: enable mute(true), or not (false)
 *
 *****************************************************************************************
 */
extern void app_bt_mic_mute(bool mute);

/**
 *****************************************************************************************
 * @brief Function for volume up.
 *****************************************************************************************
 */
extern void app_bt_volp_action(void);

/**
 *****************************************************************************************
 * @brief Function for volume down.
 *****************************************************************************************
 */
extern void app_bt_volm_action(void);

/**
 *****************************************************************************************
 * @brief Function for volume mute.
 *
 * @param[mute] mute: enable mute(true), or not (false)
 *
 *****************************************************************************************
 */
extern void app_bt_vol_mute(bool mute);

/**
 *****************************************************************************************
 * @brief Function for the mic gain set
 *
 * @param[in] ana_gain     : call volume level analog gain to set, 0~13
 * @param[in] dig_gain      : call volume level digital to set,        0~45
 *
 *****************************************************************************************
 */
extern void app_bt_mic_gain_set(uint8_t ana_gain, uint8_t dig_gain);

/**
 *****************************************************************************************
 * @brief Function for mic gain get
 *****************************************************************************************
 */
extern void app_bt_mic_gain_get(void);

/**
 *****************************************************************************************
 * @brief Function for start mic test
 *****************************************************************************************
 */
extern void app_bt_mic_test_start(void);

/**
 *****************************************************************************************
 * @brief Function for stop mic test
 *****************************************************************************************
 */
extern void app_bt_mic_test_stop(void);


/**
 *****************************************************************************************
 * @brief Function for factory reset bt device
 *****************************************************************************************
 */
extern void app_bt_device_factory_reset(void);

/**
 *****************************************************************************************
 * @brief Function for power down bt.
 *****************************************************************************************
 */
extern void app_bt_powerdown_bt(void);

/**
 *****************************************************************************************
 * @brief Function for wakeup bt.
 *****************************************************************************************
 */
extern void app_bt_wakeup_bt(void);

/**
 *****************************************************************************************
 * @brief Function for get pairing binding list.
 *****************************************************************************************
 */
extern void app_bt_pair_list_get(void);

/**
 *****************************************************************************************
 * @brief Function to set frequency offset.
 *
 * @param[mute] value: xo offset value
 *
 *****************************************************************************************
 */
extern void app_bt_freq_offset_set(uint8_t value);

/**
 *****************************************************************************************
 * @brief Function for get frequency offset.
 *****************************************************************************************
 */
extern void app_bt_freq_offset_get(void);

/**
 *****************************************************************************************
 * @brief Function for output log control.
 *
 * @param[mute] value: enable log(true), or not (false)
 *
 *****************************************************************************************
 */
extern void app_bt_log_ctrl_set(uint8_t value);

/**
 *****************************************************************************************
 * @brief Function for bt send data test mode
 *
 *****************************************************************************************
 */
extern void app_bt_test_data_ctl(void);

/**
 *****************************************************************************************
 * @brief Function for sync batt level to BT module.
 *
 * @param[level] value: 0~100. only report to phone at every 10 level changed: 10, 20. 30, 40...
 *
 *****************************************************************************************
 */
 extern void app_bt_batt_lvl_info_sync(uint8_t level);

/**
 *****************************************************************************************
 * @brief Function for get BT current volume
 *
 *****************************************************************************************
 */
extern void app_bt_cur_vol_get(void);

/**
 *****************************************************************************************
 * @brief Function for get BT volume mute state
 *
 *****************************************************************************************
 */
 extern void app_bt_vol_mute_get(void);

/**
 *****************************************************************************************
 * @brief Function for get BT mic mute state
 *
 *****************************************************************************************
 */
 extern void app_bt_mic_mute_get(void);

/**
 *****************************************************************************************
 * @brief Function for start to play drainage vibration audio
 *
 *****************************************************************************************
 */
extern void app_bt_drain_vibra_start(void);

/**
 *****************************************************************************************
 * @brief Function for stop to play drainage vibration audio
 *
 *****************************************************************************************
 */
extern void app_bt_drain_vibra_stop(void);

/**
 *****************************************************************************************
 * @brief Function for start to play search device audio
 *
 *****************************************************************************************
 */
extern void app_bt_search_dev_start(void);

/**
 *****************************************************************************************
 * @brief Function for start to play search device audio
 *
 *****************************************************************************************
 */
extern void app_bt_search_dev_stop(void);

/**
 *****************************************************************************************
 * @brief Function for notice volume level set.
 *
 * @param[in] vol_level     : notice volume level set:0x0~0x10
 *
 *****************************************************************************************
 */
 extern void app_bt_notice_level_set(uint8_t vol_level);

/**
 *****************************************************************************************
 * @brief Function to get notice volume level .
 *
 *****************************************************************************************
 */
 extern void app_bt_notice_level_get(void);

/**
 *****************************************************************************************
 * @brief Function for register ble adv start callback.
 *****************************************************************************************
 */
extern void app_bt_register_ble_adv_ptr(void (*ptr)(void ));

/**
 *****************************************************************************************
 * @brief Function for bt hardware reset, it's only valid in active mode.
 *
 *****************************************************************************************
 */
extern void app_bt_reset_hw(void);

/**
 *****************************************************************************************
 * @brief Function for bt enter ultra_deep_sleep
 *
 *****************************************************************************************
 */
extern void app_bt_ultra_deep_sleep(void);

/**
 *****************************************************************************************
 * @brief Function for bt debug.
 *****************************************************************************************
 */
extern void bt_debug_task(void);

#endif

