#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "app_drv_config.h"
#include "app_gpiote.h"
#include "app_io.h"
#include "app_log.h"
#include "app_qspi.h"
#include "app_qspi_dma.h"
#include "display_fls_amo139_360p_qspi_drv.h"


/*
 * Defines
 *****************************************************************************************
 */
#define SCREEN_RES_TYPE                          FLS_DISPLAY_RES_360

#define SCREEN_QSPI_ID                           APP_QSPI_ID_2

#define DEFAULT_SCRN_MODE_CONFIG                 {DMA1, DMA_Channel0, 3000, 0}
#define DEFAULT_SCRN_QSPI_CONFIG                 {2, QSPI_CLOCK_MODE_3, 0}
#define DEFAULT_SCRN_PARAM_CONFIG                {SCREEN_QSPI_ID, g_qspi_pin_groups[QSPI2_PIN_GROUP_0], DEFAULT_SCRN_MODE_CONFIG, DEFAULT_SCRN_QSPI_CONFIG}

#if APP_DRIVER_CHIP_TYPE == APP_DRIVER_GR5525X
#if GR5625_SK
    /* 5525.SK Board */
    #define DISP_RST_CONFIG                  { APP_IO_TYPE_GPIOA, APP_IO_PIN_13, APP_IO_MODE_OUTPUT,  APP_IO_NOPULL, NULL}
#if FLS_DISPLAY_TE_ENABLED
    #define DISP_TE_CONFIG                   { APP_IO_TYPE_GPIOA, APP_IO_PIN_12, APP_IO_MODE_IT_FALLING, APP_IO_NOPULL, _display_te_evt_callback}
#else
    #define DISP_TE_CONFIG                   { APP_IO_TYPE_AON, APP_IO_PIN_5, APP_IO_MODE_INPUT,   APP_IO_NOPULL, NULL}
#endif // FLS_DISPLAY_TE_ENABLED
    #define DISP_DC_CONFIG                   { APP_IO_TYPE_GPIOA, APP_IO_PIN_14, APP_IO_MODE_OUTPUT,  APP_IO_NOPULL, NULL}
#else
    /* 5525.SK Board */
    #define DISP_RST_CONFIG                  { APP_IO_TYPE_AON, APP_IO_PIN_6, APP_IO_MODE_OUTPUT,  APP_IO_NOPULL, NULL}
#if FLS_DISPLAY_TE_ENABLED
    #define DISP_TE_CONFIG                   { APP_IO_TYPE_AON, APP_IO_PIN_5, APP_IO_MODE_IT_FALLING, APP_IO_NOPULL, _display_te_evt_callback}
#else
    #define DISP_TE_CONFIG                   { APP_IO_TYPE_AON, APP_IO_PIN_5, APP_IO_MODE_INPUT,   APP_IO_NOPULL, NULL}
#endif // FLS_DISPLAY_TE_ENABLED
    #define DISP_DC_CONFIG                   { APP_IO_TYPE_AON, APP_IO_PIN_7, APP_IO_MODE_OUTPUT,  APP_IO_NOPULL, NULL}
#endif // GR5625_SK
#elif APP_DRIVER_CHIP_TYPE == APP_DRIVER_GR5526X
    /* misc pins */
    #if 0 // EVB_BOARD
        #define DISP_RST_CONFIG                  { APP_IO_TYPE_AON, APP_IO_PIN_5, APP_IO_MODE_OUTPUT, APP_IO_NOPULL, NULL}
        #define DISP_TE_CONFIG                   { APP_IO_TYPE_AON, APP_IO_PIN_1, APP_IO_MODE_INPUT,   APP_IO_NOPULL, NULL}
        #define DISP_DC_CONFIG                   { APP_IO_TYPE_AON, APP_IO_PIN_6, APP_IO_MODE_INPUT,   APP_IO_NOPULL, NULL}
    #else //SK Board
        #define DISP_RST_CONFIG                  { APP_IO_TYPE_AON, APP_IO_PIN_6, APP_IO_MODE_OUTPUT,  APP_IO_NOPULL, NULL}
        #define DISP_TE_CONFIG                   { APP_IO_TYPE_AON, APP_IO_PIN_5, APP_IO_MODE_INPUT,   APP_IO_NOPULL, NULL}
        #define DISP_DC_CONFIG                   { APP_IO_TYPE_AON, APP_IO_PIN_7, APP_IO_MODE_INPUT,   APP_IO_NOPULL, NULL}
    #endif
