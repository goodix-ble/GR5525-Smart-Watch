#include "lvgl.h"
#include "app_bt.h"
#include "lv_img_dsc_list.h"
#include "lv_volume_ctrl.h"
#include "bt_music_controller.h"

#include <stdio.h>

#if !LV_GDX_PATCH_CACHE_LABEL_LINE_INFO
#define update_line_info(...)
#endif // !LV_GDX_PATCH_CACHE_LABEL_LINE_INFO

#define MAX_VOLUME 0x10

#define LV_STATE_PAUSED LV_STATE_USER_1

static const lv_style_const_prop_t MUSIC_TITLE_STYLE_PROPS[] = {
    LV_STYLE_CONST_WIDTH(DISP_HOR_RES),
    LV_STYLE_CONST_X(0),
    LV_STYLE_CONST_Y(120),
    LV_STYLE_CONST_PAD_LEFT(15),
    LV_STYLE_CONST_PAD_RIGHT(15),
    LV_STYLE_CONST_TEXT_FONT(&lv_font_montserrat_28),
    LV_STYLE_CONST_TEXT_COLOR(LV_COLOR_MAKE(0xFF, 0xFF, 0xFF)),
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(0, 0, 0)),
    LV_STYLE_CONST_TEXT_ALIGN(LV_TEXT_ALIGN_CENTER),
    LV_STYLE_PROP_INV,
};

static const lv_style_const_prop_t MUSIC_ARTIST_STYLE_PROPS[] = {
    LV_STYLE_CONST_WIDTH(DISP_HOR_RES),
    LV_STYLE_CONST_X(0),
    LV_STYLE_CONST_Y(165),
    LV_STYLE_CONST_PAD_LEFT(15),
    LV_STYLE_CONST_PAD_RIGHT(15),
    LV_STYLE_CONST_TEXT_FONT(&lv_font_montserrat_20),
    LV_STYLE_CONST_TEXT_COLOR(LV_COLOR_MAKE(0xFF, 0xFF, 0xFF)),
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(0, 0, 0)),
    LV_STYLE_CONST_TEXT_ALIGN(LV_TEXT_ALIGN_CENTER),
    LV_STYLE_PROP_INV,
};

static const lv_style_const_prop_t MUSIC_BUTTON_PLAY_STYLE_PROPS[] = {
    LV_STYLE_CONST_WIDTH(80),
    LV_STYLE_CONST_HEIGHT(80),
    LV_STYLE_CONST_X((DISP_HOR_RES - 80)/2),
    LV_STYLE_CONST_Y(200),
    LV_STYLE_CONST_BG_IMG_SRC(&wd_img_music_pause),
    LV_STYLE_PROP_INV,
};

static const lv_style_const_prop_t MUSIC_BUTTON_PLAY_PAUSED_STYLE_PROPS[] = {
    LV_STYLE_CONST_BG_IMG_SRC(&wd_img_music_play),
    LV_STYLE_PROP_INV,
};

static const lv_style_const_prop_t MUSIC_LYRIC_PROPS[] = {
    LV_STYLE_CONST_WIDTH(260),
    LV_STYLE_CONST_X(50),
    LV_STYLE_CONST_Y(300),
    LV_STYLE_CONST_TEXT_FONT(&lv_font_montserrat_20),
    LV_STYLE_CONST_TEXT_COLOR(LV_COLOR_MAKE(0xFF, 0xFF, 0xFF)),
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(0, 0, 0)),
    LV_STYLE_CONST_TEXT_ALIGN(LV_TEXT_ALIGN_CENTER),
    LV_STYLE_CONST_PAD_LEFT(15),
    LV_STYLE_CONST_PAD_RIGHT(15),
    LV_STYLE_PROP_INV,
};

