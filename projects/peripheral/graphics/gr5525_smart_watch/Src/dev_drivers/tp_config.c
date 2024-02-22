#include "tp_config.h"
#include "app_gpiote.h"
#include "app_i2c.h"
#include "grx_hal.h"
#include <stdbool.h>
#include "lvgl.h"

static tp_int_cb_func_t s_tp_int_cb = NULL;

static void tp_interrupt_callback(app_io_evt_t *p_evt)
{
    if(p_evt->pin == TP_INT_IO_PIN)
    {
       if(s_tp_int_cb != NULL)
       {
           s_tp_int_cb();
       }
    }
}


static void tp_pin_init(void)
{
    app_io_init_t io_init = APP_IO_DEFAULT_CONFIG;

    /**
     * reset pin
     */
    io_init.pull = APP_IO_PULLUP;
    io_init.mode = APP_IO_MODE_OUTPUT;
    io_init.pin  = TP_RST_IO_PIN;
    io_init.mux  = TP_RST_IO_MUX;
    app_io_init(TP_RST_IO_TYPE, &io_init);


     /**
     * interrupt pin
     */
    app_gpiote_param_t param[1] =
    {
         TP_INT_IO_TYPE,
         TP_INT_IO_PIN,
         APP_IO_MODE_IT_FALLING,
         APP_IO_PULLUP,
         tp_interrupt_callback,
     };
    app_gpiote_init(param, 1);
    GPIO2->INTENSET = 0x1;
}


void tp_rst(void)
{
    app_io_write_pin(TP_RST_IO_TYPE, TP_RST_IO_PIN, APP_IO_PIN_RESET);
    delay_ms(100);
    app_io_write_pin(TP_RST_IO_TYPE, TP_RST_IO_PIN, APP_IO_PIN_SET);
    delay_ms(100);
}


bool tp_write(uint8_t reg_addr, uint8_t value)
{
    uint16_t ret = 0;
    ret = app_i2c_mem_write_sync(TP_I2C_ID, TP_I2C_ADDR, reg_addr, I2C_MEMADD_SIZE_8BIT, &value, 1, 1000);
    if (ret != APP_DRV_SUCCESS)
    {
        return false;
    }
    return true;
}


bool tp_read(uint8_t reg_addr, uint8_t *buffer, uint16_t len)
{
    uint16_t ret = 0;
    ret = app_i2c_mem_read_sync(TP_I2C_ID, TP_I2C_ADDR, reg_addr, I2C_MEMADD_SIZE_8BIT, buffer, len, 1000);
    if (ret != APP_DRV_SUCCESS)
    {
        return false;
    }
    return true;
}

app_i2c_params_t tp_params = TP_I2C_PARAM_CONFIG;
void tp_config_init(tp_int_cb_func_t tp_int_cb)
{
    s_tp_int_cb = tp_int_cb;
    app_i2c_init(&tp_params, NULL);

    tp_pin_init();
    tp_rst();
    delay_ms(50);
}

