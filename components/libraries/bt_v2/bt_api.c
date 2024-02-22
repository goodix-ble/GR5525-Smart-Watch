/**
 ****************************************************************************************
 *
 * @file bt_api.c
 *
 * @brief Classic Bluetooth API
 *
 ****************************************************************************************
 * @attention
  #####Copyright (c) 2023 GOODIX
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

/*
 * INCLUDE FILES
 *****************************************************************************************
 */
#include "bt_api.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "app_drv_error.h"
#include "app_log.h"
#include "app_timer.h"
#include "bt_ifce.h"
#include "task.h"

#include "bt_ctl_debug_interface.h"

/*
 * DEFINE
 *****************************************************************************************
 */
#define ARRAY_SIZE(arr)      (sizeof((arr)) / sizeof((arr[0])))

#define BT_API_MALLOC(size)  pvPortMalloc((size))
#define BT_API_FREE(ptr)     vPortFree((ptr))
#define BT_API_GET_TICK_MS() xTaskGetTickCount()

/**
 * @brief Indicates whether to do hardware reset on init.
 */
#define BT_API_POWER_ON_RESET 1

/**
 * @brief Indicates whether to verify checksum of the incoming message.
 */
#define BT_API_MSG_CHECKSUM_ENABLED 1

/**
 * @brief Indicates whether to use dynamic memory allocation for received message.
 */
#define BT_API_MSG_USE_DYN_MEM 1

/**
 * @brief Indicates how many commands can be queued before actually sent to BT module.
 */
#define BT_API_COMMAND_QUEUE_SIZE 8

/**
 * @brief Indicates the time to waited before send another command without receiving the previous one's ack, in ms.
 */
#define BT_API_RESP_TIMEOUT_MS 500

/**
 * @brief Indicates whether to print debug log or not.
 */
#define BT_API_PRINT_DEBUG_LOG (APP_LOG_ENABLE && 1)

#if BT_API_PRINT_DEBUG_LOG
#define BT_DEBUG_LOG APP_LOG_DEBUG
#else
#define BT_DEBUG_LOG(...)
#endif // BT_API_PRINT_DEBUG_LOG

/*
 * STRUCT DEFINE
 *****************************************************************************************
 */

typedef enum {
    READER_STATE_HEADER,
    READER_STATE_LOG,
    READER_STATE_DATA,
    READER_STATE_CHECKSUM,
} bt_api_reader_state_t;

typedef struct
{
    bt_api_reader_state_t state;
    bt_api_msg_t msg;
    union
    {
        struct
        {
            uint8_t header[sizeof(bt_api_rx_header_t)];
            uint8_t offset;
        } header;

        struct
        {
            uint8_t *buf;
            uint16_t offset;
        } data;

        struct
        {
            uint8_t *buf;
        } checksum;
    } cache;
} bt_api_reader_fsm_t;

/*
 * LOCAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */

static struct
{
    uint8_t *buffer;
    uint32_t length;
} s_tx_queue[BT_API_COMMAND_QUEUE_SIZE];

static uint8_t s_tx_idx = 0;

static bool s_responsed = true;
static app_timer_id_t s_response_timeout_timer;

#if !BT_API_MSG_USE_DYN_MEM
static uint8_t s_msg_data_buffer[BT_IFCE_UART_RX_MAX_LEN - sizeof(bt_api_rx_header_t) + 1];
#endif  // !BT_API_MSG_USE_DYN_MEM

static bool s_sync_wake_required = false;

static bt_api_reader_fsm_t reader_fsm = {
    .state = READER_STATE_HEADER,
    .msg = {0},
    .cache.header.offset = 0,
};

static uint8_t *s_dbg_tx_buffer = NULL;
static uint8_t *s_dbg_rx_buffer = NULL;

