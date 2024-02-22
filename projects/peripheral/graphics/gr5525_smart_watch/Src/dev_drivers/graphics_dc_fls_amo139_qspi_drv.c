#include "graphics_dc_lcd_drv.h"
#include "FreeRTOS.h"
#include "app_gpiote.h"
#include "app_io.h"
#include "app_log.h"

#include "settings_storage.h"

/*
 * Defines
 *****************************************************************************************
 */
#define _DC_QSPI_PINS_CFG()              \
    {                                    \
        .csn = {APP_IO_PULLUP, ENABLE},  \
        .clk = {APP_IO_PULLUP, ENABLE},  \
        .io0 = {APP_IO_PULLUP, ENABLE},  \
        .io1 = {APP_IO_PULLUP, ENABLE},  \
        .io2 = {APP_IO_PULLUP, ENABLE},  \
        .io3 = {APP_IO_PULLUP, ENABLE},  \
        .dcx = {APP_IO_PULLUP, DISABLE}, \
    }

#define FLSAMO139_DC_QSPI_CFG()                         \
    {                                                   \
        .mspi_mode = GDC_MODE_QSPI,                     \
        .clock_freq = GDC_CLOCK_FREQ_12MHz,             \
        .clock_mode = GDC_CLOCK_MODE_0,                 \
        .tcsu_cycle = GDC_TCSU_CYCLE_1,                 \
        .layer_mode = GDC_ONE_LAYER_MODE,               \
        .mipicfg_format = GDC_MIPICFG_QSPI_RGB565_OPT0, \
        .resolution_x = 360,                            \
        .resolution_y = 360,                            \
        .pins_cfg = _DC_QSPI_PINS_CFG(),                \
    }

#define LCD_RESET_PIN APP_IO_PIN_6
#define LCD_RESET_GPIO APP_IO_TYPE_AON

#define LCD_VCI_PIN APP_IO_PIN_1
#define LCD_VCI_GPIO APP_IO_TYPE_GPIOC

#define DISP_VCI_EN                                                         \
    {                                                                       \
        LCD_VCI_GPIO, APP_IO_PIN_1, APP_IO_MODE_OUTPUT, APP_IO_PULLUP, NULL \
    }

#define LCD_RES_W 360u
#define LCD_RES_H 360u

/*
 * Declaration
 *****************************************************************************************
 */
static void _lcd_reset_set(uint8_t level);
static void _lcd_vci_set(uint8_t level);
static void _lcd_init(lcd_res_e res, lcd_pixel_mode_e pixel_mode);
static void _lcd_write(uint8_t cmd, uint8_t *data, uint32_t data_len);
static void _lcd_send_ca(uint8_t cmd);
static void _lcd_send_ca1p(uint8_t cmd, uint8_t param);
static void _lcd_send_ca2p(uint8_t cmd, uint8_t p1, uint8_t p2);
static void _lcd_ca_ra_set(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2);
static void _lcd_cmd_sequence(lcd_res_e res, lcd_pixel_mode_e pixel_mode);
static void _lcd_cmd_set_brightness(uint8_t brightness_percent);
static void _lcd_cmd_set_aod_brightness(uint8_t brightness_percent);
static void _lcd_cmd_set_display_state(bool dsplay_on);
static void _dc_irq_event_notify(uint32_t evt);

static bool s_screen_is_on = false;
/*
 * Public
 *****************************************************************************************
 */

static lcd_res_e                    s_res;
static lcd_pixel_mode_e             s_pixel_mode;
static graphics_dc_mipi_format_e    s_mipi_format;
void graphics_dc_flsamo139_qspi_lcd_init(lcd_res_e res, lcd_pixel_mode_e pixel_mode, graphics_dc_mipi_format_e mipi_format) {
    app_graphics_dc_params_t dc_params = FLSAMO139_DC_QSPI_CFG();
    dc_params.mipicfg_format = mipi_format;
    graphics_dc_init(&dc_params, _dc_irq_event_notify);
    _lcd_init(res, pixel_mode);
    s_screen_is_on = true;
    
    s_res = res;
    s_pixel_mode = pixel_mode;
    s_mipi_format = mipi_format;
}

void graphics_dc_flsamo139_qspi_lcd_goto_sleep(void) {
    
    extern void power_display_io_deinit(void);

    power_display_io_deinit();

    app_graphics_dc_set_power_state(GDC_POWER_STATE_SLEEP);
    s_screen_is_on = false;
}

void graphics_dc_flsamo139_qspi_lcd_wakeup(void) {

    app_graphics_dc_set_power_state(GDC_POWER_STATE_ACTIVE);
    extern void graphics_dc_pins_reinit(void);
    graphics_dc_pins_reinit();
    _lcd_init(s_res, s_pixel_mode);
    s_screen_is_on = true;
}


