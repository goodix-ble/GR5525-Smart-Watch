/**
 *****************************************************************************************
 *
 * @file bt_ctl_uart.c
 *
 * @brief  blt<-->bt uart Function Implementation.
 *
 *****************************************************************************************
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

/*
 * INCLUDE FILES
 *****************************************************************************************
 */
#include "gr_includes.h"
#include "hal_flash.h"
#include "custom_config.h"
#include "app_log.h"
#include "board_SK.h"
#include "bt_ctl_ind_parse.h"
#include "bt_sync_tim.h"
#include "bt_ctl_uart.h"
#include "bt_ctl_debug_interface.h"

/*
 * LOCAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */
static uint8_t bt_rx_buf[BT_MAX_RX_LEN] = {0};
static uint8_t rx_data_offset = 0;
static RX_STATUS_T rx_state = RX_HEADER;
static bt_rx_info g_bt_rx_info;
static uint8_t rx_data_ref_len = 0;
static uint8_t rx_data_ref_offset = 0;
static uint8_t* ref_data_buffer = NULL;

static uint8_t s_uart_tx_buffer[WATCH_DEMO_UART_BUFF_TX_LEN];
uint8_t s_uart_rx_buffer[WATCH_DEMO_UART_BUFF_RX_LEN];

static uint8_t s_uart_ring_buffer_data[WATCH_DEMO_UART_BUFF_RX_LEN] = {0};
static ring_buffer_t s_rx_ring_buffer;

uint32_t WATCH_DEMO_UART_BAUDRATE = WATCH_DEMO_UART_BAUDRATE_ORI;
app_uart_params_t ble_bt_uart_param;

static uint8_t s_debug_uart_tx_buffer[DEBUG_TX_BUFF_SIZE];
static uint8_t s_debug_uart_rx_buffer[DEBUG_RX_BUFF_SIZE];

/*
 * GLOBAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */

extern uint16_t bt_test_mode_al_cnt;
extern uint16_t bt_test_mode_l_cnt;
extern uint16_t bt_test_mode_s_cnt;
extern uint16_t bt_test_mode_w_cnt;

extern bool bt_sniff_state;
extern volatile BT_OTA_STATUS_T bt_ota_state ;

extern int32_t app_bt_ota_procedure(void);
extern void app_ota_tim_init(void);
extern void app_bt_ota_reponse_handle(uint8_t *buf, uint16_t len);
extern void app_bt_register_ble_adv_ptr(void (*ptr)(void ));

/*
 * LOCAL  FUNCTION DEFINITIONS
 *****************************************************************************************
 */
static void app_uart_bt_evt_handler(app_uart_evt_t *p_evt)
{
    switch(p_evt->type) {
        case APP_UART_EVT_RX_DATA:
            ble_bt_set_sync_io();
            //if OTA mode, do not need ring buffer.
#ifdef BLE_BT_OTA_SUPPORT
            if (bt_ota_state != BT_OTA_IDLE)
            {
                app_bt_ota_reponse_handle(s_uart_rx_buffer, p_evt->data.size);
            }
            else
#endif
            {
                if (ring_buffer_write(&s_rx_ring_buffer, s_uart_rx_buffer, p_evt->data.size) != p_evt->data.size)
                {
                    APP_LOG_ERROR("Ring buffer set too small!");
                }
                if (bt_debug_mode_is_active())
                {
                    app_bt_debug_reponse_handle(s_uart_rx_buffer, p_evt->data.size);
                }
            }
            memset(s_uart_rx_buffer, 0, WATCH_DEMO_UART_BUFF_RX_LEN);
            app_uart_receive_async(WATCH_DEMO_UART_ID, s_uart_rx_buffer, WATCH_DEMO_UART_BUFF_RX_LEN);
            break;
        case APP_UART_EVT_ERROR:
            APP_LOG_DEBUG("watch demo uart err_code = %d\n", p_evt->data.error_code);
            memset(s_uart_rx_buffer, 0, WATCH_DEMO_UART_BUFF_RX_LEN);
            app_uart_receive_async(WATCH_DEMO_UART_ID, s_uart_rx_buffer, WATCH_DEMO_UART_BUFF_RX_LEN);
            break;
        default:
            break;
    }
}

