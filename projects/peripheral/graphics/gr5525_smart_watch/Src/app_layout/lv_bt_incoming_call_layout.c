#include "lvgl.h"
#include "lv_img_dsc_list.h"
#include "bt_phonecall_controller.h"
#include "lv_layout_router.h"

#define INCOMING_CALL_BTN_ACCEPT 0
#define INCOMING_CALL_BTN_REJECT 1

static const lv_style_const_prop_t INCOMING_CALL_PHONE_NUMBER_STYLE_PROPS[] = {
    LV_STYLE_CONST_WIDTH(DISP_HOR_RES),
    LV_STYLE_CONST_X(0),
    LV_STYLE_CONST_Y(75),
    LV_STYLE_CONST_TEXT_FONT(&lv_font_montserrat_28),
    LV_STYLE_CONST_TEXT_COLOR(LV_COLOR_MAKE(0xFF, 0xFF, 0xFF)),
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(0, 0, 0)),
    LV_STYLE_CONST_TEXT_ALIGN(LV_TEXT_ALIGN_CENTER),
    LV_STYLE_PROP_INV,
};

LV_STYLE_CONST_INIT(INCOMING_CALL_PHONE_NUMBER_STYLE, INCOMING_CALL_PHONE_NUMBER_STYLE_PROPS);

static void on_incoming_call_btn_clicked(lv_event_t *event);
static void on_call_state_updated(bt_phonecall_state_t state);

static lv_obj_t *s_incoming_call_window = NULL;

lv_obj_t *lv_bt_incoming_call_layout_create(lv_obj_t *parent)
{
    bt_phonecall_ctx_t *p_ctx = bt_phonecall_controller_get_context();

    lv_obj_t *p_window = lv_obj_create(parent);
    lv_obj_remove_style_all(p_window);
    lv_obj_set_size(p_window, DISP_HOR_RES, DISP_VER_RES);
    lv_obj_set_style_bg_color(p_window, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(p_window, LV_OPA_COVER, 0);

    // Phone Number
    lv_obj_t *phone_number = lv_label_create(p_window);
    lv_obj_add_style(phone_number, (lv_style_t *)&INCOMING_CALL_PHONE_NUMBER_STYLE, 0);
    lv_label_set_text_static(phone_number, p_ctx->phone_number);

    // Reminder Icon
    lv_obj_t *reminder = lv_img_create(p_window);
    lv_img_set_src(reminder, &wd_img_incoming_call_reminder);
    lv_obj_align(reminder, LV_ALIGN_TOP_MID, 0, 115);

    // Reject
    lv_obj_t *reject = lv_img_create(p_window);
    lv_obj_add_flag(reject, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(reject, &wd_img_call_reject_large);
    lv_obj_align(reject, LV_ALIGN_TOP_MID, -70, 235);
    lv_obj_add_event_cb(reject, on_incoming_call_btn_clicked, LV_EVENT_SHORT_CLICKED, (void *)((uintptr_t)INCOMING_CALL_BTN_REJECT));

    // Accept
    lv_obj_t *accept = lv_img_create(p_window);
    lv_obj_add_flag(accept, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(accept, &wd_img_call_accept_large);
    lv_obj_align(accept, LV_ALIGN_TOP_MID, 70, 235);
    lv_obj_add_event_cb(accept, on_incoming_call_btn_clicked, LV_EVENT_SHORT_CLICKED, (void *)((uintptr_t)INCOMING_CALL_BTN_ACCEPT));

    bt_phonecall_controller_set_call_state_updated_cb(on_call_state_updated);

    s_incoming_call_window = p_window;

    return p_window;
}

void bt_phonecall_controller_on_incoming_call(void *param)
{
    lv_bt_incoming_call_layout_create(lv_disp_get_layer_top(NULL));
}

static void on_incoming_call_btn_clicked(lv_event_t *event)
{
    uint8_t btn_id = (uint8_t)((uintptr_t)event->user_data);

    if (btn_id == INCOMING_CALL_BTN_REJECT)
    {
        bt_api_call_reject();
    }
    else if (btn_id == INCOMING_CALL_BTN_ACCEPT)
    {
        bt_api_call_accept();
    }
}

static void on_call_state_updated(bt_phonecall_state_t state)
{
    if (state == CALL_STATE_ACCEPTED)
    {
        bt_phonecall_controller_set_call_state_updated_cb(NULL);
        lv_layout_router_show_calling_layout(NULL);
        lv_obj_del(s_incoming_call_window);
        s_incoming_call_window = NULL;
    }
    if (state == CALL_STATE_END)
    {
        bt_phonecall_controller_set_call_state_updated_cb(NULL);
        lv_obj_del(s_incoming_call_window);
        s_incoming_call_window = NULL;
    }
}
