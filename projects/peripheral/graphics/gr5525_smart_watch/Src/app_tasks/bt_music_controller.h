#ifndef __BT_MUSIC_CONTROLLER_H__
#define __BT_MUSIC_CONTROLLER_H__

#include "app_bt.h"

typedef struct
{
    uint8_t state;
    uint8_t volume;
    union
    {
        uint8_t value;
        struct
        {
            uint8_t name : 1;
            uint8_t artist : 1;
            uint8_t lyric : 1;
            uint8_t state : 1;
            uint8_t volume : 1;
            uint8_t /* padding */ : 3;
        } filed;
    } changed;
    char *name;
    char *artist;
    char *lyric;
} bt_music_ctx_t;

typedef void (*bt_music_ctx_listener_t)(bt_music_ctx_t *ctx, void *user_data);

void bt_music_controller_ind_handler(const bt_api_msg_t *msg);
bt_music_ctx_t *bt_music_controller_get_ctx(void);
void bt_music_controller_set_ctx_listener(bt_music_ctx_listener_t listener, void *user_data);

#endif // __BT_MUSIC_CONTROLLER_H__