static void app_bt_deal_data(uint8_t *buf, uint16_t len)
{
    g_bt_rx_info.cmd        = buf[0] | buf[1]<<8;
    g_bt_rx_info.opcode   = buf[2];
    g_bt_rx_info.length    =  buf[3] | buf[4]<<8;
    if (g_bt_rx_info.length > 0)
    {
        g_bt_rx_info.data       =&buf[5];
    }
    else
    {
        g_bt_rx_info.data = NULL;
    }

    if (g_bt_rx_info.cmd == BT_CTL_CMD_ACK_HEAD)
    {
        app_bt_ack_handle(&g_bt_rx_info);
    }
    else if (g_bt_rx_info.cmd == BT_CTL_IND_HEAD)
    {
        app_bt_ind_handle(&g_bt_rx_info);
    }
    
//    APP_LOG_RAW_INFO("\r\n");
//    while(len --)
//    {
//        APP_LOG_RAW_INFO("%02x", *buf);
//        buf++;
//    }
//    APP_LOG_RAW_INFO("\r\n");

    memset(&g_bt_rx_info, 0, sizeof(bt_rx_info));
}

#if SUM_CALCULATE
uint8_t app_bt_calc_checksum(uint8_t *p_buf, uint16_t len)
{
    uint8_t checksum = 0;

    for (int i = 0; i < len; i++) {
        checksum += p_buf[i];
    }

    checksum = ~checksum + 1;

    return checksum;
}
#endif

