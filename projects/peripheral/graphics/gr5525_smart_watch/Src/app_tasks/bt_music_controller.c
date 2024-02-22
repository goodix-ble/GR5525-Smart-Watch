#include "bt_music_controller.h"
#include "FreeRTOS.h"
#include "lvgl.h"
#include "app_log.h"

static bt_music_ctx_t s_bt_music_ctx = {
    .state = AUDIO_STATE_PAUSED,
    .volume = 0,
    .name = NULL,
    .artist = NULL,
    .lyric = NULL,
    .changed.value = 0,
};

static bt_music_ctx_listener_t s_ctx_listener = NULL;
static void *s_listener_user_data = NULL;

static char *_find_and_copy_line(char* str, char **pos);
static void listener_async_call_wrapper(void *user_data);

void bt_music_controller_ind_handler(const bt_api_msg_t *msg)
{
    // s_bt_music_ctx.changed.value = 0;
    if (msg->opcode == IND_AUDIO_STATE)
    {
        // Update Playing State
        s_bt_music_ctx.state = msg->data.indication->audio_state;
        APP_LOG_DEBUG("Music State: %s\n", s_bt_music_ctx.state == AUDIO_STATE_PLAYING ? "Playing" : "Paused");
        s_bt_music_ctx.changed.filed.state = 1;
    }
    else if (msg->opcode == IND_AUDIO_SONG_INFO)
    {
        // Update Song Info
        // Name
        char *str_end;
        char *new_name = _find_and_copy_line((char *)&msg->data.indication->audio_song_info, &str_end);

        if (s_bt_music_ctx.name)
        {
            if (0 != strcmp(s_bt_music_ctx.name, new_name))
            {
                // Remove old lyric info if song is changed
                if (s_bt_music_ctx.lyric)
                {
                    vPortFree(s_bt_music_ctx.lyric);
                    s_bt_music_ctx.lyric = NULL;
                    s_bt_music_ctx.changed.filed.lyric = 1;
                }
            }
            vPortFree(s_bt_music_ctx.name);
        }
        s_bt_music_ctx.name = new_name;

        // Artist
        if (s_bt_music_ctx.artist)
        {
            vPortFree(s_bt_music_ctx.artist);
        }
        str_end += 2;
        s_bt_music_ctx.artist = _find_and_copy_line(str_end, NULL);
        s_bt_music_ctx.changed.filed.name = 1;
        s_bt_music_ctx.changed.filed.artist = 1;
        APP_LOG_DEBUG("Song Info Updated\nName:   \"%s\"\nArtist: \"%s\"\n", s_bt_music_ctx.name, s_bt_music_ctx.artist);

    }
    else if (msg->opcode == IND_AUDIO_SONG_WORD_INFO)
    {
        // Update Lyric
        if (s_bt_music_ctx.lyric)
        {
            vPortFree(s_bt_music_ctx.lyric);
        }
        s_bt_music_ctx.lyric = pvPortMalloc(msg->length + 1);
        memcpy(s_bt_music_ctx.lyric, &msg->data.indication->audio_song_word_info, msg->length);
        s_bt_music_ctx.lyric[msg->length] = 0;
        s_bt_music_ctx.changed.filed.lyric = 1;
        APP_LOG_DEBUG("Lyric Updated: \"%s\"\n", s_bt_music_ctx.lyric);
    }
    else if (msg->opcode == IND_AUDIO_VOL_INFO)
    {
        // Update Volume
        s_bt_music_ctx.volume = msg->data.indication->audio_vol_info;
        s_bt_music_ctx.changed.filed.volume = 1;
        APP_LOG_DEBUG("Audio Volume Update: %d\n", s_bt_music_ctx.volume);
    }
    else if (msg->opcode == IND_PROFILE_STATE)
    {
        if (msg->data.indication->profile_state.a2dp == BT_API_CONN_STATE_CONNECTED)
        {
            bt_api_vol_level_get();
        }
    }

    if (s_ctx_listener && s_bt_music_ctx.changed.value > 0)
    {
        lv_async_call(listener_async_call_wrapper, NULL);
    }
}

bt_music_ctx_t *bt_music_controller_get_ctx(void)
{
    return &s_bt_music_ctx;
}

void bt_music_controller_set_ctx_listener(bt_music_ctx_listener_t listener, void *user_data)
{
    s_ctx_listener = listener;
    s_listener_user_data = user_data;
}

static char *_find_and_copy_line(char* str, char **pos)
{
    char *s = str;
    while(*s != '\r')
    {
        s++;
    }
    if (s == str)
    {
        return NULL;
    }
    char *ret = (char *)pvPortMalloc(s - str + 1);
    memcpy(ret, str, s - str);
    ret[s - str] = 0;
    if (pos)
    {
        *pos = s;
    }
    return ret;
}

static void listener_async_call_wrapper(void *user_data)
{
    LV_UNUSED(user_data);
    if (s_ctx_listener)
    {
        s_ctx_listener(&s_bt_music_ctx, s_listener_user_data);
    }
}
