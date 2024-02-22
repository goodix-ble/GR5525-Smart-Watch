/*
 * INCLUDE FILES
 *****************************************************************************************
 */
#include <stdio.h>
#include <string.h>
#include "app_log.h"
#include "app_io.h"
#include "app_gpiote.h"
#include "board_SK.h"
#include "bt_ctl_uart.h"
#include "bt_sync_tim.h"
/*
 * DEFINES
 *****************************************************************************************
 */

/*
 * GLOBAL FUNCTION DECLARATION
 ****************************************************************************************
 */

static void app_io_event_handler(app_io_evt_t *p_evt);
volatile bool bt_wakeup_gpio_pin_up = false;;// BT_WAKEUP_GPIO_PIN up/down state
/*
 * GLOBAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */

app_gpiote_param_t gpiote_param =
{
    .type = BLE_WAKEUP_GPIO_TYPE,
    .pin = BLE_WAKEUP_GPIO_PIN,
    .mode = APP_IO_MODE_IT_RISING,
    .pull = APP_IO_PULLDOWN ,
    .io_evt_cb = app_io_event_handler,
};

static void app_io_event_handler(app_io_evt_t *p_evt)
{
    ble_bt_uart_recv_async();
    ble_bt_reset_sync_io();
}

void ble_bt_sync_io_init(void)
{
    app_io_init_t io_init;

    io_init.pin  = BT_WAKEUP_GPIO_PIN;
    io_init.mode = APP_IO_MODE_OUTPUT;
    io_init.pull = APP_IO_PULLUP;
    io_init.mux  = APP_IO_MUX;
    app_io_init(BT_WAKEUP_GPIO_TYPE, &io_init);

    bt_wakeup_gpio_pin_up = true;
}

void ble_bt_sync_io_deinit(void)
{
    app_io_init_t io_init;

    io_init.pin  = BT_WAKEUP_GPIO_PIN;
    io_init.mode = APP_IO_MODE_INPUT;
    io_init.pull = APP_IO_NOPULL;
    io_init.mux  = APP_IO_MUX;
    app_io_init(BT_WAKEUP_GPIO_TYPE, &io_init);
    bt_wakeup_gpio_pin_up = false;
}

void ble_bt_reset_sync_io(void)
{
    bt_wakeup_gpio_pin_up = false;
    app_io_write_pin(BT_WAKEUP_GPIO_TYPE, BT_WAKEUP_GPIO_PIN, APP_IO_PIN_RESET);
}

void ble_bt_set_sync_io(void)
{
    bt_wakeup_gpio_pin_up = true;
    app_io_write_pin(BT_WAKEUP_GPIO_TYPE, BT_WAKEUP_GPIO_PIN, APP_IO_PIN_SET);
}

bool ble_bt_set_sync_io_is_up(void)
{
    return bt_wakeup_gpio_pin_up;
}

void bt_ble_sync_gpiote_init(void)
{
    ble_bt_sync_io_init();
    app_gpiote_init(&gpiote_param, sizeof(gpiote_param) / sizeof(app_gpiote_param_t));
}
