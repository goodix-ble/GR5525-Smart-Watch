/**
 ****************************************************************************************
 *
 * @file    gr55xx_ll_aon_pmu.h
 * @author  BLE Driver Team
 * @brief   Header file containing functions prototypes of PMU LL library.
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
 ****************************************************************************************
 */

/** @addtogroup PERIPHERAL Peripheral Driver
  * @{
  */

/** @addtogroup LL_DRIVER LL Driver
  * @{
  */

/** @defgroup LL_PMU PMU LL Module Driver
  * @brief PMU LL module driver.
  * @{
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GR55XX_LL_PMU_H_
#define __GR55XX_LL_PMU_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "gr55xx_hal.h"

/** @defgroup AON_PMU_LL_DRIVER_FUNCTIONS Functions
  * @{
  */
/**
  * @brief  Enable the RTC
  *
  *  Register|BitsName
  *  --------|--------
  *  RF_REG_0 | RTC_EN
  *
  * @retval None
  *
  */
__STATIC_INLINE void ll_aon_pmu_enable_rtc(void)
{
    SET_BITS(AON_PMU->RF_REG_0, AON_PMU_RF_REG_0_RTC_EN);
}

/**
  * @brief  Enable the RTC and bypass GM
  *
  *  Register|BitsName
  *  --------|--------
  *  RF_REG_0 | RTC_EN
  *           | CGM_MODE
  *
  * @retval None
  *
  */
__STATIC_INLINE void ll_aon_pmu_enable_rtc_cgm(void)
{
    SET_BITS(AON_PMU->RF_REG_0, AON_PMU_RF_REG_0_RTC_EN_GM);
}

/**
  * @brief  Disable the RTC
  *
  *  Register|BitsName
  *  --------|--------
  *  RF_REG_0 | RTC_EN
  *
  */
__STATIC_INLINE void ll_aon_pmu_disable_rtc(void)
{
    CLEAR_BITS(AON_PMU->RF_REG_0, AON_PMU_RF_REG_0_RTC_EN);
}

/**
  * @brief  Set RTC GM
  *
  *  Register|BitsName
  *  --------|--------
  *  RF_REG_0 | EN
  *
  * @param value: The rtc gm value.
  *
  */
__STATIC_INLINE void ll_aon_pmu_set_rtc_gm(uint32_t value)
{
    MODIFY_REG(AON_PMU->RF_REG_0, AON_PMU_RF_REG_0_RTC_GM, (value << AON_PMU_RF_REG_0_RTC_GM_Pos));
}

/**
  * @brief  Set lv,default is set to 1.8V,LSB = 8.5mv
  *
  *  Register|BitsName
  *  --------|--------
  *  RF_REG_0 | EN
  *
  * @param value: The io ldo vout value.
  *
  */
__STATIC_INLINE void ll_aon_pmu_set_io_ldo_vout(uint32_t value)
{
    MODIFY_REG(AON_PMU->RF_REG_0, AON_PMU_RF_REG_0_IO_LDO_REG1, (value << AON_PMU_RF_REG_0_IO_LDO_REG1_Pos));
}

/**
  * @brief  Get the io ldo vout
  *
  *  Register|BitsName
  *  --------|--------
  *  RF_REG_0 | vref_sel_lv_6_0
  *
  * @retval The io ldo vout value..
  *
  */
__STATIC_INLINE uint32_t ll_aon_pmu_get_io_ldo_vout(void)
{
    return (READ_BITS(AON_PMU->RF_REG_0, AON_PMU_RF_REG_0_IO_LDO_REG1) >> AON_PMU_RF_REG_0_IO_LDO_REG1_Pos);
}

/**
  * @brief  Set stb io ldo
  *
  *  Register|BitsName
  *  --------|--------
  *  RF_REG_3 | EN
  *
  * @param value: The io ldo vout value.
  *
  */
