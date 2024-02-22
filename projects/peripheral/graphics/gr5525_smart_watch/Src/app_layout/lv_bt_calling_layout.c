#include "lvgl.h"
#include "lv_volume_ctrl.h"
#include "lv_img_dsc_list.h"
#include "lv_layout_router.h"
#include "bt_phonecall_controller.h"

#if !LV_GDX_PATCH_CACHE_LABEL_LINE_INFO
#define update_line_info(...)
#endif // !LV_GDX_PATCH_CACHE_LABEL_LINE_INFO

#define CALLING_BTN_HANG_UP 0
#define CALLING_BTN_MUTE 1
#define CALL_END_DELAY_TIME 1200

#define MAX_VOLUME 0x10

static const lv_style_const_prop_t CALLING_PHONE_NUMBER_STYLE_PROPS[] = {
    LV_STYLE_CONST_WIDTH(DISP_HOR_RES),
    LV_STYLE_CONST_X(0),
    LV_STYLE_CONST_Y(130),
    LV_STYLE_CONST_TEXT_FONT(&lv_font_montserrat_28),
    LV_STYLE_CONST_TEXT_COLOR(LV_COLOR_MAKE(0xFF, 0xFF, 0xFF)),
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(0, 0, 0)),
    LV_STYLE_CONST_TEXT_ALIGN(LV_TEXT_ALIGN_CENTER),
    LV_STYLE_PROP_INV,
};

static const lv_style_const_prop_t CALLING_STATE_STYLE_PROPS[] = {
    LV_STYLE_CONST_WIDTH(DISP_HOR_RES),
    LV_STYLE_CONST_X(0),
    LV_STYLE_CONST_Y(165),
    LV_STYLE_CONST_TEXT_FONT(&lv_font_montserrat_20),
    LV_STYLE_CONST_TEXT_COLOR(LV_COLOR_MAKE(0x04, 0xa0, 0x04)),
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(0, 0, 0)),
    LV_STYLE_CONST_TEXT_ALIGN(LV_TEXT_ALIGN_CENTER),
    LV_STYLE_PROP_INV,
};

LV_STYLE_CONST_INIT(CALLING_PHONE_NUMBER_STYLE, CALLING_PHONE_NUMBER_STYLE_PROPS);
LV_STYLE_CONST_INIT(CALLING_STATE_STYLE, CALLING_STATE_STYLE_PROPS);

static char s_call_state[8] = "Calling";
static struct
{
    uint16_t minute;
    uint16_t seconds;
} s_calling_time;
static lv_timer_t *s_calling_time_timer = NULL;
static lv_obj_t *s_calling_state_label = NULL;
static lv_obj_t *s_volume_ctrl = NULL;

static void on_calling_btn_clicked(lv_event_t *event);
static void lv_bt_calling_layout_lifecycle_event(lv_event_t *event);
static void on_volume_changed(lv_event_t *event);
static void on_call_state_updated(bt_phonecall_state_t state);
static void on_vol_info_updated(bt_api_msg_t *rx_info);
static void calling_time_timer_cb(lv_timer_t *p_timer);
static void call_end_delay_timer_cb(lv_timer_t *p_timer);

