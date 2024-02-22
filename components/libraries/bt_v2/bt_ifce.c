/**
 ****************************************************************************************
 *
 * @file bt_ifce.c
 *
 * @brief Classic Bluetooth Communication Interface
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
#include "bt_ifce.h"

#include <stdbool.h>

#include "app_io.h"
#include "app_log.h"
#include "app_uart.h"
#include "app_uart_dma.h"

#include "bt_ctl_debug_interface.h"

/*
 * DEFINE
 *****************************************************************************************
 */
#define SYNC_SIGNAL_DELAY() delay_ms(1)

#if BT_IFCE_UART_USE_RX_DMA
#define APP_UART_RX(...) app_uart_dma_receive_async(__VA_ARGS__)
#else
#define APP_UART_RX(...) app_uart_receive_async(__VA_ARGS__)
#endif // BT_IFCE_UART_USE_RX_DMA

#if BT_IFCE_UART_USE_TX_DMA
#define APP_UART_TX(...) app_uart_dma_transmit_async(__VA_ARGS__)
#else
#define APP_UART_TX(...) app_uart_transmit_async(__VA_ARGS__)
#endif // BT_IFCE_UART_USE_TX_DMA

#define BT_UART_START_RX()                                                             \
    do                                                                                 \
    {                                                                                  \
        APP_UART_RX(BT_IFCE_UART_ID, s_uart_rx_buffer, sizeof(s_uart_rx_buffer));      \
        s_uart_rx_ongoing = true;                                                      \
        app_io_write_pin(BT_IFCE_SYNC_IO_TYPE, BT_IFCE_SYNC_IO_PIN, APP_IO_PIN_RESET); \
        SYNC_SIGNAL_DELAY();                                                           \
        app_io_write_pin(BT_IFCE_SYNC_IO_TYPE, BT_IFCE_SYNC_IO_PIN, APP_IO_PIN_SET);   \
    } while (0)

#define RETURN_IF_DRV_ERROR(result)    \
    do                                 \
    {                                  \
        if (result != APP_DRV_SUCCESS) \
        {                              \
            return result;             \
        }                              \
    } while (0)

/*
 * LOCAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */
static uint8_t s_uart_tx_buffer[BT_IFCE_UART_TX_MAX_LEN];
static uint8_t s_uart_rx_buffer[BT_IFCE_UART_RX_MAX_LEN];
static uint8_t s_rx_ring_buffer_data[BT_IFCE_UART_RX_MAX_LEN * 2];
static ring_buffer_t s_rx_ring_buffer;

static uint16_t s_pending_data_req = 0;
static bool s_uart_rx_ongoing = false;

static app_uart_params_t s_bt_ifce_uart_param = {
    .id = BT_IFCE_UART_ID,
    .init = {
        .baud_rate =       BT_IFCE_UART_BAUDRATE_NORMAL,
        .data_bits =       BT_IFCE_UART_DATABITS,
        .stop_bits =       BT_IFCE_UART_STOPBITS,
        .parity =          BT_IFCE_UART_PARITY,
        .hw_flow_ctrl =    BT_IFCE_UART_HW_FLOW_CTRL,
        .rx_timeout_mode = BT_IFCE_UART_RX_TIMEOUT,
    },
    .pin_cfg = {
        .rx = {
            .type = BT_IFCE_UART_RX_IO_TYPE,
            .pin =  BT_IFCE_UART_RX_IO_PIN,
            .pull = BT_IFCE_UART_RX_PULL,
            .mux =  BT_IFCE_UART_RX_IO_MUX,
        },
        .tx = {
            .type = BT_IFCE_UART_TX_IO_TYPE,
            .pin =  BT_IFCE_UART_TX_IO_PIN,
            .pull = BT_IFCE_UART_TX_PULL,
            .mux =  BT_IFCE_UART_TX_IO_MUX,
        },
    },
#if BT_IFCE_UART_USE_TX_DMA
    .dma_cfg.tx_dma_instance = BT_IFCE_UART_DMA_INST,
    .dma_cfg.tx_dma_channel =  BT_IFCE_UART_TX_DMA_CHANNEL,
#endif // BT_IFCE_UART_USE_TX_DMA
#if BT_IFCE_UART_USE_RX_DMA
    .dma_cfg.rx_dma_instance = BT_IFCE_UART_DMA_INST,
    .dma_cfg.rx_dma_channel =  BT_IFCE_UART_RX_DMA_CHANNEL,
#endif // BT_IFCE_UART_USE_RX_DMA
};

