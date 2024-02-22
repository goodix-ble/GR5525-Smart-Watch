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

 #ifndef _BT_CTL_IND_PARSE_H_
#define _BT_CTL_IND_PARSE_H_

#include "bt_ctl_comm_interface.h"

/*
 * GLOBAL FUNCTION DECLARATION
 *****************************************************************************************
 */

typedef struct
{
    uint8_t v2:4;
    uint8_t v1:4;
    uint8_t v4:4;
    uint8_t v3:4;
    char place_c;
    uint8_t app_v2:4;
    uint8_t app_v1:4;
}BLE_BT_VERSION_T;

enum
{
    VOL_SITE_PHONE,
    VOL_SITE_DEVICE,
};

enum
{
    AUDIO_VOLUME = 0x01,
    HFP_VOLUME,
};

enum{
    SIRI_ON_STATE = 0x01,
    SIRI_OFF_STATE
};

enum
{
    UNMUTE_STATE,
    MUTE_STATE,
};
/**
 *****************************************************************************************
 * @brief Function to hand bt model IND message
 *****************************************************************************************
 */
void app_bt_ind_handle(bt_rx_info *g_bt_rx_info_p);

#endif    /*    _BT_CTL_IND_PARSE_H_    */

