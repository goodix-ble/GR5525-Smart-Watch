/**
 ****************************************************************************************
 *
 * @file    gr55xx_hal_gpadc.h
 * @author  BLE Driver Team
 * @brief   Header file containing functions prototypes of GPADC HAL library.
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

/** @addtogroup HAL_DRIVER HAL Driver
  * @{
  */

/** @defgroup HAL_GPADC GPADC
  * @brief GPADC HAL module driver.
  * @{
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GR55xx_HAL_GPADC_H__
#define __GR55xx_HAL_GPADC_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "gr55xx_ll_gpadc.h"
#include "gr55xx_hal_def.h"
#include "gr55xx_hal_dma.h"

/* Exported types ------------------------------------------------------------*/
/** @addtogroup HAL_GPADC_ENUMERATIONS Enumerations
  * @{
  */

/** @defgroup HAL_GPADC_state HAL GPADC State
  * @{
  */

/**
  * @brief HAL GPADC State Enumerations definition
  */
typedef enum
{
    HAL_GPADC_STATE_RESET        = 0x00,    /**< Peripheral not initialized                          */
    HAL_GPADC_STATE_READY        = 0x01,    /**< Peripheral initialized and ready for use            */
    HAL_GPADC_STATE_BUSY         = 0x02,    /**< An internal process is ongoing                      */
    HAL_GPADC_STATE_ERROR        = 0x04     /**< Peripheral in error                                 */

} hal_gpadc_state_t;

/** @} */

/** @} */

/** @addtogroup HAL_GPADC_STRUCTURES Structures
  * @{
  */

/** @defgroup GPADC_Configuration GPADC Configuration
  * @{
  */

/**
  * @brief   GPADC init structure definition
  */
typedef ll_gpadc_init_t       gpadc_init_t;
/** @} */

/** @defgroup GPADC_handle GPADC Handle
  * @{
  */

/**
  * @brief GPADC handle Structure definition
  */
typedef struct _gpadc_handle
{
    gpadc_init_t              init;           /**< GPADC configuration parameters      */

    uint16_t               *p_buffer;       /**< Pointer to GPADC conversion buffer  */

    __IO uint32_t           buff_size;      /**< Conversion buffer size            */

    __IO uint32_t           buff_count;     /**< Conversion buffer counter         */

    dma_handle_t           *p_dma;          /**< GPADC DMA Handle parameters         */

    __IO hal_lock_t         lock;           /**< Locking object                    */

    __IO hal_gpadc_state_t    state;          /**< GPADC communication state           */

    __IO uint32_t           error_code;     /**< GPADC error code                    */

    uint32_t                retention[20];   /**< GPADC important register information. */
} gpadc_handle_t;
/** @} */

/** @} */

/** @addtogroup HAL_GPADC_CALLBACK_STRUCTURES Callback Structures
  * @{
  */

/** @defgroup GPADC_Callback GPADC Callback
  * @{
  */

/**
  * @brief HAL GPADC Callback function definition
  */
typedef struct _hal_gpadc_callback
{
    void (*gpadc_msp_init)(gpadc_handle_t *p_gpadc);                  /**< GPADC init MSP callback                  */
    void (*gpadc_msp_deinit)(gpadc_handle_t *p_gpadc);                /**< GPADC de-init MSP callback               */
    void (*gpadc_conv_cplt_callback)(gpadc_handle_t *p_gpadc);        /**< GPADC conversion completed callback      */
} hal_gpadc_callback_t;

/** @} */

/** @} */

/**
  * @defgroup  HAL_GPADC_MACRO Defines
  * @{
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup GPADC_Exported_Constants GPADC Exported Constants
  * @{
  */
/** @defgroup GPADC_P_INPUT_SRC GPADC P_Input Source
  * @{
  */