#else
    #error "NOT SUPPORT"
#endif

#define TE_WAITING_USE_SEMA 1

/*
 * Declaration
 *****************************************************************************************
 */
static void _display_rst_ctrl(uint8_t level);
static void _display_vci_set(uint8_t level);
static void _display_timing_init(uint32_t res);
static void _display_io_init(app_gpiote_param_t io_para);
static void _display_misc_pins_init(void);
static void _display_spi_send(uint8_t cmd, uint8_t *data, uint32_t data_len);
static void _display_send_ca(uint8_t cmd);
static void _display_send_ca1p(uint8_t cmd, uint8_t param);
static void _display_send_ca2p(uint8_t cmd, uint8_t p1, uint8_t p2);
static void _display_ca_ra_set(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2);
static void _display_cmd_sequence(uint32_t res);
static void _display_set_brightness(uint8_t brightness_percent);
static void _display_set_aod_brightness(uint8_t brightness_percent);
static void _display_set_display_state(bool dsplay_on);
static void _display_qspi_callback(app_qspi_evt_t *p_evt);
#if FLS_DISPLAY_TE_ENABLED
static void _display_te_evt_callback(app_io_evt_t *p_evt);
#endif // FLS_DISPLAY_TE_ENABLED


static app_qspi_params_t        g_qspi_screen_params;
static app_qspi_evt_handler_t   display_evt_handler;
// static volatile uint8_t         g_master_tdone = 0;
// static volatile uint8_t         g_master_rdone = 0;


#if FLS_DISPLAY_TE_ENABLED
#if TE_WAITING_USE_SEMA
static SemaphoreHandle_t s_te_sema = false;
#else
static volatile bool s_te_sema = false;
#endif // TE_WAITING_USE_SEMA
static volatile bool s_te_waiting = false;
static bool s_skip_te = false;
// static uint32_t s_last_edge = 0;
#endif // FLS_DISPLAY_TE_ENABLED
/*
 * Public
 *****************************************************************************************
 */
uint32_t display_fls_amo139_init(app_qspi_id_t id, uint32_t clock_prescaler, app_qspi_evt_handler_t evt_handler)
{
    uint32_t psram_id = 0;
    uint16_t ret;
    app_qspi_params_t p_params = DEFAULT_SCRN_PARAM_CONFIG;

    display_evt_handler = evt_handler;

    g_qspi_screen_params                       = p_params;
    g_qspi_screen_params.id                    = id;
    g_qspi_screen_params.init.clock_prescaler  = clock_prescaler;

    ret = app_qspi_init(&g_qspi_screen_params, _display_qspi_callback);
    if (ret != 0)
    {
        APP_LOG_ERROR("QSPI initial failed! Please check the input paraments.");
        return 1;
    }

    ret = app_qspi_dma_init(&g_qspi_screen_params);
    if (ret != 0)
    {
        APP_LOG_ERROR("QSPI initial dma failed! Please check the input paraments.");
        return 1;
    }

    _display_misc_pins_init();

    _display_timing_init(SCREEN_RES_TYPE);

#if TE_WAITING_USE_SEMA && FLS_DISPLAY_TE_ENABLED
    s_te_sema = xSemaphoreCreateBinary();
    xSemaphoreGive(s_te_sema);
#endif // TE_WAITING_USE_SEMA

#if FLS_DISPLAY_TE_DBG_SIGNAL
    gpio_init_t init = {
        .pin = GPIO_PIN_8 | GPIO_PIN_9,
        .mode = GPIO_MODE_OUTPUT,
        .mux = GPIO_MUX_8,
        .pull = GPIO_NOPULL,
        .speed = GPIO_SPEED_HIGH,
        .strength = GPIO_STRENGTH_MEDIUM,
    };

    hal_gpio_init(GPIO0, &init);
    hal_gpio_write_pin(GPIO0, GPIO_PIN_8 | GPIO_PIN_9, GPIO_PIN_SET);

    init.pin = GPIO_PIN_0;
    hal_gpio_init(GPIO1, &init);
    hal_gpio_write_pin(GPIO1, GPIO_PIN_0, GPIO_PIN_SET);
#endif // FLS_DISPLAY_TE_DBG_SIGNAL

    return psram_id;
}