__STATIC_INLINE void ll_aon_pmu_set_stb_io_ldo_vout(uint32_t value)
{
    MODIFY_REG(AON_PMU->RF_REG_3, AON_PMU_RF_REG_3_IO_LDO_REG2, ((value | 0x03) << AON_PMU_RF_REG_3_IO_LDO_REG2_Pos));
}

/**
  * @brief  Set retention level
  *
  *  Register|BitsName
  *  --------|--------
  *  RF_REG_0 | ctrl_ret
  *
  * @param value: The retention level value.
  *
  */
__STATIC_INLINE void ll_aon_pmu_set_retention_level(uint32_t value)
{
    MODIFY_REG(AON_PMU->RF_REG_0, AON_PMU_RF_REG_0_CTRL_RET, (value << AON_PMU_RF_REG_0_CTRL_RET_Pos));
}


/**
  * @brief  Get retention level
  *
  *  Register|BitsName
  *  --------|--------
  *  RF_REG_0 | ctrl_ret
  *
  * @retval The current retention level.
  *
  */
__STATIC_INLINE uint32_t ll_aon_pmu_get_retention_level(void)
{
    return (READ_BITS(AON_PMU->RF_REG_0, AON_PMU_RF_REG_0_CTRL_RET) >> AON_PMU_RF_REG_0_CTRL_RET_Pos);
}

/**
  * @brief  Set dcdc ref_cntrl_b_lv_3_0,vreg defaulted to 1.1V.
  *
  *  Register|BitsName
  *  --------|--------
  *  PMU_DCDC_VREF | DCDC_VREF_REG_VAL
  *
  * @param value: the dcdc vreg value.
  *
  */
__STATIC_INLINE void ll_aon_pmu_set_dcdc_vreg(uint32_t value)
{
    MODIFY_REG(AON_PMU->PMU_DCDC_VREF, AON_PMU_DCDC_VREF_REG_VAL, (value << AON_PMU_DCDC_VREF_REG_VAL_Pos));
}

/**
  * @brief  Get dcdc vreg
  *
  *  Register|BitsName
  *  --------|--------
  *  PMU_DCDC_VREF | DCDC_VREF_REG_VAL
  *
  * @retval The dcdc vreg value.
  *
  */
__STATIC_INLINE uint32_t ll_aon_pmu_get_dcdc_vreg(void)
{
    return (READ_BITS(AON_PMU->PMU_DCDC_VREF, AON_PMU_DCDC_VREF_REG_VAL) >> AON_PMU_DCDC_VREF_REG_VAL_Pos);
}

/**
  * @brief  Enable the io ldo bypass
  *
  *  Register|BitsName
  *  --------|--------
  *  RF_REG_3 | BYPASS_EN
  *
  */
__STATIC_INLINE void ll_aon_pmu_enable_io_ldo_bypass(void)
{
    SET_BITS(AON_PMU->RF_REG_0, AON_PMU_RF_REG_0_IO_LDO_BYPASS);
}

/**
  * @brief  Disable the io ldo bypass
  *
  *  Register|BitsName
  *  --------|--------
  *  RF_REG_3 | BYPASS_EN
  *
  */
__STATIC_INLINE void ll_aon_pmu_disable_io_ldo_bypass(void)
{
    CLEAR_BITS(AON_PMU->RF_REG_0, AON_PMU_RF_REG_0_IO_LDO_BYPASS);
}

/**
  * @brief  Set dig ldo coarse and fine code
  *
  *  Register|BitsName
  *  --------|--------
  *  RF_REG_4 | DIG_LDO_EN
  *
  * @param coarse_code: The dig ldo coarse.
  * @param fine_code: The fine code value.
  *
  */
__STATIC_INLINE void ll_aon_pmu_set_dig_ldo_coarse_fine_code(uint32_t coarse_code, uint32_t fine_code)
{
    MODIFY_REG(AON_PMU->RF_REG_4, AON_PMU_RF_REG_4_CORE_VO_SEL,   ((coarse_code & 0x03) << AON_PMU_RF_REG_4_CORE_VO_SEL_Pos));
    MODIFY_REG(AON_PMU->PMU_CORE_LDO_VREF, AON_PMU_CORE_LDO_VREF, ((fine_code & 0xF)    << 9U));
}

