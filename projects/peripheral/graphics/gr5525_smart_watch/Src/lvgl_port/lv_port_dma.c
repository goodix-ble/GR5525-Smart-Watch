#include "lv_conf.h"
#include "lv_port_dma.h"
#include "FreeRTOS.h"
#include "system_manager.h"


#define DMA_SYNC_MODE                   1       // 0 : CPU Wait ; 1 : Semhr Wait
#define DMA_LLP_NODE_DYNAMIC_ALLOC      1u

#if DMA_LLP_NODE_DYNAMIC_ALLOC == 0u
dma_block_config_t s_dma_llp_block[DISP_VER_RES + 1];
#endif

static app_dma_params_t s_dma_param;
static dma_id_t         s_dma_id;
//static volatile bool is_dma_xfer_done = false;
//static volatile bool is_dma_blk_done  = false;
static volatile bool is_dma_xfer_err  = false;

static volatile uint8_t s_dma_xfer_flag = 0x00;
static volatile uint8_t s_dma_over_flag = 0x00;

#define _DMA_FLAG_XFER_DONE  0x01
#define _DMA_FLAG_BLK_DONE   0x02

static void app_dma_evt_handler(app_dma_evt_type_t type) {
    switch(type) {
        case APP_DMA_EVT_TFR:
            is_dma_xfer_err = false;
            //is_dma_xfer_done = true;
            s_dma_xfer_flag  |= _DMA_FLAG_XFER_DONE;
            break;

        case APP_DMA_EVT_BLK:
            is_dma_xfer_err = false;
            //is_dma_blk_done = true;
            s_dma_xfer_flag  |= _DMA_FLAG_BLK_DONE;
            break;

        case APP_DMA_EVT_ERROR:
            s_dma_xfer_flag = s_dma_over_flag;
            is_dma_xfer_err = true;
            break;
    }
#if DMA_SYNC_MODE != 0
    if(s_dma_over_flag == s_dma_xfer_flag)
        sys_sem_give(g_semphr.dma_mem_xfer_sem);
#endif
}

void lv_dma_init(void) {

    s_dma_param.p_instance      = DMA0;
    s_dma_param.channel_number  = DMA_Channel0;

    s_dma_param.init.direction     = DMA_MEMORY_TO_MEMORY;
    s_dma_param.init.src_increment = DMA_SRC_INCREMENT;
    s_dma_param.init.dst_increment = DMA_DST_INCREMENT;
    s_dma_param.init.src_data_alignment = DMA_SDATAALIGN_HALFWORD;
    s_dma_param.init.dst_data_alignment = DMA_DDATAALIGN_HALFWORD;
    s_dma_param.init.mode     = DMA_NORMAL;
    s_dma_param.init.priority = DMA_PRIORITY_HIGH;

    s_dma_id =  app_dma_init(&s_dma_param, app_dma_evt_handler);
    sys_sem_init(&g_semphr.dma_mem_xfer_sem);
    //sys_sem_give(g_semphr.dma_mem_xfer_sem);
}

#if 1
static void _reconfig_dma(uint32_t src_inc, uint32_t dst_inc, uint32_t src_width, uint32_t dst_width, uint32_t extra) {
    if(1) {
    ll_dma_config_transfer(s_dma_param.p_instance, s_dma_param.channel_number,
                           DMA_MEMORY_TO_MEMORY | \
                           src_inc              | \
                           dst_inc              | \
                           src_width            | \
                           dst_width            | \
                           LL_DMA_SRC_BURST_LENGTH_4 | \
                           LL_DMA_DST_BURST_LENGTH_4 |
                           extra);
    }
}
#endif

#define DMA_LLP_BLOCK_CTL_CONST_FLAGS (DMA_CTLL_INI_EN | DMA_SRC_INCREMENT | DMA_SRC_GATHER_ENABLE | DMA_DST_SCATTER_ENABLE | DMA_LLP_SRC_ENABLE | DMA_LLP_DST_ENABLE | DMA_MEMORY_TO_MEMORY)

