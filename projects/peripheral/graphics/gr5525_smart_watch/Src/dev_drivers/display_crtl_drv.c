#include "app_qspi.h"
#include "app_graphics_qspi.h"
#include "display_crtl_drv.h"
#include "display_rm69330_drv.h"
#include "display_fls_amo139_360p_qspi_drv.h"
#include "lv_refr.h"
#include "system_manager.h"
#include "app_log.h"


#define FLUSH_SYNC_MODE         1  // 0 - cpu sync wait; 1 - sem async wait
#define DISPLAY_IF_MODE         1  // 0 - Display in SPI Mode ; 1 - Display in QSPI Mode
#define QSPI_DISP_USE_DYN_MEM   0

#if LV_GDX_PATCH_SET_CLIP_AREA_ONCE
static lv_area_t const *s_clip_area = NULL;
#endif // LV_GDX_PATCH_SET_CLIP_AREA_ONCE

static void     display_qspi_evt_handler(app_qspi_evt_t *p_evt);

static void     lv_fps_update(void);
static void     lv_refresh_fps_draw(void);

#if FLUSH_SYNC_MODE == 0
static volatile bool is_display_cmplt = false;
static void     qspi_display_clear_flag(void);
static void     qspi_display_wait_cplt(void);
#else
static void _display_sem_init(void) {
    sys_sem_init(&g_semphr.display_sync_sem);
}

static void _display_sem_take(void) {
    sys_sem_take(g_semphr.display_sync_sem, portMAX_DELAY);
}

static void _display_sem_give(void) {
    sys_sem_give(g_semphr.display_sync_sem);
}
#endif


void disp_crtl_init(void) {
#if SCREEN_TYPE == 0
    qspi_screen_init(APP_QSPI_ID_2, 2, display_qspi_evt_handler);
#elif SCREEN_TYPE == 1
    display_fls_amo139_init(APP_QSPI_ID_2, 2, display_qspi_evt_handler);
#endif

#if FLUSH_SYNC_MODE == 1
    _display_sem_init();
    _display_sem_give();
#endif

    extern void diag_gpio_pins_init(void) ;
    diag_gpio_pins_init();
    return;
}

#if !QSPI_DISP_USE_DYN_MEM
static dma_block_config_t s_node_pool[8];
static uint8_t s_node_bitmap = 0;
#endif // QSPI_DISP_USE_DYN_MEM

void * _q_malloc(uint32_t size) {
#if QSPI_DISP_USE_DYN_MEM
    return pvPortMalloc(size);
#else
    uint8_t bitmap = s_node_bitmap;
    uint8_t idx = 0;
    while((bitmap & 0x1) && idx < 8)
    {
        idx++;
        bitmap >>= 1;
    };
    if (idx == 8)
    {
        printf("Node Pool Full!\n");
        return NULL;
    }
    s_node_bitmap |= 1 << idx;
    return &s_node_pool[idx];
#endif // QSPI_DISP_USE_DYN_MEM
}

void _q_free(void * ptr) {
#if QSPI_DISP_USE_DYN_MEM
    vPortFree(ptr);
#else
    uint8_t idx = 0;
    while(idx < 8 && ptr != &s_node_pool[idx])
    {
        idx++;
    }
    if (!(s_node_bitmap & (1 << idx)))
    {
        printf("Invalid free!\n");
    }
    s_node_bitmap &= ~(1 << idx);
#endif // QSPI_DISP_USE_DYN_MEM
}

extern void diag_gpioa_pin_set(uint32_t, bool);

volatile bool g_need_te_sync = false;

