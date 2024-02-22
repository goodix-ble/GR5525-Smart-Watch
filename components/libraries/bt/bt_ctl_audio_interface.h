/**
 ****************************************************************************************
 *
 * @file bt_ctl_audio_interface.h
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

 #ifndef _BT_CTL_AUDIO_INTERFACE_H_
#define _BT_CTL_AUDIO_INTERFACE_H_

#include "bt_ctl_comm_interface.h"

typedef enum {
    AUDIO_PLAY_STATE = 1,
    AUDIO_PAUSE_STATE,
}BT_AUDIO_STATUS_T;

/*
 * GLOBAL FUNCTION DECLARATION
 *****************************************************************************************
 */

/**
 *****************************************************************************************
 * @brief Function to connect or disconnect a2dp
 *****************************************************************************************
 */
extern void app_bt_a2dp_status_set(void);

/**
 *****************************************************************************************
 * @brief Function to play start or stop .
 *****************************************************************************************
 */
extern void app_bt_avrcp_status_set(void);

/**
 *****************************************************************************************
 * @brief Function to play next song.
 *****************************************************************************************
 */
extern void app_bt_avrcp_play_next(void);

/**
 *****************************************************************************************
 * @brief Function to play prev song.
 *****************************************************************************************
 */
extern void app_bt_avrcp_play_prev(void);

/**
 *****************************************************************************************
 * @brief Function to play rewind.
 * some palyer may not support
 *****************************************************************************************
 */
extern void app_bt_avrcp_play_rewind(void);

/**
 *****************************************************************************************
 * @brief Function to play forward.
 *****************************************************************************************
 */
extern void app_bt_avrcp_play_forward(void);

/**
 *****************************************************************************************
 * @brief Function for volume level set.
 *
 * @param[in] vol_level     : volume level set
 *
 *****************************************************************************************
 */
extern void app_bt_vol_level_set(uint8_t vol_level);

/**
 *****************************************************************************************
 * @brief Function for volume level get.
 *****************************************************************************************
 */
extern void app_bt_vol_level_get(void);

/**
 *****************************************************************************************
 * @brief Function for volume gain set.
 *
 * @param[in] vol_level     : audio volume level to set,                 0~16
 * @param[in] ana_gain     : audio volume level analog gain to set, 0~7(0db~7db)
 * @param[in] dig_gain      : audio volume level digital to set,        1~45(-44db ~ 0db), 0 = Mute
 *
 *****************************************************************************************
 */
extern void app_bt_vol_gain_set(uint8_t vol_level, uint8_t ana_gain, uint8_t dig_gain);

/**
 *****************************************************************************************
 * @brief Function for volume gain get.
 *****************************************************************************************
 */
extern void app_bt_vol_gain_get(void);

/**
 *****************************************************************************************
 * @brief Function for volume information get, include singer, album, the length of the song, and so on.
 *****************************************************************************************
 */
extern void app_bt_audio_info_get(void);

#endif