/*
 * dst_addr
 * row_pixel    : unit in pixel
 * stride_pixel : unit in pixel
 * col_pixel    : pixel count of coloumn
 * color        : color value
 *
 */
static uint32_t src_color = 0;
__attribute__((section("RAM_CODE"))) void lv_dma_fill_color(uint32_t dst_addr, uint32_t row_pixel, uint32_t stride_pixel, uint32_t col_pixel, uint32_t color) {
    uint16_t status ;
    uint16_t color_bytes = 2;//DISP_PIXEL_DEPTH;
    src_color                  = color;
    uint32_t p_src_addr        = (uint32_t) &src_color;
    uint32_t p_dst_addr        = (uint32_t) dst_addr;
    uint32_t i                 = 0;

    dma_block_config_t * p_llp_block      = NULL;
    dma_block_config_t * p_llp_block_prev = NULL;
    lv_color_t  c ;

    uint32_t total_pixel = row_pixel * col_pixel;

    c.full = color;
    if(total_pixel <= 60*60) {
        for(int y = 0; y < col_pixel; y++) {
            lv_color_fill((lv_color_t*)dst_addr, c, row_pixel);
            dst_addr += stride_pixel*color_bytes;
        }

        return;
    }

    uint32_t block_count = 0;
    uint32_t block_left  = 0;
    uint32_t block_pixel = 0;
    uint32_t block_stride= 0;
    uint32_t xfer_width  = 0;
    uint32_t divider = (4092/row_pixel)*row_pixel;

    if(row_pixel == stride_pixel) {
        block_count  = total_pixel / divider;
        block_left   = total_pixel % divider;
        block_pixel  = divider;
        block_stride = divider;
    } else {
        block_count  = col_pixel;
        block_left   = 0;
        block_pixel  = row_pixel;
        block_stride = stride_pixel;
    }

    if(color_bytes == 4) {
        xfer_width = DMA_SDATAALIGN_WORD     | DMA_DDATAALIGN_WORD ;
    } else if(color_bytes == 2) {
        xfer_width = DMA_SDATAALIGN_HALFWORD | DMA_DDATAALIGN_HALFWORD ;
    }

    // SG LLP Config
    dma_sg_llp_config_t sg_llp_config = {
        .gather_config = {
            .src_gather_en = DMA_SRC_GATHER_DISABLE,
        },
        .scatter_config = {
            .dst_dsc = 1,//row_pixel,
            .dst_dsi = 0,//(stride_pixel - row_pixel),
            .dst_scatter_en = DMA_DST_SCATTER_ENABLE,
        },
        .llp_config = {
            .llp_src_en = DMA_LLP_SRC_DISABLE,
            .llp_dst_en = DMA_LLP_DST_ENABLE,
            .head_lli = NULL,
        },
    };

    for(i = 0; i < block_count; i++) {
#if DMA_LLP_NODE_DYNAMIC_ALLOC > 0u
        p_llp_block = pvPortMalloc(sizeof(dma_block_config_t));
#else
        p_llp_block = &s_dma_llp_block[i];
#endif
        p_llp_block->src_address = p_src_addr;
        p_llp_block->dst_address = p_dst_addr;
        p_llp_block->src_status  = 0x00;
        p_llp_block->dst_status  = 0x00;
        p_llp_block->CTL_L       = DMA_CTLL_INI_EN
                                    | DMA_SRC_NO_CHANGE
                                    | DMA_DST_INCREMENT
                                    | DMA_SRC_GATHER_DISABLE
                                    | DMA_DST_SCATTER_ENABLE
                                    | DMA_LLP_SRC_DISABLE
                                    | DMA_LLP_DST_ENABLE
                                    | DMA_MEMORY_TO_MEMORY
                                    | xfer_width;
        p_llp_block->CTL_H       = block_pixel;
        p_llp_block->p_lli       = NULL;

        if(p_llp_block_prev == NULL) {
            sg_llp_config.llp_config.head_lli = p_llp_block;
        } else {
            p_llp_block_prev->p_lli = p_llp_block;
        }
        p_llp_block_prev = p_llp_block;

        p_dst_addr += block_stride * color_bytes;
    }

    if(block_left > 0) {
#if DMA_LLP_NODE_DYNAMIC_ALLOC > 0u
        p_llp_block = pvPortMalloc(sizeof(dma_block_config_t));
#else
        p_llp_block = &s_dma_llp_block[i];
#endif
        p_llp_block->src_address = p_src_addr;
        p_llp_block->dst_address = p_dst_addr;
        p_llp_block->src_status  = 0x00;
        p_llp_block->dst_status  = 0x00;
        p_llp_block->CTL_L       = DMA_CTLL_INI_EN
                                    | DMA_SRC_NO_CHANGE
                                    | DMA_DST_INCREMENT
                                    | DMA_SRC_GATHER_DISABLE
                                    | DMA_DST_SCATTER_ENABLE
                                    | DMA_LLP_SRC_DISABLE
                                    | DMA_LLP_DST_ENABLE
                                    | DMA_MEMORY_TO_MEMORY
                                    | xfer_width;
        p_llp_block->CTL_H       = block_left;
        p_llp_block->p_lli       = NULL;

        if(p_llp_block_prev == NULL) {
            sg_llp_config.llp_config.head_lli = p_llp_block;
        } else {
            p_llp_block_prev->p_lli = p_llp_block;
        }
        p_llp_block_prev = p_llp_block;
    }

    s_dma_xfer_flag  = 0x00;
    s_dma_over_flag  = _DMA_FLAG_XFER_DONE | _DMA_FLAG_BLK_DONE;

    status = app_dma_start_sg_llp(s_dma_id, p_src_addr, p_dst_addr, block_pixel, &sg_llp_config);
    if(status == 0) {
#if DMA_SYNC_MODE == 0
        while(s_dma_xfer_flag != s_dma_over_flag) {}
#else
        sys_sem_take(g_semphr.dma_mem_xfer_sem, portMAX_DELAY);
#endif
    } else {
        printf("dma xfer err :%d!\r\n", status);
    }

#if DMA_LLP_NODE_DYNAMIC_ALLOC > 0u
    // Free LLP nodes
    if (sg_llp_config.llp_config.head_lli)
    {
        dma_block_config_t * p = sg_llp_config.llp_config.head_lli;
        dma_block_config_t * q = NULL;
        while (p)
        {
            q = p->p_lli;
            vPortFree(p);
            p = q;
        }
    }
#else
    memset(&s_dma_llp_block[0], 0, (DISP_VER_RES + 1)*sizeof(dma_block_config_t));
#endif

    return;
}