/**
  * @brief  Get dig ldo coarse code
  *
  *  Register|BitsName
  *  --------|--------
  *  RF_REG_4 | core_vo_sel
  *
  */
__STATIC_INLINE uint32_t ll_aon_pmu_get_dig_ldo_coarse_code(void)
{
    return (READ_BITS(AON_PMU->RF_REG_4, AON_PMU_RF_REG_4_CORE_VO_SEL) >> AON_PMU_RF_REG_4_CORE_VO_SEL_Pos);
}

/**
  * @brief  Get dig ldo fine code
  *
  *  Register|BitsName
  *  --------|--------
  *  PMU_CORE_LDO_VREF | reg_aon_pmu_core_ldo_vref
  *
  */
__STATIC_INLINE uint32_t ll_aon_pmu_get_dig_ldo_fine_code(void)
{
    return (READ_BITS(AON_PMU->PMU_CORE_LDO_VREF, AON_PMU_CORE_LDO_VREF) >> 9U);
}



/**
  * @brief  Set dig ldo out
  *
  *  Register|BitsName
  *  --------|--------
  *  RF_REG_4 | DIG_LDO_EN
  *
  * @param value: The dig ldo out value.
  *
  */
__STATIC_INLINE void ll_aon_pmu_set_dig_ldo_out(uint32_t value)
{
    MODIFY_REG(AON_PMU->RF_REG_4, AON_PMU_RF_REG_4_CORE_VO_SEL, (((value >> 6) & 0x03) << AON_PMU_RF_REG_4_CORE_VO_SEL_Pos));
    MODIFY_REG(AON_PMU->PMU_CORE_LDO_VREF, AON_PMU_CORE_LDO_VREF_REG_SEL, (((value >> 0)& 0x01) << AON_PMU_CORE_LDO_VREF_REG_SEL_Pos));
    MODIFY_REG(AON_PMU->PMU_CORE_LDO_VREF, AON_PMU_CORE_LDO_VREF, (((value >> 1)& 0x1F) << AON_PMU_CORE_LDO_VREF_Pos));
}

/**
  * @brief  Get dig ldo out
  *
  *  Register|BitsName
  *  --------|--------
  *  RF_REG_4 | DIG_LDO_EN
  *
  */
__STATIC_INLINE uint32_t ll_aon_pmu_get_dig_ldo_out(void)
{
    return (READ_BITS(AON_PMU->PMU_CORE_LDO_VREF, AON_PMU_CORE_LDO_VREF) >> AON_PMU_CORE_LDO_VREF_Pos);
}


/**
  * @brief  Enable the dig ldo bypass
  *
  *  Register|BitsName
  *  --------|--------
  *  RF_REG_4 | BYPASS_EN
  *
  */
__STATIC_INLINE void ll_aon_pmu_enable_dig_ldo_bypass(void)
{
    SET_BITS(AON_PMU->RF_REG_4, AON_PMU_RF_REG_4_DIG_LDO_BYPASS_EN);
}

/**
  * @brief  Disable the dig ldo bypass
  *
  *  Register|BitsName
  *  --------|--------
  *  RF_REG_4 | BYPASS_EN
  *
  */
__STATIC_INLINE void ll_aon_pmu_disable_dig_ldo_bypass(void)
{
    CLEAR_BITS(AON_PMU->RF_REG_4, AON_PMU_RF_REG_4_DIG_LDO_BYPASS_EN);
}

/**
  * @brief  Set the dig ldo bypass
  *
  *  Register|BitsName
  *  --------|--------
  *  RF_REG_4 | BYPASS_EN
  *
  * @param enable: Aon pmu set dig ldo bypass enable.
  * 
  */