static uint8_t  app_bt_buff_rx_format(uint8_t *buf, uint16_t len)
{
    uint16_t buff_idx = 0;
    uint16_t buff_orig_idx = 0;

    uint16_t opcode_max = 0;
    uint16_t cmd_head = 0;
    uint16_t length = 0;
#if SUM_CALCULATE
    uint8_t checksum = 0;
#endif
    bool  has_log = true;
    uint8_t ret = 0;

    //reset last un-complete rx data  
    rx_data_ref_len = 0;
    rx_data_offset = 0;

    if (len < BT_CTL_DATA_LEN_MIN)
    {
        rx_data_ref_len = BT_CTL_DATA_LEN_MIN;
        ret = 1;
        APP_LOG_RAW_INFO("short message length = %d \r\n", len);
        return ret;
    }

    bt_test_mode_al_cnt++;

    while(1)
    {
        bt_rx_buf[rx_data_offset] = buf[buff_idx + buff_orig_idx];
        rx_data_offset++;
        buff_idx ++;

        switch(rx_state)
        {
            case RX_HEADER:
                if (rx_data_offset == BT_CTL_HEAD_LEN)
                {
                    cmd_head = (((uint16_t)bt_rx_buf[rx_data_offset-BT_CTL_HEAD_LEN]) | ((uint16_t)bt_rx_buf[rx_data_offset-BT_CTL_HEAD_LEN+1]<<8));
                    if (BT_CTL_CMD_ACK_HEAD == cmd_head)
                    {
                        rx_state= RX_OPCODE;
                        opcode_max = OPCODE_BT_MAX;
                    }
                    else if (BT_CTL_IND_HEAD == cmd_head)
                    {
                        rx_state= RX_OPCODE;
                        opcode_max = IND_MESSAGE_MAX;
                    }
                    else
                    {
                        // err format, go to head
                        rx_state = RX_HEADER;
                        buff_orig_idx ++;
                        rx_data_offset = 0;
                        buff_idx = 0;
                        //APP_LOG_RAW_INFO("line %d, return head ptr %02x -->", __LINE__, buf[buff_idx + buff_orig_idx]);
                    }
                }
                break;
            case RX_OPCODE:
                if (rx_data_offset == BT_CTL_HEAD_LEN + BT_CTL_OPCODE_LEN)
                {
                    if ((opcode_max > bt_rx_buf[rx_data_offset-1]) || (OPCODE_BT_TEST_CMD == bt_rx_buf[rx_data_offset-1]))
                    {
                        rx_state= RX_DATA_LENTH;
                    }
                    else
                    {
                        // err format, go to head
                        APP_LOG_DEBUG("invalid %s :0x%02x\r\n", (BT_CTL_IND_HEAD == cmd_head)?"message":"opcode", bt_rx_buf[rx_data_offset-1]);

                        rx_state = RX_HEADER;
                        buff_orig_idx ++;
                        rx_data_offset = 0;
                        buff_idx = 0;
                        //APP_LOG_RAW_INFO("line %d, return head ptr %02x -->", __LINE__, buf[buff_idx + buff_orig_idx]);
                    }
                }
                break;
            case RX_DATA_LENTH:
                if (rx_data_offset == BT_CTL_HEAD_LEN + BT_CTL_OPCODE_LEN + BT_CTL_DATA_LEN)
                {
                    length = (((uint16_t)bt_rx_buf[rx_data_offset-BT_CTL_DATA_LEN]) | ((uint16_t)bt_rx_buf[rx_data_offset-BT_CTL_DATA_LEN+1]<<8));
                    if ((BT_MAX_RX_LEN >= length) && (length <= len - BT_CTL_DATA_LEN_MIN))
                    {
                        if (length > 0)
                        {
                            rx_state = RX_DATA;
                        }
                        else
                        {
#if SUM_CALCULATE
                            rx_state = RX_SUM;
#else
                            rx_state = RX_CMP;
#endif
                        }
                    }
                    else
                    {
                        APP_LOG_DEBUG("invalid length :%d, len : %d\r\n", length, len);
                        if (length > len - BT_CTL_DATA_LEN_MIN)
                        {
                            rx_data_ref_len = length + BT_CTL_DATA_LEN_MIN;
                        }

                        // err format, go to head
                        rx_state = RX_HEADER;
                        buff_orig_idx ++;
                        rx_data_offset = 0;
                        buff_idx = 0;
                        ret = 1;
                        //APP_LOG_RAW_INFO("line %d, return head ptr %02x -->", __LINE__, buf[buff_idx + buff_orig_idx]);
                        return ret;
                    }
                }
                break;
            case RX_DATA:
                if (rx_data_offset == length + BT_CTL_HEAD_LEN + BT_CTL_OPCODE_LEN + BT_CTL_DATA_LEN)
                {
#if SUM_CALCULATE
                    rx_state = RX_SUM;
#else
                    rx_state = RX_CMP;
#endif
                }
                break;
#if SUM_CALCULATE
            case RX_SUM:
                checksum = app_bt_calc_checksum(&bt_rx_buf[0], rx_data_offset-1);
                if (checksum == bt_rx_buf[rx_data_offset-1])
                {
                    rx_state = RX_CMP;
                }
                else
                {
                    // err format, go to head
                    rx_state = RX_HEADER;
                    buff_orig_idx ++;
                    rx_data_offset = 0;
                    buff_idx = 0;
                    //APP_LOG_RAW_INFO("line %d, return head ptr %02x -->", __LINE__, buf[buff_idx + buff_orig_idx]);
                    APP_LOG_DEBUG("Recevice Command checksum fail");
                }
                break;
#endif
            default:
                break;
        }

        if (rx_state == RX_CMP)
        {
            //APP_LOG_DEBUG("deal_message+++++++++++++");
            app_bt_deal_data(bt_rx_buf, rx_data_offset);
            // deal done, continue
            rx_state = RX_HEADER;
            buff_orig_idx += buff_idx;
            rx_data_offset = 0;
            buff_idx = 0;

            has_log = false;
        }
        else
        {
            has_log = true;
        }

        // all buffer has been resolved.
        if ((buff_idx + buff_orig_idx) >= len)
        {
            if ((rx_state != RX_CMP) && (rx_state != RX_HEADER)) //not complete data
            {
                ret = 1;
            }
            else
            {
                rx_data_offset = 0;
            }
            break;
        }
    }

    if (has_log)
    {
        bt_test_mode_w_cnt++;
        APP_LOG_RAW_INFO("other log info: ");
        while(len--)
        {
            APP_LOG_RAW_INFO("%c", *buf);
            buf++;
        }
        APP_LOG_RAW_INFO("\r\n");
    }

    if (bt_test_mode_al_cnt == 200)
    {
        APP_LOG_INFO("bt_test_mode_l_cnt = %d, bt_test_mode_s_cnt = %d, bt_test_mode_w_cnt = %d, bt_test_mode_al_cnt = %d \r\n",
                                                                                                                                                                            bt_test_mode_l_cnt,
                                                                                                                                                                            bt_test_mode_s_cnt,
                                                                                                                                                                            bt_test_mode_w_cnt,
                                                                                                                                                                            bt_test_mode_al_cnt);
    }

    return ret;
}