void disp_crtl_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p) {

    lv_disp_t * disp = _lv_refr_get_disp_refreshing();
    lv_disp_draw_buf_t * draw_buf = lv_disp_get_draw_buf(disp);

    if(disp_drv->draw_buf->flushing_last) {
        lv_fps_update();
        lv_refresh_fps_draw();
    }

    app_qspi_screen_command_t   screen_cmd;
    app_qspi_screen_info_t      screen_info;
#if DISPLAY_IF_MODE > 0
    /* QSPI Mode */
    screen_cmd.instruction              = 0x12;
    screen_cmd.instruction_size         = QSPI_INSTSIZE_08_BITS;
    screen_cmd.leading_address          = 0x002C00;
    screen_cmd.ongoing_address          = 0x003C00;
    screen_cmd.address_size             = QSPI_ADDRSIZE_24_BITS;
    screen_cmd.data_size                = QSPI_DATASIZE_16_BITS;
    screen_cmd.instruction_address_mode = QSPI_INST_IN_SPI_ADDR_IN_SPIFRF;
    screen_cmd.dummy_cycles             = 0;
    screen_cmd.data_mode                = QSPI_DATA_MODE_QUADSPI;
    screen_cmd.is_one_take_cs           = 1;
#else
    /* SPI Mode */
    screen_cmd.instruction              = 0x02;
    screen_cmd.instruction_size         = QSPI_INSTSIZE_08_BITS;
    screen_cmd.leading_address          = 0x002C00;
    screen_cmd.ongoing_address          = 0x003C00;
    screen_cmd.address_size             = QSPI_ADDRSIZE_24_BITS;
    screen_cmd.data_size                = QSPI_DATASIZE_16_BITS;
    screen_cmd.instruction_address_mode = QSPI_INST_ADDR_ALL_IN_SPI;
    screen_cmd.dummy_cycles             = 0;
    screen_cmd.data_mode                = QSPI_DATA_MODE_SPI;
    screen_cmd.is_one_take_cs           = 1;
#endif

    screen_info.scrn_pixel_width  = area->x2 - area->x1 + 1;
    screen_info.scrn_pixel_height = area->y2 - area->y1 + 1;
    screen_info.scrn_pixel_depth  = 2;

#if FLUSH_SYNC_MODE == 1
    _display_sem_take();
#endif

    diag_gpioa_pin_set(APP_IO_PIN_3, true);
    diag_gpioa_pin_set(APP_IO_PIN_4, true);

#if LV_GDX_PATCH_SET_CLIP_AREA_ONCE
    if (s_clip_area)
    {
#if SCREEN_TYPE == 0
        qspi_screen_set_show_area(s_clip_area->x1, s_clip_area->x2, s_clip_area->y1, s_clip_area->y2);
#elif SCREEN_TYPE == 1
		display_fls_amo139_set_show_area(s_clip_area->x1, s_clip_area->x2, s_clip_area->y1, s_clip_area->y2);
#endif
        s_clip_area = NULL;
    }
    else
    {
        // Do whatever "first call" do except send leading address
        screen_cmd.leading_address = screen_cmd.ongoing_address;
    }

#if SCREEN_TYPE == 1
    if (g_need_te_sync)
    {
        display_fls_amo139_wait_te_signal();
        g_need_te_sync = false;
    }
#endif // SCREEN_TYPE == 1

#else
	#if SCREEN_TYPE == 0
		qspi_screen_set_show_area(area->x1, area->x2, area->y1, area->y2);
	#elif SCREEN_TYPE == 1
		display_fls_amo139_set_show_area(area->x1, area->x2, area->y1, area->y2);
	#endif
#endif // LV_GDX_PATCH_SET_CLIP_AREA_ONCE

#if FLUSH_SYNC_MODE == 0
    qspi_display_clear_flag();
#endif

    app_qspi_send_display_frame(APP_QSPI_ID_2, &screen_cmd, &screen_info, (const uint8_t *)draw_buf->buf_act);

#if FLUSH_SYNC_MODE == 0
    qspi_display_wait_cplt();
#endif
}

void disp_crtl_set_show(void) {

}

#if LV_GDX_PATCH_SET_CLIP_AREA_ONCE
void disp_crtl_set_clip_area(const lv_area_t *clip_area)
{
    s_clip_area = clip_area;
}
#endif // LV_GDX_PATCH_SET_CLIP_AREA_ONCE


