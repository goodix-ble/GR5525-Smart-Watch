/**
 ****************************************************************************************
 *
 * @file    gr55xx_ll_gpadc.h
 * @author  BLE Driver Team
 * @brief   Header file containing functions prototypes of GPADC LL library.
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

/** @defgroup LL_ADC ADC
  * @brief ADC LL module driver.
  * @{
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GR55XX_LL_GPADC_H__
#define __GR55XX_LL_GPADC_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "gr55xx.h"

#if defined(AON_CTL)

/** @defgroup ADC_LL_STRUCTURES Structures
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup ADC_LL_ES_INIT ADC Exported init structures
  * @{
  */

/**
  * @brief LL ADC init Structure definition
  */
typedef struct _ll_gpadc_init
{
    uint32_t channel_p;     /**< Specifies the input source to GPADC channel P.
                                 This parameter can be any value of @ref ADC_LL_EC_P_INPUT_SRC.*/

    uint32_t channel_n;     /**< Specifies the input source to GPADC channel N.
                                 This parameter can be any value of @ref ADC_LL_EC_N_INPUT_SRC.*/

    uint32_t input_mode;    /**< Specifies the operation mode for the GPADC sample.
                                 This parameter can be a value of @ref GPADC_LL_EC_INPUT_MODE.*/

    uint32_t vdd_value;     /**< Specifies the power supply value of the GPADC,which is also the max input of GPADC.
                                 This parameter can be a value of @ref GPADC_LL_EC_VDD_VALUE.*/

    uint32_t sp_rate;         /**< Specifies the sample rate of GPADC.
                                 This parameter can be a value of @ref GPADC_LL_EC_SP_RATE.*/

    uint32_t pga_mode;         /**< Specifies whether using PGA to gain the input or not.
                                 This parameter can be a value of @ref GPADC_LL_EC_PGA_MODE.*/

    uint32_t pga_gain;         /**< Specifies whether using PGA to gain the input or not, and the gain value if using PGA.
                                 This parameter can be a value of @ref GPADC_LL_EC_PGA_GAIN.*/
} ll_gpadc_init_t;
/** @} */

/** @} */

/**
  * @defgroup  GPADC_LL_MACRO Defines
  * @{
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup GPADC_LL_Exported_Constants GPADC Exported Constants
  * @{
  */

/** @defgroup the first set value of gpadc regs
  * @{
  */
#define LL_GPADC_CTRL0_DEFAULT_VALUE               (0x882004ae)//M = 4'he(m+n=32), P = 2'h2(fixed), N = 6'h12(fixed), conv_num = 4'h0, td_t = 8'h20, ts_div = 3'h0, test_mode=1'd1(continu output); clk_inv=1'd0, cali_cap =1'h0, adc_en = 1'h0, conv_sel = 1'h1.
#define LL_GPADC_CTRL1_DEFAULT_VALUE               (0x0000097b)
#define LL_GPADC_CONST_DEFAULT_VALUE               (0x10e910e0)
#define LL_GPADC_ANA_CTRL_DEFAULT_VALUE            (0x00022072)//Vref_pd=0,bypass pga,chp=2,chn=2.
/** @} */

/** @defgroup ADC_LL_EC_P_INPUT_SRC GPADC Input Source
  * @{
  */
#define LL_GPADC_P_INPUT_SRC_VCM        (0UL)  /**< Select vcm(=vdd/2) as input   */
#define LL_GPADC_P_INPUT_SRC_IO1        (1UL)  /**< Select MSIOB1 as input       */
#define LL_GPADC_P_INPUT_SRC_IO2        (2UL)  /**< Select MSIOB2 as input       */
#define LL_GPADC_P_INPUT_SRC_IO3        (3UL)  /**< Select MSIOB3 as input       */
#define LL_GPADC_P_INPUT_SRC_IO4        (4UL)  /**< Select MSIOB4 as input       */
#define LL_GPADC_P_INPUT_SRC_IO5        (5UL)  /**< Select MSIOB5 as input       */
#define LL_GPADC_P_INPUT_SRC_IO6        (6UL)  /**< Select MSIOB6 as input       */
#define LL_GPADC_P_INPUT_SRC_IO7        (7UL)  /**< Select MSIOB7 as input       */
/** @} */