#if BT_API_PRINT_DEBUG_LOG
static const char *const OPCODE_NAME[] = {
    [OPCODE_A2DP_STATUS_SET] = "A2DP_STATUS_SET",
    [OPCODE_PLAY_STATUS_SET] = "PLAY_STATUS_SET",
    [OPCODE_PLAY_NEXT]       = "PLAY_NEXT",
    [OPCODE_PLAY_PREV]       = "PLAY_PREV",
    [OPCODE_PLAY_REWIND]     = "PLAY_REWIND",
    [OPCODE_PLAY_FORWARD]    = "PLAY_FORWARD",
    [OPCODE_AUDIO_VOL_SET]   = "AUDIO_VOL_SET",
    [OPCODE_AUDIO_VOL_GET]   = "AUDIO_VOL_GET",
    [OPCODE_VOL_GAIN_SET]    = "VOL_GAIN_SET",
    [OPCODE_VOL_GAIN_GET]    = "VOL_GAIN_GET",
    [OPCODE_AUDIO_INFO_GET]  = "AUDIO_INFO_GET",
    [OPCODE_AUDIO_WORDS_GET] = "AUDIO_WORDS_GET",

    [OPCODE_CALL_ADDR_BOOK_GET]       = "CALL_ADDR_BOOK_GET",
    [OPCODE_CALL_ACCEPT]              = "CALL_ACCEPT",
    [OPCODE_CALL_DTMF_SET]            = "CALL_DTMF_SET",
    [OPCODE_CALL_END]                 = "CALL_END",
    [OPCODE_CALL_REJECT]              = "CALL_REJECT",
    [OPCODE_CALL_REDIAL]              = "CALL_REDIAL",
    [OPCODE_CALL_VOL_SET]             = "CALL_VOL_SET",
    [OPCODE_CALL_VOL_GET]             = "CALL_VOL_GET",
    [OPCODE_CALL_VOL_GAIN_SET]        = "CALL_VOL_GAIN_SET",
    [OPCODE_CALL_VOL_GAIN_GET]        = "CALL_VOL_GAIN_GET",
    [OPCODE_CALL_VOL_SWITCH]          = "CALL_VOL_SWITCH",
    [OPCODE_CALL_HOLD_FIR_ACCEPT_SEC] = "CALL_HOLD_FIR_ACCEPT_SEC",
    [OPCODE_CALL_END_FIR_ACCEPT_SEC]  = "CALL_END_FIR_ACCEPT_SEC",
    [OPCODE_CALL_HOLD_FIR_REJECT_SEC] = "CALL_HOLD_FIR_REJECT_SEC",
    [OPCODE_CALL_LINE_SWITCH]         = "CALL_LINE_SWITCH",
    [OPCODE_CALL_NUMBER_GET]          = "CALL_NUMBER_GET",
    [OPCODE_CALL_DIAL_NUMBER]         = "CALL_DIAL_NUMBER",
    [OPCODE_CALL_RING_STATE_SET]      = "CALL_RING_STATE_SET",
    [OPCODE_CALL_RING_STATE_GET]      = "CALL_RING_STATE_GET",

    [OPCODE_BT_RESET]              = "BT_RESET",
    [OPCODE_BT_INQUIRY]            = "BT_INQUIRY",
    [OPCODE_BT_INQUIRY_SCAN]       = "BT_INQUIRY_SCAN",
    [OPCODE_BT_CONNECT]            = "BT_CONNECT",
    [OPCODE_BT_CONNECT_ABORT]      = "BT_CONNECT_ABORT",
    [OPCODE_BT_PAGE_SCAN]          = "BT_PAGE_SCAN",
    [OPCODE_BT_PAIRING_BEGIN]      = "BT_PAIRING_BEGIN",
    [OPCODE_BT_QUICK_CONNECT]      = "BT_QUICK_CONNECT",
    [OPCODE_BT_DISCONNECT]         = "BT_DISCONNECT",
    [OPCODE_BT_SWITCH_PHONE]       = "BT_SWITCH_PHONE",
    [OPCODE_BT_PAIRING_INFO_STORE] = "BT_PAIRING_INFO_STORE",
    [OPCODE_BT_PAIRING_INFO_CLEAN] = "BT_PAIRING_INFO_CLEAN",
    [OPCODE_BT_ACTIVE_TIMEOUT_SET] = "BT_ACTIVE_TIMEOUT_SET",
    [OPCODE_BT_SNIFF_INTERVAL_SET] = "BT_SNIFF_INTERVAL_SET",
    [OPCODE_BT_QUICK_CON_TIMES]    = "BT_QUICK_CON_TIMES",
    [OPCODE_BT_QUICK_CON_MODE_SET] = "BT_QUICK_CON_MODE_SET",
    [OPCODE_BT_BD_ADDR_GET]        = "BT_BD_ADDR_GET",
    [OPCODE_BT_DEVICE_NAME_SET]    = "BT_DEVICE_NAME_SET",
    [OPCODE_BT_DEVICE_NAME_GET]    = "BT_DEVICE_NAME_GET",

    [OPCODE_BT_THRPUT_BEGIN] = "BT_THRPUT_BEGIN",
    [OPCODE_BT_THRPUT_DATA]  = "BT_THRPUT_DATA",
    [OPCODE_BT_THRPUT_END]   = "BT_THRPUT_END",

    [OPCODE_BT_FW_VERSION_GET] = "BT_FW_VERSION_GET",
    [OPCODE_BT_OTA_BEGIN]      = "BT_OTA_BEGIN",

    [OPCODE_BT_SIRI_WAKE]           = "BT_SIRI_WAKE",
    [OPCODE_BT_SWITCH_NOTICE]       = "BT_SWITCH_NOTICE",
    [OPCODE_BT_MIC_MUTE]            = "BT_MIC_MUTE",
    [OPCODE_VOL_P]                  = "VOL_P",
    [OPCODE_VOL_M]                  = "VOL_M",
    [OPCODE_VOL_MUTE]               = "VOL_MUTE",
    [OPCODE_BT_MIC_GAIN_SET]        = "BT_MIC_GAIN_SET",
    [OPCODE_BT_MIC_GAIN_GET]        = "BT_MIC_GAIN_GET",
    [OPCODE_BT_MIC_TEST_START]      = "BT_MIC_TEST_START",
    [OPCODE_BT_MIC_TEST_STOP]       = "BT_MIC_TEST_STOP",
    [OPCODE_BT_MIC_AUTO_TEST]       = "BT_MIC_AUTO_TEST",
    [OPCODE_BT_CUSTOM_NOTICE_PLAY]  = "BT_CUSTOM_NOTICE_PLAY",
    [OPCODE_BT_CUSTOM_NOTICE_PAUSE] = "BT_CUSTOM_NOTICE_PAUSE",

    [OPCODE_BT_UART_MAX_LENGTH]   = "BT_UART_MAX_LENGTH",
    [OPCODE_BT_FACTORY_RESET]     = "BT_FACTORY_RESET",
    [OPCODE_BT_POWERDOWN]         = "BT_POWERDOWN",
    [OPCODE_BT_WAKEUP]            = "BT_WAKEUP",
    [OPCODE_BT_PAIR_DEV_LIST_GET] = "BT_PAIR_DEV_LIST_GET",
    [OPCODE_BT_CALL_OFFSET_SET]   = "BT_CALL_OFFSET_SET",
    [OPCODE_BT_CALL_OFFSET_GET]   = "BT_CALL_OFFSET_GET",
    [OPCODE_BT_LOG_CTRL]          = "BT_LOG_CTRL",
    [OPCODE_BT_BD_ADDR_SET]       = "BT_BD_ADDR_SET",
    [OPCODE_BT_INQ_PAGE_GET]      = "BT_INQ_PAGE_GET",
    [OPCODE_BT_CONN_STATE_GET]    = "BT_CONN_STATE_GET",

    [OPCODE_BT_BATT_LV_SYNC] = "BT_BATT_LV_SYNC",

    [OPCODE_CUR_VOL_GET]       = "CUR_VOL_GET",
    [OPCODE_CUR_GAIN_GET]      = "CUR_GAIN_GET",
    [OPCODE_VOL_MUTE_GET]      = "VOL_MUTE_GET",
    [OPCODE_MIC_MUTE_GET]      = "MIC_MUTE_GET",
    [OPCODE_DRAIN_VIBRA_START] = "DRAIN_VIBRA_START",
    [OPCODE_DRAIN_VIBRA_STOP]  = "DRAIN_VIBRA_STOP",
    [OPCODE_SEARCH_DEV_START]  = "SEARCH_DEV_START",
    [OPCODE_SEARCH_DEV_STOP]   = "SEARCH_DEV_STOP",

    [OPCODE_BT_SIRI_SHUTDOWN]  = "BT_SIRI_SHUTDOWN",
    [OPCODE_BT_SIRI_STATE_GET] = "BT_SIRI_STATE_GET",

    [OPCODE_BT_NOTICE_LVL_SET] = "BT_NOTICE_LVL_SET",
    [OPCODE_BT_NOTICE_LVL_GET] = "BT_NOTICE_LVL_GET",
};

