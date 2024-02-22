/**
 ****************************************************************************************
 *
 * @file bt_ctl_call_interface.h
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

 #ifndef _BT_CTL_CALL_INTERFACE_H_
#define _BT_CTL_CALL_INTERFACE_H_

#include "bt_ctl_comm_interface.h"

typedef enum {
    CALL_INCOMING_STATE,
    CALL_ACCEPT_STATE,
    CALL_HOLD_STATE,
    CALL_END_STATE,
    CALL_DIAL_STATE,
    CALL_DIAL_CANCEL_STATE,
    CALL_REJECT_STATE,
}BT_CALLING_STATUS_T;

typedef enum {
    RING_NORMAL,
    RING_DND_MODE,
}BT_INCOMING_RING_STATE_T;


/*
 * GLOBAL FUNCTION DECLARATION
 *****************************************************************************************
 */

/**
 *****************************************************************************************
 * @brief Function for getting phone book .
 *****************************************************************************************
 */
extern void app_bt_call_address_book_get(void);

/**
 *****************************************************************************************
 * @brief Function for accept the incoming call.
 *****************************************************************************************
 */
extern void app_bt_call_accept(void);

/**
 *****************************************************************************************
 * @brief Function for hang up current call.
 *****************************************************************************************
 */
extern void app_bt_call_end(void);

/**
 *****************************************************************************************
 * @brief Function for reject the incoming call.
 *****************************************************************************************
 */
extern void app_bt_call_reject(void);

/**
 *****************************************************************************************
 * @brief Function for re-dail last call number.
 *****************************************************************************************
 */
extern void app_bt_call_redial(void);

/**
 *****************************************************************************************
 * @brief Function for call volume level set.
 *
 * @param[in] vol_level     : call volume level set
 *
 *****************************************************************************************
 */
extern void app_bt_call_vol_level_set(uint8_t vol_level);

/**
 *****************************************************************************************
 * @brief Function for call volume level get.
 *
 *****************************************************************************************
 */
extern void app_bt_call_vol_level_get(void);

/**
 *****************************************************************************************
 * @brief Function for call volume gain set.
 *
 * @param[in] vol_level     : call volume level to set,                 0~16
 * @param[in] ana_gain     : call volume level analog gain to set, 0~7(0db~7db)
 * @param[in] dig_gain      : call volume level digital to set,        1~45(-44db ~ 0db), 0 = Mute
 *
 *****************************************************************************************
 */
extern void app_bt_call_vol_gain_set(uint8_t vol_level, uint8_t ana_gain, uint8_t dig_gain);

/**
 *****************************************************************************************
 * @brief Function for call volume gain get.
 *
 *****************************************************************************************
 */
extern void app_bt_call_vol_gain_get(void);


/**
 *****************************************************************************************
 * @brief Function for switch between phone and headphones.
 *****************************************************************************************
 */
extern void app_bt_call_vol_switch(void);

/**
 *****************************************************************************************
 * @brief Function for three-way calling control
 *
 * @param[in] first_state_set     : set first call state.
 * @param[in] second_state_set: set second call state.
 *
 *****************************************************************************************
 */
extern void app_bt_three_way_call_ctl(BT_CALLING_STATUS_T first_state_set, BT_CALLING_STATUS_T second_state_set);

/**
 *****************************************************************************************
 * @brief Function for three-way calling switch calling lines
 *****************************************************************************************
 */
extern void app_bt_three_way_call_switch(void);

/**
 *****************************************************************************************
 * @brief Function for calling number get
 *****************************************************************************************
 */
extern void app_bt_calling_number_get(void);

/**
 *****************************************************************************************
 * @brief Function for place a call
 *
 * @param[in] number: call number to place, number shall be char : '1', '2', ...
 * @param[in] length: call number length.
 *
 *****************************************************************************************
 */
extern void app_bt_place_call_number(uint8_t *number, uint8_t lenth);

/**
 *****************************************************************************************
 * @brief Function for set DTMF command
 *
 * @param[in] buf: shall be char : '*', '#', '1', '2', ...
 * @param[in] length: buf length.
 *
 *****************************************************************************************
 */
extern void app_bt_call_dtmf_set(uint8_t *buf, uint8_t length);

#endif

