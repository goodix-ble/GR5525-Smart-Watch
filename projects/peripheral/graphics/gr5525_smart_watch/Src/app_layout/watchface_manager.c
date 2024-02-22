#include <stdio.h>
#include "lv_img_dsc_list.h"
#include "watchface_manager.h"
#include "app_log.h"


extern lv_obj_t *lv_watchface_black_clock_layout_create(lv_obj_t *parent);
extern lv_obj_t *lv_watchface_vivid_clock_layout_create(lv_obj_t *parent);
extern lv_obj_t *lv_watchface_digital_clock_layout_create(lv_obj_t *parent);

static void wf_mngr_lifecycle_cb(lv_event_t *event);

/*
 * STATIC VARS DEFINITIONS
 *****************************************************************************************
 */
static watchface_info_t s_watchfaces[MAX_WATCHFACE_SUPPORTED] = {
    {
        .p_caption = "Black",
        .thumbnail = &wd_img_black_clock_thumbnail,
        .create_func = lv_watchface_black_clock_layout_create,
    },
    {
        .p_caption = "Vivid",
        .thumbnail = &wd_img_vivid_clock_thumbnail,
        .create_func = lv_watchface_vivid_clock_layout_create,
    },
    {
        .p_caption = "Digital",
        .thumbnail = &wd_img_digital_clock_thumbnail,
        .create_func = lv_watchface_digital_clock_layout_create,
    },
};

static watchface_info_t *p_cur_watchface = NULL;
static lv_timer_t *s_wf_refr_timer = NULL;

/*
 * GLOBAL METHODS IMPLEMENT
 *****************************************************************************************
 */
lv_obj_t *wf_mngr_create_watchface_layout(lv_obj_t *obj)
{
    if (!p_cur_watchface || !p_cur_watchface->create_func)
    {
        p_cur_watchface = &s_watchfaces[0];
    }
    lv_obj_t *p_wf = p_cur_watchface->create_func(obj);
    lv_obj_add_event_cb(p_wf, wf_mngr_lifecycle_cb, LV_EVENT_ALL, NULL);
    if (s_wf_refr_timer && s_wf_refr_timer->timer_cb)
    {
        s_wf_refr_timer->timer_cb(s_wf_refr_timer);
    }
    return p_wf;
}

int32_t wf_mngr_add_watchface(char *caption, const lv_img_dsc_t *thumbnail, watchface_create_func_t create_func)
{
    uint32_t idx = 0;
    for (; idx < MAX_WATCHFACE_SUPPORTED; idx++)
    {
        if (s_watchfaces[idx].create_func == NULL)
        {
            break;
        }
    }

    if (idx == MAX_WATCHFACE_SUPPORTED)
    {
        // Full
        return -1;
    }

    s_watchfaces[idx].p_caption = caption;
    s_watchfaces[idx].thumbnail = thumbnail;
    s_watchfaces[idx].create_func = create_func;

    return idx;
}

watchface_info_t *wf_mngr_get_watchface_list(void)
{
    return s_watchfaces;
}

watchface_info_t *wf_mngr_get_current_watchface(void)
{
    return p_cur_watchface;
}

void wf_mngr_set_current_watchface(int32_t idx)
{
    if (idx >= 0 && s_watchfaces[idx].create_func)
    {
        p_cur_watchface = &s_watchfaces[idx];
    }
    else
    {
        // APP_LOG_WARNING("[WARNING] Invalid Index!");
        p_cur_watchface = &s_watchfaces[0];
        idx = 0;
    }
}

void wf_mngr_init(int32_t startup_idx)
{
    s_wf_refr_timer = lv_timer_create_basic();
    lv_timer_pause(s_wf_refr_timer);
    wf_mngr_set_current_watchface(startup_idx);
}

void *wf_mngr_create_refr_ctx_impl(lv_obj_t *p_window, uint32_t ctx_size, uint16_t interval, lv_timer_cb_t refr_cb)
{
    p_window->user_data = lv_mem_alloc(ctx_size);
    if (!p_window->user_data)
    {
        APP_LOG_ERROR("Failed to alloc wf ctx!");
        while(1);
    }
    if (!s_wf_refr_timer)
    {
        s_wf_refr_timer = lv_timer_create_basic();
    }
    s_wf_refr_timer->timer_cb = refr_cb;
    s_wf_refr_timer->user_data = (void *)p_window;
    return p_window->user_data;
}

static void wf_mngr_lifecycle_cb(lv_event_t *event)
{
    lv_obj_t *p_wf = event->current_target;
    lv_event_code_t code = event->code;
    switch (code)
    {
        case LV_EVENT_CANCEL:
            if (s_wf_refr_timer)
            {
                lv_timer_pause(s_wf_refr_timer);
            }
            break;

        case LV_EVENT_DELETE:
            if (p_wf->user_data)
            {
                lv_mem_free(p_wf->user_data);
            }
            break;

        case LV_EVENT_READY:
            // if current watchface is not creating refr context, timer will be disabled
            if (p_wf->user_data)
            {
                if (s_wf_refr_timer)
                {
                    lv_timer_resume(s_wf_refr_timer);
                    lv_timer_ready(s_wf_refr_timer);
                }
            }

    }
}