__STATIC_INLINE void ll_aon_pmu_set_dig_ldo_bypass(bool enable)
{
    MODIFY_REG(AON_PMU->RF_REG_4, AON_PMU_RF_REG_4_DIG_LDO_BYPASS_EN, (enable << AON_PMU_RF_REG_4_DIG_LDO_BYPASS_EN_Pos));
}

/**
  * @brief  Get the dig ldo bypass
  *
  *  Register|BitsName
  *  --------|--------
  *  RF_REG_4 | BYPASS_EN
  *
  * @retval The dig ldo bypass enable value.
  *
  */
__STATIC_INLINE uint32_t ll_aon_pmu_get_dig_ldo_bypass(void)
{
    return (READ_BITS(AON_PMU->RF_REG_4, AON_PMU_RF_REG_4_DIG_LDO_BYPASS_EN) >> AON_PMU_RF_REG_4_DIG_LDO_BYPASS_EN_Pos);
}


/**
  * @brief  Get dig ldo out
  *
  *  Register|BitsName
  *  --------|--------
  *  RF_REG_4 | DIG_LDO_EN
  *
  */
__STATIC_INLINE void ll_aon_pmu_disable_sysldo(void)
{
    MODIFY_REG(AON_PMU->FS_REG_0, AON_PMU_FS_REG_0_DCDC_EN, (0 << AON_PMU_FS_REG_0_DCDC_EN_Pos));
    MODIFY_REG(AON_PMU->FS_REG_0, AON_PMU_FS_REG_0_SYSLDO_EN, (0 << AON_PMU_FS_REG_0_SYSLDO_EN_Pos));
}

/**
  * @brief  Set clk period
  *
  *  Register|BitsName
  *  --------|--------
  *  RF_REG_4 | CLK_PERIOD
  *
  * @param value: The clock period value.
  * @retval None
  *
  */
__STATIC_INLINE void ll_aon_pmu_set_clk_period(uint32_t value)
{
    MODIFY_REG(AON_PMU->RF_REG_4, AON_PMU_RF_REG_4_CLK_PERIOD, (value << AON_PMU_RF_REG_4_CLK_PERIOD_Pos));
}

/**
  * @brief Enables clock injection from XO to ring oscillator.
  *
  *  Register|BitsName
  *  --------|--------
  *  RF_REG_1 | EN_INJ_ON
  *
  * @param value: The clock period value.
  * @retval None
  *
  */
__STATIC_INLINE void ll_aon_pmu_set_clk_inject(uint32_t value)
{
    MODIFY_REG(AON_PMU->RF_REG_1, AON_PMU_RF_REG_1_EN_INJ, (value << AON_PMU_RF_REG_1_EN_INJ_Pos));
}

/**
  * @brief  Enable the dcdc ton startup
  *
  *  Register|BitsName
  *  --------|--------
  *  DCDC_LDO_REG_0 | TON_STARTUP
  *
  */
__STATIC_INLINE void ll_aon_pmu_enable_ton_startup_overide(void)
{
    SET_BITS(AON_PMU->RF_REG_1, AON_PMU_RF_REG_1_TON_STARTUP);
}


/**
  * @brief Set the dcdc power source.
  *
  *  Register|BitsName
  *  --------|--------
  *  RF_REG_1 | vddint_sel
  *
  * @param value: Dcdc power source.
  * @retval None
  *
  */
__STATIC_INLINE void ll_aon_pmu_set_dcdc_pwr_src(uint32_t value)
{
    MODIFY_REG(AON_PMU->RF_REG_1, AON_PMU_RF_REG_1_DCDC_PWR_SRC_Msk, (value << AON_PMU_RF_REG_1_DCDC_PWR_SRC_Pos));
}


/**
  * @brief  Set the rtc cur cap
  *
  *  Register|BitsName
  *  --------|--------
  *  RC_RTC_REG_0 | RTC_CAP
  *
  * @param value: The rtc current cap value.
  *
  */