/** @defgroup ADC_LL_EC_N_INPUT_SRC GPADC Input Source
  * @{
  */
#define LL_GPADC_N_INPUT_SRC_VCM        (0UL)  /**< Select vcm(=vdd/2) as input   */
#define LL_GPADC_N_INPUT_SRC_IO0        (1UL)  /**< Select MSIOB0 as input       */
#define LL_GPADC_N_INPUT_SRC_IO1        (2UL)  /**< Select MSIOB1 as input       */
#define LL_GPADC_N_INPUT_SRC_IO2        (3UL)  /**< Select MSIOB2 as input       */
#define LL_GPADC_N_INPUT_SRC_IO3        (4UL)  /**< Select MSIOB3 as input       */
#define LL_GPADC_N_INPUT_SRC_IO4        (5UL)  /**< Select MSIOB4 as input       */
#define LL_GPADC_N_INPUT_SRC_IO5        (6UL)  /**< Select MSIOB5 as input       */
#define LL_GPADC_N_INPUT_SRC_IO6        (7UL)  /**< Select MSIOB6 as input       */
/** @} */

/** @defgroup GPADC_LL_EC_INPUT_MODE GPADC Input Mode
  * @{
  */
#define LL_GPADC_INPUT_DIFFERENTIAL   (0UL)      /**< Differential mode */
#define LL_GPADC_INPUT_SINGLE         (1UL )     /**< Single ended mode */
/** @} */

/** @defgroup GPADC_LL_EC_VDD_VALUE GPADC Vdd Value
  * @{
  */
#define LL_GPADC_VDD_VALUE_1P8        (0UL)   /**< Vdd = 1.8 V */
#define LL_GPADC_VDD_VALUE_2P3        (5UL)   /**< Vdd = 2.3 V */
/** @} */

/** @defgroup GPADC_LL_EC_SP_RATE GPADC Sample Rate
  * @{
  */
#define LL_GPADC_SPR_1M          (0UL)   /**<sample rate = 1Msps */
#define LL_GPADC_SPR_500K        (1UL)   /**<sample rate = 500Ksps */
#define LL_GPADC_SPR_250K        (2UL)   /**<sample rate = 250Ksps */
#define LL_GPADC_SPR_125K        (3UL)   /**<sample rate = 125Ksps */
/** @} */

/** @defgroup GPADC_LL_EC_PGA_MODE GPADC Pga Mode
  * @{
  */
#define LL_GPADC_PGA_MODE_BYPASS          (0UL)   /**<bypass mode, means not use pga*/
#define LL_GPADC_PGA_MODE_NORMAL          (1UL)   /**<use pga*/
/** @} */

/** @defgroup GPADC_LL_EC_PGA_GAIN GPADC Pga Gain
  * @{
  */
#define LL_GPADC_PGA_GAIN_0DB             (0UL)   /**<pga gain = 0db*/
#define LL_GPADC_PGA_GAIN_5DB             (5UL)   /**<pga gain = 5db*/
#define LL_GPADC_PGA_GAIN_10DB            (10UL)  /**<pga gain = 10db*/
#define LL_GPADC_PGA_GAIN_20DB            (20UL)  /**<pga gain = 20db*/

/** @} */

