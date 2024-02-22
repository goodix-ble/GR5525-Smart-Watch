/**
 *****************************************************************************************
 *
 * @file user_efuse.h
 *
 * @brief efuse access API.
 *
 *****************************************************************************************
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
#ifndef __USER_EFUSE_H__
#define __USER_EFUSE_H__

#include <stdint.h>
#include <stdbool.h>

/**
 * @defgroup USER_EFUSE_MAROC Defines
 * @{
 */
#define USER_EFUSE_BASE_OFFSET                        EFUSE_OFFSET_USER_DSVD              /**< User eFuse offset        */
#define USER_EFUSE_SIZE                               (0x0020UL)                           /**< User eFuse size          */

#define USER_EFUSE_ERROR_NONE                         ((uint32_t)0x00000000)              /**< No error                 */
#define USER_EFUSE_INIT_ERROR                         ((uint32_t)0x00000001)              /**< INIT error               */
#define USER_EFUSE_WRITE_ERROR                        ((uint32_t)0x00000002)              /**< WRITE error              */
#define USER_EFUSE_READ_ERROR                         ((uint32_t)0x00000004)              /**< READ error               */
#define USER_EFUSE_ERROR_INVALID_PARAM                ((uint32_t)0x00000008)              /**< Invalid parameters error */
/** @} */

/**
 * @defgroup USER_EFUSE_FUNCTION Functions
 * @{
 */
/**
 *****************************************************************************************
 * @brief  efuse write
 *
 * @param[in] offset:       Offset
 * @param[in] efuse_value:  Pointer to data
 * @param[in] size_word:    Write data size
 *
 * @return the error code of this funciton
 *****************************************************************************************
 */
uint32_t user_efuse_write(uint8_t offset, uint32_t *efuse_value, uint8_t size_word);

/**
 *****************************************************************************************
 * @brief  efuse read
 *
 * @param[in] word_offset:  Offset
 * @param[in] data:         Pointer to data
 * @param[in] size_word:    Read data size
 *
 * @return the error code of this funciton
 *****************************************************************************************
 */
uint32_t user_efuse_read(uint8_t word_offset, uint32_t *data, uint8_t size_word);
/** @} */

#endif