__attribute__((section("RAM_CODE"))) void lv_dma_memset(uint32_t dst_addr, uint32_t row_pixel, uint32_t stride_pixel, uint32_t col_pixel) {
    uint16_t status ;
    const uint16_t color_bytes = DISP_PIXEL_DEPTH;
    static uint32_t color      = 0x00000000;
    uint32_t p_src_addr        = (uint32_t) &color;
    uint32_t p_dst_addr        = (uint32_t) dst_addr;

    dma_block_config_t * p_llp_block      = NULL;
    dma_block_config_t * p_llp_block_prev = NULL;

    uint32_t row_bytes = row_pixel * color_bytes;
    bool is_word_xfer  = false; //(row_bytes % 4 != 0) ? false : true;

    uint32_t total_bytes = row_bytes * col_pixel;

    if(row_pixel*col_pixel <= 60*60)
    {
        if(row_pixel == stride_pixel) {
            memset((void*)dst_addr, 0x00, color_bytes * row_pixel * col_pixel);
            dst_addr += col_pixel*stride_pixel*color_bytes;
        } else {
            for(int y = 0; y < col_pixel; y++) {
                memset((void*)dst_addr, 0x00, color_bytes * row_pixel);
                dst_addr += stride_pixel*color_bytes;
            }
        }

        return;
    }

    uint32_t block_count = 0;
    uint32_t block_left  = 0;
    uint32_t block_size  = 0;
    uint32_t stride_size = 0;
    uint32_t block_beat  = 0;
    uint32_t xfer_width  = 0;
    int i = 0;

    if(is_word_xfer) {
        xfer_width = DMA_SDATAALIGN_WORD     | DMA_DDATAALIGN_WORD ;
    } else {
        xfer_width = DMA_SDATAALIGN_HALFWORD | DMA_DDATAALIGN_HALFWORD ;
    }

    if(row_pixel == stride_pixel) {

        block_beat  = (4092/row_pixel)*row_pixel;

        if(is_word_xfer) {
            block_size  = block_beat*4;
        } else {
            block_size  = block_beat*2;
        }

        block_count = total_bytes / block_size;
        block_left  = total_bytes % block_size;
        stride_size = block_size;
    } else {
        block_size  = row_bytes;
        if(is_word_xfer) {
            block_beat  = row_bytes >> 2;
        } else {
            block_beat  = row_bytes >> 1;
        }
        block_count = col_pixel;
        block_left  = 0;
        stride_size = stride_pixel * color_bytes;
    }

    // SG LLP Config
    dma_sg_llp_config_t sg_llp_config = {
        .gather_config = {
            .src_gather_en = DMA_SRC_GATHER_DISABLE,
        },
        .scatter_config = {
            .dst_scatter_en = DMA_DST_SCATTER_ENABLE,
            .dst_dsc = 1,//is_word_xfer ? (row_bytes >> 2) : (row_bytes >> 1),
            .dst_dsi = 0,//is_word_xfer ? (((stride_pixel - row_pixel)*color_bytes) >> 2) : (((stride_pixel - row_pixel)*color_bytes) >> 1),
        },
        .llp_config = {
            .llp_src_en = DMA_LLP_SRC_DISABLE,
            .llp_dst_en = DMA_LLP_DST_ENABLE,
            .head_lli = NULL,
        },
    };

    for(i = 0; i < block_count; i++) {
#if DMA_LLP_NODE_DYNAMIC_ALLOC > 0u
        p_llp_block = pvPortMalloc(sizeof(dma_block_config_t));
#else
        p_llp_block = &s_dma_llp_block[i];
#endif
        p_llp_block->src_address = p_src_addr;
        p_llp_block->dst_address = p_dst_addr;
        p_llp_block->src_status  = 0x00;
        p_llp_block->dst_status  = 0x00;

        /* memset in word mode */
        p_llp_block->CTL_L       = DMA_CTLL_INI_EN
                                    | DMA_SRC_NO_CHANGE
                                    | DMA_DST_INCREMENT
                                    | DMA_SRC_GATHER_DISABLE
                                    | DMA_DST_SCATTER_ENABLE
                                    | DMA_LLP_SRC_DISABLE
                                    | DMA_LLP_DST_ENABLE
                                    | DMA_MEMORY_TO_MEMORY
                                    | xfer_width;

        p_llp_block->CTL_H       = block_beat;
        p_llp_block->p_lli       = NULL;

        if(p_llp_block_prev == NULL) {
            sg_llp_config.llp_config.head_lli = p_llp_block;
        } else {
            p_llp_block_prev->p_lli = p_llp_block;
        }
        p_llp_block_prev = p_llp_block;

        p_dst_addr += stride_size;
    }

    if(block_left > 0) {
#if DMA_LLP_NODE_DYNAMIC_ALLOC > 0u
        p_llp_block = pvPortMalloc(sizeof(dma_block_config_t));
#else
        p_llp_block = &s_dma_llp_block[i];
#endif
        p_llp_block->src_address = p_src_addr;
        p_llp_block->dst_address = p_dst_addr;
        p_llp_block->src_status  = 0x00;
        p_llp_block->dst_status  = 0x00;
        p_llp_block->CTL_L       = DMA_CTLL_INI_EN
                                   | DMA_SRC_NO_CHANGE
                                   | DMA_DST_INCREMENT
                                   | DMA_SRC_GATHER_DISABLE
                                   | DMA_DST_SCATTER_ENABLE
                                   | DMA_LLP_SRC_DISABLE
                                   | DMA_LLP_DST_ENABLE
                                   | DMA_MEMORY_TO_MEMORY
                                   | xfer_width;
        p_llp_block->CTL_H       = is_word_xfer ? (block_left >> 2) : (block_left >> 1);
        p_llp_block->p_lli       = NULL;

        if(p_llp_block_prev == NULL) {
            sg_llp_config.llp_config.head_lli = p_llp_block;
        } else {
            p_llp_block_prev->p_lli = p_llp_block;
        }
        p_llp_block_prev = p_llp_block;
    }

    s_dma_xfer_flag  = 0x00;
    s_dma_over_flag  = _DMA_FLAG_XFER_DONE | _DMA_FLAG_BLK_DONE;
    status = app_dma_start_sg_llp(s_dma_id, p_src_addr, p_dst_addr, block_beat, &sg_llp_config);
    if(status == 0) {
#if DMA_SYNC_MODE == 0
        while(s_dma_xfer_flag != s_dma_over_flag) {}
#else
        sys_sem_take(g_semphr.dma_mem_xfer_sem, portMAX_DELAY);
#endif
    } else {
        printf("dma xfer err :%d!\r\n", status);
    }

#if DMA_LLP_NODE_DYNAMIC_ALLOC > 0u
    // Free LLP nodes
    if (sg_llp_config.llp_config.head_lli)
    {
        dma_block_config_t * p = sg_llp_config.llp_config.head_lli;
        dma_block_config_t * q = NULL;
        while (p)
        {
            q = p->p_lli;
            vPortFree(p);
            p = q;
        }
    }
#else
    memset(&s_dma_llp_block[0], 0, (DISP_VER_RES + 1)*sizeof(dma_block_config_t));
#endif
    return;
}

