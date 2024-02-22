/**
 * @file lv_draw_label.h
 *
 */

#ifndef LV_DRAW_LABEL_H
#define LV_DRAW_LABEL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../misc/lv_bidi.h"
#include "../misc/lv_txt.h"
#include "../misc/lv_color.h"
#include "../misc/lv_style.h"

/*********************
 *      DEFINES
 *********************/
#define LV_DRAW_LABEL_NO_TXT_SEL (0xFFFF)

/**********************
 *      TYPEDEFS
 **********************/
#define LV_GDX_PATCH_LABEL_LINE_CACHE_MAX_NUM (5)

#if LV_GDX_PATCH_CACHE_LABEL_LINE_INFO > 0u
typedef struct
{
    bool is_need_update;
    int32_t line_index;
    uint32_t line_end[LV_GDX_PATCH_LABEL_LINE_CACHE_MAX_NUM];
    int32_t line_width[LV_GDX_PATCH_LABEL_LINE_CACHE_MAX_NUM];
} lv_draw_label_fast_line_cache_t;
#endif

typedef struct {
    const lv_font_t * font;
#if LV_GCX_PATCH_DISABLE_LABEL_SEL_FUNC
    uint32_t sel_start;
    uint32_t sel_end;
#endif // LV_GCX_PATCH_DISABLE_LABEL_SEL_FUNC
    lv_color_t color;
#if LV_GCX_PATCH_DISABLE_LABEL_SEL_FUNC
    lv_color_t sel_color;
    lv_color_t sel_bg_color;
#endif // LV_GCX_PATCH_DISABLE_LABEL_SEL_FUNC
    lv_coord_t line_space;
    lv_coord_t letter_space;
    lv_coord_t ofs_x;
    lv_coord_t ofs_y;
    lv_opa_t opa;
    lv_base_dir_t bidi_dir;
    lv_text_align_t align;
    lv_text_flag_t flag;
    lv_text_decor_t decor : 3;
    lv_blend_mode_t blend_mode: 3;
#if LV_GDX_PATCH_DRAW_FONT_WITH_PALETTE
    uint8_t bg_opaque: 1;
    lv_color_t background_color;
    uint16_t color_palette[17];
#endif
#if LV_GDX_PATCH_CACHE_LABEL_LINE_INFO > 0u
    lv_draw_label_fast_line_cache_t *fast_line_cache;
#endif
#if LV_GDX_PATCH_GLYPH_IN_LABEL_DSC
    lv_font_glyph_dsc_t * p_glyph_dsc_cache; // invavailable if NULL.
    const uint8_t * p_glyph_bitmap;          // invavailable if NULL.
#endif
} lv_draw_label_dsc_t;

/** Store some info to speed up drawing of very large texts
 * It takes a lot of time to get the first visible character because
 * all the previous characters needs to be checked to calculate the positions.
 * This structure stores an earlier (e.g. at -1000 px) coordinate and the index of that line.
 * Therefore the calculations can start from here.*/
typedef struct _lv_draw_label_hint_t {
    /** Index of the line at `y` coordinate*/
    int32_t line_start;

    /** Give the `y` coordinate of the first letter at `line start` index. Relative to the label's coordinates*/
    int32_t y;

    /** The 'y1' coordinate of the label when the hint was saved.
     * Used to invalidate the hint if the label has moved too much.*/
    int32_t coord_y;
} lv_draw_label_hint_t;

struct _lv_draw_ctx_t;
/**********************
 * GLOBAL PROTOTYPES
 **********************/
#if LV_GDX_PATCH_CACHE_LABEL_LINE_INFO && LV_GDX_PATCH_GLYPH_IN_LABEL_DSC && LV_GDX_PATCH_DRAW_FONT_WITH_PALETTE
LV_ATTRIBUTE_FAST_MEM void lv_draw_label_dsc_init_opt(lv_draw_label_dsc_t * dsc);
LV_ATTRIBUTE_FAST_MEM void lv_draw_label_opt(struct _lv_draw_ctx_t * draw_ctx, lv_draw_label_dsc_t * dsc, const lv_area_t * coords, const char * txt, lv_draw_label_hint_t * hint);
void lv_draw_letter_opt(struct _lv_draw_ctx_t * draw_ctx, const lv_draw_label_dsc_t * dsc,  const lv_point_t * pos_p, uint32_t letter);

#define lv_draw_label_dsc_init(dsc) lv_draw_label_dsc_init_opt(dsc)
#define lv_draw_label(draw_ctx, dsc, coords, txt, hint) lv_draw_label_opt(draw_ctx, dsc, coords, txt, hint)
#define lv_draw_letter(draw_ctx, dsc, pos_p, letter) lv_draw_letter_opt(draw_ctx, dsc, pos_p, letter)

#else // !(LV_GDX_PATCH_CACHE_LABEL_LINE_INFO && LV_GDX_PATCH_GLYPH_IN_LABEL_DSC)

LV_ATTRIBUTE_FAST_MEM void lv_draw_label_dsc_init(lv_draw_label_dsc_t * dsc);

/**
 * Write a text
 * @param coords coordinates of the label
 * @param mask the label will be drawn only in this area
 * @param dsc pointer to draw descriptor
 * @param txt `\0` terminated text to write
 * @param hint pointer to a `lv_draw_label_hint_t` variable.
 * It is managed by the draw to speed up the drawing of very long texts (thousands of lines).
 */
    #if LV_GDX_PATCH_DRAW_FONT_WITH_PALETTE
LV_ATTRIBUTE_FAST_MEM void lv_draw_label(struct _lv_draw_ctx_t * draw_ctx, const lv_draw_label_dsc_t * dsc,
                                         const lv_area_t * coords, const char * txt, lv_draw_label_hint_t * hint);
    #else
LV_ATTRIBUTE_FAST_MEM void lv_draw_label(struct _lv_draw_ctx_t * draw_ctx, const lv_draw_label_dsc_t * dsc,
                                         const lv_area_t * coords, const char * txt, lv_draw_label_hint_t * hint);
    #endif
void lv_draw_letter(struct _lv_draw_ctx_t * draw_ctx, const lv_draw_label_dsc_t * dsc,  const lv_point_t * pos_p,
                    uint32_t letter);
#endif

/***********************
 * GLOBAL VARIABLES
 ***********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_LABEL_H*/