static const char *const IND_NAME[] = {
    [IND_PAIRING_INFO]         = "PAIRING_INFO",
    [IND_PROFILE_STATE]        = "PROFILE_STATE",
    [IND_INQUIRY_INFO]         = "INQUIRY_INFO",
    [IND_CONNECTION_STATE]     = "CONNECTION_STATE",
    [IND_BT_BD_ADDR]           = "BT_BD_ADDR",
    [IND_BT_DEV_NAME]          = "BT_DEV_NAME",
    [IND_THROUGHPUT_CMP]       = "THROUGHPUT_CMP",
    [IND_FW_VERSION_INFO]      = "FW_VERSION_INFO",
    [IND_VOL_LEVEL_INFO]       = "VOL_LEVEL_INFO",
    [IND_MIC_GAIN_INFO]        = "MIC_GAIN_INFO",
    [IND_ADDR_BOOK_INFO]       = "ADDR_BOOK_INFO",
    [IND_CALL_LINE_STATE]      = "CALL_LINE_STATE",
    [IND_CALL_RING_STATE]      = "CALL_RING_STATE",
    [IND_VOL_SITE_INFO]        = "VOL_SITE_INFO",
    [IND_AUDIO_SONG_INFO]      = "AUDIO_SONG_INFO",
    [IND_AUDIO_SONG_WORD_INFO] = "AUDIO_SONG_WORD_INFO",
    [IND_AUDIO_STATE]          = "AUDIO_STATE",
    [IND_POWER_ON_STATE]       = "POWER_ON_STATE",
    [IND_PAIR_DEV_INFO]        = "PAIR_DEV_INFO",
    [IND_BT_CALL_OFFSET]       = "BT_CALL_OFFSET",
    [IND_BT_LOG_STATE]         = "BT_LOG_STATE",
    [IND_BT_INQ_PAGE_STATE]    = "BT_INQ_PAGE_STATE",
    [IND_BT_SNIFF_STATE]       = "BT_SNIFF_STATE",
    [IND_HFP_VOL_INFO]         = "HFP_VOL_INFO",
    [IND_AUDIO_VOL_INFO]       = "AUDIO_VOL_INFO",
    [IND_HFP_GAIN_INFO]        = "HFP_GAIN_INFO",
    [IND_AUDIO_GAIN_INFO]      = "AUDIO_GAIN_INFO",
    [IND_VOL_MUTE_INFO]        = "VOL_MUTE_INFO",
    [IND_MIC_MUTE_INFO]        = "MIC_MUTE_INFO",
    [IND_LOCAL_VOICE_STATE]    = "LOCAL_VOICE_STATE",
    [IND_SIRI_STATE]           = "SIRI_STATE",
    [IND_NOTICE_LVL_STATE]     = "NOTICE_LVL_STATE",
    [IND_MESSAGE_MAX]          = "MESSAGE_MAX",
};