/*
 * GLOBAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */

/*
 * LOCAL FUNCTION DECLARATION
 *****************************************************************************************
 */

/**
 * @brief Bluetooth Interface UART event callback
 *
 * @param p_evt UART event
 */
static void bt_ifce_uart_evt_handler(app_uart_evt_t *p_evt);

/**
 * @brief Bluetooth Interface indication GPIO event callback
 *
 * @param p_evt IO event
 */
static void app_bt_ifce_io_evt_handler(app_io_evt_t *p_evt);

/*
 * GLOBAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
uint16_t bt_ifce_init()
{
    uint16_t result;

    // Init UART
    app_uart_tx_buf_t tx_buffer = {
        .tx_buf = s_uart_tx_buffer,
        .tx_buf_size = sizeof(s_uart_tx_buffer),
    };

    app_uart_deinit(BT_IFCE_UART_ID);
    result = app_uart_init(&s_bt_ifce_uart_param, bt_ifce_uart_evt_handler, &tx_buffer);
    RETURN_IF_DRV_ERROR(result);
#if BT_IFCE_UART_USE_TX_DMA || BT_IFCE_UART_USE_RX_DMA
    result = app_uart_dma_init(&s_bt_ifce_uart_param);
    RETURN_IF_DRV_ERROR(result);
#endif // BT_IFCE_UART_USE_TX_DMA || BT_IFCE_UART_USE_RX_DMA

    app_io_init_t io_init;

    // Init BT -> BLE Indication IO
    io_init.pin = BT_IFCE_IND_IO_PIN;
    io_init.mode = BT_IFCE_IND_IO_MODE;
    io_init.pull = BT_IFCE_IND_IO_PULL;
    io_init.mux = BT_IFCE_IND_IO_MUX;
    result = app_io_event_register_cb(BT_IFCE_IND_IO_TYPE, &io_init, app_bt_ifce_io_evt_handler, NULL);
    RETURN_IF_DRV_ERROR(result);

    // Init BLE -> BT Synchronization IO
    io_init.pin = BT_IFCE_SYNC_IO_PIN;
    io_init.mode = BT_IFCE_SYNC_IO_MODE;
    io_init.pull = BT_IFCE_SYNC_IO_PULL;
    io_init.mux = BT_IFCE_SYNC_IO_MUX;
    result = app_io_init(BT_IFCE_SYNC_IO_TYPE, &io_init);
    RETURN_IF_DRV_ERROR(result);

    // Init BT Reset IO
    io_init.pin = BT_IFCE_RESET_IO_PIN;
    io_init.mode = BT_IFCE_RESET_IO_MODE;
    io_init.pull = BT_IFCE_RESET_IO_PULL;
    io_init.mux = BT_IFCE_RESET_IO_MUX;
    result = app_io_init(BT_IFCE_RESET_IO_TYPE, &io_init);
    RETURN_IF_DRV_ERROR(result);

    // Init RX Data Ring Buffer
    ring_buffer_init(&s_rx_ring_buffer, s_rx_ring_buffer_data, sizeof(s_rx_ring_buffer_data));

    // Power On
    app_io_write_pin(BT_IFCE_RESET_IO_TYPE, BT_IFCE_RESET_IO_PIN, APP_IO_PIN_SET);

    // Always Trigger SYNC IO to make sure it could come back to life
    bt_ifce_io_wake(BT_SYNC_WAKEUP_PULSE_WIDTH_FROM_ULTRASLEEP);

    return APP_DRV_SUCCESS;
}

ring_buffer_t *bt_ifce_get_rx_ring_buffer()
{
    return &s_rx_ring_buffer;
}

void bt_ifce_hw_reset()
{
    app_io_write_pin(BT_IFCE_RESET_IO_TYPE, BT_IFCE_RESET_IO_PIN, APP_IO_PIN_RESET);
    delay_ms(100);
    app_io_write_pin(BT_IFCE_RESET_IO_TYPE, BT_IFCE_RESET_IO_PIN, APP_IO_PIN_SET);
    APP_UART_RX(BT_IFCE_UART_ID, s_uart_rx_buffer, sizeof(s_uart_rx_buffer));
}

void bt_ifce_send_async(uint8_t *data, uint16_t size)
{
    APP_UART_TX(BT_IFCE_UART_ID, data, size);
}

void bt_ifce_send_sync(uint8_t *data, uint16_t size, uint32_t timeout)
{
    app_uart_transmit_sync(BT_IFCE_UART_ID, data, size, timeout);
}

void bt_ifce_deinit()
{
    // Stop any UART activity
    app_uart_abort(BT_IFCE_UART_ID);
    app_uart_deinit(BT_IFCE_UART_ID);

    // Set SYNC and RESET IO to input-pulldown mode to get rid of leakage
    app_io_init_t io_init;

    io_init.pin = BT_IFCE_SYNC_IO_PIN;
    io_init.mode = APP_IO_MODE_INPUT;
    io_init.pull = APP_IO_NOPULL;
    io_init.mux = APP_IO_MUX;
    app_io_init(BT_IFCE_SYNC_IO_TYPE, &io_init);

    io_init.pin = BT_IFCE_RESET_IO_PIN;
    io_init.mode = APP_IO_MODE_INPUT;
    io_init.pull = APP_IO_NOPULL;
    io_init.mux = APP_IO_MUX;
    app_io_init(BT_IFCE_RESET_IO_TYPE, &io_init);

#if BT_IFCE_UART_USE_TX_DMA || BT_IFCE_UART_USE_RX_DMA
    app_uart_dma_deinit(BT_IFCE_UART_ID);
#endif // BT_IFCE_UART_USE_TX_DMA || BT_IFCE_UART_USE_RX_DMA
}

void bt_ifce_io_wake(uint32_t pulse_width)
{
    app_io_write_pin(BT_IFCE_SYNC_IO_TYPE, BT_IFCE_SYNC_IO_PIN, APP_IO_PIN_RESET);
    delay_ms(pulse_width);
    app_io_write_pin(BT_IFCE_SYNC_IO_TYPE, BT_IFCE_SYNC_IO_PIN, APP_IO_PIN_SET);
}

void bt_ifce_force_rx(void)
{
    BT_UART_START_RX();
}

__WEAK void bt_ifce_data_available()
{
}

/*
 * LOCAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */

