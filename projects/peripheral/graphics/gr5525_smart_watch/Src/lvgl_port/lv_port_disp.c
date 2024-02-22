
/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp.h"
#include "lv_conf.h"
#include "app_io.h"

#include "display_crtl_drv.h"
#include "flash_driver.h"
#include "lv_port_dma.h"

#include "app_drv_config.h"


/**********************
 *   LOCAL FUNCTIONS AND VARIABLES
 **********************/
static void disp_init(void);
static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
#if LV_GDX_PATCH_SET_CLIP_AREA_ONCE
static void disp_set_clip_area(struct _lv_disp_drv_t * disp_drv, const lv_area_t * clip_area);
#endif // LV_GDX_PATCH_SET_CLIP_AREA_ONCE
#if (SCREEN_TYPE == 1) && TE_SIGNAL_ENABLED
static void disp_start_render(struct _lv_disp_drv_t * disp_drv);
#endif // (SCREEN_TYPE == 1) && TE_SIGNAL_ENABLED
static void rounder_cb(lv_disp_drv_t * disp_drv, lv_area_t * area);
static volatile bool g_lvgl_refr_enable = true;
static volatile bool g_lvgl_disp_enable = true;

#if APP_DRIVER_CHIP_TYPE == APP_DRIVER_GR5525X
    #define VER_BUFF_LINE               40u
#elif APP_DRIVER_CHIP_TYPE == APP_DRIVER_GR5526X
    #define VER_BUFF_LINE               DISP_VER_RES
#else
    #error "NOT SUPPORT"
#endif

__align(16) static uint8_t s_framebuffer_1[DISP_HOR_RES * VER_BUFF_LINE * 2];
__align(16) static uint8_t s_framebuffer_2[DISP_HOR_RES * VER_BUFF_LINE * 2];

__align(16) uint32_t s_magic = 0xDEADBEAF;

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lv_refresh_enable_set(bool enable)
{
    g_lvgl_refr_enable = enable;
    s_magic = s_magic;
}

bool lv_refresh_enable_get(void)
{
    return g_lvgl_refr_enable;
}

void lv_display_enable_set(bool enable)
{
    g_lvgl_disp_enable = enable;
}

bool lv_display_enable_get(void)
{
    return g_lvgl_disp_enable;
}

void lv_port_disp_init(void)
{
    /*-------------------------
     * Initialize your display
     * -----------------------*/
    disp_init();

    s_magic = s_magic;

    /*-----------------------------
     * Create two fixed frame buffers for drawing and never release it
     *----------------------------*/
    static lv_disp_draw_buf_t draw_buf_dsc;
    lv_disp_draw_buf_init(&draw_buf_dsc, (lv_color_t*)s_framebuffer_1, (lv_color_t*)s_framebuffer_2, DISP_HOR_RES * VER_BUFF_LINE);

    /*-----------------------------------
     * Register the display in LVGL
     *----------------------------------*/
    static lv_disp_drv_t disp_drv;                  /*Descriptor of a display driver*/
    lv_disp_drv_init(&disp_drv);                    /*Basic initialization*/

    /*Set the resolution of the display*/
    disp_drv.hor_res = DISP_HOR_RES;
    disp_drv.ver_res = DISP_VER_RES;
    disp_drv.full_refresh = 1;

    /*Used to copy the buffer's content to the display*/
    disp_drv.flush_cb = disp_flush;

    /*Set a display buffer*/
    disp_drv.draw_buf = &draw_buf_dsc;

    disp_drv.rounder_cb = &rounder_cb;

#if LV_GDX_PATCH_SET_CLIP_AREA_ONCE
    disp_drv.set_clip_area_cb = disp_set_clip_area;
#endif // LV_GDX_PATCH_SET_CLIP_AREA_ONCE

#if (SCREEN_TYPE == 1) && TE_SIGNAL_ENABLED
    disp_drv.render_start_cb = disp_start_render;
#endif // (SCREEN_TYPE == 1) && TE_SIGNAL_ENABLED

    lv_disp_t * disp = lv_disp_drv_register(&disp_drv);
    // 不绘制背景颜色
    disp->bg_color = lv_color_black();
    disp->bg_opa = LV_OPA_TRANSP;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
/* Initialize your display and the required peripherals. */
static void disp_init(void)
{
    disp_crtl_init();

    lv_dma_init();
}

/* Flush the content of the internal buffer the specific area on the display
 * You can use DMA or any hardware acceleration to do this operation in the background but
 * 'lv_disp_flush_ready()' has to be called when finished. */
static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    if(lv_display_enable_get())
    {
        disp_crtl_flush(disp_drv, area, color_p);
    }
    disp_drv->draw_buf->flushing = 0;
    disp_drv->draw_buf->flushing_last = 0;
}

static void rounder_cb(lv_disp_drv_t * disp_drv, lv_area_t * area)
{
  /* Per RM69330 datasheet, start coord and size must be even*/
#if (SCREEN_TYPE == 1) && TE_SIGNAL_ENABLED
    area->x1 = 0;
    area->x2 = 359;
    // area->y1 = area->y1 & ~1;
    area->y1 = 0;
    if ((area->y2 - area->y1 + 1) & 1) {
        area->y2 = area->y2 + 1;
    }
#else
    area->x1 = area->x1 & ~1;
    area->y1 = area->y1 & ~1;

    if ((area->x2 - area->x1 + 1) & 1) {
        area->x2 = area->x2 + 1;
    }
    if ((area->y2 - area->y1 + 1) & 1) {
        area->y2 = area->y2 + 1;
    }
#endif // (SCREEN_TYPE == 1) && TE_SIGNAL_ENABLED
}

#if LV_GDX_PATCH_SET_CLIP_AREA_ONCE
static void disp_set_clip_area(struct _lv_disp_drv_t * disp_drv, const lv_area_t * clip_area)
{
    disp_crtl_set_clip_area(clip_area);
}
#endif // LV_GDX_PATCH_SET_CLIP_AREA_ONCE

#if (SCREEN_TYPE == 1) && TE_SIGNAL_ENABLED
static void disp_start_render(struct _lv_disp_drv_t * disp_drv)
{
    extern volatile bool g_need_te_sync;
    g_need_te_sync = true;
}
#endif // (SCREEN_TYPE == 1) && TE_SIGNAL_ENABLED

/* This dummy typedef exists purely to silence -Wpedantic. */
typedef int keep_pedantic_happy;
