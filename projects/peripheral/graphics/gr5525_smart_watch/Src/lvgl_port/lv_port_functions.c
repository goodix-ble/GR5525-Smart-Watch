#include "lv_port_functions.h"

extern bool lv_is_tileview_scrolling(void);

bool lv_timer_is_filtered(lv_timer_t * timer) {
#if LV_GDX_PATCH_TIMER_PRIO > 0u
    if(lv_is_tileview_scrolling() && timer->sys_prio != 1) {
        return true;
    }
#endif
    return false;
}
