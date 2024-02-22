/**
 *****************************************************************************************
 *
 * @file user_periph_setup.c
 *
 * @brief  User Periph Init Function Implementation.
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

/*
 * INCLUDE FILES
 *****************************************************************************************
 */
#include "user_periph_setup.h"
#include "grx_sys.h"
#include "board_SK.h"
#include "app_log.h"
#include "app_assert.h"
#include "app_dual_tim.h"
#include "dfu_port.h"

/*
 * DEFINES
 *****************************************************************************************
 */
#define UART_TX_BUFFER_SIZE      0x200
#define UART_RX_BUFFER_SIZE      10
#define DUAL_TIM0_PARAM           { APP_DUAL_TIM_ID_0, { DUAL_TIMER_PRESCALER_DIV0, DUAL_TIMER_COUNTERMODE_LOOP, SystemCoreClock/20000 - 1 }}

/*
 * LOCAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */

extern dfu_pro_callback_t *dfu_pro_cb_get(void);

// static void app_log_assert_init(void)
// {
//     app_log_init_t log_init;

//     log_init.filter.level                 = APP_LOG_LVL_DEBUG;
//     log_init.fmt_set[APP_LOG_LVL_ERROR]   = APP_LOG_FMT_ALL & (~APP_LOG_FMT_TAG);
//     log_init.fmt_set[APP_LOG_LVL_WARNING] = APP_LOG_FMT_LVL;
//     log_init.fmt_set[APP_LOG_LVL_INFO]    = APP_LOG_FMT_LVL;
//     log_init.fmt_set[APP_LOG_LVL_DEBUG]   = APP_LOG_FMT_LVL;

//     app_log_init(&log_init, bsp_uart_send, bsp_uart_flush);
//     app_assert_init();
// }

/*
 * GLOBAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */

void diag_gpio_pins_init(void) 
{
//    app_io_init_t io_init = APP_IO_DEFAULT_CONFIG;

//    /**
//     * reset pin
//     */
//    io_init.pull = APP_IO_PULLUP;
//    io_init.mode = APP_IO_MODE_OUTPUT;
//    io_init.pin  = APP_IO_PIN_3 | APP_IO_PIN_4;
//    io_init.mux  = APP_IO_MUX;
//    app_io_init(APP_IO_TYPE_AON, &io_init);
//    
//    app_io_write_pin(APP_IO_TYPE_AON, APP_IO_PIN_3, APP_IO_PIN_RESET);
//    app_io_write_pin(APP_IO_TYPE_AON, APP_IO_PIN_4, APP_IO_PIN_RESET);

}


void diag_gpioa_pin_set(uint32_t pin, bool state) {
//    app_io_write_pin(APP_IO_TYPE_AON, pin, state ? APP_IO_PIN_SET : APP_IO_PIN_RESET);
}

/**@brief Bluetooth device address. */
static const uint8_t s_bd_addr[SYS_BD_ADDR_LEN] = {0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA};
void app_periph_init(void)
{
    SYS_SET_BD_ADDR(s_bd_addr);
    board_init();

    pwr_mgmt_mode_set(PMR_MGMT_ACTIVE_MODE);

    diag_gpio_pins_init();
}

