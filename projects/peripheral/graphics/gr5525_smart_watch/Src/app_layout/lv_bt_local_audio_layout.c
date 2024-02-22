#include "lvgl.h"
#include "app_bt.h"
#include "lv_img_dsc_list.h"

static bool s_local_audio_playing = false;

static const lv_style_const_prop_t TITLE_STYLE_PROPS[] = {
    LV_STYLE_CONST_HEIGHT(40),
    LV_STYLE_CONST_WIDTH(100),
    LV_STYLE_CONST_TEXT_COLOR(LV_COLOR_MAKE(0xFF, 0xFF, 0xFF)),
    LV_STYLE_CONST_TEXT_FONT(&lv_font_montserrat_20),
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(0, 0, 0)),
    LV_STYLE_CONST_TEXT_ALIGN(LV_TEXT_ALIGN_CENTER),
    LV_STYLE_PROP_INV,
};

LV_STYLE_CONST_INIT(TITLE_STYLE, TITLE_STYLE_PROPS);

static void on_drain_voice_state_changed(bt_api_msg_t *msg)
{
    if (msg->data.indication->local_voice_state == BT_API_LOCAL_VOICE_STATE_STOPPED)
    {
        if (s_local_audio_playing)
        {
            bt_api_drain_vibra_start();
        }
    }
}

static void on_findme_voice_state_changed(bt_api_msg_t *msg)
{
    if (msg->data.indication->local_voice_state == BT_API_LOCAL_VOICE_STATE_STOPPED)
    {
        if (s_local_audio_playing)
        {
            bt_api_search_dev_start();
        }
    }
}

static void on_drain_button_clicked(lv_event_t *event)
{
    lv_obj_t *drain = event->current_target;
    lv_obj_t *findme = (lv_obj_t *)event->user_data;

    // Judge by image source, dirty but simple
    if (lv_img_get_src(drain) == &wd_img_music_pause)
    {
        // Do Pause
        lv_img_set_src(drain, &wd_img_music_play);
        lv_obj_clear_state(findme, LV_STATE_DISABLED);
        s_local_audio_playing = false;
        bt_api_drain_vibra_stop();
        bt_api_deregister_indication_callback(on_drain_voice_state_changed);
    }
    else
    {
        // Do Play
        lv_img_set_src(drain, &wd_img_music_pause);
        lv_obj_add_state(findme, LV_STATE_DISABLED);
        s_local_audio_playing = true;
        bt_api_drain_vibra_start();
        bt_api_register_indication_callback(on_drain_voice_state_changed, IND_LOCAL_VOICE_STATE);
    }
}

static void on_findme_button_clicked(lv_event_t *event)
{
    lv_obj_t *findme = event->current_target;
    lv_obj_t *drain = (lv_obj_t *)event->user_data;

    // Judge by image source, dirty but simple
    if (lv_img_get_src(findme) == &wd_img_music_pause)
    {
        // Do Pause
        lv_img_set_src(findme, &wd_img_music_play);
        lv_obj_clear_state(drain, LV_STATE_DISABLED);
        s_local_audio_playing = false;
        bt_api_search_dev_stop();
        bt_api_deregister_indication_callback(on_findme_voice_state_changed);
    }
    else
    {
        // Do Play
        lv_img_set_src(findme, &wd_img_music_pause);
        lv_obj_add_state(drain, LV_STATE_DISABLED);
        s_local_audio_playing = true;
        bt_api_search_dev_start();
        bt_api_register_indication_callback(on_findme_voice_state_changed, IND_LOCAL_VOICE_STATE);
    }
}

lv_obj_t *lv_bt_local_audio_layout_create(lv_obj_t *parent)
{
    lv_obj_t *p_window = lv_obj_create(parent);
    lv_obj_set_size(p_window, DISP_HOR_RES, DISP_VER_RES);

    // Drain
    lv_obj_t *drain_button = lv_img_create(p_window);
    lv_obj_add_flag(drain_button, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(drain_button, &wd_img_music_play);
    lv_obj_set_pos(drain_button, 80, 165);

    lv_obj_t *draw_label = lv_label_create(p_window);
    lv_obj_add_style(draw_label, (lv_style_t *)&TITLE_STYLE, 0);
    lv_label_set_text_static(draw_label, "Drain");
    lv_obj_set_pos(draw_label, 70, 125);

    // Find Me
    lv_obj_t *findme_button = lv_img_create(p_window);
    lv_obj_add_flag(findme_button, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(findme_button, &wd_img_music_play);
    lv_obj_set_pos(findme_button, 200, 165);

    lv_obj_t *findme_label = lv_label_create(p_window);
    lv_obj_add_style(findme_label, (lv_style_t *)&TITLE_STYLE, 0);
    lv_label_set_text_static(findme_label, "Find Me");
    lv_obj_set_pos(findme_label, 190, 125);

    lv_obj_add_event_cb(drain_button, on_drain_button_clicked, LV_EVENT_CLICKED, (void *)findme_button);
    lv_obj_add_event_cb(findme_button, on_findme_button_clicked, LV_EVENT_CLICKED, (void *)drain_button);

    return p_window;
}

bool lv_bt_local_audio_is_playing(void)
{
    return s_local_audio_playing;
}