const char *const ACK_NAME[] = {
    [RESP_BT_NO_ERR]                   = "BT_NO_ERR",
    [RESP_BT_INVALID_PARAM]            = "BT_INVALID_PARAM",
    [RESP_BT_A2DP_NO_CONN_STATE]       = "BT_A2DP_NO_CONN_STATE",
    [RESP_BT_HFP_NO_CONN_STATE]        = "BT_HFP_NO_CONN_STATE",
    [RESP_BT_NO_CALL_LINE]             = "BT_NO_CALL_LINE",
    [RESP_BT_NO_CONN_STATE]            = "BT_NO_CONN_STATE",
    [RESP_BT_PHONE_BOOK_ACCESS_DENIED] = "BT_PHONE_BOOK_ACCESS_DENIED",
    [RESP_BT_ADDRESS_INEXISTENCE]      = "BT_ADDRESS_INEXISTENCE",
    [RESP_BT_OP_CONDITION_NOT_MET]     = "BT_OP_CONDITION_NOT_MET",
    [RESP_BT_IS_CONNECTTED]            = "BT_IS_CONNECTTED",
    [RESP_BT_NO_PAIR_RECORD]           = "BT_NO_PAIR_RECORD",
    [RESP_BT_EXIST_CALL_LINE]          = "BT_EXIST_CALL_LINE",
};

const char *const CALL_LINE_STATE_NAME[] = {
    [BT_API_CALL_LINE_STATE_INCOMING]      = "INCOMING",
    [BT_API_CALL_LINE_STATE_ACCEPT]        = "ACCEPT",
    [BT_API_CALL_LINE_STATE_HOLD]          = "HOLD",
    [BT_API_CALL_LINE_STATE_END]           = "END",
    [BT_API_CALL_LINE_STATE_DIAL]          = "DIAL",
    [BT_API_CALL_LINE_STATE_DIAL_CANCELED] = "DIAL_CANCELED",
};
#endif  // BT_API_PRINT_DEBUG_LOG

/*
 * GLOBAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */

/*
 * LOCAL FUNCTION DECLARATION
 *****************************************************************************************
 */

/**
 * @brief Calculate message checksum by adding all the byte
 *
 * @param[in] p_buf BT message buffer.
 * @param[in] len Buffer length.
 *
 * @return uint8_t Checksum Value.
 */
static uint8_t calc_msg_checksum(uint8_t *p_buf, uint16_t len);

#if BT_API_PRINT_DEBUG_LOG
/**
 * @brief Print out indication message content
 *
 * @param[in] msg Arriving message.
 */
static void print_ind_msg(const bt_api_msg_t *msg);
#endif // BT_DEBUG_LOG

/**
 * @brief Handle the arrived message before dispatch
 *
 * @param[in] msg Arriving message
 *
 * @return true Intercept, the message will not be dispatched to user callback
 * @return false Bypass, the message will be dispatched to user callback
 */
static bool bt_msg_internal_handler(const bt_api_msg_t *msg);

/**
 * @brief Handle command response timeout
 *
 * @param[in] arg Unused
 */
static void bt_response_timeout_event_handler(void *arg);

/**
 * @brief bt_ifce_send_sync wrapper for debug interface
 *
 * @param[in] buf Buffer to send.
 * @param[in] length Buffer length.
 */
static void bt_dbg_send_sync_wrapper(uint8_t *buf, uint16_t length);

#if BT_API_PRINT_DEBUG_LOG
/**
 * @brief Print BT message for debug usage.
 *
 * @param msg[in] BT message.
 */
static void bt_api_print_msg(const bt_api_msg_t *msg);
#endif // BT_API_PRINT_DEBUG_LOG