void display_fls_amo139_deinit(app_qspi_id_t id)
{
    app_qspi_deinit(id);
}

void display_fls_amo139_set_show_area(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2)
{
    static uint16_t last_x1 = 0;
    static uint16_t last_y1 = 0;
    static uint16_t last_x2 = 0;
    static uint16_t last_y2 = 0;

    if (last_x1 == x1 && last_y1 == y1 && last_x2 == x2 && last_y2 == y2)
    {
        return;
    }

    last_x1 = x1;
    last_y1 = y1;
    last_x2 = x2;
    last_y2 = y2;

    // 360x360
    _display_ca_ra_set(x1, x2, y1, y2);
#if FLS_DISPLAY_TE_ENABLED
    // uint16_t scanline = y1 + 15;
    // _display_send_ca2p(0x44, scanline >> 8, scanline & 0xFF);
#endif // FLS_DISPLAY_TE_ENABLED

    // 240x280
    //_display_ca_ra_set(60 + x1, 60 + x2, 40 + y1, 40 + y2);
}

/* Set Brightness 
 *  1. AOD means Always on Display) 
 *  2. Percent [0, 100]
 */
void display_fls_amo139_set_brightness(bool is_aod, uint8_t percent)
{
    if(is_aod) {
        _display_set_aod_brightness(percent);
    } else {
        _display_set_brightness(percent);
    }
}

void display_fls_amo139_set_display_state(bool display_on)
{
    _display_set_display_state(display_on);
}

#if FLS_DISPLAY_TE_ENABLED
void display_fls_amo139_wait_te_signal(void)
{
    if (s_skip_te) return;
    hal_gpio_write_pin(GPIO1, GPIO_PIN_0, GPIO_PIN_RESET);
    s_te_waiting = true;
#if TE_WAITING_USE_SEMA
    xSemaphoreTake(s_te_sema, portMAX_DELAY);
#else
    while(!s_te_sema);
    s_te_sema = false;
#endif // TE_WAITING_USE_SEMA
    hal_gpio_write_pin(GPIO0, GPIO_PIN_8, GPIO_PIN_RESET);
    hal_gpio_write_pin(GPIO1, GPIO_PIN_0, GPIO_PIN_SET);
    s_te_waiting = false;
}

void display_fls_amo139_set_te_enable(bool enable)
{
    s_skip_te = !enable;
}
#endif // FLS_DISPLAY_TE_ENABLED


/*
 * Static Functions
 *****************************************************************************************
 */
static void _display_timing_init(uint32_t res)
{
    _display_vci_set(1);
    delay_ms(1);
    _display_rst_ctrl(0);
    delay_ms(100);
    _display_rst_ctrl(1);
    delay_ms(100);
    _display_cmd_sequence(res);
}

static void _display_misc_pins_init(void)
{
    app_gpiote_param_t io_para1 = DISP_RST_CONFIG;
    _display_io_init(io_para1);

    app_gpiote_param_t io_para2 = DISP_TE_CONFIG;
    _display_io_init(io_para2);

    app_gpiote_param_t io_para3 = DISP_DC_CONFIG;
    _display_io_init(io_para3);
}


static void _display_vci_set(uint8_t level)
{

}