#define GPADC_P_INPUT_SRC_VCM        LL_GPADC_P_INPUT_SRC_VCM  /**< Select vcm(=vdd/2) as input   */
#define GPADC_P_INPUT_SRC_IO1        LL_GPADC_P_INPUT_SRC_IO1  /**< Select MSIOB1 as input       */
#define GPADC_P_INPUT_SRC_IO2        LL_GPADC_P_INPUT_SRC_IO2  /**< Select MSIOB2 as input       */
#define GPADC_P_INPUT_SRC_IO3        LL_GPADC_P_INPUT_SRC_IO3  /**< Select MSIOB3 as input       */
#define GPADC_P_INPUT_SRC_IO4        LL_GPADC_P_INPUT_SRC_IO4  /**< Select MSIOB4 as input       */
#define GPADC_P_INPUT_SRC_IO5        LL_GPADC_P_INPUT_SRC_IO5  /**< Select MSIOB5 as input       */
#define GPADC_P_INPUT_SRC_IO6        LL_GPADC_P_INPUT_SRC_IO6  /**< Select MSIOB6 as input       */
#define GPADC_P_INPUT_SRC_IO7        LL_GPADC_P_INPUT_SRC_IO7  /**< Select MSIOB7 as input       */
/** @} */

/** @defgroup GPADC_N_INPUT_SRC GPADC N_Input Source
  * @{
  */
#define GPADC_N_INPUT_SRC_VCM        LL_GPADC_N_INPUT_SRC_VCM  /**< Select vcm(=vdd/2) as input   */
#define GPADC_N_INPUT_SRC_IO0        LL_GPADC_N_INPUT_SRC_IO0  /**< Select MSIOB0 as input       */
#define GPADC_N_INPUT_SRC_IO1        LL_GPADC_N_INPUT_SRC_IO1  /**< Select MSIOB1 as input       */
#define GPADC_N_INPUT_SRC_IO2        LL_GPADC_N_INPUT_SRC_IO2  /**< Select MSIOB2 as input       */
#define GPADC_N_INPUT_SRC_IO3        LL_GPADC_N_INPUT_SRC_IO3  /**< Select MSIOB3 as input       */
#define GPADC_N_INPUT_SRC_IO4        LL_GPADC_N_INPUT_SRC_IO4  /**< Select MSIOB4 as input       */
#define GPADC_N_INPUT_SRC_IO5        LL_GPADC_N_INPUT_SRC_IO5  /**< Select MSIOB5 as input       */
#define GPADC_N_INPUT_SRC_IO6        LL_GPADC_N_INPUT_SRC_IO6  /**< Select MSIOB6 as input       */
/** @} */

/** @defgroup GPADC_INPUT_MODE GPADC Input Mode
  * @{
  */
#define GPADC_INPUT_DIFFERENTIAL   LL_GPADC_INPUT_DIFFERENTIAL      /**< Differential mode */
#define GPADC_INPUT_SINGLE         LL_GPADC_INPUT_SINGLE            /**< Single ended mode */
/** @} */

/** @defgroup GPADC_VDD_VALUE GPADC Vdd Value
  * @{
  */
#define GPADC_VDD_VALUE_1P8        LL_GPADC_VDD_VALUE_1P8   /**< Vdd = 1.8 V */
#define GPADC_VDD_VALUE_2P3        LL_GPADC_VDD_VALUE_2P3   /**< Vdd = 2.3 V */
/** @} */

/** @defgroup GPADC_SP_RATE GPADC Sample Rate
  * @{
  */
#define GPADC_SPR_1M          LL_GPADC_SPR_1M     /**<sample rate = 1Msps */
#define GPADC_SPR_500K        LL_GPADC_SPR_500K   /**<sample rate = 500Ksps */
#define GPADC_SPR_250K        LL_GPADC_SPR_250K   /**<sample rate = 250Ksps */
#define GPADC_SPR_125K        LL_GPADC_SPR_125K   /**<sample rate = 125Ksps */
/** @} */

/** @defgroup GPADC_PGA_MODE GPADC Pga Mode
  * @{
  */
#define GPADC_PGA_MODE_BYPASS          LL_GPADC_PGA_MODE_BYPASS   /**<bypass mode, means not use pga*/
#define GPADC_PGA_MODE_NORMAL          LL_GPADC_PGA_MODE_NORMAL   /**<use pga*/
/** @} */

/** @defgroup GPADC_PGA_GAIN GPADC Pga Gain
  * @{
  */
