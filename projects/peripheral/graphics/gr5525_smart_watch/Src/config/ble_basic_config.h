/**
 ****************************************************************************************
 *
 * @file ble_basic_config.h
 *
 * @brief BLE Basic configuration file for applications.
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

/*
 * DEFINES
 *****************************************************************************************
 */
#ifndef __BLE_BASIC_CONFIG_H__
#define __BLE_BASIC_CONFIG_H__

// <<< Use Configuration Wizard in Context Menu >>>

// <h> BLE Basic configuration
// <i> Note: The total number of BLE Activities(CONNECTIONS+ADVS+2*PER_ADVS+SYNCS+SCAN) should not exceed the limit 12.

// <o> BLE controller only(use for extern host or HCI uart transport)
// <0=> Support BLE controller and host
// <1=> Support BLE controller only
#ifndef CFG_CONTROLLER_ONLY
#define CFG_CONTROLLER_ONLY             0
#endif

// <o> Support maximum number of BLE profiles <0-64>
// <i> Range: 0-64
#ifndef CFG_MAX_PRFS
#define CFG_MAX_PRFS                    1
#endif

// <o> Support maximum number of bonded devices <1-10>
// <i> Range: 1-10
#ifndef CFG_MAX_BOND_DEVS
#define CFG_MAX_BOND_DEVS               1
#endif

// <o> Support maximum number of BLE Links <1-10>
// <i> Range: 1-10
#ifndef CFG_MAX_CONNECTIONS
#define CFG_MAX_CONNECTIONS             1
#endif

// <o> Support maximum number of BLE Legacy/Extended Advertisings <0-5>
// <i> Range: 0-5
// <i> Note: The total number of BLE Legacy/Extended/Periodic Advertisings should not exceed the limit 5.
#ifndef CFG_MAX_ADVS
#define CFG_MAX_ADVS            1
#endif

// <o> scan support
// <0=> NOT SUPPORT
// <1=> SUPPORT
#ifndef CFG_SCAN_SUPPORT
#define CFG_SCAN_SUPPORT                0
#endif

// <o> master support
// <0=> NOT SUPPORT
// <1=> SUPPORT
#ifndef CFG_MASTER_SUPPORT
#define CFG_MASTER_SUPPORT              0
#endif

// <o> slave support
// <0=> NOT SUPPORT
// <1=> SUPPORT
#ifndef CFG_SLAVE_SUPPORT
#define CFG_SLAVE_SUPPORT               1
#endif

// <o> Legacy pair support
// <0=> NOT SUPPORT
// <1=> SUPPORT
#ifndef CFG_LEGACY_PAIR_SUPPORT
#define CFG_LEGACY_PAIR_SUPPORT         1
#endif

// <o> Sc pair support
// <0=> NOT SUPPORT
// <1=> SUPPORT
#ifndef CFG_SC_PAIR_SUPPORT
#define CFG_SC_PAIR_SUPPORT             1
#endif

// <o> Coc support
// <0=> NOT SUPPORT
// <1=> SUPPORT
#ifndef CFG_COC_SUPPORT
#define CFG_COC_SUPPORT                 0
#endif

// <o> Gatts support
// <0=> NOT SUPPORT
// <1=> SUPPORT
#ifndef CFG_GATTS_SUPPORT
#define CFG_GATTS_SUPPORT               1
#endif

// <o> Gattc support
// <0=> NOT SUPPORT
// <1=> SUPPORT
#ifndef CFG_GATTC_SUPPORT
#define CFG_GATTC_SUPPORT               1
#endif

// <o> connection aoa/aod support
// <0=> NOT SUPPORT
// <1=> SUPPORT
#ifndef CFG_CONN_AOA_AOD_SUPPORT
#define CFG_CONN_AOA_AOD_SUPPORT        0
#endif

// <o> Connectionless aoa/aod support
// <0=> NOT SUPPORT
// <1=> SUPPORT
#ifndef CFG_CONNLESS_AOA_AOD_SUPPORT
#define CFG_CONNLESS_AOA_AOD_SUPPORT    0
#endif

// <o> Ranging support
// <0=> NOT SUPPORT
// <1=> SUPPORT
#ifndef CFG_RANGING_SUPPORT
#define CFG_RANGING_SUPPORT             0
#endif

// <o> MESH support
// <0=> NOT SUPPORT
// <1=> SUPPORT
#ifndef CFG_MESH_SUPPORT
#define CFG_MESH_SUPPORT                0
#endif
// </h>

// <h> LCP support configuration
// <o> LCP support
// <0=> NOT SUPPORT
// <1=> SUPPORT
#ifndef CFG_LCP_SUPPORT
#define CFG_LCP_SUPPORT         0
#endif
// </h>

// <h> ranging memory allocation configuration
// <o> ranging memory allocation
// <0=> NOT ALLOCATE
// <1=> ALLOCATE
#ifndef CFG_RANGING_ALLOCATE
#define CFG_RANGING_ALLOCATE        0
#endif
// </h>

// <h> LCP support configuration
// <o> LCP support
// <0=> NOT SUPPORT
// <1=> SUPPORT
#ifndef CFG_POWER_CONTROL_SUPPORT
#define CFG_POWER_CONTROL_SUPPORT         0
#endif
// </h>

// <<< end of configuration section >>>
#endif //__BLE_BASIC_CONFIG_H__
