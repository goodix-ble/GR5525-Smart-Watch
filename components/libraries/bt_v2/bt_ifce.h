/**
 ****************************************************************************************
 *
 * @file bt_ifce.h
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

#ifndef __BT_IFCE_H__
#define __BT_IFCE_H__

#include "bt_common.h"
#include "ring_buffer.h"

/**
 * @brief Init BT module hardware
 *
 * @return uint16_t Result of initialization.
 */
uint16_t bt_ifce_init(void);

/**
 * @brief Get ring buffer that contains RX data
 *
 * @return ring_buffer_t* Bluetooth's RX ring buffer
 */
ring_buffer_t *bt_ifce_get_rx_ring_buffer(void);

/**
 * @brief Reset BT module via hardware
 *
 * This is the cleanest way to do BT reset
 */
void bt_ifce_hw_reset(void);

/**
 * @brief  data asynchronously to Bluetooth module.
 *
 * @param[in] data Pointer to data to send.
 * @param[in] size Size of data, in bytes.
 */
void bt_ifce_send_async(uint8_t *data, uint16_t size);

/**
 * @brief data synchronously to Bluetooth module.
 *
 * @param[in] data Pointer to data to send.
 * @param[in] size Size of data, in bytes.
 * @param[in] timeout Maximum time to wait for transmission, in ms.
 */
void bt_ifce_send_sync(uint8_t *data, uint16_t size, uint32_t timeout);

/**
 * @brief Configure all IO's into ultra sleep mode to get minimum power consumption
 */
void bt_ifce_deinit(void);

/**
 * @brief Wake up BT module by pulling down SYNC IO.
 *
 * @param[in] pulse_width Indicates how long should SYNC be pulled down, in ms.
 */
void bt_ifce_io_wake(uint32_t pulse_width);

/**
 * @brief Start RX procedure despite no IND signal is coming
 */
void bt_ifce_force_rx(void);

/**
 * @brief BT data available callback
 * @note This function should not be modified. When the callback is needed,
 * implement this function in user's file.
 */
void bt_ifce_data_available(void);

#endif // __BT_IFCE_H__