/** @} */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup ADC_LL_Exported_Macros ADC Exported Macros
  * @{
  */

/** @defgroup ADC_LL_EM_WRITE_READ Common Write and read registers Macros
  * @{
  */

/**
  * @brief  Write a value in ADC register
  * @param  __instance__ ADC instance
  * @param  __REG__ Register to be written
  * @param  __VALUE__ Value to be written in the register
  * @retval None
  */
#define LL_GPADC_WriteReg(__instance__, __REG__, __VALUE__) WRITE_REG((__instance__)->__REG__, (__VALUE__))

/**
  * @brief  Read a value in ADC register
  * @param  __instance__ ADC instance
  * @param  __REG__ Register to be read
  * @retval Register value
  */
#define LL_GPADC_ReadReg(__instance__, __REG__) READ_REG((__instance__)->__REG__)

/** @} */

/** @} */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/** @defgroup ADC_LL_Private_Macros ADC Private Macros
  * @{
  */

/** @} */

/** @} */

/* Exported functions --------------------------------------------------------*/
/** @defgroup GPADC_LL_DRIVER_FUNCTIONS Functions
  * @{
  */

/** @defgroup GPADC_LL_EF_Configuration Basic Configuration
  * @{
  */

/**
  * @brief  Enable GPADC module.
  *
  *  Register|BitsName
  *  --------|--------
  *  GPADC_CTRL0 | GPADC_CTRL0_ADC_EN_Msk
  *
  * @retval None
  */
__STATIC_INLINE void ll_gpadc_enable(void)
{
    SET_BITS(GPADC->CTRL0, GPADC_CTRL0_ADC_EN_Msk);
}

/**
  * @brief  Disable ADC module.
  *
  *  Register|BitsName
  *  --------|--------
  *  GPADC_CTRL0 | GPADC_CTRL0_ADC_EN_Msk
  *
  * @retval None
  */
__STATIC_INLINE void ll_gpadc_disable(void)
{
    CLEAR_BITS(GPADC->CTRL0, GPADC_CTRL0_ADC_EN_Msk);
}

/**
  * @brief  Check if GPADC module is enabled.
  *
  *  Register|BitsName
  *  --------|--------
  *  GPADC_CTRL0 | GPADC_CTRL0_ADC_EN_Msk
  *
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t ll_gpadc_is_enabled(void)
{
    return (READ_BITS(GPADC->CTRL0, GPADC_CTRL0_ADC_EN_Msk) == (GPADC_CTRL0_ADC_EN_Msk));
}

/**
  * @brief  Set GPADC Vref.
  *
  *  Register|BitsName
  *  --------|--------
  *  ANA_CTRL | GPADC_ANA_CTRL_VREF_SEL
  *
  * @param  vref_sel value
  * @retval None
  */
__STATIC_INLINE void ll_gpadc_set_vref_sel(uint32_t vref_sel)
{
    MODIFY_REG(GPADC->ANA_CTRL, GPADC_ANA_CTRL_VREF_SEL_Msk, vref_sel << GPADC_ANA_CTRL_VREF_SEL_POS);
}

/**
  * @brief  Set GPADC input para.
  *
  *  Register|BitsName
  *  --------|--------
  *  ANA_CTRL | GPADC_ANA_CTRL_ADC_INPUT GPADC_ANA_CTRL_ADC_INPUT_SEL
  *
  * @param  input input value
  * @param  input_sel input sel value
  * @retval None
  */
__STATIC_INLINE void ll_gpadc_set_input_para(uint32_t input,uint32_t input_sel)
{
    MODIFY_REG(GPADC->ANA_CTRL, GPADC_ANA_CTRL_ADC_INPUT_Msk, input << GPADC_ANA_CTRL_ADC_INPUT_POS);
    MODIFY_REG(GPADC->ANA_CTRL, GPADC_ANA_CTRL_ADC_INPUT_SEL_Msk, input_sel << GPADC_ANA_CTRL_ADC_INPUT_SEL_POS);
}

/**
  * @brief  Set GPADC cali sel.
  *
  *  Register|BitsName
  *  --------|--------
  *  ANA_CTRL | GPADC_ANA_CTRL_CALI_SEL
  *
  * @param  cali_sel value
  * @retval None
  */
__STATIC_INLINE void ll_gpadc_set_cali_sel(uint32_t cali_sel)
{
    MODIFY_REG(GPADC->ANA_CTRL,  GPADC_ANA_CTRL_CALI_SEL_Msk , cali_sel << GPADC_ANA_CTRL_CALI_SEL_POS);
}

/**
  * @brief  Set GPADC chanel p sel.
  *
  *  Register|BitsName
  *  --------|--------
  *  ANA_CTRL | GPADC_ANA_CTRL_CH_SEL_P
  *
  * @param  chp_sel value
  * @retval None
  */
__STATIC_INLINE void ll_gpadc_set_chp_sel(uint32_t chp_sel)
{
    MODIFY_REG(GPADC->ANA_CTRL,  GPADC_ANA_CTRL_CH_SEL_P_Msk , chp_sel << GPADC_ANA_CTRL_CH_SEL_P_POS);
}

/**
  * @brief  Set GPADC chanel n sel.
  *
  *  Register|BitsName
  *  --------|--------
  *  ANA_CTRL | GPADC_ANA_CTRL_CH_SEL_N
  *
  * @param  chn_sel value
  * @retval None
  */
__STATIC_INLINE void ll_gpadc_set_chn_sel(uint32_t chn_sel)
{
    MODIFY_REG(GPADC->ANA_CTRL,  GPADC_ANA_CTRL_CH_SEL_N_Msk , chn_sel << GPADC_ANA_CTRL_CH_SEL_N_POS);
}

/**
  * @brief  Set GPADC clock division.
  *
  *  Register|BitsName
  *  --------|--------
  *  CTRL0   | GPADC_CTRL0_TS_DIV
  *
  * @param  div division value
  * @retval None
  */
__STATIC_INLINE void ll_gpadc_set_clk_div(uint32_t div)
{
    MODIFY_REG(GPADC->CTRL0,  GPADC_CTRL0_TS_DIV_Msk, div << GPADC_CTRL0_TS_DIV_POS);
}

/**
* @brief  Set GPADC data out mode
  *
  *  Register|BitsName
  *  --------|--------
  *  CTRL0   | GPADC_CTRL0_SAMPLE_MODE
  *
  * @param  mode 0:output data only once mode 1:output data continually
  * @retval None
  */
__STATIC_INLINE void ll_gpadc_set_data_out_mode(uint32_t mode)
{
    MODIFY_REG(GPADC->CTRL0,  GPADC_CTRL0_SAMPLE_MODE_Msk, mode << GPADC_CTRL0_SAMPLE_MODE_POS);
}

/**
* @brief  Set GPADC output data mode,raw data or converted data
  *
  *  Register|BitsName
  *  --------|--------
  *  CTRL0   | GPADC_CTRL0_CONV_SEL
  *
  * @param  conv_sel 0:output raw data; sel 1:output converted data
  * @retval None
  */
__STATIC_INLINE void ll_gpadc_set_conv_sel(uint32_t conv_sel)
{
    MODIFY_REG(GPADC->CTRL0,  GPADC_CTRL0_CONV_SEL_Msk, conv_sel << GPADC_CTRL0_CONV_SEL_POS);
}

/**
* @brief  Set GPADC vref power down to save power
  *
  *  Register|BitsName
  *  --------|--------
  *  ANA_CTRL| GPADC_ANA_CTRL_VREF_PD
  *
  * @param  pd 0:normal work; sel 1:vref power down
  * @retval None
  */
__STATIC_INLINE void ll_gpadc_set_vref_pd(uint32_t pd)
{
    MODIFY_REG(GPADC->ANA_CTRL,  GPADC_ANA_CTRL_VREF_PD_Msk, pd << GPADC_ANA_CTRL_VREF_PD_POS);
}

/**
* @brief  Set GPADC whether enter calibration mode or not
  *
  *  Register|BitsName
  *  --------|--------
  *  CTRL0   | GPADC_CTRL0_CALI_CAP
  *
  * @param  mode 0:normal mode; mode 1:calibration mode
  * @retval None
  */
__STATIC_INLINE void ll_gpadc_set_operation_mode(uint32_t mode)
{
    MODIFY_REG(GPADC->CTRL0,  GPADC_CTRL0_CALI_CAP_Msk, mode << GPADC_CTRL0_CALI_CAP_POS);
}

/**
  * @brief  Enable GPADC offset function.
  *
  *  Register|BitsName
  *  --------|--------
  *  GPADC_OFFSET | GPADC_OFFSET_OFFSET_EN
  *
  * @retval None
  */
__STATIC_INLINE void ll_gpadc_offset_enable(void)
{
    SET_BITS(GPADC->OFFSET, GPADC_OFFSET_OFFSET_EN_Msk);
}

/**
  * @brief  Disable GPADC offset function.
  *
  *  Register|BitsName
  *  --------|--------
  *  GPADC_OFFSET | GPADC_OFFSET_OFFSET_EN
  *
  * @retval None
  */
__STATIC_INLINE void ll_gpadc_offset_disable(void)
{
    CLEAR_BITS(GPADC->OFFSET, GPADC_OFFSET_OFFSET_EN_Msk);
}

/**
  * @brief  Enable GPADC offset auto load function.
  *
  *  Register|BitsName
  *  --------|--------
  *  GPADC_OFFSET | GPADC_OFFSET_OFFSET_AUTO_LOAD
  *
  * @retval None
  */
__STATIC_INLINE void ll_gpadc_offset_auto_load_enable(void)
{
    SET_BITS(GPADC->OFFSET, GPADC_OFFSET_OFFSET_AUTO_LOAD_Msk );
}

/**
  * @brief  Disable GPADC offset auto load function.
  *
  *  Register|BitsName
  *  --------|--------
  *  GPADC_OFFSET | GPADC_OFFSET_OFFSET_AUTO_LOAD
  *
  * @retval None
  */
__STATIC_INLINE void ll_gpadc_offset_auto_load_disable(void)
{
    CLEAR_BITS(GPADC->OFFSET, GPADC_OFFSET_OFFSET_AUTO_LOAD_Msk );
}

/**
  * @brief  Return offset value of ADC FIFO.
  *
  *  Register|BitsName
  *  --------|--------
  *  GPADC_OFFSET | GPADC_OFFSET_OFFSET_VALUE
  *
  * @retval Returned offset value can be a value between: 0 ~ ffff
  */
__STATIC_INLINE uint32_t ll_gpadc_get_offset_value(void)
{
    return (uint32_t)(READ_BITS(GPADC->OFFSET, GPADC_OFFSET_OFFSET_VALUE_Msk) >> GPADC_OFFSET_OFFSET_VALUE_POS);
}

/**
  * @brief  Return GPADC value P.
  *
  *  Register|BitsName
  *  --------|--------
  *  GPADC_DATA | GPADC_DATA_DATA_P
  *
  * @retval Returned GPADC value P
  */
__STATIC_INLINE uint32_t ll_gpadc_get_data_p(void)
{
    return (uint32_t)(READ_BITS(GPADC->DATA, GPADC_DATA_DATA_P_Msk) >> GPADC_DATA_DATA_P_POS);
}

/**
  * @brief  Return GPADC value N.
  *
  *  Register|BitsName
  *  --------|--------
  *  GPADC_DATA | GPADC_DATA_DATA_N
  *
  * @retval Return GPADC value N.
  */
__STATIC_INLINE uint32_t ll_gpadc_get_data_n(void)
{
    return (uint32_t)(READ_BITS(GPADC->DATA, GPADC_DATA_DATA_N_Msk) >> GPADC_DATA_DATA_N_POS);
}

/**
* @brief  Set the value of coefx,x=coef_num,value=coef_value.
  *
  *  Register|BitsName
  *  --------|--------
  *  GPADC_COEF|
  *
  * @param  coef_num The numble of coefx
  * @param  coef_value The value of coefx
  * @retval None
  */
__STATIC_INLINE void ll_gpadc_set_coefx(uint8_t coef_num,uint32_t coef_value)
{
    MODIFY_REG(*(&(GPADC->COEF0)+4*coef_num),  GPADC_COEF0_COE0_Msk, coef_value << GPADC_COEF0_COE0_POS);
}

/**
  * @brief  Return the value of coefx,x=coef_num.
  *
  *  Register|BitsName
  *  --------|--------
  *  GPADC_COEF |
  *
  * @param  coef_num The numble of coefx
  * @retval Return the value of coefx,x=coef_num
  */
__STATIC_INLINE uint32_t ll_gpadc_get_coefx(uint8_t coef_num)
{
    return (uint32_t)(READ_BITS(GPADC->COEF0+4*coef_num, GPADC_COEF0_COE0_Msk) >> GPADC_COEF0_COE0_POS);
}

/**
  * @brief  Return samples value of GPADC by reading FIFO.
  * @note   There are two value in the register, both of them is 16bits.
  *
  *  Register|BitsName
  *  --------|--------
  *
  * @retval Smaples value of input
  */
__STATIC_INLINE uint32_t ll_gpadc_read_fifo(void)
{
    return (uint32_t)(READ_REG(GPADC->FIFO_RD));
}

/**
  * @brief  Set threshold of GPADC FIFO.
  *
  *  Register|BitsName
  *  --------|--------
  *
  * @param  thresh This parameter can be a value between: 0 ~ 63
  * @retval None
  */
__STATIC_INLINE void ll_gpadc_set_thresh(uint32_t thresh)
{
    MODIFY_REG(GPADC->FIFO_THD, GPADC_FIFO_THD_Msk, (thresh & 0x3F) << GPADC_FIFO_THD_POS);
}

/**
  * @brief  Check if GPADC FIFO is not empty.
  *
  *  Register|BitsName
  *  --------|--------
  *
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t ll_gpadc_is_fifo_notempty(void)
{
    return (uint32_t)(READ_BITS(GPADC->FIFO_STAT, GPADC_FIFO_STAT_VALID) == GPADC_FIFO_STAT_VALID);
}

/**
  * @brief  Return count of GPADC FIFO.
  *
  *  Register|BitsName
  *  --------|--------
  *
  * @retval Returned value can be a value between: 0 ~ 64
  */
__STATIC_INLINE uint32_t ll_gpadc_get_fifo_count(void)
{
    return (uint32_t)(READ_BITS(GPADC->FIFO_STAT, GPADC_FIFO_STAT_COUNT) >> GPADC_FIFO_STAT_COUNT_POS);
}

/**
  * @brief  Flush GPADC FIFO.
  *
  *  Register|BitsName
  *  --------|--------
  *
  * @retval void
  */
__STATIC_INLINE void ll_gpadc_flush_fifo(void)
{
    SET_BITS(GPADC->FIFO_STAT, GPADC_FIFO_STAT_FLUSH_Msk);
    CLEAR_BITS(GPADC->FIFO_STAT, GPADC_FIFO_STAT_FLUSH_Msk);
}

/**
* @brief  Set the value of ldo23
  *
  *  Register|BitsName
  *  --------|--------
  *
  * @param value The value of ldo23
  */
__STATIC_INLINE void ll_gpadc_set_ldo23(uint32_t value)
{
    MODIFY_REG(GPADC->ANA_MBG,  GPADC_ANA_MBG_LDO23_SEL_Msk, value << GPADC_ANA_MBG_LDO23_SEL_POS);
}

/**
  * @brief  Enable PGA.
  *
  *  Register|BitsName
  *  --------|--------
  *
  * @retval None
  */
__STATIC_INLINE void ll_gpadc_pga_enable(void)
{
    CLEAR_BITS(GPADC->ANA_PGA, GPADC_ANA_PGA_PD_Msk);
}

/**
  * @brief  Disable PGA.
  *
  *  Register|BitsName
  *  --------|--------
  *
  * @retval None
  */
__STATIC_INLINE void ll_gpadc_pga_disable(void)
{
    SET_BITS(GPADC->ANA_PGA, GPADC_ANA_PGA_PD_Msk);
}
/**
* @brief  Set the value of PGA Gain
  *
  *  Register|BitsName
  *  --------|--------
  *
  *
  * @param value The value of PGA Gain
  */
__STATIC_INLINE void ll_gpadc_set_gain(uint32_t value)
{
    MODIFY_REG(GPADC->ANA_PGA,  GPADC_ANA_PGA_GAIN_CTRL_Msk, value << GPADC_ANA_PGA_GAIN_CTRL_POS);
}

/*
**
  * @brief  De-initialize GPADC registers (Registers restored to their default values).
  * @retval An error_status_t enumeration value:
  *          - SUCCESS: GPADC registers are de-initialized
  *          - ERROR: GPADC registers are not de-initialized
  */
error_status_t ll_gpadc_deinit(void);

/**
  * @brief  Initialize GPADC registers according to the specified.
  *         parameters in p_gpadc_init.
  * @param  p_gpadc_init Pointer to a ll_gpadc_init_t structure that contains the configuration
  *                             information for the specified GPADC peripheral.
  * @retval An error_status_t enumeration value:
  *          - SUCCESS: GPADC registers are initialized according to p_gpadc_init content
  *          - ERROR: Problem occurred during GPADC Registers initialization
  */
error_status_t ll_gpadc_init(ll_gpadc_init_t *p_gpadc_init);

/**
  * @brief Set each field of a @ref ll_gpadc_init_t type structure to default value.
  * @param p_gpadc_init  Pointer to a @ref ll_gpadc_init_t structure
  *                             whose fields will be set to default values.
  * @retval None
  */
void ll_gpadc_struct_init(ll_gpadc_init_t *p_gpadc_init);

/** @} */

/** @} */

#endif /* AON */

#ifdef __cplusplus
}
#endif

#endif /* __GR55XX_LL_ADC_H__ */

/** @} */

/** @} */

/** @} */