LV_STYLE_CONST_INIT(MUSIC_TITLE_STYLE, MUSIC_TITLE_STYLE_PROPS);
LV_STYLE_CONST_INIT(MUSIC_ARTIST_STYLE, MUSIC_ARTIST_STYLE_PROPS);
LV_STYLE_CONST_INIT(MUSIC_BUTTON_PLAY_STYLE, MUSIC_BUTTON_PLAY_STYLE_PROPS);
LV_STYLE_CONST_INIT(MUSIC_BUTTON_PLAY_PAUSED_STYLE, MUSIC_BUTTON_PLAY_PAUSED_STYLE_PROPS);
LV_STYLE_CONST_INIT(MUSIC_LYRIC, MUSIC_LYRIC_PROPS);

static void on_play_button_clicked(lv_event_t *event);
static void on_side_button_clicked(lv_event_t *event);
static void on_volume_changed(lv_event_t *event);
static void on_window_lifecycle_event(lv_event_t *event);
static void bt_music_ctx_listener(bt_music_ctx_t *ctx, void *user_data);

lv_obj_t *lv_bt_music_layout_create(lv_obj_t *parent)
{
    lv_obj_t *p_window = lv_obj_create(parent);
    lv_obj_remove_style_all(p_window);
    lv_obj_set_size(p_window, DISP_HOR_RES, DISP_VER_RES);
    lv_obj_add_event_cb(p_window, on_window_lifecycle_event, LV_EVENT_ALL, NULL);

    bt_music_ctx_t *music_ctx = bt_music_controller_get_ctx();

    // Volume Control
    lv_obj_t *p_volume = lv_volume_ctrl_create(p_window);
    lv_obj_align(p_volume, LV_ALIGN_TOP_MID, 0, 55);
    lv_obj_add_event_cb(p_volume, on_volume_changed, LV_EVENT_VALUE_CHANGED, NULL);

    // Music Title
    lv_obj_t *p_title = lv_label_create(p_window);
    lv_obj_add_style(p_title, (lv_style_t *)&MUSIC_TITLE_STYLE, 0);
    lv_label_set_long_mode(p_title, LV_LABEL_LONG_SCROLL);

    // Artist
    lv_obj_t *p_artist = lv_label_create(p_window);
    lv_obj_add_style(p_artist, (lv_style_t *)&MUSIC_ARTIST_STYLE, 0);
    lv_label_set_long_mode(p_artist, LV_LABEL_LONG_SCROLL);

    // Pause
    lv_obj_t *p_play = lv_obj_create(p_window);
    lv_obj_add_style(p_play, (lv_style_t *)&MUSIC_BUTTON_PLAY_STYLE, 0);
    lv_obj_add_style(p_play, (lv_style_t *)&MUSIC_BUTTON_PLAY_PAUSED_STYLE, LV_STATE_PAUSED);
    lv_obj_add_event_cb(p_play, on_play_button_clicked, LV_EVENT_SHORT_CLICKED, NULL);

    // Prev
    lv_obj_t *p_prev = lv_img_create(p_window);
    lv_obj_add_flag(p_prev, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(p_prev, &wd_img_music_backward);
    lv_obj_align(p_prev, LV_ALIGN_TOP_MID, -100, 220);
    lv_obj_add_event_cb(p_prev, on_side_button_clicked, LV_EVENT_SHORT_CLICKED, (void *)((intptr_t)-1));
    lv_obj_add_event_cb(p_prev, on_side_button_clicked, LV_EVENT_LONG_PRESSED_REPEAT, (void *)((intptr_t)-1));

    // Next
    lv_obj_t *p_next = lv_img_create(p_window);
    lv_obj_add_flag(p_next, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(p_next, &wd_img_music_forward);
    lv_obj_align(p_next, LV_ALIGN_TOP_MID, 100, 220);
    lv_obj_add_event_cb(p_next, on_side_button_clicked, LV_EVENT_SHORT_CLICKED, (void *)((intptr_t)1));
    lv_obj_add_event_cb(p_next, on_side_button_clicked, LV_EVENT_LONG_PRESSED_REPEAT, (void *)((intptr_t)1));

    // Lyric
    lv_obj_t *p_lyric = lv_label_create(p_window);
    lv_obj_add_style(p_lyric, (lv_style_t *)&MUSIC_LYRIC, 0);
    lv_label_set_long_mode(p_lyric, LV_LABEL_LONG_SCROLL);

    music_ctx->changed.value = 0xFF;
    bt_music_ctx_listener(music_ctx, (void *)p_window);

    return p_window;
}

static void on_play_button_clicked(lv_event_t *event)
{
    bt_api_avrcp_status_set();
}

static void on_side_button_clicked(lv_event_t *event)
{
    int8_t dir = (int8_t)((intptr_t)(event->user_data));
    if (event->code == LV_EVENT_SHORT_CLICKED)
    {
        if (dir > 0)
        {
            // Next
            bt_api_avrcp_play_next();
        }
        else
        {
            // Prev
            bt_api_avrcp_play_prev();
        }
    }
    else if (event->code == LV_EVENT_LONG_PRESSED_REPEAT)
    {
        if (dir > 0)
        {
            // Next
            bt_api_avrcp_play_forward();
        }
        else
        {
            // Prev
            bt_api_avrcp_play_rewind();
        }
    }
}

static void on_volume_changed(lv_event_t *event)
{
    int8_t dir = *((int8_t *)event->param);
    if (dir > 0)
    {
        bt_api_volp_action();
    }
    else
    {
        bt_api_volm_action();
    }
}

static void on_window_lifecycle_event(lv_event_t *event)
{
    lv_event_code_t code = event->code;
    if (code == LV_EVENT_READY)
    {
        bt_api_vol_level_get();
        bt_music_controller_set_ctx_listener(bt_music_ctx_listener, event->current_target);
    }
    else if (code == LV_EVENT_DELETE)
    {
        bt_music_controller_set_ctx_listener(NULL, NULL);
    }
}

static void bt_music_ctx_listener(bt_music_ctx_t *ctx, void *user_data)
{
    lv_obj_t *p_window = (lv_obj_t *)user_data;
    lv_obj_t *p_volume = lv_obj_get_child(p_window, 0);
    lv_obj_t *p_title = lv_obj_get_child(p_window, 1);
    lv_obj_t *p_artist = lv_obj_get_child(p_window, 2);
    lv_obj_t *p_play = lv_obj_get_child(p_window, 3);
    lv_obj_t *p_lyric = lv_obj_get_child(p_window, 6);

    if (ctx->changed.filed.volume)
    {
        uint8_t new_vol = 100 * (ctx->volume / (MAX_VOLUME * 1.f));
        lv_volume_ctrl_set_value(p_volume, new_vol);
    }

    if (ctx->changed.filed.name)
    {
        if (ctx->name)
        {
            lv_label_set_text(p_title, ctx->name);
        }
        else
        {
            lv_label_set_text_static(p_title, "No Music");
        }
        update_line_info(p_title);
        ctx->changed.filed.name = 0;
    }

    if (ctx->changed.filed.artist)
    {
        if (ctx->artist)
        {
            lv_label_set_text(p_artist, ctx->artist);
        }
        else
        {
            lv_label_set_text_static(p_artist, "-");
        }
        update_line_info(p_artist);
        ctx->changed.filed.artist = 0;
    }

    if (ctx->changed.filed.state)
    {
        if (ctx->state == AUDIO_STATE_PAUSED)
        {
            lv_obj_add_state(p_play, LV_STATE_PAUSED);
        }
        else
        {
            lv_obj_clear_state(p_play, LV_STATE_PAUSED);
        }
        ctx->changed.filed.state = 0;
    }

    if (ctx->changed.filed.lyric)
    {
        if (ctx->lyric)
        {
            lv_label_set_text(p_lyric, ctx->lyric);
        }
        else
        {
            lv_label_set_text_static(p_lyric, "No Lyric");
        }
        update_line_info(p_lyric);
        ctx->changed.filed.lyric = 0;
    }
}