static uint8_t ble_bt_uart_irq_handler(uint8_t *buf, uint16_t len)
{
    uint8_t ret = 0;
    if ((0==(ret = app_bt_buff_rx_format(buf, len))) && (*(int32_t *)BT_DEBUG_MODE_CTL_ADDR != BT_DEBUG_POLLING))
    {
#ifdef BLE_BT_OTA_SUPPORT
        if (bt_ota_state == BT_OTA_IDLE)
#endif
        {
            if(ble_bt_set_sync_io_is_up())
            {
                app_uart_abort(WATCH_DEMO_UART_ID);
            }
        }
    }

    return ret;
}

__WEAK void fast_advertising_start(void)
{
    ;
}

/*
 * GLOBAL  FUNCTION DEFINITIONS
 *****************************************************************************************
 */

void ble_bt_uart_init(void)
{
    app_uart_tx_buf_t uart_buffer;
    uint16_t ret = 0;

    uart_buffer.tx_buf       = s_uart_tx_buffer;
    uart_buffer.tx_buf_size  = WATCH_DEMO_UART_BUFF_TX_LEN;

    app_uart_deinit(WATCH_DEMO_UART_ID);

    ble_bt_uart_param.id                                = WATCH_DEMO_UART_ID;
    ble_bt_uart_param.init.baud_rate            = WATCH_DEMO_UART_BAUDRATE;
    ble_bt_uart_param.init.data_bits              = UART_DATABITS_8;
    ble_bt_uart_param.init.stop_bits              = UART_STOPBITS_1;
    ble_bt_uart_param.init.parity                    = UART_PARITY_NONE;
    ble_bt_uart_param.init.hw_flow_ctrl         = UART_HWCONTROL_NONE;
    ble_bt_uart_param.init.rx_timeout_mode = UART_RECEIVER_TIMEOUT_ENABLE;
    ble_bt_uart_param.pin_cfg.rx.type            = WATCH_DEMO_UART_RX_IO_TYPE;
    ble_bt_uart_param.pin_cfg.rx.pin              = WATCH_DEMO_UART_RX_PIN;
    ble_bt_uart_param.pin_cfg.rx.mux             = WATCH_DEMO_UART_RX_PINMUX;
    ble_bt_uart_param.pin_cfg.rx.pull            = WATCH_DEMO_UART_RX_PULL;
    ble_bt_uart_param.pin_cfg.tx.type            = WATCH_DEMO_UART_TX_IO_TYPE;
    ble_bt_uart_param.pin_cfg.tx.pin              = WATCH_DEMO_UART_TX_PIN;
    ble_bt_uart_param.pin_cfg.tx.mux            = WATCH_DEMO_UART_TX_PINMUX;
    ble_bt_uart_param.pin_cfg.tx.pull            = WATCH_DEMO_UART_TX_PULL;

    ret = app_uart_init(&ble_bt_uart_param, app_uart_bt_evt_handler, &uart_buffer);
    if(APP_DRV_SUCCESS != ret)
    {
       APP_LOG_DEBUG("watch demo uart init failed: 0x%02x", ret);
    }

    app_uart_receive_async(WATCH_DEMO_UART_ID, s_uart_rx_buffer, WATCH_DEMO_UART_BUFF_RX_LEN);
}

void ble_bt_uart_recv_async(void)
{
    app_uart_receive_async(WATCH_DEMO_UART_ID, s_uart_rx_buffer, WATCH_DEMO_UART_BUFF_RX_LEN);
}

void ble_bt_uart_deinit(void)
{
    app_uart_deinit(WATCH_DEMO_UART_ID);
}

void ble_bt_resetpin_init(void)
{
    /* GPIO28 outputs a high level to ensure that the 3296 can release the reset signal. */
    app_io_init_t io_init;

    io_init.pin  = BT_RESET_GPIO_PIN;
    io_init.mode = APP_IO_MODE_OUTPUT;
    io_init.pull = APP_IO_NOPULL;
    io_init.mux  = APP_IO_MUX;
    app_io_init(BT_RESET_GPIO_TYPE, &io_init);
    app_io_write_pin(BT_RESET_GPIO_TYPE, BT_RESET_GPIO_PIN, APP_IO_PIN_SET);
}

