/**
 * @file lv_draw_label.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw.h"
#include "lv_draw_label.h"
#include "../misc/lv_math.h"
#include "../hal/lv_hal_disp.h"
#include "../core/lv_refr.h"
#include "../misc/lv_bidi.h"
#include "../misc/lv_assert.h"

/*********************
 *      DEFINES
 *********************/
#define LABEL_RECOLOR_PAR_LENGTH 6
#define LV_LABEL_HINT_UPDATE_TH 1024 /*Update the "hint" if the label's y coordinates have changed more then this*/

/**********************
 *      TYPEDEFS
 **********************/
enum {
    CMD_STATE_WAIT,
    CMD_STATE_PAR,
    CMD_STATE_IN,
};
typedef uint8_t cmd_state_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static uint8_t hex_char_to_num(char hex);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *  GLOBAL VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_label_dsc_init_opt(lv_draw_label_dsc_t *dsc)
{
    lv_memset_00(dsc, sizeof(lv_draw_label_dsc_t));
    dsc->opa = LV_OPA_COVER;
    dsc->color = lv_color_black();
    dsc->font = LV_FONT_DEFAULT;
#if LV_GCX_PATCH_DISABLE_LABEL_SEL_FUNC
    dsc->sel_start = LV_DRAW_LABEL_NO_TXT_SEL;
    dsc->sel_end = LV_DRAW_LABEL_NO_TXT_SEL;
    dsc->sel_color = lv_color_black();
    dsc->sel_bg_color = lv_palette_main(LV_PALETTE_BLUE);
#endif
    dsc->bidi_dir = LV_BASE_DIR_LTR;
}

#if LV_GDX_PATCH_CACHE_LABEL_LINE_INFO > 0u
static void line_info_cache(const lv_draw_label_dsc_t *dsc, const lv_area_t *coords, int32_t w, const char *txt)
{
    lv_draw_label_fast_line_cache_t *p_fast_line_cache = dsc->fast_line_cache;
    const lv_font_t *font = dsc->font;

    int32_t line_end = 0;
    int32_t line_width = 0;
    int32_t line_start = 0;
    int32_t line_idx = 0;

    while (txt[line_start] != '\0')
    {
        line_start = line_end;
        line_end += _lv_txt_get_next_line(&txt[line_start], font, dsc->letter_space, w, NULL, dsc->flag);
        line_width = lv_txt_get_width(&txt[line_start], line_end - line_start, font, dsc->letter_space, dsc->flag);

        if (line_idx < LV_GDX_PATCH_LABEL_LINE_CACHE_MAX_NUM)
        {
            p_fast_line_cache->line_width[line_idx] = line_width;
            p_fast_line_cache->line_end[line_idx] = line_end;
            line_idx++;
        }
        else
        {
            p_fast_line_cache->is_need_update = false;
            break;
        }
    }
    p_fast_line_cache->is_need_update = false;
}
#endif

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
LV_ATTRIBUTE_FAST_MEM void lv_draw_label_opt(lv_draw_ctx_t *draw_ctx, lv_draw_label_dsc_t *dsc,
                                         const lv_area_t *coords, const char *txt, lv_draw_label_hint_t *hint)
#else
LV_ATTRIBUTE_FAST_MEM void lv_draw_label_opt(lv_draw_ctx_t *draw_ctx, const lv_draw_label_dsc_t *dsc,
                                         const lv_area_t *coords, const char *txt, lv_draw_label_hint_t *hint)