__STATIC_INLINE void ll_aon_pmu_set_rtc_cs(uint32_t value)
{
    MODIFY_REG(AON_PMU->RC_RTC_REG_0, AON_PMU_RC_RTC_REG0_RTC_CS, (value << AON_PMU_RC_RTC_REG0_RTC_CS_Pos));
}

/**
  * @brief  Set the rtc cur cap
  *
  *  Register|BitsName
  *  --------|--------
  *  RC_RTC_REG_0 | RTC_CAP
  *
  * @param value: The rtc current cap value.
  *
  */
__STATIC_INLINE void ll_aon_pmu_set_rtc_cap(uint32_t value)
{
    MODIFY_REG(AON_PMU->RC_RTC_REG_0, AON_PMU_RC_RTC_REG0_RTC_CAP, (value << AON_PMU_RC_RTC_REG0_RTC_CAP_Pos));
}

/**
  * @brief  Get the rtc cur cap
  *
  *  Register|BitsName
  *  --------|--------
  *  RC_RTC_REG_0 | RTC_CAP
  *
  * @retval The rtc current cap value.
  *
  */
__STATIC_INLINE uint32_t ll_aon_pmu_get_rtc_cap(void)
{
    return (READ_BITS(AON_PMU->RC_RTC_REG_0, AON_PMU_RC_RTC_REG0_RTC_CAP) >> AON_PMU_RC_RTC_REG0_RTC_CAP_Pos);
}

/**
  * @brief  Enable the RCOSC
  *
  *  Register|BitsName
  *  --------|--------
  *  RC_RTC_REG_0 | RCOSC
  *
  */
__STATIC_INLINE void ll_aon_pmu_enable_rcosc(void)
{
    SET_BITS(AON_PMU->RC_RTC_REG_0, AON_PMU_RC_RTC_REG0_RCOSC);
}

/**
  * @brief  Disable the RCOSC
  *
  *  Register|BitsName
  *  --------|--------
  *  RC_RTC_REG_0 | RCOSC
  *
  */
__STATIC_INLINE void ll_aon_pmu_disable_rcosc(void)
{
    CLEAR_BITS(AON_PMU->RC_RTC_REG_0, AON_PMU_RC_RTC_REG0_RCOSC);
}

/**
  * @brief  modify lpd active
  *
  *  Register|BitsName
  *  --------|--------
  *  PMU_LPD_CFG | LPD_VAON_ACTIVE
  *
  * @param value: Lpd active.
  * 
  */
__STATIC_FORCEINLINE void ll_aon_pmu_set_lpd_active(uint32_t value)
{
    MODIFY_REG(AON_PMU->PMU_LPD_CFG, AON_PMU_LPD_CFG_VAON_ACTIVE, (value << AON_PMU_LPD_CFG_VAON_ACTIVE_Pos));
}
/**
  * @brief  modify lpd sleep
  *
  *  Register|BitsName
  *  --------|--------
  *  PMU_LPD_CFG | LPD_VAON_SLEEP
  *
  * @param value: Lpd sleep.
  * 
  */
__STATIC_INLINE void ll_aon_pmu_set_lpd_sleep(uint32_t value)
{
    MODIFY_REG(AON_PMU->PMU_LPD_CFG, AON_PMU_LPD_CFG_VAON_SLEEP, (value << AON_PMU_LPD_CFG_VAON_SLEEP_Pos));
}
/**
  * @brief  modify ton on
  *
  *  Register|BitsName
  *  --------|--------
  *  PMU_TON_CFG | AON_PMU_TON_CTRL_ON
  *
  * @param value: Ton on.
  * 
  */
__STATIC_INLINE void ll_aon_pmu_set_tx_ton_val(uint32_t value)
{
    MODIFY_REG(AON_PMU->PMU_TON_CFG, AON_PMU_TON_CFG_CTL_ON_VAL, (value << AON_PMU_TON_CFG_CTL_ON_VAL_Pos));
}