void graphics_dc_flsamo139_qspi_set_show_area(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2)
{
    _lcd_ca_ra_set(x1, x2, y1, y2);
}

/* Set Brightness 
 *  1. AOD means Always on Display) 
 *  2. Percent [0, 100]
 */
void graphics_dc_flsamo139_qspi_set_brightness(bool is_aod, uint8_t percent)
{
    if(is_aod) {
        _lcd_cmd_set_aod_brightness(percent);
    } else {
        _lcd_cmd_set_brightness(percent);
    }
}

void graphics_dc_flsamo139_qspi_set_display_state(bool display_on)
{
    _lcd_cmd_set_display_state(display_on);
}

void graphics_dc_flsamo139_qspi_send_frame(app_graphics_dc_framelayer_t layer0, uint32_t lcd_w, uint32_t lcd_h) {

    app_graphics_dc_cmd_t   dc_cmd;

    dc_cmd.command       = 0x12;
    dc_cmd.address       = 0x002C00;
    dc_cmd.address_width = GDC_FRAME_ADDRESS_WIDTH_24BIT;
    dc_cmd.frame_timing  = GDC_QSPI_FRAME_TIMING_1;

    uint16_t x1 = (((lcd_w -  layer0.resolution_x)/2)/2)*2;
    uint16_t x2 = x1 + layer0.resolution_x - 1;
    uint16_t y1 = (((lcd_h - layer0.resolution_y)/2)/2)*2;
    uint16_t y2 =  y1 + layer0.resolution_y - 1;

    graphics_dc_flsamo139_qspi_set_show_area(x1, x2, y1, y2);
    app_graphics_dc_send_single_frame(GRAPHICS_DC_LAYER_0, &layer0, &dc_cmd, GDC_ACCESS_TYPE_SYNC);

    return;
}

void graphics_dc_flsamo139_set_screen_state(bool on)
{
    APP_LOG_DEBUG("Switch Screen %s", on ? "ON" : "OFF");
    //uint8_t cmd = on ? 0x29 : 0x28;
    app_graphics_dc_set_power_state(GDC_POWER_STATE_ACTIVE);

    if(on) {
        // Sleep Out Then Display On
        //_lcd_write(0x11, NULL, 0);
        _lcd_write(0x29, NULL, 0);
    } else {
        //Display Off Then Sleep in
        _lcd_write(0x28, NULL, 0);
//        delay_ms(10);
//        _lcd_write(0x10, NULL, 0);
    }

    app_graphics_dc_set_power_state(GDC_POWER_STATE_SLEEP);
    s_screen_is_on = on;
}


void graphics_dc_flsamo139_set_screen_dpd(void)
{
    printf("Set Screen DeepSleep\n");

    app_graphics_dc_set_power_state(GDC_POWER_STATE_ACTIVE);
    _lcd_send_ca1p(0x4F, 0x01);
    app_graphics_dc_set_power_state(GDC_POWER_STATE_SLEEP);

}


bool graphics_dc_flsamo139_get_screen_state(void)
{
    return s_screen_is_on;
}

/*
 * Static
 *****************************************************************************************
 */
static void _dc_irq_event_notify(uint32_t evt)
{

}

/* USE GPIO31 to control RESET */
static void _lcd_reset_set(uint8_t level)
{
    app_io_init_t io_init ;
    io_init.mode = APP_IO_MODE_OUTPUT ;
    io_init.mux  = APP_IO_MUX_7;
    io_init.pin  = LCD_RESET_PIN;
    io_init.pull = APP_IO_PULLUP;

    app_io_init(LCD_RESET_GPIO, &io_init);

    if(level) {
        app_io_write_pin(LCD_RESET_GPIO, LCD_RESET_PIN, APP_IO_PIN_SET);
    } else {
        app_io_write_pin(LCD_RESET_GPIO, LCD_RESET_PIN, APP_IO_PIN_RESET);
    }
}

static void _lcd_vci_set(uint8_t level)
{
    app_io_init_t io_init;
    app_gpiote_param_t vci_pin = DISP_VCI_EN;
    app_io_pin_state_t pin_state;

    io_init.pin  = vci_pin.pin;
    io_init.mode = vci_pin.mode;
    io_init.pull = vci_pin.pull;
    io_init.mux  = APP_IO_MUX_7;
    pin_state = level ? APP_IO_PIN_SET : APP_IO_PIN_RESET;

    app_io_init(vci_pin.type, &io_init);
    app_io_write_pin(vci_pin.type, io_init.pin, pin_state);
}

