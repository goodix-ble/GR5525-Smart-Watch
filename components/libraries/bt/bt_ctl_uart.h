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

 #ifndef _BT_CTL_UART_H_
#define _BT_CTL_UART_H_

#include "gr_includes.h"
#include "app_uart.h"
#include "bt_ctl_comm_interface.h"

/*******HCI UART IO CONFIG***********************/
#define WATCH_DEMO_UART_ID                 APP_UART_ID_1
#define WATCH_DEMO_UART_TX_IO_TYPE         APP_IO_TYPE_GPIOB
#define WATCH_DEMO_UART_RX_IO_TYPE         APP_IO_TYPE_GPIOB
#define WATCH_DEMO_UART_TX_PIN             APP_IO_PIN_14
#define WATCH_DEMO_UART_RX_PIN             APP_IO_PIN_10
#define WATCH_DEMO_UART_TX_PINMUX          APP_IO_MUX_1
#define WATCH_DEMO_UART_RX_PINMUX          APP_IO_MUX_1
#define WATCH_DEMO_UART_RX_PULL            APP_UART_RX_PULL
#define WATCH_DEMO_UART_TX_PULL            APP_UART_TX_PULL
#define WATCH_DEMO_UART_TYPE               APP_UART_TYPE_INTERRUPT

/******* THE DEFAULT UART BAUDRATE *******/
#define WATCH_DEMO_UART_BAUDRATE_ORI       2000000
/******* THE STAY ROM STATE, UART BAUDTATE. DO NO MODIFY *******/
#define WATCH_DEMO_UART_BAUDRATE_STAY_ROM  115200
/******* THE OTA STATE, CAN BE CONFIGURED BY CUSTOM *******/
#define WATCH_DEMO_UART_BAUDRATE_OTA       460800//1000000

#define WATCH_DEMO_UART_BUFF_RX_LEN     1024
#define WATCH_DEMO_UART_BUFF_TX_LEN     200


/*
 * GLOBAL FUNCTION DECLARATION
 *****************************************************************************************
 */
/**
 *****************************************************************************************
 * @brief Function for bt uart tx.
 *
 * @param[in] buf: tx buffer to bt device.
 * @param[in] len: buffer length.
 *
 ****************************************************************************************
  */
extern void watch_uart_data_tx_send(uint8_t *buf, uint16_t length);

/**
 *****************************************************************************************
 * @brief Function for bt uart tx, only use in upgrade FW.
 *
 * @param[in] buf: tx buffer to bt device.
 * @param[in] len: buffer length.
 *
 ****************************************************************************************
  */
extern void watch_uart_data_tx_sync_send(uint8_t *buf, uint16_t length);

/**
 *****************************************************************************************
 * @brief handle uart rx buffer
 *****************************************************************************************
 */
extern void handle_uart_ring_buffer_data(void);

/**
 *****************************************************************************************
 * @brief ble <-->BT receive data
 *****************************************************************************************
 */
extern void ble_bt_uart_recv_async(void);

/**
 *****************************************************************************************
 * @brief uart init
 *****************************************************************************************
 */
extern void ble_bt_uart_init(void);

/**
 *****************************************************************************************
 * @brief handle uart rx buffer
 *****************************************************************************************
 */
extern void ble_bt_resetpin_deinit(void);

/**
 *****************************************************************************************
 * @brief init ble<==>bt interface
 *****************************************************************************************
 */
extern void ble_bt_interface_init(void);

#endif /*  _BT_CTL_UART_H_  */