/**
  * @brief  Get tx ton value
  *
  *  Register|BitsName
  *  --------|--------
  *  PMU_TON_CFG | AON_PMU_TON_CTRL_ON
  *
  * @retval The current tx ton value.
  *
  */
__STATIC_INLINE uint32_t ll_aon_pmu_get_tx_ton_val(void)
{
    return (READ_BITS(AON_PMU->PMU_TON_CFG, AON_PMU_TON_CFG_CTL_ON_VAL) >> AON_PMU_TON_CFG_CTL_ON_VAL_Pos);
}

/**
  * @brief  modify ton off
  *
  *  Register|BitsName
  *  --------|--------
  *  PMU_TON_CFG | AON_PMU_TON_CTRL_OFF
  *
  * @param value: Ton off.
  * 
  */
__STATIC_INLINE void ll_aon_pmu_set_non_tx_ton_val(uint32_t value)
{
    MODIFY_REG(AON_PMU->PMU_TON_CFG, AON_PMU_TON_CFG_CTL_OFF_VAL, (value << AON_PMU_TON_CFG_CTL_OFF_VAL_Pos));
}
/**
  * @brief  modify rng bump
  *
  *  Register|BitsName
  *  --------|--------
  *  RF_REG_0 | AON_PMU_RF_REG_0_RNG_CLK_BUMP
  *
  * @param value: Rng bump.
  * 
  */
__STATIC_INLINE void ll_aon_pmu_set_rng_clk_bump_val(uint32_t value)
{
    MODIFY_REG(AON_PMU->RF_REG_0, AON_PMU_RF_REG_0_RNG_CLK_BUMP, (value << AON_PMU_RF_REG_0_RNG_CLK_BUMP_Pos));
}
/**
  * @brief  modify rng cont
  *
  *  Register|BitsName
  *  --------|--------
  *  RF_REG_0 | AON_PMU_RF_REG_0_RNG_FREQ_CONT
  *
  * @param value: Rng cont.
  * 
  */
__STATIC_INLINE void ll_aon_pmu_set_rng_freq_cont(uint32_t value)
{
    MODIFY_REG(AON_PMU->RF_REG_0, AON_PMU_RF_REG_0_RNG_FREQ_CONT, (value << AON_PMU_RF_REG_0_RNG_FREQ_CONT_Pos));
}
/**
  * @brief  modify bg en
  *
  *  Register|BitsName
  *  --------|--------
  *  RF_REG_4 | AON_PMU_RF_REG_4_BG_EN
  *
  * @param value: Bg en.
  * 
  */
__STATIC_INLINE void ll_aon_pmu_set_bg_en(uint32_t value)
{
    MODIFY_REG(AON_PMU->RF_REG_4, AON_PMU_RF_REG_4_BG_EN, (value << AON_PMU_RF_REG_4_BG_EN_Pos));
}
/** @} */


/**
  * @brief  Enable short aon digcore
  *
  *  Register|BitsName
  *  --------|--------
  *  RF_REG4 | rg_core2aon_sw_en
  *
  * @retval None
  *
  */
__STATIC_FORCEINLINE void ll_aon_pmu_enable_short_aon_digcore(void)
{
    SET_BITS(AON_PMU->RF_REG_4, AON_PMU_RF_REG_4_SHORT_AON_DIGCORE);
}

/**
  * @brief  Disable short aon digcore
  *
  *  Register|BitsName
  *  --------|--------
  *  RF_REG4 | rg_core2aon_sw_en
  *
  * @retval None
  *
  */
__STATIC_FORCEINLINE void ll_aon_pmu_disable_short_aon_digcore(void)
{
    CLEAR_BITS(AON_PMU->RF_REG_4, AON_PMU_RF_REG_4_SHORT_AON_DIGCORE);
}

#endif

/** @} */

/** @} */

/** @} */