static void _display_io_init(app_gpiote_param_t io_para)
{
    app_io_init_t io_init;
    io_init.pin  = io_para.pin;
    io_init.mode = io_para.mode;
    io_init.pull = io_para.pull;
#if APP_DRIVER_CHIP_TYPE == APP_DRIVER_GR5525X
    io_init.mux  = APP_IO_MUX_8;
#elif APP_DRIVER_CHIP_TYPE == APP_DRIVER_GR5526X
    io_init.mux  = APP_IO_MUX_7;
#endif

    if (io_para.io_evt_cb)
    {
        app_io_event_register_cb(io_para.type, &io_init, io_para.io_evt_cb, NULL);
    }
    else
    {
        app_io_init(io_para.type, &io_init);
    }
}


static void _display_rst_ctrl(uint8_t level)
{
    app_io_pin_state_t pin_state;
    app_gpiote_param_t rst_pin = DISP_RST_CONFIG;
    app_io_type_t pin_type = rst_pin.type;
    uint32_t pin = rst_pin.pin;

    pin_state = level ? APP_IO_PIN_SET : APP_IO_PIN_RESET;
    app_io_write_pin(pin_type, pin, pin_state);
}

static void _display_spi_send(uint8_t cmd, uint8_t *data, uint32_t data_len)
{
#if 0
    static uint8_t data_cache[32];
    uint16_t ret = 0;

    memset(&data_cache[0], 0, 32);

    data_cache[0] = 0x02;
    data_cache[1] = 0x00;
    data_cache[2] = cmd;
    data_cache[3] = 0x00;

    if(data != NULL && data_len !=0) {
        memcpy(&data_cache[4], data, data_len);
    }

    ret = app_qspi_dma_transmit_async_ex(SCREEN_QSPI_ID, QSPI_DATA_MODE_SPI, QSPI_DATASIZE_08_BITS, &data_cache[0], 4 + data_len);
    if(ret == 0)
        while(g_master_tdone == 0);
    else
        printf("Fail:%d \r\n", ret);

    return;
#else
    uint16_t ret;
    app_qspi_command_t qspi_cmd;
    qspi_cmd.instruction        = 0x02;
    qspi_cmd.address            = ((uint16_t)cmd) << 8;
    qspi_cmd.instruction_size   = QSPI_INSTSIZE_08_BITS;
    qspi_cmd.address_size       = QSPI_ADDRSIZE_24_BITS;
    qspi_cmd.dummy_cycles       = 0;
    qspi_cmd.data_size          = QSPI_DATASIZE_08_BITS;
    qspi_cmd.instruction_address_mode = QSPI_INST_ADDR_ALL_IN_SPI;
    qspi_cmd.data_mode          = QSPI_DATA_MODE_SPI;
    qspi_cmd.length             = data_len;
    qspi_cmd.clock_stretch_en   = LL_QSPI_CLK_STRETCH_ENABLE;
    uint8_t _wtf = 0;
    portDISABLE_INTERRUPTS();
    ret = app_qspi_command_transmit_sync(SCREEN_QSPI_ID, &qspi_cmd, data ? data : &_wtf, 0xFFFFFFFF);
    portENABLE_INTERRUPTS();
    if (ret != 0)
    {
        printf("QSPI Command Transmit Failed!: %d\n", ret);
    }
#endif // 0

}

/* send CA and 0 parameter */
static void _display_send_ca(uint8_t cmd)
{
    _display_spi_send(cmd, NULL, 0);
    return;
}

/* send CA and 1 parameter */
static void _display_send_ca1p(uint8_t cmd, uint8_t param)
{
    _display_spi_send(cmd, &param, 1);
}

/* send CA and 2 parameter */
static void _display_send_ca2p(uint8_t cmd, uint8_t p1, uint8_t p2)
{
    uint8_t p[2] = {p1, p2};
    _display_spi_send(cmd, &p[0], 2);
}

static void _display_ca_ra_set(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2)
{
    uint8_t data[4];

    data[0] = (x1 & 0xff00) >> 8;
    data[1] = x1 & 0x00ff;
    data[2] = (x2 & 0xff00) >> 8;
    data[3] = x2 & 0x00ff;
    _display_spi_send(0x2a, data, 4);

    data[0] = (y1 & 0xff00) >> 8;
    data[1] = y1 & 0x00ff;
    data[2] = (y2 & 0xff00) >> 8;
    data[3] = y2 & 0x00ff;
    _display_spi_send(0x2b, data, 4);
}