#define GPADC_PGA_GAIN_0DB             LL_GPADC_PGA_GAIN_0DB    /**<pga gain = 0db*/
#define GPADC_PGA_GAIN_5DB             LL_GPADC_PGA_GAIN_5DB    /**<pga gain = 5db*/
#define GPADC_PGA_GAIN_10DB            LL_GPADC_PGA_GAIN_10DB    /**<pga gain = 10db*/
#define GPADC_PGA_GAIN_20DB            LL_GPADC_PGA_GAIN_20DB    /**<pga gain = 20db*/

/** @} */

/** @defgroup GPADC_Error_Code GPADC Error Code
  * @{
  */
#define HAL_GPADC_ERROR_NONE              ((uint32_t)0x00000000)  /**< No error           */
#define HAL_GPADC_ERROR_TIMEOUT           ((uint32_t)0x00000001)  /**< Timeout error      */
#define HAL_GPADC_ERROR_DMA               ((uint32_t)0x00000004)  /**< DMA transfer error */
#define HAL_GPADC_ERROR_INVALID_PARAM     ((uint32_t)0x00000008)  /**< Invalid parameter error */
/** @} */

/** @} */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup GPADC_Exported_Macros GPADC Exported Macros
  * @{
  */

/** @brief  Reset GPADC handle states.
  * @param  __HANDLE__ GPADC handle.
  * @retval None
  */
#define __HAL_GPADC_RESET_HANDLE_STATE(__HANDLE__)               ((__HANDLE__)->state = HAL_GPADC_STATE_RESET)

/** @brief  Enable the specified GPADC peripheral.
  * @param  __HANDLE__ Specify the GPADC Handle.
  * @retval None
  */
#define __HAL_GPADC_ENABLE(__HANDLE__)                           ll_gpadc_enable()

/** @brief  Disable the specified GPADC peripheral.
  * @param  __HANDLE__ Specify the GPADC Handle.
  * @retval None
  */
#define __HAL_GPADC_DISABLE(__HANDLE__)                          ll_gpadc_disable()

/** @brief  Check the FIFO is not empty.
  * @param  __HANDLE__ Specify the GPADC Handle.
  * @retval The new state of notempty flag (TRUE or FALSE).
  */
#define __HAL_GPADC_GET_FLAG_NOTEMPTY(__HANDLE__)                ll_gpadc_is_fifo_notempty()

/** @brief  Flush the FIFO.
  * @param  __HANDLE__ Specify the GPADC Handle.
  * @retval None
  */
#define __HAL_GPADC_FLUSH_FIFO(__HANDLE__)                       do {                                 \
                                                                        while(ll_gpadc_is_fifo_notempty()) \
                                                                        {                                \
                                                                            ll_gpadc_flush_fifo();          \
                                                                        }                                \
                                                                    } while(0)
/** @} */

/** @} */

/* Exported functions --------------------------------------------------------*/
/** @addtogroup HAL_GPADC_DRIVER_FUNCTIONS Functions
  * @{
  */

/** @addtogroup GPADC_Exported_Functions_Group1 Initialization and de-initialization Functions
  *  @brief    Initialization and Configuration functions
  * @{
  */

/**
 ****************************************************************************************
 * @brief  Initialize the GPADC according to the specified parameters
 *         in the gpadc_init_t and initialize the associated handle.
 *
 * @param[in]  p_gpadc: Pointer to an GPADC handle which contains the configuration information for
 *                    the specified GPADC module.
 *
 * @retval ::HAL_OK: Operation is OK.
 * @retval ::HAL_ERROR: Parameter error or operation not supported.
 * @retval ::HAL_BUSY: Driver is busy.
 * @retval ::HAL_TIMEOUT: Timeout occurred.
 ****************************************************************************************
 */
hal_status_t hal_gpadc_init(gpadc_handle_t *p_gpadc);

/**
 ****************************************************************************************
 * @brief  De-initialize the GPADC peripheral.
 *
 * @param[in]  p_gpadc: Pointer to an GPADC handle which contains the configuration information for
 *                    the specified GPADC module.
 *
 * @retval ::HAL_OK: Operation is OK.
 * @retval ::HAL_ERROR: Parameter error or operation not supported.
 * @retval ::HAL_BUSY: Driver is busy.
 * @retval ::HAL_TIMEOUT: Timeout occurred.
 ****************************************************************************************
 */
