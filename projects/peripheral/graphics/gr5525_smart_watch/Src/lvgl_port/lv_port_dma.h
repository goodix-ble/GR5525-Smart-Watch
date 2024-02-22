#ifndef __LV_PORT_DMA_H__
#define __LV_PORT_DMA_H__

#include "stdint.h"
#include "app_dma.h"
#include "lv_color.h"

#define LV_DMA_MAX_XFER_SIZE_ONCE           (4092u)

#define COLOR_ALMOST_BLACK(r, g, b)         (((r <= 5) && ( g<= 5) && (b <= 5)) ? 1 : 0)

typedef struct {
    uint32_t src_img_address;           /**< source address, just offset in storage device */
    uint32_t src_img_w;                 /**< pixel width of source image */
    uint32_t src_img_h;                 /**< pixel height of source image */
    uint32_t src_img_x;                 /**< x-coordinate of source image, left-top point is Coordinate origin */
    uint32_t src_img_x_delta;           /**< blit image width in pixel, do not multiple pexel depth */
    uint32_t src_img_y;                 /**< y-coordinate of source image, left-top point is Coordinate origin */
    uint32_t src_img_y_delta;           /**< blit image height in pixel, do not multiple pexel depth */
    uint32_t dst_buff_address;          /**< destination buffer address */
    uint32_t dst_buff_width;            /**< pixel width of destination buffer */
    uint32_t dst_buff_height;           /**< pixel height of destination buffer */
    uint32_t dst_buff_x;                /**< x-coordinate of destination buffer, left-top point is Coordinate origin */
    uint32_t dst_buff_y;                /**< y-coordinate of destination buffer, left-top point is Coordinate origin */
    uint32_t pixel_depth;               /**< pixel depth in byte, just support RGB565 now */
} _dma_fill_image_config_t;


void   lv_dma_init(void);
void * lv_dma_memcpy(void * dst, void * src, uint32_t byte_len);
void   lv_dma_fill_color(uint32_t dst_addr, uint32_t row_pixel, uint32_t stride_pixel, uint32_t col_pixel, uint32_t color);
void   lv_dma_memset(uint32_t dst_addr, uint32_t row_pixel, uint32_t stride_pixel, uint32_t col_pixel);
bool   lv_dma_fill_rect_rgb565_image(_dma_fill_image_config_t * img_fill_cfg);

#endif /*__LV_PORT_DMA_H__*/