static void _display_cmd_sequence(uint32_t res)
{
    switch(res) {
        case FLS_DISPLAY_RES_360:
        {
            /* QSPI Mode */
            _display_send_ca(0x11);                  /* sleep out */
            delay_ms(10);

            // Change Refresh Rate
            /* Refresh rate to 0xB1 value LUT:
            60Hz - 0x07 0x00
            55Hz - 0x2A 0x00
            50Hz - 0x54 0x00
            45Hz - 0x86 0x00
            40Hz - 0xC8 0x00
            35Hz - 0x18 0x01
            30Hz - 0x7A 0x01
            RegisterValue = 22.88 * IdleTime - 9.1
            */
#if FLS_DISPLAY_TE_ENABLED
            _display_send_ca2p(0xC0, 0x5A, 0x5A);
            _display_send_ca1p(0xB0, 0x08);
            _display_send_ca2p(0xB1, 0xC8, 0x00);
            _display_send_ca2p(0xC0, 0xA5, 0xA5);
#endif // FLS_DISPLAY_TE_ENABLED

            //_display_send_ca(0x13);                /* Normal Display mode on */
            _display_send_ca1p(0x36, 0x00);          /* RGB Order */
            _display_send_ca1p(0x3A, 0x75);          /* IFPF: 16bit color */
            //_display_send_ca(0x47);                /* SPI read On */
            //_display_send_ca(0x49);                /* AON Mode on */

            //_display_send_ca2p(0x4A, 0xFF, 0x03);    /* AOD Brightness Value */
            _display_set_aod_brightness(90);
            _display_send_ca1p(0x4F, 0x00);          /* Disable deep sleep */
            _display_send_ca2p(0x51, 0xFF, 0x03);    /* Brightness Value */
            _display_send_ca1p(0xc4, 0x00);          /* Set Quad-SPI Mode*/

            _display_ca_ra_set(0, 360 - 1, 0, 360 - 1);

            _display_send_ca2p(0x44, 0x00, 15);    /* Set Tear Scan Line */
            _display_send_ca1p(0x35, 0x00);          /* Tearing effect line on */
            _display_send_ca1p(0x53, 0x28);          /* Brightness Control On & Display Dimming Off */

            delay_ms(40);
            _display_send_ca(0x29);                  /* Display on */
            delay_ms(40);
        }
        break;
    }
}

static void _display_set_brightness(uint8_t brightness_percent) {
    uint32_t bright = 0;
    uint8_t  high = 0, low = 0;
    brightness_percent = brightness_percent % 101;
    bright = (brightness_percent * 0xFF) / 100;
    high = (bright >> 8) & 0x03;
    low  = (bright >> 0) & 0xff; 

    _display_send_ca2p(0x51, low, high);
}

static void _display_set_aod_brightness(uint8_t brightness_percent) {
    uint32_t bright = 0;
    uint8_t  high = 0, low = 0;
    brightness_percent = brightness_percent % 101;
    bright = (brightness_percent * 0xFF) / 100;
    high = (bright >> 8) & 0x03;
    low  = (bright >> 0) & 0xff; 

    _display_send_ca2p(0x4A, low, high);
}

static void _display_set_display_state(bool dsplay_on) {
    if(dsplay_on) {
        _display_send_ca(0x29);
    } else {
        _display_send_ca(0x28);
    }
}


static void _display_qspi_callback(app_qspi_evt_t *p_evt)
{
    if (display_evt_handler != NULL)
    {
        display_evt_handler(p_evt);
    }
}

#if FLS_DISPLAY_TE_ENABLED
static void _display_te_evt_callback(app_io_evt_t *p_evt)
{
    if (s_te_waiting)
    {
#if TE_WAITING_USE_SEMA
        xSemaphoreGiveFromISR(s_te_sema, NULL);
#else
        s_te_sema = true;
#endif // TE_WAITING_USE_SEMA
    }
}
#endif // FLS_DISPLAY_TE_ENABLED