hal_status_t hal_gpadc_deinit(gpadc_handle_t *p_gpadc);

/**
 ****************************************************************************************
 * @brief  Initialize the GPADC MSP.
 *
 * @note   This function should not be modified. When the callback is needed,
 *          the hal_gpadc_msp_deinit can be implemented in the user file.
 *
 * @param[in]  p_gpadc: Pointer to an GPADC handle which contains the configuration information for
 *                the specified GPADC module.
 ****************************************************************************************
 */
void hal_gpadc_msp_init(gpadc_handle_t *p_gpadc);

/**
 ****************************************************************************************
 * @brief  De-initialize the GPADC MSP.
 *
 * @note   This function should not be modified. When the callback is needed,
 *          the hal_gpadc_msp_deinit can be implemented in the user file.
 *
 * @param[in]  p_gpadc: Pointer to an GPADC handle which contains the configuration information for
 *                    the specified GPADC module.
 ****************************************************************************************
 */
void hal_gpadc_msp_deinit(gpadc_handle_t *p_gpadc);

/**
 ****************************************************************************************
 * @brief  Polling for conversion.
 *
 * @param[in]  p_gpadc:  Pointer to an GPADC handle.
 * @param[in]  p_data: Pointer to data buffer which to store GPADC conversion results.
 * @param[in]  length: Length of data buffer.
 *
 * @retval ::HAL_OK: Operation is OK.
 * @retval ::HAL_ERROR: Parameter error or operation not supported.
 * @retval ::HAL_BUSY: Driver is busy.
 * @retval ::HAL_TIMEOUT: Timeout occurred.
 ****************************************************************************************
 */
hal_status_t hal_gpadc_poll_for_conversion(gpadc_handle_t *p_gpadc, uint16_t *p_data, uint32_t length);

/**
 ****************************************************************************************
 * @brief  DMA for conversion.
 *
 * @param[in]  p_gpadc:  Pointer to an GPADC handle.
 * @param[in]  p_data: Pointer to data buffer which to store GPADC conversion results.
 * @param[in]  length: Length of data buffer,  ranging between 0 and 4095.
 *
 * @retval ::HAL_OK: Operation is OK.
 * @retval ::HAL_ERROR: Parameter error or operation not supported.
 * @retval ::HAL_BUSY: Driver is busy.
 * @retval ::HAL_TIMEOUT: Timeout occurred.
 ****************************************************************************************
 */
hal_status_t hal_gpadc_start_dma(gpadc_handle_t *p_gpadc, uint16_t *p_data, uint32_t length);

/**
 ****************************************************************************************
 * @brief  DMA for conversion.
 *
 * @param[in]  p_gpadc:  Pointer to an GPADC handle.
 * @param[in]  p_data: Pointer to data buffer which to store GPADC conversion results.
 * @param[in]  length: Length of data buffer,  ranging between 0 and 4095.
 * @param[in] sg_llp_config: The config of source and destination's SG and LLP functions.
 *
 * @retval ::HAL_OK: Operation is OK.
 * @retval ::HAL_ERROR: Parameter error or operation not supported.
 * @retval ::HAL_BUSY: Driver is busy.
 * @retval ::HAL_TIMEOUT: Timeout occurred.
 ****************************************************************************************
 */
hal_status_t hal_gpadc_start_dma_sg_llp(gpadc_handle_t *p_gpadc, uint16_t *p_data, uint32_t length, dma_sg_llp_config_t *sg_llp_config);


/**
 ****************************************************************************************
 * @brief  Abort ongoing conversion (blocking mode).
 *
 * @note   This procedure could be only used for aborting conversion started in DMA mode.
 *         This procedure performs following operations:
 *           - Disable GPADC clock, stop conversion
 *           - Abort DMA transfer by calling hal_dma_abort (in case of transfer in DMA mode)
 *           - Set handle State to READY.
 *         This procedure is executed in blocking mode: when exiting function, Abort is considered as completed.
 *
 * @param[in]  p_gpadc: GPADC handle.
 *
 * @retval ::HAL_OK: Operation is OK.
 * @retval ::HAL_ERROR: Parameter error or operation not supported.
 * @retval ::HAL_BUSY: Driver is busy.
 * @retval ::HAL_TIMEOUT: Timeout occurred.
 ****************************************************************************************
 */