void ble_bt_resetpin_deinit(void)
{
    /* GPIO28 outputs a high level to ensure that the 3296 can release the reset signal. */
    app_io_init_t io_init;

    io_init.pin  = BT_RESET_GPIO_PIN;
    io_init.mode = APP_IO_MODE_INPUT;
    io_init.pull = APP_IO_NOPULL;
    io_init.mux  = APP_IO_MUX;
    app_io_init(BT_RESET_GPIO_TYPE, &io_init);
}

void watch_uart_data_tx_send(uint8_t *p_data, uint16_t length)
{
    app_bt_wakeup_bt();
    app_uart_transmit_async(WATCH_DEMO_UART_ID, p_data, length);
}

void watch_uart_data_tx_sync_send(uint8_t *p_data, uint16_t length)
{
    app_bt_wakeup_bt();
    app_uart_transmit_sync(WATCH_DEMO_UART_ID, p_data, length, 100);
}

void handle_uart_ring_buffer_data(void)
{
    uint32_t idx = 0;
    bt_debug_task();

    // check if received data
    if (s_rx_ring_buffer.read_index != s_rx_ring_buffer.write_index)
    {
        uint32_t data_len = ring_buffer_items_count_get(&s_rx_ring_buffer);
        uint8_t* data_buffer = sys_malloc((data_len+rx_data_ref_offset) * sizeof(uint8_t));

        //combine last un-complete data
        if (ref_data_buffer != NULL)
        {
            memcpy(data_buffer, ref_data_buffer, rx_data_ref_offset);

            sys_free(ref_data_buffer);
            ref_data_buffer = NULL;
            APP_LOG_DEBUG("continue data process : %d %d", rx_data_ref_len, data_len);
        }
        ring_buffer_read(&s_rx_ring_buffer, data_buffer+rx_data_ref_offset, data_len);

        data_len += rx_data_ref_offset;
        rx_data_ref_offset = 0;

        //check the combined complete data
        if (rx_data_ref_len)
        {
            while(data_len - idx)
            {
                APP_LOG_RAW_INFO("%02x", data_buffer[idx]);
                idx ++;
            }
            APP_LOG_RAW_INFO("\r\n");
        }

        // Process the received data
        if(ble_bt_uart_irq_handler(data_buffer, data_len) && (rx_data_ref_len > data_len) && (data_len < (2*BT_MAX_RX_LEN)))
        {
            if (ref_data_buffer != NULL)
            {
                sys_free(ref_data_buffer);
                ref_data_buffer = NULL;
            }

            //reserve to combine with next data
            ref_data_buffer = data_buffer;
            rx_data_ref_offset = data_len;

            APP_LOG_DEBUG("begin data process : %d %d", rx_data_ref_len, data_len);
        }
        else
        {
            sys_free(data_buffer);
            data_buffer = NULL;
        }
    }

#ifdef BLE_BT_OTA_SUPPORT
    app_bt_ota_procedure();
#endif
}

void ble_bt_interface_init(void)
{
    ble_bt_resetpin_init();
    bt_ble_sync_gpiote_init();
#ifdef BLE_BT_OTA_SUPPORT
    app_ota_tim_init();
#endif
    ring_buffer_init(&s_rx_ring_buffer, s_uart_ring_buffer_data, WATCH_DEMO_UART_BUFF_RX_LEN);
    ble_bt_uart_init();
    bt_sniff_state = true;
    app_bt_reset_hw();
    delay_ms(100);

    app_bt_register_ble_adv_ptr(fast_advertising_start);
}

void bt_debug_on_enter_debug_mode(void)
{
    bt_debug_set_buffer(s_debug_uart_tx_buffer, DEBUG_TX_BUFF_SIZE, s_debug_uart_rx_buffer, DEBUG_RX_BUFF_SIZE);
    bt_debug_set_bt_transmit_func(watch_uart_data_tx_sync_send);
}

void bt_debug_open_to_receive(void)
{
    app_uart_abort(WATCH_DEMO_UART_ID);
    memset(s_uart_rx_buffer, 0, WATCH_DEMO_UART_BUFF_RX_LEN);
    app_uart_receive_async(WATCH_DEMO_UART_ID, s_uart_rx_buffer, WATCH_DEMO_UART_BUFF_RX_LEN);
}