static void bt_ifce_uart_evt_handler(app_uart_evt_t *p_evt)
{
    if (p_evt->type == APP_UART_EVT_RX_DATA)
    {
        // TODO: OTA Support
        if (bt_debug_mode_is_active())
        {
            app_bt_debug_reponse_handle(s_uart_rx_buffer, p_evt->data.size);
        }
        else
        {
            if (ring_buffer_write(&s_rx_ring_buffer, s_uart_rx_buffer, p_evt->data.size) != p_evt->data.size)
            {
                APP_LOG_ERROR("[BT_IFCE] RingBuffer Full");
                return;
            }

            bt_ifce_data_available();
        }
        memset(s_uart_rx_buffer, 0, p_evt->data.size);
    }
    else if (p_evt->type == APP_UART_EVT_ERROR)
    {
        APP_LOG_WARNING("[BT_IFCE] RX Error: %d", p_evt->data.error_code);
        // Add a pending count to start RX proc again
        s_pending_data_req++;
    }
    else
    {
        return;
    }

    s_uart_rx_ongoing = false;
    if (s_pending_data_req > 0)
    {
        s_pending_data_req--;
        BT_UART_START_RX();
    }
}

static void app_bt_ifce_io_evt_handler(app_io_evt_t *p_evt)
{
    // Data Indication
    if (s_uart_rx_ongoing)
    {
        s_pending_data_req++;
    }
    else
    {
        BT_UART_START_RX();
    }
}