hal_status_t hal_gpadc_stop_dma(gpadc_handle_t *p_gpadc);

/**
 ****************************************************************************************
 * @brief  Conversion completed callback.
 *
 * @note   This function should not be modified. When the callback is needed,
 *          the hal_gpadc_msp_deinit can be implemented in the user file.
 *
 * @param[in]  p_gpadc: Pointer to an GPADC handle which contains the configuration information for
 *                    the specified GPADC module.
 ****************************************************************************************
 */
void hal_gpadc_conv_cplt_callback(gpadc_handle_t* p_gpadc);

/**
 ****************************************************************************************
 * @brief  Return the GPADC handle state.
 *
 * @param[in]  p_gpadc: Pointer to an GPADC handle which contains the configuration information for
 *                    the specified GPADC module.
 *
 * @retval ::HAL_GPADC_STATE_RESET: Peripheral not initialized.
 * @retval ::HAL_GPADC_STATE_READY: Peripheral initialized and ready for use.
 * @retval ::HAL_GPADC_STATE_BUSY: An internal process is ongoing.
 * @retval ::HAL_GPADC_STATE_ERROR: Peripheral in error.
 ****************************************************************************************
 */
hal_gpadc_state_t hal_gpadc_get_state(gpadc_handle_t *p_gpadc);

/**
 ****************************************************************************************
 * @brief  Return the GPADC error code.
 *
 * @param[in]  p_gpadc: Pointer to an GPADC handle which contains the configuration information for
 *                    the specified GPADC module.
 *
 * @return GPADC error code in bitmap format
 ****************************************************************************************
 */
uint32_t hal_gpadc_get_error(gpadc_handle_t *p_gpadc);

/**
 ****************************************************************************************
 * @brief  Suspend some registers related to GPADC configuration before sleep.
 * @param[in] p_gpadc: Pointer to a GPADC handle which contains the configuration
 *                     information for the specified GPADC module.
 * @retval ::HAL_OK:      Operation is OK.
 * @retval ::HAL_ERROR:   Parameter error or operation not supported.
 * @retval ::HAL_BUSY:    Driver is busy.
 * @retval ::HAL_TIMEOUT: Timeout occurred.
 ****************************************************************************************
 */
hal_status_t hal_gpadc_suspend_reg(gpadc_handle_t *p_gpadc);

/**
 ****************************************************************************************
 * @brief  Restore some registers related to GPADC configuration after sleep.
 *         This function must be used in conjunction with the hal_gpadc_suspend_reg().
 * @param[in] p_gpadc: Pointer to a GPADC handle which contains the configuration
 *                     information for the specified GPADC module.
 * @retval ::HAL_OK:      Operation is OK.
 * @retval ::HAL_ERROR:   Parameter error or operation not supported.
 * @retval ::HAL_BUSY:    Driver is busy.
 * @retval ::HAL_TIMEOUT: Timeout occurred.
 ****************************************************************************************
 */
hal_status_t hal_gpadc_resume_reg(gpadc_handle_t *p_gpadc);

/**
 ****************************************************************************************
 * @brief  Convert the GPADC conversion results to a voltage value.
 *
 * @param[in]  p_gpadc: Pointer to a GPADC handle which contains the configuration information for
 *                    the specified GPADC module.
 * @param[in]  inbuf: Pointer to data buffer which storage GPADC conversion results.
 * @param[out] outbuf: Pointer to data buffer which to storage voltage results.
 * @param[in]  buflen: Length of data buffer,  ranging between 0 and 4095.
 *
 * @return Result of operation.
 ****************************************************************************************
 */
void hal_gr55xx_gpadc_code_to_voltage(gpadc_handle_t *p_gpadc, uint16_t *inbuf, float *outbuf, uint32_t buflen);

/** @} */

/** @} */


#ifdef __cplusplus
}
#endif

#endif /* __GR55xx_HAL_GPADC_H__ */

/** @} */

/** @} */

/** @} */