#endif
{
    if(dsc->opa <= LV_OPA_MIN) return;
    if(dsc->font == NULL) {
        LV_LOG_WARN("dsc->font == NULL");
        return;
    }

    if(draw_ctx->draw_letter == NULL) {
        LV_LOG_WARN("draw->draw_letter == NULL (there is no function to draw letters)");
        return;
    }

    lv_draw_label_dsc_t dsc_mod = *dsc;

    const lv_font_t * font = dsc->font;
    int32_t w;

    /*No need to waste processor time if string is empty*/
    if(txt == NULL || txt[0] == '\0')
        return;

    lv_area_t clipped_area;
    bool clip_ok = _lv_area_intersect(&clipped_area, coords, draw_ctx->clip_area);
    if(!clip_ok) return;

#if LV_GDX_PATCH_DRAW_FONT_WITH_PALETTE
    uint16_t *p_color_palette = dsc_mod.color_palette;
    uint16_t background_red = dsc->background_color.ch.red;
    uint16_t background_blue = dsc->background_color.ch.blue;
    uint16_t background_green = dsc->background_color.ch.green;
    uint16_t foreground_red = dsc_mod.color.ch.red;
    uint16_t foreground_blue = dsc_mod.color.ch.blue;
    uint16_t foreground_green = dsc_mod.color.ch.green;

    for (int i = 0; i < 16; i++)
    {
        lv_color16_t src_color;
        src_color.ch.red = (background_red * (15 - i) + foreground_red * i) / 15;
        src_color.ch.blue = (background_blue * (15 - i) + foreground_blue * i) / 15;
        src_color.ch.green = (background_green * (15 - i) + foreground_green * i) / 15;
        p_color_palette[i] = src_color.full;
    }
#endif

    lv_text_align_t align = dsc->align;
    lv_base_dir_t base_dir = dsc->bidi_dir;

    lv_bidi_calculate_align(&align, &base_dir, txt);

    if((dsc->flag & LV_TEXT_FLAG_EXPAND) == 0) {
        /*Normally use the label's width as width*/
        w = lv_area_get_width(coords);
    }
    else {
        /*If EXPAND is enabled then not limit the text's width to the object's width*/
        lv_point_t p;
        lv_txt_get_size(&p, txt, dsc->font, dsc->letter_space, dsc->line_space, LV_COORD_MAX,
                        dsc->flag);
        w = p.x;
    }

#if LV_GDX_PATCH_CACHE_LABEL_LINE_INFO > 0u
    lv_draw_label_fast_line_cache_t *p_fast_line_cache = dsc->fast_line_cache;
    if (p_fast_line_cache->is_need_update)
    {
        line_info_cache(dsc, coords, w, txt);
    }
    p_fast_line_cache->line_index = 0;
#endif

    int32_t line_height_font = lv_font_get_line_height(font);
    int32_t line_height = line_height_font + dsc->line_space;

    /*Init variables for the first line*/
    int32_t line_width = 0;
    lv_point_t pos;
    pos.x = coords->x1;
    pos.y = coords->y1;

    int32_t x_ofs = 0;
    int32_t y_ofs = 0;
    x_ofs = dsc->ofs_x;
    y_ofs = dsc->ofs_y;
    pos.y += y_ofs;

    uint32_t line_start     = 0;
    int32_t last_line_start = -1;

    /*Check the hint to use the cached info*/
    if(hint && y_ofs == 0 && coords->y1 < 0) {
        /*If the label changed too much recalculate the hint.*/
        if(LV_ABS(hint->coord_y - coords->y1) > LV_LABEL_HINT_UPDATE_TH - 2 * line_height) {
            hint->line_start = -1;
        }
        last_line_start = hint->line_start;
    }

    /*Use the hint if it's valid*/
    if(hint && last_line_start >= 0) {
        line_start = last_line_start;
        pos.y += hint->y;
    }

#if LV_GDX_PATCH_CACHE_LABEL_LINE_INFO > 0u
    uint32_t line_end = p_fast_line_cache->line_end[p_fast_line_cache->line_index];
    line_width = p_fast_line_cache->line_width[p_fast_line_cache->line_index];
    p_fast_line_cache->line_index++;
#else
    uint32_t line_end = line_start + _lv_txt_get_next_line(&txt[line_start], font, dsc->letter_space, w, NULL, dsc->flag);
#endif

    /*Go the first visible line*/
    while(pos.y + line_height_font < draw_ctx->clip_area->y1) {
        /*Go to next line*/
        line_start = line_end;

#if LV_GDX_PATCH_CACHE_LABEL_LINE_INFO > 0u
        if (p_fast_line_cache->line_index < LV_GDX_PATCH_LABEL_LINE_CACHE_MAX_NUM)
        {
            line_end = p_fast_line_cache->line_end[p_fast_line_cache->line_index];
            line_width = p_fast_line_cache->line_width[p_fast_line_cache->line_index];
            p_fast_line_cache->line_index++;
        }
        else
        {
            line_end += _lv_txt_get_next_line(&txt[line_start], font, dsc->letter_space, w, NULL, dsc->flag);
            line_width = lv_txt_get_width(&txt[line_start], line_end - line_start, font, dsc->letter_space, dsc->flag);
        }
#else
        line_end += _lv_txt_get_next_line(&txt[line_start], font, dsc->letter_space, w, NULL, dsc->flag);
#endif
        pos.y += line_height;

        /*Save at the threshold coordinate*/
        if(hint && pos.y >= -LV_LABEL_HINT_UPDATE_TH && hint->line_start < 0) {
            hint->line_start = line_start;
            hint->y          = pos.y - coords->y1;
            hint->coord_y    = coords->y1;
        }

        if(txt[line_start] == '\0') return;
    }

#if LV_GDX_PATCH_CACHE_LABEL_LINE_INFO > 0u
    /*Align to middle*/
    if (align == LV_TEXT_ALIGN_CENTER) {
        pos.x += (lv_area_get_width(coords) - line_width) / 2;
    }
    /*Align to the right*/
    else if (align == LV_TEXT_ALIGN_RIGHT)
    {
        pos.x += lv_area_get_width(coords) - line_width;
    }
#else
    /*Align to middle*/
    if (align == LV_TEXT_ALIGN_CENTER) {
        line_width = lv_txt_get_width(&txt[line_start], line_end - line_start, font, dsc->letter_space, dsc->flag);

        pos.x += (lv_area_get_width(coords) - line_width) / 2;

    }
    /*Align to the right*/
    else if(align == LV_TEXT_ALIGN_RIGHT) {
        line_width = lv_txt_get_width(&txt[line_start], line_end - line_start, font, dsc->letter_space, dsc->flag);
        pos.x += lv_area_get_width(coords) - line_width;
    }
#endif

#if LV_GCX_PATCH_DISABLE_LABEL_SEL_FUNC
    uint32_t sel_start = dsc->sel_start;
    uint32_t sel_end = dsc->sel_end;
    if(sel_start > sel_end) {
        uint32_t tmp = sel_start;
        sel_start = sel_end;
        sel_end = tmp;
    }
#endif // LV_GCX_PATCH_DISABLE_LABEL_SEL_FUNC
    lv_draw_line_dsc_t line_dsc;

    if((dsc->decor & LV_TEXT_DECOR_UNDERLINE) || (dsc->decor & LV_TEXT_DECOR_STRIKETHROUGH)) {
        lv_draw_line_dsc_init(&line_dsc);
        line_dsc.color = dsc->color;
        line_dsc.width = font->underline_thickness ? font->underline_thickness : 1;
        line_dsc.opa = dsc->opa;
        line_dsc.blend_mode = dsc->blend_mode;
    }

    cmd_state_t cmd_state = CMD_STATE_WAIT;
    uint32_t i;
    uint32_t par_start = 0;
    lv_color_t recolor  = lv_color_black();
    lv_color_t color = lv_color_black();
    int32_t letter_w;
    #if LV_GDX_PATCH_GLYPH_IN_LABEL_DSC
    lv_font_glyph_dsc_t glyph_dsc_cache;
    dsc_mod.p_glyph_dsc_cache = &glyph_dsc_cache;
    #endif

#if LV_GCX_PATCH_DISABLE_LABEL_SEL_FUNC
    lv_draw_rect_dsc_t draw_dsc_sel;
    lv_draw_rect_dsc_init(&draw_dsc_sel);
    draw_dsc_sel.bg_color = dsc->sel_bg_color;
#endif // LV_GCX_PATCH_DISABLE_LABEL_SEL_FUNC

    int32_t pos_x_start = pos.x;
    /*Write out all lines*/
    while(txt[line_start] != '\0') {
        pos.x += x_ofs;

        /*Write all letter of a line*/
        cmd_state = CMD_STATE_WAIT;
        i         = 0;
#if LV_USE_BIDI
        char * bidi_txt = lv_mem_buf_get(line_end - line_start + 1);
        _lv_bidi_process_paragraph(txt + line_start, bidi_txt, line_end - line_start, base_dir, NULL, 0);
#else
        const char * bidi_txt = txt + line_start;
#endif

        while(i < line_end - line_start) {
#if LV_GCX_PATCH_DISABLE_LABEL_SEL_FUNC
            uint32_t logical_char_pos = 0;
            if(sel_start != 0xFFFF && sel_end != 0xFFFF) {
#if LV_USE_BIDI
                logical_char_pos = _lv_txt_encoded_get_char_id(txt, line_start);
                uint32_t t = _lv_txt_encoded_get_char_id(bidi_txt, i);
                logical_char_pos += _lv_bidi_get_logical_pos(bidi_txt, NULL, line_end - line_start, base_dir, t, NULL);
#else
                logical_char_pos = _lv_txt_encoded_get_char_id(txt, line_start + i);
#endif
            }
#endif

            uint32_t letter;
            uint32_t letter_next;
            _lv_txt_encoded_letter_next_2(bidi_txt, &letter, &letter_next, &i);
            /*Handle the re-color command*/
            if((dsc->flag & LV_TEXT_FLAG_RECOLOR) != 0) {
                if(letter == (uint32_t)LV_TXT_COLOR_CMD[0]) {
                    if(cmd_state == CMD_STATE_WAIT) { /*Start char*/
                        par_start = i;
                        cmd_state = CMD_STATE_PAR;
                        continue;
                    }
                    else if(cmd_state == CMD_STATE_PAR) {   /*Other start char in parameter escaped cmd. char*/
                        cmd_state = CMD_STATE_WAIT;
                    }
                    else if(cmd_state == CMD_STATE_IN) {   /*Command end*/
                        cmd_state = CMD_STATE_WAIT;
                        continue;
                    }
                }

                /*Skip the color parameter and wait the space after it*/
                if(cmd_state == CMD_STATE_PAR) {
                    if(letter == ' ') {
                        /*Get the parameter*/
                        if(i - par_start == LABEL_RECOLOR_PAR_LENGTH + 1) {
                            char buf[LABEL_RECOLOR_PAR_LENGTH + 1];
                            lv_memcpy_small(buf, &bidi_txt[par_start], LABEL_RECOLOR_PAR_LENGTH);
                            buf[LABEL_RECOLOR_PAR_LENGTH] = '\0';
                            int r, g, b;
                            r       = (hex_char_to_num(buf[0]) << 4) + hex_char_to_num(buf[1]);
                            g       = (hex_char_to_num(buf[2]) << 4) + hex_char_to_num(buf[3]);
                            b       = (hex_char_to_num(buf[4]) << 4) + hex_char_to_num(buf[5]);
                            recolor = lv_color_make(r, g, b);
                        }
                        else {
                            recolor.full = dsc->color.full;
                        }
                        cmd_state = CMD_STATE_IN; /*After the parameter the text is in the command*/
                    }
                    continue;
                }
            }

            color = dsc->color;

            if(cmd_state == CMD_STATE_IN) color = recolor;

            #if LV_GDX_PATCH_GLYPH_IN_LABEL_DSC
            dsc_mod.p_glyph_bitmap = lv_font_get_glyph_bitmap_and_dsc(font, &glyph_dsc_cache, letter, letter_next);
            // lv_font_get_glyph_dsc(font, &glyph_dsc_cache, letter, letter_next);
            letter_w = glyph_dsc_cache.adv_w;
            // dsc_mod.p_glyph_dsc_cache = &glyph_dsc_cache; only asign once.
            #else //LV_GDX_PATCH_GLYPH_IN_LABEL_DSC
            letter_w = lv_font_get_glyph_width(font, letter, letter_next);
            #endif

#if LV_GCX_PATCH_DISABLE_LABEL_SEL_FUNC
            if(sel_start != 0xFFFF && sel_end != 0xFFFF) {
                if(logical_char_pos >= sel_start && logical_char_pos < sel_end) {
                    lv_area_t sel_coords;
                    sel_coords.x1 = pos.x;
                    sel_coords.y1 = pos.y;
                    sel_coords.x2 = pos.x + letter_w + dsc->letter_space - 1;
                    sel_coords.y2 = pos.y + line_height - 1;
                    lv_draw_rect(draw_ctx, &draw_dsc_sel, &sel_coords);
                    color = dsc->sel_color;
                }
            }
#endif // LV_GCX_PATCH_DISABLE_LABEL_SEL_FUNC

            dsc_mod.color = color;
            lv_draw_letter(draw_ctx, &dsc_mod, &pos, letter);

            if(letter_w > 0) {
                pos.x += letter_w + dsc->letter_space;
            }
        }

        if(dsc->decor & LV_TEXT_DECOR_STRIKETHROUGH) {
            lv_point_t p1;
            lv_point_t p2;
            p1.x = pos_x_start;
            p1.y = pos.y + (dsc->font->line_height / 2)  + line_dsc.width / 2;
            p2.x = pos.x;
            p2.y = p1.y;
            line_dsc.color = color;
            lv_draw_line(draw_ctx, &line_dsc, &p1, &p2);
        }

        if(dsc->decor  & LV_TEXT_DECOR_UNDERLINE) {
            lv_point_t p1;
            lv_point_t p2;
            p1.x = pos_x_start;
            p1.y = pos.y + dsc->font->line_height - dsc->font->base_line - font->underline_position;
            p2.x = pos.x;
            p2.y = p1.y;
            line_dsc.color = color;
            lv_draw_line(draw_ctx, &line_dsc, &p1, &p2);
        }

#if LV_USE_BIDI
        lv_mem_buf_release(bidi_txt);
        bidi_txt = NULL;
#endif
        /*Go to next line*/
#if LV_GDX_PATCH_CACHE_LABEL_LINE_INFO > 0u
        line_start = line_end;
        if (p_fast_line_cache->line_index < LV_GDX_PATCH_LABEL_LINE_CACHE_MAX_NUM)
        {
            line_end = p_fast_line_cache->line_end[p_fast_line_cache->line_index];
            line_width = p_fast_line_cache->line_width[p_fast_line_cache->line_index];
            p_fast_line_cache->line_index++;
        }
        else
        {
            line_end += _lv_txt_get_next_line(&txt[line_start], font, dsc->letter_space, w, NULL, dsc->flag);
            line_width = lv_txt_get_width(&txt[line_start], line_end - line_start, font, dsc->letter_space, dsc->flag);
        }

        pos.x = coords->x1;
        /*Align to middle*/

        if (align == LV_TEXT_ALIGN_CENTER)
        {
            pos.x += (lv_area_get_width(coords) - line_width) / 2;
        }
        /*Align to the right*/
        else if (align == LV_TEXT_ALIGN_RIGHT)
        {
            pos.x += lv_area_get_width(coords) - line_width;
        }
#else
        line_start = line_end;
        line_end += _lv_txt_get_next_line(&txt[line_start], font, dsc->letter_space, w, NULL, dsc->flag);

        pos.x = coords->x1;
        /*Align to middle*/
        if(align == LV_TEXT_ALIGN_CENTER) {
            line_width =
                lv_txt_get_width(&txt[line_start], line_end - line_start, font, dsc->letter_space, dsc->flag);

            pos.x += (lv_area_get_width(coords) - line_width) / 2;

        }
        /*Align to the right*/
        else if(align == LV_TEXT_ALIGN_RIGHT) {
            line_width =
                lv_txt_get_width(&txt[line_start], line_end - line_start, font, dsc->letter_space, dsc->flag);
            pos.x += lv_area_get_width(coords) - line_width;
        }
#endif

        /*Go the next line position*/
        pos.y += line_height;

        if(pos.y > draw_ctx->clip_area->y2) return;
    }

    LV_ASSERT_MEM_INTEGRITY();
}

void lv_draw_letter_opt(lv_draw_ctx_t * draw_ctx, const lv_draw_label_dsc_t * dsc,  const lv_point_t * pos_p,
                    uint32_t letter)
{
    draw_ctx->draw_letter(draw_ctx, dsc, pos_p, letter);
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Convert a hexadecimal characters to a number (0..15)
 * @param hex Pointer to a hexadecimal character (0..9, A..F)
 * @return the numerical value of `hex` or 0 on error
 */
static uint8_t hex_char_to_num(char hex)
{
    uint8_t result = 0;

    if(hex >= '0' && hex <= '9') {
        result = hex - '0';
    }
    else {
        if(hex >= 'a') hex -= 'a' - 'A'; /*Convert to upper case*/

        switch(hex) {
            case 'A':
                result = 10;
                break;
            case 'B':
                result = 11;
                break;
            case 'C':
                result = 12;
                break;
            case 'D':
                result = 13;
                break;
            case 'E':
                result = 14;
                break;
            case 'F':
                result = 15;
                break;
            default:
                result = 0;
                break;
        }
    }

    return result;
}

