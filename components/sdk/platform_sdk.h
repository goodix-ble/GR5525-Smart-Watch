/**
 ******************************************************************************
 *
 * @file platform_sdk.h
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


/**
 * @addtogroup SYSTEM
 * @{
 */
 /**
  @addtogroup Plat_SDK Platform SDK
  @{
  @brief Definitions and prototypes for the Platform SDK
 */

#ifndef _PLATFORM_SDK_H
#define _PLATFORM_SDK_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "system_gr55xx.h"
#include "gr55xx_hal_def.h"
#include "gr55xx_hal.h"

/**@brief Clock calibration macro. */
#define XO_CRYSTAL_FREQ             32000000  //XO 32MHz
#define CLK_CAL_STAT_BUSY           0x00
#define CLK_CAL_STAT_DONE           0x01
#define CLK_CAL_STAT_OVERFLOW       0x100


/**@addtogroup PlAT_SDK_ENUM Enumerations
 * @{ */

/**@brief system clock and run mode. */
typedef enum
{
   XIP_64M = 0,            /**< XIP 64M. */
   XIP_48M,                /**< XIP 48M. */
   XIP_XO16M,              /**< XIP XO 16M. */
   XIP_24M,                /**< XIP 24M. */
   XIP_16M,                /**< XIP 16M. */
   XIP_32M,                /**< XIP 32M. */
   MIRROR_64M,             /**< MIRROR 64M. */
   MIRROR_48M,             /**< MIRROR 48M. */
   MIRROR_XO16M,           /**< MIRROR X) 16M. */
   MIRROR_24M,             /**< MIRROR 24M. */
   MIRROR_16M,             /**< MIRROR 16M. */
   MIRROR_32M,             /**< MIRROR 32M. */
} table_idx_t;

/**@brief sdk clock type. */
typedef enum
{
    RNG_OSC_CLK = 0,       /**< RNG OSC CLOCK. */
    RTC_OSC_CLK,           /**< RTC OSC CLOCK. */
    RNG_OSC_CLK2,          /**< RNG OSC CLOCK2. */
} sdk_clock_type_t;


/**@brief memory power setting mode. */
typedef enum
{
   MEM_POWER_FULL_MODE = 0,   /**< Full mode. */
   MEM_POWER_AUTO_MODE,       /**< Auto mode. */
} mem_power_t;
/** @} */

/** @addtogroup PLAT_SDK_FUNCTIONS Functions
 * @{ */

/**@brief clock calibration notify callback. */
typedef void (*clock_calib_notify_cb_t)(float SlowClockFreq);

/**
 ****************************************************************************************
 * @brief   platform sdk init function.
 * @retval :  void
 ****************************************************************************************
 */
void platform_sdk_init(void);

/**
 ****************************************************************************************
 * @brief  set the memory power state to full. This function is mainly used in some
 *         scenarios where all SRAMs need to be powered on
 * @retval : void
 ****************************************************************************************
 */
void mem_pwr_mgmt_full_power_set(void);

/**
 ****************************************************************************************
 * @brief  Set the memory power management mode, which can be automatic mode or full power on mode.
 * @param[in] mem_pwr_mode : MEM_POWER_FULL_MODE or MEM_POWER_AUTO_MODE.
 * @retval : void
 ****************************************************************************************
 */
void mem_pwr_mgmt_mode_set(mem_power_t mem_pwr_mode);

/**
 ****************************************************************************************
 * @brief  Control the memory power supply by specifying start address and length.
 * @param[in] start_addr : the start address of memory that user want to config
 * @param[in] size       : the size of memory that user want to config
 * @retval : void
 ****************************************************************************************
 */
void mem_pwr_mgmt_mode_set_from(uint32_t start_addr, uint32_t size);

/**
 ****************************************************************************************
 * @brief  memory check process
 * @retval : void
 ****************************************************************************************
 */
void mem_pwr_mgmt_init(void);