/*
 * GLOBAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void bt_api_init(void)
{
    uint16_t res = bt_ifce_init();
    if (APP_DRV_SUCCESS != res)
    {
        APP_LOG_ERROR("[BT_API] bt_ifce_init error: %d", res);
    }

    memset(s_tx_queue, 0, sizeof(s_tx_queue));
    s_tx_idx = 0;

    reader_fsm.state = READER_STATE_HEADER;
#if BT_API_MSG_USE_DYN_MEM
    if (reader_fsm.cache.data.buf)
    {
        BT_API_FREE(reader_fsm.cache.data.buf);
    }
#endif  // BT_API_MSG_USE_DYN_MEM
    reader_fsm.cache.header.offset = 0;

    // Create response timeout timer
    res = app_timer_create(&s_response_timeout_timer, ATIMER_ONE_SHOT, bt_response_timeout_event_handler);
    if (SDK_SUCCESS != res)
    {
        APP_LOG_ERROR("[BT_API] app_timer_create error: %d", res);
    }

#if BT_API_POWER_ON_RESET
    bt_ifce_hw_reset();
#endif  // BT_API_POWER_ON_RESET
}

void bt_api_deinit(bool force)
{
    if (force)
    {
        uint8_t powerdown_seq[] = {
            BT_DATA_HEAD_CMD & 0xFF,
            BT_DATA_HEAD_CMD >> 8,
            OPCODE_BT_POWERDOWN,
            0,
            0,
            0x55,
        };
        bt_ifce_send_sync(powerdown_seq, sizeof(powerdown_seq), 0xFFFFFFFFU);
    }
    else
    {
        bt_api_send_command_queue(OPCODE_BT_POWERDOWN, 0, 0);
    }
}

void bt_api_msg_handler(void)
{
    // Handle Incoming Message
    bt_api_reader_state_t state_next = READER_STATE_HEADER;
    ring_buffer_t *rb = bt_ifce_get_rx_ring_buffer();

    while (ring_buffer_items_count_get(rb) > 0)
    {
        if (reader_fsm.state == READER_STATE_HEADER)
        {
            // Check the first 2 bytes to see if this is really a response or indication
            if (reader_fsm.cache.header.offset == 0)
            {
                uint16_t cmd;
                ring_buffer_pick(rb, (uint8_t *)&cmd, 2);
                if (cmd != BT_DATA_HEAD_ACK && cmd != BT_DATA_HEAD_IND)
                {
                    // Treat as BT log
                    APP_LOG_INFO("[BT_LOG]:");
                    reader_fsm.state = READER_STATE_LOG;
                    continue;
                }
            }

            uint16_t expect = sizeof(bt_api_rx_header_t) - reader_fsm.cache.header.offset;
            uint16_t actual = ring_buffer_read(rb, reader_fsm.cache.header.header + reader_fsm.cache.header.offset, expect);
            if (expect == actual)
            {
                bt_api_rx_header_t *p_header = (bt_api_rx_header_t *)reader_fsm.cache.header.header;
                bool valid = false;

                // Sanity Check
                if (p_header->cmd == BT_DATA_HEAD_ACK && p_header->opcode < OPCODE_BT_MAX)
                {
                    valid = true;
                }
                else if (p_header->cmd == BT_DATA_HEAD_IND && p_header->opcode < IND_MESSAGE_MAX)
                {
                    valid = true;
                }

                if (valid)
                {
                    reader_fsm.msg.cmd = p_header->cmd;
                    reader_fsm.msg.length = p_header->length;
                    reader_fsm.msg.opcode = p_header->opcode;
                    state_next = READER_STATE_DATA;
                    // Copy header as well for simpler checksum calculation
#if BT_API_MSG_USE_DYN_MEM
                    uint8_t *p_buf = BT_API_MALLOC(p_header->length + sizeof(bt_api_rx_header_t) + 1);
#else
                    uint8_t *p_buf = s_msg_data_buffer;
#endif  // BT_API_MSG_USE_DYN_MEM
                    memcpy(p_buf, p_header, sizeof(bt_api_rx_header_t));
                    reader_fsm.cache.data.buf = p_buf;
                    reader_fsm.cache.data.offset = sizeof(bt_api_rx_header_t);
                }
                else
                {
                    // try read from start
                    APP_LOG_ERROR("Invalid Command 0x%04x or OPCODE %d!", p_header->cmd, p_header->opcode);
                    reader_fsm.cache.header.offset = 0;
                    state_next = READER_STATE_HEADER;
                }
            }
            else
            {
                reader_fsm.cache.header.offset += actual;
                state_next = READER_STATE_HEADER;
            }
        }
        else if (reader_fsm.state == READER_STATE_LOG)
        {
            uint16_t cmd;
            ring_buffer_pick(rb, (uint8_t *)&cmd, 2);
            if (cmd != BT_DATA_HEAD_ACK && cmd != BT_DATA_HEAD_IND)
            {
                // Treat as BT log
                ring_buffer_read(rb, (uint8_t *)&cmd, 1);
                APP_LOG_RAW_INFO("%02x", ((uint8_t *)&cmd)[0]);
                reader_fsm.state = READER_STATE_LOG;
                continue;
            }
            else
            {
                APP_LOG_INFO("[BT_API] BT Log End");
                reader_fsm.state = READER_STATE_HEADER;
                continue;
            }
        }
        else if (reader_fsm.state == READER_STATE_DATA)
        {
            // Read data to length
            uint16_t expect = reader_fsm.msg.length - reader_fsm.cache.data.offset + sizeof(bt_api_rx_header_t);
            uint16_t actual = ring_buffer_read(rb, reader_fsm.cache.data.buf + reader_fsm.cache.data.offset, expect);
            if (actual == expect)
            {
                reader_fsm.msg.data.data = reader_fsm.cache.data.buf + sizeof(bt_api_rx_header_t);
                reader_fsm.cache.checksum.buf = reader_fsm.cache.data.buf;
                state_next = READER_STATE_CHECKSUM;
            }
            else
            {
                reader_fsm.cache.data.offset += actual;
                state_next = READER_STATE_DATA;
            }
        }
        else if (reader_fsm.state == READER_STATE_CHECKSUM)
        {
            uint8_t checksum;
            ring_buffer_read(rb, &checksum, 1);
#if BT_API_MSG_CHECKSUM_ENABLED
            uint8_t checksum_calc = calc_msg_checksum(reader_fsm.cache.checksum.buf, reader_fsm.msg.length + sizeof(bt_api_rx_header_t));
#else
            uint8_t checksum_calc = checksum;
#endif  // BT_API_MSG_CHECKSUM_ENABLED
            if (checksum_calc == checksum)
            {
                // Valid
                // Add a '\0' to the end will make string process (like printf) much easier
                reader_fsm.msg.data.data[reader_fsm.msg.length] = 0;
#if BT_API_PRINT_DEBUG_LOG
                bt_api_print_msg(&reader_fsm.msg);
#endif // BT_API_PRINT_DEBUG_LOG

                // Handle internally first
                if (!bt_msg_internal_handler(&reader_fsm.msg))
                {
                    // Dispatch to user callback if not intercepted
                    bt_api_on_msg_arrived(&reader_fsm.msg);
                }
            }
            else
            {
                // Invalid checksum, nothing we can do
                APP_LOG_ERROR("Checksum Mismatched %d vs %d", checksum, checksum_calc);
            }
#if BT_API_MSG_USE_DYN_MEM
            BT_API_FREE(reader_fsm.cache.checksum.buf);
#endif  // BT_API_MSG_USE_DYN_MEM
            reader_fsm.cache.header.offset = 0;
            state_next = READER_STATE_HEADER;
        }

        reader_fsm.state = state_next;
    }

    // Handle Outgoing Message
    // printf("r: %d, b: %d\n", s_responsed, s_tx_queue[s_tx_idx].buffer);
    if (s_responsed && s_tx_queue[s_tx_idx].buffer)
    {
        // Do sync wake first
        if (s_sync_wake_required)
        {
            // TODO: Get rid of the 3ms delay by using timer
            bt_ifce_io_wake(BT_SYNC_WAKEUP_PULSE_WIDTH_FROM_SNIFF);
            s_sync_wake_required = false;
        }
        // Do send command
        uint8_t send_idx = s_tx_idx;
        uint8_t *p_buf = s_tx_queue[send_idx].buffer;
        uint32_t size = s_tx_queue[send_idx].length;
        BT_DEBUG_LOG("[BT_API] BT Command \"%s\" Send @ %d", OPCODE_NAME[p_buf[2]], send_idx);
        bt_ifce_send_async(p_buf, size);
        BT_API_FREE(p_buf);
        s_tx_idx = (send_idx + 1) % ARRAY_SIZE(s_tx_queue);
        s_tx_queue[send_idx].buffer = NULL;
        s_responsed = false;
        app_timer_start(s_response_timeout_timer, BT_API_RESP_TIMEOUT_MS, NULL);
    }
}

bool bt_api_send_command_queue(bt_api_opcode_t opcode, uint16_t length, uint8_t *data)
{
    for (uint8_t i = 0; i < ARRAY_SIZE(s_tx_queue); i++)
    {
        uint8_t idx = (i + s_tx_idx) % ARRAY_SIZE(s_tx_queue);
        if (!s_tx_queue[idx].buffer)
        {
            uint8_t *p_buf = (uint8_t *)pvPortMalloc(sizeof(bt_api_rx_header_t) + length + 1);
            p_buf[0] = BT_DATA_HEAD_CMD & 0xFF;
            p_buf[1] = BT_DATA_HEAD_CMD >> 8;
            p_buf[2] = opcode;
            p_buf[3] = length & 0xFF;
            p_buf[4] = length >> 8;
            if (length > 0 && data)
            {
                memcpy(p_buf + sizeof(bt_api_rx_header_t), data, length);
            }
            p_buf[sizeof(bt_api_rx_header_t) + length] = calc_msg_checksum(p_buf, sizeof(bt_api_rx_header_t) + length);
            s_tx_queue[idx].buffer = p_buf;
            s_tx_queue[idx].length = sizeof(bt_api_rx_header_t) + length + 1;
            BT_DEBUG_LOG("[BT_API] BT Command \"%s\" Queued @ %d", OPCODE_NAME[opcode], idx);
            bt_ifce_data_available();
            return true;
        }
    }
    return false;
}

#if BT_API_PRINT_DEBUG_LOG
static void bt_api_print_msg(const bt_api_msg_t *msg)
{
    if (msg->cmd == BT_DATA_HEAD_ACK)
    {
        if (*msg->data.response < ARRAY_SIZE(ACK_NAME))
        {
            BT_DEBUG_LOG("[BT_API] Response Result for \"%s\": %s", OPCODE_NAME[msg->opcode], ACK_NAME[*msg->data.response]);
        }
        else
        {
            BT_DEBUG_LOG("[BT_API] Response Result for \"%s\": Unknown(%d)", OPCODE_NAME[msg->opcode], *msg->data.response);
        }
    }
    else if (msg->cmd == BT_DATA_HEAD_IND)
    {
        BT_DEBUG_LOG("[BT_API] Indication Message: \"%s\" Length: %d", IND_NAME[msg->opcode], msg->length);
        print_ind_msg(msg);
    }
}
#endif // BT_API_PRINT_DEBUG_LOG

__WEAK void bt_api_on_msg_arrived(const bt_api_msg_t *msg)
{
}

void bt_debug_on_enter_debug_mode()
{
    // printf("Enter Debug Mode!!!\n");
    s_dbg_tx_buffer = BT_API_MALLOC(DEBUG_TX_BUFF_SIZE);
    s_dbg_rx_buffer = BT_API_MALLOC(DEBUG_RX_BUFF_SIZE);
    bt_debug_set_buffer(s_dbg_tx_buffer, DEBUG_TX_BUFF_SIZE, s_dbg_rx_buffer, DEBUG_RX_BUFF_SIZE);
    bt_debug_set_bt_transmit_func(bt_dbg_send_sync_wrapper);
    app_log_init(NULL, NULL, NULL);
}

void bt_debug_on_exit_debug_mode(void)
{
    BT_API_FREE(s_dbg_tx_buffer);
    BT_API_FREE(s_dbg_rx_buffer);
}

/*
 * LOCAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */

