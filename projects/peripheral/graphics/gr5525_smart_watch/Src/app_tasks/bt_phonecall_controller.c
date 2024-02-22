#include "bt_phonecall_controller.h"
#include "app_bt.h"
#include "lvgl.h"
#include "FreeRTOS.h"
#include "lv_layout_router.h"
#include "app_log.h"

#pragma diag_suppress 177

#define CONTACTS_UPDATE_MINIMUM_ELAPSE (60 * 60 * 1000)

static const char *const CALL_STATE_STR[] = {
    "INCOMING",
    "ACCEPTED",
    "HOLD",
    "END",
    "REJECTED",
};

static bt_phonecall_ctx_t s_phonecall_ctx = {0};
static lv_timer_t *s_contacts_updated_timer = NULL;
static lv_timer_t *s_call_end_delay_timer = NULL;
static uint32_t s_contacts_last_updated = 0;
static bt_phonecall_controller_contacts_updated_cb s_contacts_updated_cb = NULL;
static bt_phonecall_controller_call_state_updated_cb s_call_state_updated_cb = NULL;

static void copy_phone_number(char* dest, uint32_t max_len, const bt_api_msg_t *msg);
static char *copy_contacts_info(const bt_api_msg_t *rx_info);
static void contacts_updated_timer_cb(lv_timer_t *p_timer);
static void call_state_updated_async_wrapper(void *param);
static void incoming_call_async_wrapper(void *param);

void bt_phonecall_controller_ind_handler(const bt_api_msg_t *msg)
{
    if (msg->opcode == IND_CALL_LINE_STATE)
    {
        static const bt_phonecall_state_t CALL_STATE_LUT[] = {
            CALL_STATE_INCOMING,
            CALL_STATE_ACCEPTED,
            CALL_STATE_HOLD,
            CALL_STATE_END,
            CALL_STATE_IDLE,
        };
        s_phonecall_ctx.call_state = CALL_STATE_LUT[msg->data.indication->call_line_state.state];
        copy_phone_number(s_phonecall_ctx.phone_number, sizeof(s_phonecall_ctx.phone_number), msg);

        if (s_phonecall_ctx.call_state == CALL_STATE_INCOMING)
        {
            lv_async_call(incoming_call_async_wrapper, NULL);
        }

        if (s_call_state_updated_cb)
        {
            lv_async_call(call_state_updated_async_wrapper, NULL);
        }

        // printf("Call State Updated: %s With [%s]\n", CALL_STATE_STR[msg->data.indication->call_line_state.state], s_phonecall_ctx.phone_number);
    }
    // else if (msg->opcode == IND_ADDR_BOOK_INFO)
    // {
    //     s_contacts_last_updated = lv_tick_get();
    //     char *cc = (char *)&msg->data.indication->addr_book_info;
    //     cc[msg->length] = 0;
    //     char *contact_info = copy_contacts_info(msg);
    //     if (!contact_info)
    //     {
    //         return;
    //     }
    //     if (!s_phonecall_ctx.contacts_list.n_size)
    //     {
    //         _lv_ll_init(&s_phonecall_ctx.contacts_list, sizeof(char *));
    //     }
    //     char **node = (char **)_lv_ll_ins_tail(&s_phonecall_ctx.contacts_list);
    //     *node = contact_info;
    //     if (!s_contacts_updated_timer)
    //     {
    //         s_contacts_updated_timer = lv_timer_create(contacts_updated_timer_cb, 1000, NULL);
    //     }
    //     lv_timer_reset(s_contacts_updated_timer);
    // }
    else if (msg->opcode == IND_PROFILE_STATE)
    {
        if (msg->data.indication->profile_state.hfp == BT_API_CONN_STATE_CONNECTED)
        {
            // bt_phonecall_controller_request_contacts_update(true);
        }
    }
    else if (msg->opcode == IND_HFP_VOL_INFO)
    {
        s_phonecall_ctx.volume = msg->data.indication->hfp_vol_info;
        // APP_LOG_DEBUG("Call Volume Update: %d\n", s_phonecall_ctx.volume);
    }
    else if (msg->opcode == IND_VOL_SITE_INFO)
    {
        printf("Vol Site Updated, current site: %s\n", msg->data.indication->vol_site_info == VOL_SITE_BT ? "BT" : "Phone");
        if (msg->data.indication->vol_site_info == VOL_SITE_PHONE && s_phonecall_ctx.call_state == CALL_STATE_CALLING)
        {
            s_phonecall_ctx.call_state = CALL_STATE_END;
            if (s_call_state_updated_cb)
            {
                lv_async_call(call_state_updated_async_wrapper, NULL);
            }
        }
    }
}