/**
 ****************************************************************************************
 * @brief  Enable patch function.
 * @param[in] table_idx :  Start Index Number.
 * @param[in] dur_offset   :  duration setting.
 * @param[in] ext_offset   :  ext wakeup setting.
 * @param[in] osc_offset   :  pre-wakeup setting.
 * @retval :  void
 ****************************************************************************************
 */
void system_lp_table_update_twval(table_idx_t table_idx, int16_t dur_offset, int16_t ext_offset, int16_t osc_offset);

/**
 ****************************************************************************************
 * @brief  Leakage Protection for Chip's IO, Only used in Bali A0.
 * @retval :  void
 ****************************************************************************************
 */
void system_io_leakage_protect(void);

/**
 ****************************************************************************************
 * @brief  Platform low power clock init function.
 * @param[in] sys_clock:  System clock.
 * @param[in] clock     :  External RTC setting or internal RNG/RNG2 setting.
 * @param[in] accuracy  :  Low speed clock accuracy.
 * @param[in] xo_offset :  Clock calibration parameter.
 * @retval :  void
 ****************************************************************************************
 */
void platform_clock_init(mcu_clock_type_t sys_clock, sdk_clock_type_t clock, uint16_t accuracy, uint16_t xo_offset);

/**
 ****************************************************************************************
 * @brief  Platform rc calibration function.
 * @retval :  void
 ****************************************************************************************
 */
void platform_rc_calibration(void);

/**
 ****************************************************************************************
 * @brief  Power Management warm boot.
 *
 * @retval :  void
 ****************************************************************************************
 */
void pwr_mgmt_warm_boot(void);

/**
 ****************************************************************************************
 * @brief  Handle Clock calibration interrupt request.
 * @retval :  void
 ****************************************************************************************
 */
void clock_calibration_irq_handler(void);

/**
 ****************************************************************************************
 * @brief  Platform init function.
 * @retval :  void
 ****************************************************************************************
 */
void platform_init(void);

/**
 ****************************************************************************************
 * @brief  PMU init function.
 * @param[in] clock_type :  clock type to be configured.
 * @retval : void
 ****************************************************************************************
 */
void system_pmu_init(mcu_clock_type_t clock_type);

/**
 ****************************************************************************************
 * @brief  PMU deinit function.
 * @retval : void
 ****************************************************************************************
 */
void system_pmu_deinit(void);

/**
 ****************************************************************************************
 * @brief  the first warm boot stage.
 * @retval :  void
 ****************************************************************************************
 */
void warm_boot_first(void);

 /**
 ****************************************************************************************
 * @brief  the second warm boot stage..
 * @retval :  void
 ****************************************************************************************
 */
void warm_boot_second(void);

/**
 ****************************************************************************************
 * @brief  Warm boot process.
 * @retval :  void
 ****************************************************************************************
 */
void warm_boot(void);

/**
 ****************************************************************************************
 * @brief  PMU calibration handler.
 * @param[in] p_arg : no args.
 * @retval :  void
 ****************************************************************************************
 */
void pmu_calibration_handler(void* p_arg);

/**
 ****************************************************************************************
 * @brief  Register the clock calibration completion notification interface.
 * @param[in] calib_notify_cb : Calibration complete callback interface.
 ****************************************************************************************
 */
void clock_calib_notify_register(clock_calib_notify_cb_t calib_notify_cb);

/**
 ****************************************************************************************
 * @brief  start calibration.
 * @param[in] interval : the interval of calibration process.
 * @retval :  void
 ****************************************************************************************
 */
#ifdef ENV_USE_FREERTOS
void system_pmu_calibration_start(void);
#else
void system_pmu_calibration_start(uint32_t interval);
#endif

/**
 ****************************************************************************************
 * @brief  stop calibration.
 * @retval :  void
 ****************************************************************************************
 */
void system_pmu_calibration_stop(void);

/**
 ****************************************************************************************
 * @brief  write flash QE
 * @retval : hal status
 ****************************************************************************************
 */
hal_status_t platform_flash_enable_quad(exflash_hp_init_t *hp_init);

/** @} */

#endif

/** @} */
/** @} */