static uint8_t calc_msg_checksum(uint8_t *p_buf, uint16_t len)
{
    uint8_t ret = 0;
    for (uint16_t i = 0; i < len; i++)
    {
        ret += p_buf[i];
    }

    ret = ~ret + 1;
    return ret;
}

#if BT_API_PRINT_DEBUG_LOG
static void print_ind_msg(const bt_api_msg_t *msg)
{
    switch (msg->opcode)
    {
        case IND_PROFILE_STATE:
            BT_DEBUG_LOG("[BT_API] Profile State: A2DP %sonnected, HFP %sonnected",
                          msg->data.indication->profile_state.a2dp ? "C" : "Disc",
                          msg->data.indication->profile_state.hfp ? "C" : "Disc");
            break;

        case IND_INQUIRY_INFO:
            BT_DEBUG_LOG("[BT_API] Inquiry Info: %02x:%02x:%02x:%02x:%02x:%02x, RSSI: %d, Device Name: \"%s\"",
                          msg->data.indication->inquiry_info.bd_addr.addr[5],
                          msg->data.indication->inquiry_info.bd_addr.addr[4],
                          msg->data.indication->inquiry_info.bd_addr.addr[3],
                          msg->data.indication->inquiry_info.bd_addr.addr[2],
                          msg->data.indication->inquiry_info.bd_addr.addr[1],
                          msg->data.indication->inquiry_info.bd_addr.addr[0],
                          msg->data.indication->inquiry_info.rssi,
                          &msg->data.indication->inquiry_info.device_name);
            break;

        case IND_CONNECTION_STATE:
            BT_DEBUG_LOG("[BT_API] BT %sonnected %s %02x:%02x:%02x:%02x:%02x:%02x",
                          msg->data.indication->connection_state.status ? "C" : "Disc",
                          msg->data.indication->connection_state.status ? "to" : "from",
                          msg->data.indication->connection_state.bd_addr.addr[5],
                          msg->data.indication->connection_state.bd_addr.addr[4],
                          msg->data.indication->connection_state.bd_addr.addr[3],
                          msg->data.indication->connection_state.bd_addr.addr[2],
                          msg->data.indication->connection_state.bd_addr.addr[1],
                          msg->data.indication->connection_state.bd_addr.addr[0]);
            break;

        case IND_BT_BD_ADDR:
            BT_DEBUG_LOG("[BT_API] BT BD Address: %02x:%02x:%02x:%02x:%02x:%02x",
                          msg->data.indication->bt_bd_addr.addr[5],
                          msg->data.indication->bt_bd_addr.addr[4],
                          msg->data.indication->bt_bd_addr.addr[3],
                          msg->data.indication->bt_bd_addr.addr[2],
                          msg->data.indication->bt_bd_addr.addr[1],
                          msg->data.indication->bt_bd_addr.addr[0]);
            break;

        case IND_BT_DEV_NAME:
            BT_DEBUG_LOG("[BT_API] BT Device Name: \"%s\"", &msg->data.indication->bt_dev_name);
            break;

        case IND_FW_VERSION_INFO:
            BT_DEBUG_LOG("[BT_API] BT FW Version: %d.%d.%d.%d %c V%d.%d",
                          msg->data.indication->fw_version_info.major,
                          msg->data.indication->fw_version_info.minor,
                          msg->data.indication->fw_version_info.build,
                          msg->data.indication->fw_version_info.patch,
                          msg->data.indication->fw_version_info.place_c,
                          msg->data.indication->fw_version_info.app_v1,
                          msg->data.indication->fw_version_info.app_v2);
            break;

        case IND_VOL_LEVEL_INFO:
            BT_DEBUG_LOG("[BT_API] Current Volume: %d", msg->data.indication->vol_level_info);
            break;

        case IND_MIC_GAIN_INFO:
            BT_DEBUG_LOG("[BT_API] Mic Gain Analog: %d, Digital: %d", msg->data.indication->mic_gain_info.analog, msg->data.indication->mic_gain_info.digital);
            break;

        case IND_ADDR_BOOK_INFO:
            BT_DEBUG_LOG("[BT_API] Contact Info: %s", &msg->data.indication->addr_book_info);
            break;

        case IND_CALL_LINE_STATE:
            {
                BT_DEBUG_LOG("[BT_API] Call Line State Changed with %s: %s",
                              &msg->data.indication->call_line_state.phone_number,
                              CALL_LINE_STATE_NAME[msg->data.indication->call_line_state.state]);
            }
            break;

        case IND_CALL_RING_STATE:
            BT_DEBUG_LOG("[BT_API] Call Ring State Changed: %d", msg->data.indication->call_ring_state);
            break;

        case IND_VOL_SITE_INFO:
            BT_DEBUG_LOG("[BT_API] Current Vol Site: %s", msg->data.indication->vol_site_info == VOL_SITE_PHONE ? "Phone" : "BT");
            break;

        case IND_AUDIO_SONG_INFO:
            BT_DEBUG_LOG("[BT_API] Now Playing: %s", &msg->data.indication->audio_song_info);
            break;

        case IND_AUDIO_SONG_WORD_INFO:
            BT_DEBUG_LOG("[BT_API] Lyric: %s", &msg->data.indication->audio_song_word_info);
            break;

        case IND_AUDIO_STATE:
            BT_DEBUG_LOG("[BT_API] Music %s", (msg->data.indication->audio_state == AUDIO_STATE_PLAYING) ? "Playing" : "Paused");
            break;

        case IND_POWER_ON_STATE:
            BT_DEBUG_LOG("[BT_API] BT Moulde Powered On!");
            break;

        case IND_PAIR_DEV_INFO:
            if (msg->length > 0)
            {
                BT_DEBUG_LOG("[BT_API] Paired with %02x:%02x:%02x:%02x:%02x:%02x: \"%s\" (%02X%02X%02X)",
                              msg->data.indication->pair_dev_info.dev_addr.addr[5],
                              msg->data.indication->pair_dev_info.dev_addr.addr[4],
                              msg->data.indication->pair_dev_info.dev_addr.addr[3],
                              msg->data.indication->pair_dev_info.dev_addr.addr[2],
                              msg->data.indication->pair_dev_info.dev_addr.addr[1],
                              msg->data.indication->pair_dev_info.dev_addr.addr[0],
                              &msg->data.indication->pair_dev_info.dev_name,
                              msg->data.indication->pair_dev_info.dev_cod[0],
                              msg->data.indication->pair_dev_info.dev_cod[1],
                              msg->data.indication->pair_dev_info.dev_cod[2]);
            }
            break;

        case IND_BT_CALL_OFFSET:
            BT_DEBUG_LOG("[BT_API] Frequency Offset Calibration Value: %d", msg->data.indication->call_offset);
            break;

        case IND_BT_LOG_STATE:
            BT_DEBUG_LOG("[BT_API] Log State: %s", msg->data.indication->log_state ? "On" : "Off");
            break;

        case IND_BT_INQ_PAGE_STATE:
            BT_DEBUG_LOG("[BT_API] BT Basic State: Inquiry: %s | Inquiry Scan: %s | Page: %s | Page Scan: %s",
                          msg->data.indication->inq_page_state.inquiry ? "On" : "Off",
                          msg->data.indication->inq_page_state.inquiry_scan ? "On" : "Off",
                          msg->data.indication->inq_page_state.page ? "On" : "Off",
                          msg->data.indication->inq_page_state.page_scan ? "On" : "Off");
            break;

        case IND_HFP_VOL_INFO:
            BT_DEBUG_LOG("[BT_API] Current HFP Volume: %d", msg->data.indication->hfp_vol_info);
            break;

        case IND_AUDIO_VOL_INFO:
            BT_DEBUG_LOG("[BT_API] Current Audio Volume: %d", msg->data.indication->audio_vol_info);
            break;

        case IND_HFP_GAIN_INFO:
            BT_DEBUG_LOG("[BT_API] HFP Gain Info:");
            for (uint8_t lvl = 0; lvl < BT_API_HFP_GAIN_LEVEL_MAX; lvl++)
            {
                BT_DEBUG_LOG("    Level %d => Analog: %d, Digital: %d",
                             msg->data.indication->hfp_gain_info[lvl * 3],
                             msg->data.indication->hfp_gain_info[lvl * 3 + 1],
                             msg->data.indication->hfp_gain_info[lvl * 3 + 2]);
            }
            break;

        case IND_AUDIO_GAIN_INFO:
            BT_DEBUG_LOG("[BT_API] Audio Gain Info:");
            for (uint8_t lvl = 0; lvl < BT_API_AUDIO_GAIN_LEVEL_MAX; lvl++)
            {
                BT_DEBUG_LOG("    Level %d => Analog: %d, Digital: %d",
                             msg->data.indication->audio_gain_info[lvl * 3],
                             msg->data.indication->audio_gain_info[lvl * 3 + 1],
                             msg->data.indication->audio_gain_info[lvl * 3 + 2]);
            }
            break;

        case IND_VOL_MUTE_INFO:
            BT_DEBUG_LOG("[BT_API] Current Speaker Mute State: %s", msg->data.indication->vol_mute_info ? "Muted" : "Unmuted");
            break;

        case IND_MIC_MUTE_INFO:
            BT_DEBUG_LOG("[BT_API] Current Mic Mute State: %s", msg->data.indication->mic_mute_info ? "Muted" : "Unmuted");
            break;

        case IND_LOCAL_VOICE_STATE:
            BT_DEBUG_LOG("[BT_API] Current Local Audio State: %s", (msg->data.indication->local_voice_state == BT_API_LOCAL_VOICE_STATE_PLAYING) ? "Playing" : "Stopped");
            break;

        case IND_SIRI_STATE:
            BT_DEBUG_LOG("[BT_API] Current Siri State: %s", (msg->data.indication->siri_state == BT_API_SIRI_STATE_ACTIVE) ? "Active" : "Inactive");
            break;

        case IND_NOTICE_LVL_STATE:
            BT_DEBUG_LOG("[BT_API] Current Notice Volume: %d\n", msg->data.indication->notice_lvl_state);
            break;
    }
}
#endif // BT_API_PRINT_DEBUG_LOG