static void _lcd_init(lcd_res_e res, lcd_pixel_mode_e pixel_mode)
{
    _lcd_vci_set(1);
    delay_ms(1);
    _lcd_reset_set(0);
    delay_ms(100);
    _lcd_reset_set(1);
    delay_ms(100);
    _lcd_cmd_sequence(res, pixel_mode);
}


static void _lcd_write(uint8_t cmd, uint8_t *data, uint32_t data_len)
{
    app_graphics_dc_spi_send(0x02, ((uint32_t)cmd) << 8, data, data_len);
}

/* send CA and 0 parameter */
static void _lcd_send_ca(uint8_t cmd)
{
    app_graphics_dc_spi_send(0x02, ((uint32_t)cmd) << 8, NULL, 0);
    return;
}

/* send CA and 1 parameter */
static void _lcd_send_ca1p(uint8_t cmd, uint8_t param)
{
    _lcd_write(cmd, &param, 1);
}

/* send CA and 2 parameter */
static void _lcd_send_ca2p(uint8_t cmd, uint8_t p1, uint8_t p2)
{
    uint8_t p[2] = {p1, p2};
    _lcd_write(cmd, &p[0], 2);
}

static void _lcd_ca_ra_set(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2)
{
    uint8_t data[4];

    data[0] = (x1 & 0xff00) >> 8;
    data[1] = x1 & 0x00ff;
    data[2] = (x2 & 0xff00) >> 8;
    data[3] = x2 & 0x00ff;
    _lcd_write(0x2a, data, 4);

    data[0] = (y1 & 0xff00) >> 8;
    data[1] = y1 & 0x00ff;
    data[2] = (y2 & 0xff00) >> 8;
    data[3] = y2 & 0x00ff;
    _lcd_write(0x2b, data, 4);
}

static void _lcd_cmd_sequence(lcd_res_e res, lcd_pixel_mode_e pixel_mode)
{
    switch(res) {
        case LCD_RES_360:
        {
            /* QSPI Mode */
            _lcd_send_ca(0x11);                  /* sleep out */
            delay_ms(10);
            //_lcd_send_ca(0x13);                /* Normal Display mode on */
            _lcd_send_ca1p(0x36, 0x00);          /* RGB Order */
            _lcd_send_ca1p(0x3A, 0x75);          /* IFPF: 16bit color */
            //_lcd_send_ca(0x47);                /* SPI read On */
            //_lcd_send_ca(0x49);                /* AON Mode on */

            //_lcd_send_ca2p(0x4A, 0xFF, 0x03);    /* AOD Brightness Value */
            _lcd_cmd_set_aod_brightness(g_settings.brightness);
            _lcd_send_ca1p(0x4F, 0x00);          /* Disable deep sleep */
            _lcd_send_ca2p(0x51, 0xFF, 0x03);    /* Brightness Value */
            _lcd_send_ca1p(0xc4, 0x00);          /* Set Quad-SPI Mode*/

            _lcd_ca_ra_set(0, 360 - 1, 0, 360 - 1);

            _lcd_send_ca2p(0x44, 0x01, 0xc5);    /* Set Tear Scan Line */
            _lcd_send_ca1p(0x35, 0x00);          /* Tearing effect line on */
            _lcd_send_ca1p(0x53, 0x28);          /* Brightness Control On & Display Dimming Off */

            delay_ms(20);
            _lcd_send_ca(0x29);                  /* Display on */
            delay_ms(20);
        }
        break;
    }
}

static void _lcd_cmd_set_brightness(uint8_t brightness_percent) {
    uint32_t bright = 0;
    uint8_t  high = 0, low = 0;
    brightness_percent = brightness_percent % 101;
    bright = (brightness_percent * 0xFF) / 100;
    high = (bright >> 8) & 0x03;
    low  = (bright >> 0) & 0xff; 

    // APP_LOG_DEBUG("Bright: %d, reg: %d, high: 0x%02x, low: 0x%02x", brightness_percent, bright, high, low);

    _lcd_send_ca2p(0x51, low, high);
}

static void _lcd_cmd_set_aod_brightness(uint8_t brightness_percent) {
    uint32_t bright = 0;
    uint8_t  high = 0, low = 0;
    brightness_percent = brightness_percent % 101;
    bright = (brightness_percent * 0xFF) / 100;
    high = (bright >> 8) & 0x03;
    low  = (bright >> 0) & 0xff; 

    _lcd_send_ca2p(0x4A, low, high);
}

static void _lcd_cmd_set_display_state(bool dsplay_on) {
    if(dsplay_on) {
        _lcd_send_ca(0x29);
    } else {
        _lcd_send_ca(0x28);
    }
}
