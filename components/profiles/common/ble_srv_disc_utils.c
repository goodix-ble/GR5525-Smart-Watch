/**
 *******************************************************************************
 *
 * @file  ble_srv_disc_utils.c
 *
 * @brief Implementation of ble_srv_disc_utils
 *
 *******************************************************************************
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
 *******************************************************************************
 */
#include "ble_srv_disc_utils.h"

/*
 * LOCAL VARIABLE DEFINITIONS
 *******************************************************************************
 */
static ble_srv_disc_state_t srv_disc_procedure[BLE_SRV_DISC_PROC_NB];

/*
 * GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 */
void ble_srv_disc_proc_state_set(uint8_t srv_disc_proc_id, ble_srv_disc_state_t srv_disc_state)
{
    if (BLE_SRV_DISC_PROC_MAX <= srv_disc_proc_id)
    {
        return;
    }

    srv_disc_procedure[srv_disc_proc_id] = srv_disc_state;
}

ble_srv_disc_state_t ble_srv_disc_proc_state_get(uint8_t srv_disc_proc_id)
{
    return srv_disc_procedure[srv_disc_proc_id];
}