static bool bt_msg_internal_handler(const bt_api_msg_t *msg)
{
    bool intercept = false;
    // If something received, it means BT is not in sniff mode and don't need sync wake
    s_sync_wake_required = false;
    // Check if command is responsed
    if (msg->cmd == BT_DATA_HEAD_ACK && msg->opcode != OPCODE_BT_RESET)
    {
        s_responsed = true;
    }
    else if (msg->cmd == BT_DATA_HEAD_IND && msg->opcode == IND_POWER_ON_STATE)
    {
        s_responsed = true;
    }

    // Check if BT is in sniff mode
    if (msg->cmd == BT_DATA_HEAD_IND && msg->opcode == IND_BT_SNIFF_STATE)
    {
        // BT in sniff mode, need sync wakeup for next command
        s_sync_wake_required = true;
        intercept = true;
    }

    // Check if BT is powered down
    if (msg->cmd == BT_DATA_HEAD_ACK && msg->opcode == OPCODE_BT_POWERDOWN)
    {
        bt_ifce_deinit();
    }

    return intercept;
}

static void bt_response_timeout_event_handler(void *arg)
{
    s_responsed = true;
    bt_ifce_data_available();
}

static void bt_dbg_send_sync_wrapper(uint8_t *buf, uint16_t length)
{
    bt_ifce_io_wake(BT_SYNC_WAKEUP_PULSE_WIDTH_FROM_SNIFF);
    bt_ifce_send_sync(buf, length, 0xFFFFFFFFUL);
    bt_ifce_force_rx();
}