static void display_qspi_evt_handler(app_qspi_evt_t *p_evt) {

    diag_gpioa_pin_set(APP_IO_PIN_3, false);
    diag_gpioa_pin_set(APP_IO_PIN_4, false);

    switch (p_evt->type)
    {
        case APP_QSPI_EVT_TX_CPLT:
#if FLUSH_SYNC_MODE== 0
            is_display_cmplt = true;
#else
            _display_sem_give();
#endif // FLUSH_SYNC_MODE== 0
            break;

        case APP_QSPI_EVT_ERROR:
        case APP_QSPI_EVT_ABORT:
#if FLUSH_SYNC_MODE== 0
            is_display_cmplt = true;
#else
            _display_sem_give();
#endif // FLUSH_SYNC_MODE== 0
            break;

        default:
            break;
    }
}

#if FLUSH_SYNC_MODE == 0
static void qspi_display_clear_flag(void) {
    is_display_cmplt = false;
}

static void qspi_display_wait_cplt(void) {
    while(!is_display_cmplt){}
}
#endif


/*
 * FPS Configuration
 */
#define LV_SHOW_FPS_IN_LABEL                        0
#define LV_SHOW_FPS_IN_UART                         1
#define LV_CALC_AVG_FPS                             1
#define LV_FPS_CACHE_NB                             4

/*
 * EXTERNAL DECLARATION
 */
extern void lv_port_set_fb_format(uint32_t format);
extern uint32_t lv_port_get_fb_format(void);
extern uint32_t sys_cpu_usage(void);


__attribute__((unused)) static uint8_t s_fps_save[LV_FPS_CACHE_NB];
#if LV_SHOW_FPS_IN_UART
static uint32_t s_refresh_fps;
#endif
static bool s_debug_info_enable = true;

void lv_port_debug_info_enable(bool enable){
    s_debug_info_enable = enable;
}

bool lv_port_debug_info_get(void)
{
    return s_debug_info_enable;
}


static uint32_t s_last_fps_calculated = 0;
static uint8_t s_frame_flushed = 0;

static void lv_fps_update(void){

    s_frame_flushed++;
    int32_t elapse = lv_tick_elaps(s_last_fps_calculated);
    if (elapse > 1000)
    {
#if LV_SHOW_FPS_IN_UART
        //APP_LOG_DEBUG("FPS: %d, CPU: %d \n", s_refresh_fps, sys_cpu_usage());
        // printf("%.2f FPS\n", 1000.f * s_frame_flushed / elapse);
#endif
        s_frame_flushed = 0;
        s_last_fps_calculated = lv_tick_get();
    }
#if 1
     static uint32_t s_last_flush_tick_cnt = 0;
     static uint32_t s_cur_flush_tick_cnt = 0;
     s_cur_flush_tick_cnt = lv_tick_get();
     uint32_t render_diff_ms = s_cur_flush_tick_cnt - s_last_flush_tick_cnt;
     s_last_flush_tick_cnt = s_cur_flush_tick_cnt;
#if LV_SHOW_FPS_IN_UART
    s_refresh_fps = 1000 / render_diff_ms;
#endif

#if LV_CALC_AVG_FPS
     for(uint8_t i=0; i < LV_FPS_CACHE_NB-1; i++){
         s_fps_save[i] = s_fps_save[i+1];
     }
     s_fps_save[LV_FPS_CACHE_NB-1] = s_refresh_fps;
     uint32_t fps_sum = 0;
     for(uint8_t i = 0; i < LV_FPS_CACHE_NB; i++){
         fps_sum += s_fps_save[i];
     }
     #if (4 == LV_FPS_CACHE_NB)
     s_refresh_fps = fps_sum >> 2;
     #else
     s_refresh_fps = fps_sum / LV_FPS_CACHE_NB;
     #endif
#endif
#endif
}


__attribute__((unused)) static void lv_refresh_fps_draw(void) {
    if(!s_debug_info_enable) return;
#if LV_SHOW_FPS_IN_LABEL
    char info[20] = {0};
    // sprintf(info,"FPS: %d, CPU: %d", s_refresh_fps, sys_cpu_usage());
    lv_draw_label_dsc_t draw_label_dsc;
    lv_draw_label_dsc_init(&draw_label_dsc);
    draw_label_dsc.color = lv_color_white();
    draw_label_dsc.font = &lv_font_montserrat_16;
    lv_area_t coords = {.x1 = 60, .y1=0, .x2=180, .y2=40};
    lv_draw_label(&coords, &coords, &draw_label_dsc, info, NULL);
#endif
}
