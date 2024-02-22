#ifndef __WATCHFACE_MANAGER_H__
#define __WATCHFACE_MANAGER_H__

#include "lv_img.h"

#define MAX_WATCHFACE_SUPPORTED  (4)

#define DECLARE_WF_REFR_CTX_FUNC(ctxtype)                                                                   \
    static inline ctxtype *wf_mngr_create_refr_ctx(lv_obj_t *obj, uint16_t interval, lv_timer_cb_t refr_cb) \
    {                                                                                                       \
        return (ctxtype *)wf_mngr_create_refr_ctx_impl(obj, sizeof(ctxtype), interval, refr_cb);            \
    }                                                                                                       \
    static inline ctxtype *wf_mngr_get_refr_ctx(lv_timer_t *p_timer)                                        \
    {                                                                                                       \
        return (ctxtype *)((lv_obj_t *)p_timer->user_data)->user_data;                                      \
    }

typedef lv_obj_t *(*watchface_create_func_t)(lv_obj_t *);

typedef struct
{
    const char *p_caption;
    const lv_img_dsc_t *thumbnail;
    watchface_create_func_t create_func;
} watchface_info_t;

lv_obj_t *wf_mngr_create_watchface_layout(lv_obj_t *obj);

int32_t wf_mngr_add_watchface(char *caption, const lv_img_dsc_t *thumbnail, watchface_create_func_t create_func);

watchface_info_t *wf_mngr_get_watchface_list(void);

watchface_info_t *wf_mngr_get_current_watchface(void);

void wf_mngr_set_current_watchface(int32_t idx);

void wf_mngr_init(int32_t startup_idx);

void *wf_mngr_create_refr_ctx_impl(lv_obj_t *p_window, uint32_t ctx_size, uint16_t interval, lv_timer_cb_t refr_cb);

#endif