lv_obj_t *lv_bt_calling_layout_create(lv_obj_t *parent)
{
    lv_obj_t *p_window = lv_obj_create(parent);
    lv_obj_remove_style_all(p_window);
    lv_obj_set_size(p_window, DISP_HOR_RES, DISP_VER_RES);
    lv_obj_add_event_cb(p_window, lv_bt_calling_layout_lifecycle_event, LV_EVENT_ALL, NULL);

    bt_phonecall_ctx_t *p_ctx = bt_phonecall_controller_get_context();

    // VolumeControl + PhoneNumber + State + HangUp + Mute
    // VolumeControl
    lv_obj_t *volume_ctrl = lv_volume_ctrl_create(p_window);
    lv_obj_align(volume_ctrl, LV_ALIGN_TOP_MID, 0, 55);
    lv_obj_add_event_cb(volume_ctrl, on_volume_changed, LV_EVENT_VALUE_CHANGED, NULL);
    lv_volume_ctrl_set_value(volume_ctrl, 100 * (p_ctx->volume / (MAX_VOLUME * 1.f)));
    s_volume_ctrl = volume_ctrl;

    // PhoneNumber
    lv_obj_t *phone_number = lv_label_create(p_window);
    lv_obj_add_style(phone_number, (lv_style_t *)&CALLING_PHONE_NUMBER_STYLE, 0);
    lv_label_set_text_static(phone_number, p_ctx->phone_number);

    // State
    lv_obj_t *state = lv_label_create(p_window);
    lv_obj_add_style(state, (lv_style_t *)&CALLING_STATE_STYLE, 0);
    lv_label_set_text_static(state, s_call_state);
    s_calling_state_label = state;

    // Hang Up
    lv_obj_t *hang_up = lv_img_create(p_window);
    lv_obj_add_flag(hang_up, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(hang_up, &wd_img_call_reject_large);
    lv_obj_align(hang_up, LV_ALIGN_TOP_MID, -70, 235);
    lv_obj_add_event_cb(hang_up, on_calling_btn_clicked, LV_EVENT_SHORT_CLICKED, (void *)((uintptr_t)CALLING_BTN_HANG_UP));

    // Mute
    lv_obj_t *mute = lv_img_create(p_window);
    lv_obj_add_flag(mute, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(mute, &wd_img_call_mute_large);
    lv_obj_align(mute, LV_ALIGN_TOP_MID, 70, 235);
    lv_obj_add_event_cb(mute, on_calling_btn_clicked, LV_EVENT_SHORT_CLICKED, (void *)((uintptr_t)CALLING_BTN_MUTE));

    bt_phonecall_controller_set_call_state_updated_cb(on_call_state_updated);

    // Manully update one time
    on_call_state_updated(p_ctx->call_state);

    return p_window;
}

static void on_calling_btn_clicked(lv_event_t *event)
{
    uint8_t btn_id = (uint8_t)((uintptr_t)event->user_data);

    if (btn_id == CALLING_BTN_HANG_UP)
    {
        bt_api_call_end();
    }
    else if (btn_id == CALLING_BTN_MUTE)
    {
        // wild but simple way
        if (lv_img_get_src(event->current_target) == &wd_img_call_mute_large)
        {
            bt_api_mic_mute(true);
            lv_img_set_src(event->current_target, &wd_img_call_mute_large_disabled);
        }
        else
        {
            bt_api_mic_mute(false);
            lv_img_set_src(event->current_target, &wd_img_call_mute_large);
        }
    }
}

static void lv_bt_calling_layout_lifecycle_event(lv_event_t *event)
{
    if (event->code == LV_EVENT_READY)
    {
        bt_api_register_indication_callback(on_vol_info_updated, IND_HFP_VOL_INFO);
    }
    else if (event->code == LV_EVENT_DELETE)
    {
        bt_api_deregister_indication_callback(on_vol_info_updated);
        s_calling_state_label = NULL;
        s_volume_ctrl = NULL;
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

static void on_call_state_updated(bt_phonecall_state_t state)
{
    printf("calling on_call_state_updated\n");
    if (state == CALL_STATE_CALLING)
    {
        strncpy(s_call_state, "Calling", sizeof(s_call_state));
        lv_obj_invalidate(s_calling_state_label);
        lv_obj_add_state(s_volume_ctrl, LV_STATE_DISABLED);
        lv_obj_invalidate(s_volume_ctrl);
        update_line_info(s_calling_state_label);
    }
    else if (state == CALL_STATE_ACCEPTED)
    {
        lv_obj_clear_state(s_volume_ctrl, LV_STATE_DISABLED);
        lv_obj_invalidate(s_volume_ctrl);
        s_calling_time.minute = 0;
        s_calling_time.seconds = 0;
        s_calling_time_timer = lv_timer_create(calling_time_timer_cb, 1000, NULL);
        lv_timer_ready(s_calling_time_timer);
    }
    else if (state == CALL_STATE_END)
    {
        lv_obj_add_state(s_volume_ctrl, LV_STATE_DISABLED);
        lv_obj_invalidate(s_volume_ctrl);
        if (s_calling_time_timer)
        {
            lv_timer_del(s_calling_time_timer);
            s_calling_time_timer = NULL;
        }
        bt_phonecall_controller_set_call_state_updated_cb(NULL);
        strncpy(s_call_state, "End", sizeof(s_call_state));
        lv_obj_invalidate(s_calling_state_label);
        update_line_info(s_calling_state_label);
        lv_timer_create(call_end_delay_timer_cb, CALL_END_DELAY_TIME, NULL);
    }
}

static void on_vol_info_updated(bt_api_msg_t *msg)
{
    uint8_t new_vol = 100 * (msg->data.indication->hfp_vol_info / (MAX_VOLUME * 1.f));
    lv_volume_ctrl_set_value(s_volume_ctrl, new_vol);
}

static void calling_time_timer_cb(lv_timer_t *p_timer)
{
    // Refresh label before increasing time will be very useful for first-time refresh
    snprintf(s_call_state, sizeof(s_call_state), "%02d:%02d", s_calling_time.minute, s_calling_time.seconds);
    lv_obj_invalidate(s_calling_state_label);
    update_line_info(s_calling_state_label);
    s_calling_time.seconds++;
    if (s_calling_time.seconds == 60)
    {
        s_calling_time.seconds = 0;
        s_calling_time.minute++;
    }
}

static void call_end_delay_timer_cb(lv_timer_t *p_timer)
{
    lv_layout_router_back(NULL);
    lv_timer_del(p_timer);
}
