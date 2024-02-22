#ifndef _TP_CONFIG_H_
#define _TP_CONFIG_H_

#include "app_io.h"
#include <stdbool.h>

#include "app_drv_config.h"


#define IS_SK_BOARD         1       /* 0 - EVB Board
                                     * 1 - SK Board
                                     */


#if APP_DRIVER_CHIP_TYPE == APP_DRIVER_GR5525X
    /* 5525.SK Board */

    /* SK Board
     *** TP Reset     : GPIO.29
     *** TP Interrupt : GPIO.32
     *** TP SCL       : GPIO.30
     *** TP SDA       : GPIO.31
     */
#if GR5625_SK
    #define TP_RST_IO_TYPE     APP_IO_TYPE_GPIOB
    #define TP_RST_IO_MUX      APP_IO_MUX_8
    #define TP_RST_IO_PIN      APP_IO_PIN_9

    #define TP_INT_IO_TYPE     APP_IO_TYPE_GPIOB
    #define TP_INT_IO_MUX      APP_IO_MUX_8
    #define TP_INT_IO_PIN      APP_IO_PIN_8

    #define TP_SCL_IO_TYPE     APP_IO_TYPE_GPIOA
    #define TP_SCL_IO_MUX      APP_IO_MUX_1
    #define TP_SCL_IO_PIN      APP_IO_PIN_9

    #define TP_SDA_IO_TYPE     APP_IO_TYPE_GPIOA
    #define TP_SDA_IO_MUX      APP_IO_MUX_1
    #define TP_SDA_IO_PIN      APP_IO_PIN_8

    #define TP_I2C_ID          APP_I2C_ID_1
    #define TP_PERIPH_DEVICE   PERIPH_DEVICE_NUM_I2C1
#else
     #define TP_RST_IO_TYPE     APP_IO_TYPE_AON
     #define TP_RST_IO_MUX      APP_IO_MUX_8
     #define TP_RST_IO_PIN      APP_IO_PIN_3

     #define TP_INT_IO_TYPE     APP_IO_TYPE_AON
     #define TP_INT_IO_MUX      APP_IO_MUX_8
     #define TP_INT_IO_PIN      APP_IO_PIN_4

     #define TP_SCL_IO_TYPE     APP_IO_TYPE_GPIOB
     #define TP_SCL_IO_MUX      APP_IO_MUX_0
     #define TP_SCL_IO_PIN      APP_IO_PIN_14

     #define TP_SDA_IO_TYPE     APP_IO_TYPE_GPIOB
     #define TP_SDA_IO_MUX      APP_IO_MUX_0
     #define TP_SDA_IO_PIN      APP_IO_PIN_10

     #define TP_I2C_ID          APP_I2C_ID_1
     #define TP_PERIPH_DEVICE   PERIPH_DEVICE_NUM_I2C1
#endif // GR5625_SK

#elif APP_DRIVER_CHIP_TYPE == APP_DRIVER_GR5526X
    /* TP Pins */
    #if IS_SK_BOARD > 0u
         /* SK Board
         *** TP Reset     : GPIO.29
         *** TP Interrupt : GPIO.32
         *** TP SCL       : GPIO.30
         *** TP SDA       : GPIO.31
         */

         #define TP_RST_IO_TYPE     APP_IO_TYPE_GPIOB
         #define TP_RST_IO_MUX      APP_IO_MUX_7
         #define TP_RST_IO_PIN      APP_IO_PIN_13

         #define TP_INT_IO_TYPE     APP_IO_TYPE_GPIOC
         #define TP_INT_IO_MUX      APP_IO_MUX_7
         #define TP_INT_IO_PIN      APP_IO_PIN_0

         #define TP_SCL_IO_TYPE     APP_IO_TYPE_GPIOB
         #define TP_SCL_IO_MUX      APP_IO_MUX_3
         #define TP_SCL_IO_PIN      APP_IO_PIN_14

         #define TP_SDA_IO_TYPE     APP_IO_TYPE_GPIOB
         #define TP_SDA_IO_MUX      APP_IO_MUX_3
         #define TP_SDA_IO_PIN      APP_IO_PIN_15

         #define TP_I2C_ID          APP_I2C_ID_1
         #define TP_PERIPH_DEVICE   PERIPH_DEVICE_NUM_I2C1
    #else
         /* EVB Board
         *** TP Reset     : AON_GPIO.6
         *** TP Interrupt : AON_GPIO.7
         *** TP SCL       : GPIO.2
         *** TP SDA       : GPIO.3
         */
         #define TP_RST_IO_TYPE     APP_IO_TYPE_AON
         #define TP_RST_IO_MUX      APP_IO_MUX_7
         #define TP_RST_IO_PIN      APP_IO_PIN_6

         #define TP_INT_IO_TYPE     APP_IO_TYPE_AON
         #define TP_INT_IO_MUX      APP_IO_MUX_7
         #define TP_INT_IO_PIN      APP_IO_PIN_7

         #define TP_SCL_IO_TYPE     APP_IO_TYPE_GPIOA
         #define TP_SCL_IO_MUX      APP_IO_MUX_0
         #define TP_SCL_IO_PIN      APP_IO_PIN_2

         #define TP_SDA_IO_TYPE     APP_IO_TYPE_GPIOA
         #define TP_SDA_IO_MUX      APP_IO_MUX_0
         #define TP_SDA_IO_PIN      APP_IO_PIN_3

         #define TP_I2C_ID          APP_I2C_ID_0
         #define TP_PERIPH_DEVICE   PERIPH_DEVICE_NUM_I2C0
    #endif
#else
    #error "NOT SUPPORT"
#endif




#define TP_I2C_IO_CONFIG            {{ TP_SCL_IO_TYPE, TP_SCL_IO_MUX, TP_SCL_IO_PIN, APP_IO_PULLUP }, \
                                     { TP_SDA_IO_TYPE, TP_SDA_IO_MUX, TP_SDA_IO_PIN, APP_IO_PULLUP }}
#define TP_I2C_MODE_CONFIG          { DMA1, DMA1, DMA_Channel2, DMA_Channel3 }
#define TP_I2C_ATTR_CONFIG          { I2C_SPEED_400K, 0x00, I2C_ADDRESSINGMODE_7BIT, I2C_GENERALCALL_DISABLE }
#define TP_I2C_PARAM_CONFIG         { TP_I2C_ID, APP_I2C_ROLE_MASTER, TP_I2C_IO_CONFIG, TP_I2C_MODE_CONFIG, TP_I2C_ATTR_CONFIG }


#define TP_I2C_ADDR          (0x2C)

typedef void (*tp_int_cb_func_t)(void);


bool tp_write(uint8_t reg_addr, uint8_t value);
bool tp_read(uint8_t reg_addr, uint8_t *buffer, uint16_t len);
void tp_config_init(tp_int_cb_func_t tp_int_cb);

#endif