__attribute__((section("RAM_CODE")))  bool lv_dma_fill_rect_rgb565_image(_dma_fill_image_config_t * img_fill_cfg) {

    bool ret          = true;
    uint16_t status   = HAL_OK;
#if 0
    if(img_fill_cfg == NULL) {
        return false;
    }

    if(img_fill_cfg->src_img_x + img_fill_cfg->src_img_x_delta > img_fill_cfg->src_img_w) {
        return false;
    }

    if(img_fill_cfg->src_img_y + img_fill_cfg->src_img_y_delta > img_fill_cfg->src_img_h) {
        return false;
    }
#endif
        // SG LLP Config
    dma_sg_llp_config_t sg_llp_config = {
        .gather_config = {
            .src_gather_en = DMA_SRC_GATHER_DISABLE,
        },
        .scatter_config = {
            .dst_scatter_en = DMA_DST_SCATTER_DISABLE,
        },
        .llp_config = {
            .llp_src_en = DMA_LLP_SRC_DISABLE,
            .llp_dst_en = DMA_LLP_DST_DISABLE,
            .head_lli = NULL,
        },
    };

    const uint32_t src_start_address = img_fill_cfg->src_img_address + (img_fill_cfg->src_img_y * img_fill_cfg->src_img_w + img_fill_cfg->src_img_x) * img_fill_cfg->pixel_depth;
    const uint32_t dst_start_address = img_fill_cfg->dst_buff_address + (img_fill_cfg->dst_buff_y * img_fill_cfg->dst_buff_width + img_fill_cfg->dst_buff_x) * img_fill_cfg->pixel_depth;
    const uint32_t total_lines       = img_fill_cfg->src_img_y_delta;
    const uint32_t total_pixels      = img_fill_cfg->src_img_x_delta * img_fill_cfg->src_img_y_delta;

    if(total_pixels <= LV_DMA_MAX_XFER_SIZE_ONCE) {
        //by DMA S&G Way
        {
            /* source - gather */
            sg_llp_config.gather_config.src_sgc         = img_fill_cfg->src_img_x_delta ;
            sg_llp_config.gather_config.src_sgi         = (img_fill_cfg->src_img_w - img_fill_cfg->src_img_x_delta) ;
            sg_llp_config.gather_config.src_gather_en   = (sg_llp_config.gather_config.src_sgi == 0) ? DMA_SRC_GATHER_DISABLE : DMA_SRC_GATHER_ENABLE;

            /* dest - scatter */
            sg_llp_config.scatter_config.dst_dsc        = img_fill_cfg->src_img_x_delta ;
            sg_llp_config.scatter_config.dst_dsi        = (img_fill_cfg->dst_buff_width - img_fill_cfg->src_img_x_delta) ;
            sg_llp_config.scatter_config.dst_scatter_en = DMA_DST_SCATTER_ENABLE;
#if 0
            sg_llp_config.llp_config.head_lli           = NULL;
            sg_llp_config.llp_config.llp_src_en         = DMA_LLP_SRC_DISABLE;
            sg_llp_config.llp_config.llp_dst_en         = DMA_LLP_DST_DISABLE;
            sg_llp_config.llp_config.llp_src_writeback  = 0;
            sg_llp_config.llp_config.llp_dst_writeback  = 0;
#endif
        }
        
        // MUST Reconfigure the xfer width, because DMA LLP change this
        _reconfig_dma(DMA_SRC_INCREMENT, DMA_DST_INCREMENT, DMA_SDATAALIGN_HALFWORD, DMA_DDATAALIGN_HALFWORD, sg_llp_config.gather_config.src_gather_en | sg_llp_config.scatter_config.dst_scatter_en);
        s_dma_xfer_flag  = 0x00;
        s_dma_over_flag  = _DMA_FLAG_XFER_DONE ;
        status = app_dma_start_sg_llp(s_dma_id, src_start_address, dst_start_address, total_pixels, &sg_llp_config);
        if(status == 0) {
#if DMA_SYNC_MODE == 0
            while(s_dma_xfer_flag != s_dma_over_flag) {}
#else
            sys_sem_take(g_semphr.dma_mem_xfer_sem, portMAX_DELAY);
#endif
        } else {
            printf("dma xfer err!\r\n");
            return false;
        }

        return true;
    } else {
        //by DMA LLP way
        uint32_t p_src_addr        = (uint32_t) src_start_address;
        uint32_t p_dst_addr        = (uint32_t) dst_start_address;
        dma_block_config_t * p_llp_block      = NULL;
        dma_block_config_t * p_llp_block_prev = NULL;
        uint32_t xfer_width = 0;
        uint32_t xfer_beat = 0;

        if(img_fill_cfg->src_img_x_delta % 2 == 0) {
            xfer_width = DMA_SDATAALIGN_WORD | DMA_DDATAALIGN_WORD;
            xfer_beat  = img_fill_cfg->src_img_x_delta >> 1;
        } else {
            xfer_width = DMA_SDATAALIGN_HALFWORD | DMA_DDATAALIGN_HALFWORD;
            xfer_beat  = img_fill_cfg->src_img_x_delta;
        }

        for(int i = 0; i < total_lines; i++) {
#if DMA_LLP_NODE_DYNAMIC_ALLOC > 0u
            p_llp_block = pvPortMalloc(sizeof(dma_block_config_t));
#else
            p_llp_block = &s_dma_llp_block[i];
#endif
            p_llp_block->src_address = p_src_addr;
            p_llp_block->dst_address = p_dst_addr;
            p_llp_block->src_status  = 0x00;
            p_llp_block->dst_status  = 0x00;

            /* memset in word mode */
            p_llp_block->CTL_L       = DMA_CTLL_INI_EN
                                        | DMA_SRC_INCREMENT
                                        | DMA_DST_INCREMENT
                                        | DMA_SRC_GATHER_DISABLE
                                        | DMA_DST_SCATTER_DISABLE
                                        | DMA_LLP_SRC_ENABLE
                                        | DMA_LLP_DST_ENABLE
                                        | DMA_MEMORY_TO_MEMORY
                                        | xfer_width;

            p_llp_block->CTL_H       = xfer_beat;
            p_llp_block->p_lli       = NULL;

            if(p_llp_block_prev == NULL) {
                sg_llp_config.llp_config.head_lli = p_llp_block;
            } else {
                p_llp_block_prev->p_lli = p_llp_block;
            }
            p_llp_block_prev = p_llp_block;

            p_src_addr += img_fill_cfg->src_img_w*img_fill_cfg->pixel_depth;
            p_dst_addr += img_fill_cfg->dst_buff_width*img_fill_cfg->pixel_depth;;
        }

        s_dma_xfer_flag  = 0x00;
        s_dma_over_flag  = _DMA_FLAG_XFER_DONE | _DMA_FLAG_BLK_DONE;
        sg_llp_config.llp_config.llp_src_en = DMA_LLP_SRC_ENABLE;
        sg_llp_config.llp_config.llp_dst_en = DMA_LLP_DST_ENABLE;
        status = app_dma_start_sg_llp(s_dma_id, p_src_addr, p_dst_addr, xfer_beat, &sg_llp_config);
        if(status == 0) {
#if DMA_SYNC_MODE == 0
            while(s_dma_xfer_flag != s_dma_over_flag) {}
#else
            sys_sem_take(g_semphr.dma_mem_xfer_sem, portMAX_DELAY);
#endif
            ret = true;
        } else {
            printf("dma xfer err!\r\n");
            ret = false;
        }

#if DMA_LLP_NODE_DYNAMIC_ALLOC > 0u
        // Free LLP nodes
        if (sg_llp_config.llp_config.head_lli)
        {
            dma_block_config_t * p = sg_llp_config.llp_config.head_lli;
            dma_block_config_t * q = NULL;
            while (p)
            {
                q = p->p_lli;
                vPortFree(p);
                p = q;
            }
        }
#else
        memset(&s_dma_llp_block[0], 0, (DISP_VER_RES + 1)*sizeof(dma_block_config_t));
#endif
        return ret;
    }
}

__attribute__((section("RAM_CODE")))  void * lv_dma_memcpy(void * dst, void * src, uint32_t byte_len) {
    uint32_t beat_len = (byte_len >> 1) ;
    uint32_t src_addr = (uint32_t) src;
    uint32_t dst_addr = (uint32_t) dst;
    uint16_t status   = HAL_OK;
    uint16_t xfer     = 0;

    do {
        xfer = beat_len > LV_DMA_MAX_XFER_SIZE_ONCE ? LV_DMA_MAX_XFER_SIZE_ONCE : beat_len;
        s_dma_xfer_flag  = 0x00;
        s_dma_over_flag  = _DMA_FLAG_XFER_DONE ;
        status = app_dma_start(s_dma_id, src_addr, dst_addr, xfer);

        if(0 == status) {
            while(s_dma_xfer_flag != s_dma_over_flag) {}
        } else {
            printf("dma err\r\n");
        }
        beat_len -= xfer;
        src_addr += (xfer << 1);
        dst_addr += (xfer << 1);
    } while (beat_len > 0);

    return dst;
}