void bt_phonecall_controller_make_call(char* phone_number)
{
    strcpy(s_phonecall_ctx.phone_number, phone_number);
    bt_api_place_call_number((uint8_t *)phone_number, strlen(phone_number));
    s_phonecall_ctx.call_state = CALL_STATE_CALLING;
}

bt_phonecall_ctx_t *bt_phonecall_controller_get_context(void)
{
    return &s_phonecall_ctx;
}

void bt_phonecall_controller_set_contacts_updated_cb(bt_phonecall_controller_contacts_updated_cb cb)
{
    s_contacts_updated_cb = cb;
}

void bt_phonecall_controller_set_call_state_updated_cb(bt_phonecall_controller_call_state_updated_cb cb)
{
    s_call_state_updated_cb = cb;
}

void bt_phonecall_controller_request_contacts_update(bool force)
{
    if (s_contacts_updated_timer)
    {
        // Contacts is still updating
        printf("Still Updating\n");
        return;
    }

    if (lv_tick_elaps(s_contacts_last_updated) > CONTACTS_UPDATE_MINIMUM_ELAPSE)
    {
        force = true;
    }
    else if (s_contacts_last_updated == 0)
    {
        force = true;
    }

    if (force)
    {
        if (!_lv_ll_is_empty(&s_phonecall_ctx.contacts_list))
        {
            _lv_ll_clear(&s_phonecall_ctx.contacts_list);
        }
        bt_api_call_address_book_get();
    }
    else
    {
        // No need to update
        s_contacts_updated_timer = lv_timer_create(contacts_updated_timer_cb, 0, NULL);
    }
}

__attribute__((weak)) void bt_phonecall_controller_on_incoming_call(void *param)
{
}

static void copy_phone_number(char* dest, uint32_t max_len, const bt_api_msg_t *msg)
{
    max_len -= 1; // preserved for ending zero;
    char *cc = &msg->data.indication->call_line_state.phone_number + 1; // The first character is always '"'
    for (uint32_t i = 0; i < max_len; i++)
    {
        if (cc[i] == '"')
        {
            break;
        }
        *dest++ = cc[i];
    }
    *dest = 0;
}

static char *copy_contacts_info(const bt_api_msg_t *msg)
{
    uint32_t pn_len = 0;
    uint32_t data_length = msg->length;
    char *cc = (char *)&msg->data.indication->addr_book_info;
    while (data_length > 0)
    {
        if (*cc == '\r' && *(cc + 1) == '\n')
        {
            break;
        }
        cc++;
        data_length--;
    }
    cc += 2;
    data_length -= 2;
    char *pn_pos = cc;
    uint32_t data_length_pn = data_length;
    while (data_length > 0)
    {
        if ((*cc >= '0' && *cc <= '9') || *cc == '+')
        {
            pn_len++;
        }
        cc++;
        data_length--;
    }

    char *buf = (char *)pvPortMalloc(pn_len + 1);
    if (!buf)
    {
        return NULL;
    }
    cc = pn_pos;
    data_length = data_length_pn;
    char *dd = buf;
    while (data_length > 0)
    {
        if ((*cc >= '0' && *cc <= '9') || *cc == '+')
        {
            *dd = *cc;
            dd++;
        }
        cc++;
        data_length--;
    }
    buf[pn_len] = 0;
    return buf;
}

static void contacts_updated_timer_cb(lv_timer_t *p_timer)
{
    APP_LOG_DEBUG("Contacts Update Complete: %d", _lv_ll_get_len(&s_phonecall_ctx.contacts_list));
    if (s_contacts_updated_cb)
    {
        s_contacts_updated_cb(&s_phonecall_ctx.contacts_list);
    }
    lv_timer_del(s_contacts_updated_timer);
    s_contacts_updated_timer = NULL;
}

static void call_state_updated_async_wrapper(void *param)
{
    if (s_call_state_updated_cb)
    {
        s_call_state_updated_cb(s_phonecall_ctx.call_state);
    }
}

static void incoming_call_async_wrapper(void *param)
{
    void bt_phonecall_controller_on_incoming_call(void *param);
}
