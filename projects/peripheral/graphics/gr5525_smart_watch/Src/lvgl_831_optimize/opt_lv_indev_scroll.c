/**
 * @file lv_indev_scroll.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_indev.h"
#include "lv_indev_scroll.h"

/*********************
 *      DEFINES
 *********************/
#define ELASTIC_SLOWNESS_FACTOR 4   /*Scrolling on elastic parts are slower by this factor*/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
#if LV_GDX_PATCH_CUSTOM_SCROLL
static lv_obj_t * find_scroll_obj(_lv_indev_proc_t * proc, bool use_custom_scroll);
#else
static lv_obj_t * find_scroll_obj(_lv_indev_proc_t * proc);
#endif
static void init_scroll_limits(_lv_indev_proc_t * proc);
static lv_coord_t find_snap_point_x(const lv_obj_t * obj, lv_coord_t min, lv_coord_t max, lv_coord_t ofs);
static lv_coord_t find_snap_point_y(const lv_obj_t * obj, lv_coord_t min, lv_coord_t max, lv_coord_t ofs);
static void scroll_limit_diff(_lv_indev_proc_t * proc, lv_coord_t * diff_x, lv_coord_t * diff_y);
static lv_coord_t scroll_throw_predict_y(_lv_indev_proc_t * proc);
static lv_coord_t scroll_throw_predict_x(_lv_indev_proc_t * proc);
static lv_coord_t elastic_diff(lv_obj_t * scroll_obj, lv_coord_t diff, lv_coord_t scroll_start, lv_coord_t scroll_end,
                               lv_dir_t dir);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void _lv_indev_scroll_handler_opt(_lv_indev_proc_t * proc)
{
    lv_obj_t * scroll_obj = proc->types.pointer.scroll_obj;
    /*If there is no scroll object yet try to find one*/
    if(scroll_obj == NULL) {
        proc->types.pointer.scroll_sum.x += proc->types.pointer.vect.x;
        proc->types.pointer.scroll_sum.y += proc->types.pointer.vect.y;

#if LV_GDX_PATCH_CUSTOM_SCROLL
        // backup sum in case of refusing scroll
        lv_coord_t scroll_sum_x = proc->types.pointer.scroll_sum.x;
        lv_coord_t scroll_sum_y = proc->types.pointer.scroll_sum.y;

        scroll_obj = find_scroll_obj(proc, true);
        if(scroll_obj == NULL) return;

        if ((scroll_obj->flags & LV_OBJ_FLAG_SCROLL_CUSTOM) != 0) {
            lv_event_send(scroll_obj, LV_EVENT_SCROLL_BEGIN, proc);
            // not required by current custom scroll.
            if (proc->types.pointer.scroll_obj == NULL) {
                // recover param and find again and ignore custom scroll.
                proc->types.pointer.scroll_sum.x = scroll_sum_x;
                proc->types.pointer.scroll_sum.y = scroll_sum_y;
                scroll_obj = find_scroll_obj(proc, false);
                if(scroll_obj == NULL) {
                    // ignore current scroll.
                    proc->wait_until_release = 1;
                    return;
                }
                init_scroll_limits(proc);
                lv_event_send(scroll_obj, LV_EVENT_SCROLL_BEGIN, NULL);
            }
        }
        else {
            init_scroll_limits(proc);
            lv_event_send(scroll_obj, LV_EVENT_SCROLL_BEGIN, NULL);
        }
#else // LV_GDX_PATCH_CUSTOM_SCROLL
        scroll_obj = find_scroll_obj(proc);
        if(scroll_obj == NULL) return;

        init_scroll_limits(proc);
        lv_event_send(scroll_obj, LV_EVENT_SCROLL_BEGIN, NULL);
#endif // LV_GDX_PATCH_CUSTOM_SCROLL

        if(proc->reset_query) return;
    }

    /*Set new position or scroll if the vector is not zero*/
    if(proc->types.pointer.vect.x != 0 || proc->types.pointer.vect.y != 0) {
        #if LV_GDX_PATCH_CUSTOM_SCROLL
        if ((scroll_obj->flags & LV_OBJ_FLAG_SCROLL_CUSTOM) != 0) {
            lv_coord_t vect_x = proc->types.pointer.vect.x;
            lv_coord_t vect_y = proc->types.pointer.vect.y;

            // 对于自定义滚动的情况，不由框架处理控件的滚动参数
            // lv_obj_allocate_spec_attr(scroll_obj);

            // scroll_obj->spec_attr->scroll.x += vect_x;
            // scroll_obj->spec_attr->scroll.y += vect_y;

            lv_event_send(scroll_obj, LV_EVENT_SCROLL, proc);
            // lv_obj_invalidate(scroll_obj); 由控件自行控制需要刷新哪里

            if(proc->reset_query) return;
            proc->types.pointer.scroll_sum.x += vect_x;
            proc->types.pointer.scroll_sum.y += vect_y;
            return;
        }
        #endif // LV_GDX_PATCH_CUSTOM_SCROLL

        lv_coord_t diff_x = 0;
        lv_coord_t diff_y = 0;

        if(proc->types.pointer.scroll_dir == LV_DIR_HOR) {
            lv_coord_t sr = lv_obj_get_scroll_right(scroll_obj);
            lv_coord_t sl = lv_obj_get_scroll_left(scroll_obj);
            diff_x = elastic_diff(scroll_obj, proc->types.pointer.vect.x, sl, sr, LV_DIR_HOR);
        }
        else {

            lv_coord_t st = lv_obj_get_scroll_top(scroll_obj);
            lv_coord_t sb = lv_obj_get_scroll_bottom(scroll_obj);
            diff_y = elastic_diff(scroll_obj, proc->types.pointer.vect.y, st, sb, LV_DIR_VER);
        }
#if !LV_GDX_PATCH_FIX_BUG
        lv_dir_t scroll_dir = lv_obj_get_scroll_dir(scroll_obj);
        if((scroll_dir & LV_DIR_LEFT)   == 0 && diff_x > 0) diff_x = 0;
        if((scroll_dir & LV_DIR_RIGHT)  == 0 && diff_x < 0) diff_x = 0;
        if((scroll_dir & LV_DIR_TOP)    == 0 && diff_y > 0) diff_y = 0;
        if((scroll_dir & LV_DIR_BOTTOM) == 0 && diff_y < 0) diff_y = 0;
#endif
        /*Respect the scroll limit area*/
        scroll_limit_diff(proc, &diff_x, &diff_y);

        _lv_obj_scroll_by_raw(scroll_obj, diff_x, diff_y);
        if(proc->reset_query) return;
        proc->types.pointer.scroll_sum.x += diff_x;
        proc->types.pointer.scroll_sum.y += diff_y;
    }
}


void _lv_indev_scroll_throw_handler_opt(_lv_indev_proc_t * proc)
{
    lv_obj_t * scroll_obj = proc->types.pointer.scroll_obj;
    if(scroll_obj == NULL) return;
    if(proc->types.pointer.scroll_dir == LV_DIR_NONE) return;


    lv_indev_t * indev_act = lv_indev_get_act();
    lv_coord_t scroll_throw = indev_act->driver->scroll_throw;

    if(lv_obj_has_flag(scroll_obj, LV_OBJ_FLAG_SCROLL_MOMENTUM) == false) {
        proc->types.pointer.scroll_throw_vect.y = 0;
        proc->types.pointer.scroll_throw_vect.x = 0;
    }

#if LV_GDX_PATCH_CUSTOM_SCROLL
    if(lv_obj_has_flag(scroll_obj, LV_OBJ_FLAG_SCROLL_CUSTOM)) {
        lv_event_send(scroll_obj, LV_EVENT_SCROLL_END, proc);
        if(proc->reset_query) return;

        proc->types.pointer.scroll_dir = LV_DIR_NONE;
        proc->types.pointer.scroll_obj = NULL;
        return;
    }
#endif

    lv_scroll_snap_t align_x = lv_obj_get_scroll_snap_x(scroll_obj);
    lv_scroll_snap_t align_y = lv_obj_get_scroll_snap_y(scroll_obj);

    if(proc->types.pointer.scroll_dir == LV_DIR_VER) {
        proc->types.pointer.scroll_throw_vect.x = 0;
        /*If no snapping "throw"*/
        if(align_y == LV_SCROLL_SNAP_NONE) {
            proc->types.pointer.scroll_throw_vect.y =
                proc->types.pointer.scroll_throw_vect.y * (100 - scroll_throw) / 100;

            lv_coord_t sb = lv_obj_get_scroll_bottom(scroll_obj);
            lv_coord_t st = lv_obj_get_scroll_top(scroll_obj);

            proc->types.pointer.scroll_throw_vect.y = elastic_diff(scroll_obj, proc->types.pointer.scroll_throw_vect.y, st, sb,
                                                                   LV_DIR_VER);

            lv_obj_scroll_by(scroll_obj, 0, proc->types.pointer.scroll_throw_vect.y, LV_ANIM_OFF);
        }
        /*With snapping find the nearest snap point and scroll there*/
        else {
            lv_coord_t diff_y = scroll_throw_predict_y(proc);
            proc->types.pointer.scroll_throw_vect.y = 0;
            scroll_limit_diff(proc, NULL, &diff_y);
            lv_coord_t y = find_snap_point_y(scroll_obj, LV_COORD_MIN, LV_COORD_MAX, diff_y);
            lv_obj_scroll_by(scroll_obj, 0, diff_y + y, LV_ANIM_ON);
        }
    }
    else if(proc->types.pointer.scroll_dir == LV_DIR_HOR) {
        proc->types.pointer.scroll_throw_vect.y = 0;
        /*If no snapping "throw"*/
        if(align_x == LV_SCROLL_SNAP_NONE) {
            proc->types.pointer.scroll_throw_vect.x =
                proc->types.pointer.scroll_throw_vect.x * (100 - scroll_throw) / 100;

            lv_coord_t sl = lv_obj_get_scroll_left(scroll_obj);
            lv_coord_t sr = lv_obj_get_scroll_right(scroll_obj);

            proc->types.pointer.scroll_throw_vect.x = elastic_diff(scroll_obj, proc->types.pointer.scroll_throw_vect.x, sl, sr,
                                                                   LV_DIR_HOR);

            lv_obj_scroll_by(scroll_obj, proc->types.pointer.scroll_throw_vect.x, 0, LV_ANIM_OFF);
        }
        /*With snapping find the nearest snap point and scroll there*/
        else {
            lv_coord_t diff_x = scroll_throw_predict_x(proc);
            proc->types.pointer.scroll_throw_vect.x = 0;
            scroll_limit_diff(proc, &diff_x, NULL);
            lv_coord_t x = find_snap_point_x(scroll_obj, LV_COORD_MIN, LV_COORD_MAX, diff_x);
            lv_obj_scroll_by(scroll_obj, x + diff_x, 0, LV_ANIM_ON);
        }
    }

    /*Check if the scroll has finished*/
    if(proc->types.pointer.scroll_throw_vect.x == 0 && proc->types.pointer.scroll_throw_vect.y == 0) {
        /*Revert if scrolled in*/
        /*If vertically scrollable and not controlled by snap*/
        if(align_y == LV_SCROLL_SNAP_NONE) {
            lv_coord_t st = lv_obj_get_scroll_top(scroll_obj);
            lv_coord_t sb = lv_obj_get_scroll_bottom(scroll_obj);
            if(st > 0 || sb > 0) {
                if(st < 0) {
                    lv_obj_scroll_by(scroll_obj, 0, st, LV_ANIM_ON);
                }
                else if(sb < 0) {
                    lv_obj_scroll_by(scroll_obj, 0, -sb, LV_ANIM_ON);
                }
            }
        }

        /*If horizontally scrollable and not controlled by snap*/
        if(align_x == LV_SCROLL_SNAP_NONE) {
            lv_coord_t sl = lv_obj_get_scroll_left(scroll_obj);
            lv_coord_t sr = lv_obj_get_scroll_right(scroll_obj);
            if(sl > 0 || sr > 0) {
                if(sl < 0) {
                    lv_obj_scroll_by(scroll_obj, sl, 0, LV_ANIM_ON);
                }
                else if(sr < 0) {
                    lv_obj_scroll_by(scroll_obj, -sr, 0, LV_ANIM_ON);
                }
            }
        }

        lv_event_send(scroll_obj, LV_EVENT_SCROLL_END, indev_act);
        if(proc->reset_query) return;

        proc->types.pointer.scroll_dir = LV_DIR_NONE;
        proc->types.pointer.scroll_obj = NULL;
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#if LV_GDX_PATCH_CUSTOM_SCROLL
static lv_obj_t * find_scroll_obj(_lv_indev_proc_t * proc, bool use_custom_scroll)
{
    lv_indev_t * indev_act = lv_indev_get_act();
    lv_coord_t scroll_limit = indev_act->driver->scroll_limit;
    lv_obj_t * obj_act = proc->types.pointer.act_obj;
    lv_obj_t * obj_candidate = NULL;
    lv_dir_t dir_candidate = LV_DIR_NONE;

    lv_coord_t scroll_sum_x = proc->types.pointer.scroll_sum.x;
    lv_coord_t scroll_sum_y = proc->types.pointer.scroll_sum.y;
    bool is_break_scroll_limit_left;
    bool is_break_scroll_limit_right;
    bool is_break_scroll_limit_up;
    bool is_break_scroll_limit_down;
    bool hor_en = LV_ABS(scroll_sum_x) > LV_ABS(scroll_sum_y);
    bool ver_en = !hor_en;
    if(hor_en) {
        is_break_scroll_limit_left = scroll_sum_x >= scroll_limit;
        is_break_scroll_limit_right = scroll_sum_x <= - scroll_limit;
        is_break_scroll_limit_up = false;
        is_break_scroll_limit_down = false;
    }
    else {
        is_break_scroll_limit_left = false;
        is_break_scroll_limit_right = false;
        is_break_scroll_limit_up = scroll_sum_y >= scroll_limit;
        is_break_scroll_limit_down = scroll_sum_y <= - scroll_limit;
    }

    while(obj_act) {
        if (use_custom_scroll && lv_obj_has_flag(obj_act, LV_OBJ_FLAG_SCROLL_CUSTOM)) {
            bool probe_custom_scroll = false;
            lv_point_t scroll_distance;
            if(is_break_scroll_limit_left || is_break_scroll_limit_right) {
                obj_candidate = obj_act;
                dir_candidate = LV_DIR_HOR;
                probe_custom_scroll = true;
                scroll_distance.x = scroll_sum_x;
                scroll_distance.y = 0;
            }
            if(is_break_scroll_limit_up || is_break_scroll_limit_down) {
                obj_candidate = obj_act;
                dir_candidate = LV_DIR_VER;
                probe_custom_scroll = true;
                scroll_distance.x = 0;
                scroll_distance.y = scroll_sum_y;
            }
            if (probe_custom_scroll) {
                if(lv_event_send(obj_candidate, LV_EVENT_PROBE_CUSTOM_SCROLL, &scroll_distance) == LV_RES_OK) {
                    if (scroll_distance.x != 0 || scroll_distance.y != 0)
                    {
                        break;
                    }
                }
            }
        }
        else {
            if(lv_obj_has_flag(obj_act, LV_OBJ_FLAG_SCROLLABLE) == false) {
                /*If this object don't want to chain the scroll to the parent stop searching*/
                if(lv_obj_has_flag(obj_act, LV_OBJ_FLAG_SCROLL_CHAIN_HOR) == false && hor_en) break;
                if(lv_obj_has_flag(obj_act, LV_OBJ_FLAG_SCROLL_CHAIN_VER) == false && ver_en) break;

                #if LV_GDX_PATCH_GET_PARENT_DIRECTLY
                obj_act = obj_act->parent;
                #else
                /*optimized*/obj_act = lv_obj_get_parent(obj_act);
                #endif
                continue;
            }

            /*Consider both up-down or left/right scrollable according to the current direction*/
            bool up_en = is_break_scroll_limit_up;
            bool down_en = is_break_scroll_limit_down;
            bool left_en = is_break_scroll_limit_left;
            bool right_en = is_break_scroll_limit_right;

            /*The object might have disabled some directions.*/
            lv_dir_t scroll_dir = lv_obj_get_scroll_dir(obj_act);
            if((scroll_dir & LV_DIR_LEFT) == 0) left_en = false;
            if((scroll_dir & LV_DIR_RIGHT) == 0) right_en = false;
            if((scroll_dir & LV_DIR_TOP) == 0) up_en = false;
            if((scroll_dir & LV_DIR_BOTTOM) == 0) down_en = false;

            /*The object is scrollable to a direction if its content overflow in that direction.*/
            lv_coord_t st = lv_obj_get_scroll_top(obj_act);
            lv_coord_t sb = lv_obj_get_scroll_bottom(obj_act);
            lv_coord_t sl = lv_obj_get_scroll_left(obj_act);
            lv_coord_t sr = lv_obj_get_scroll_right(obj_act);

            /*If this object is scrollable into the current scroll direction then save it as a candidate.
            *It's important only to be scrollable on the current axis (hor/ver) because if the scroll
            *is propagated to this object it can show at least elastic scroll effect.
            *But if not hor/ver scrollable do not scroll it at all (so it's not a good candidate)*/
            if((st > 0 || sb > 0) && (up_en || down_en)) {
                obj_candidate = obj_act;
                dir_candidate = LV_DIR_VER;
            }

            if((sl > 0 || sr > 0) && (left_en || right_en)) {
                obj_candidate = obj_act;
                dir_candidate = LV_DIR_HOR;
            }

            if(st <= 0) up_en = false;
            if(sb <= 0) down_en = false;
            if(sl <= 0) left_en = false;
            if(sr <= 0) right_en = false;

            /*If the object really can be scrolled into the current direction the use it.*/
            if(left_en || right_en || up_en || down_en) {
                proc->types.pointer.scroll_dir = hor_en ? LV_DIR_HOR : LV_DIR_VER;
                break;
            }
        }

        /*If this object don't want to chain the scroll to the parent stop searching*/
        if(lv_obj_has_flag(obj_act, LV_OBJ_FLAG_SCROLL_CHAIN_HOR) == false && hor_en) break;
        if(lv_obj_has_flag(obj_act, LV_OBJ_FLAG_SCROLL_CHAIN_VER) == false && ver_en) break;

        /*Try the parent*/
        #if LV_GDX_PATCH_GET_PARENT_DIRECTLY
        obj_act = obj_act->parent;
        #else
        /*optimized*/obj_act = lv_obj_get_parent(obj_act);
        #endif
    }

    /*Use the last candidate*/
    if(obj_candidate) {
        proc->types.pointer.scroll_dir = dir_candidate;
        proc->types.pointer.scroll_obj = obj_candidate;
        proc->types.pointer.scroll_sum.x = 0;
        proc->types.pointer.scroll_sum.y = 0;
    }

    return obj_candidate;
}
#else // LV_GDX_PATCH_CUSTOM_SCROLL
static lv_obj_t * find_scroll_obj(_lv_indev_proc_t * proc)
{
    lv_obj_t * obj_candidate = NULL;
    lv_dir_t dir_candidate = LV_DIR_NONE;
    lv_indev_t * indev_act = lv_indev_get_act();
    lv_coord_t scroll_limit = indev_act->driver->scroll_limit;

    /*Go until find a scrollable object in the current direction
     *More precisely:
     * 1. Check the pressed object and all of its ancestors and try to find an object which is scrollable
     * 2. Scrollable means it has some content out of its area
     * 3. If an object can be scrolled into the current direction then use it ("real match"")
     * 4. If can be scrolled on the current axis (hor/ver) save it as candidate (at least show an elastic scroll effect)
     * 5. Use the last candidate. Always the "deepest" parent or the object from point 3*/
    lv_obj_t * obj_act = proc->types.pointer.act_obj;

    /*Decide if it's a horizontal or vertical scroll*/
    bool hor_en = false;
    bool ver_en = false;
    if(LV_ABS(proc->types.pointer.scroll_sum.x) > LV_ABS(proc->types.pointer.scroll_sum.y)) {
        hor_en = true;
    }
    else {
        ver_en = true;
    }

    while(obj_act) {
        if(lv_obj_has_flag(obj_act, LV_OBJ_FLAG_SCROLLABLE) == false) {
            /*If this object don't want to chain the scroll to the parent stop searching*/
            if(lv_obj_has_flag(obj_act, LV_OBJ_FLAG_SCROLL_CHAIN_HOR) == false && hor_en) break;
            if(lv_obj_has_flag(obj_act, LV_OBJ_FLAG_SCROLL_CHAIN_VER) == false && ver_en) break;

            #if LV_GDX_PATCH_GET_PARENT_DIRECTLY
            obj_act = obj_act->parent;
            #else
            /*optimized*/obj_act = lv_obj_get_parent(obj_act);
            #endif
            continue;
        }

        /*Consider both up-down or left/right scrollable according to the current direction*/
        bool up_en = ver_en;
        bool down_en = ver_en;
        bool left_en = hor_en;
        bool right_en = hor_en;

        /*The object might have disabled some directions.*/
        lv_dir_t scroll_dir = lv_obj_get_scroll_dir(obj_act);
        if((scroll_dir & LV_DIR_LEFT) == 0) left_en = false;
        if((scroll_dir & LV_DIR_RIGHT) == 0) right_en = false;
        if((scroll_dir & LV_DIR_TOP) == 0) up_en = false;
        if((scroll_dir & LV_DIR_BOTTOM) == 0) down_en = false;

        /*The object is scrollable to a direction if its content overflow in that direction.*/
        lv_coord_t st = lv_obj_get_scroll_top(obj_act);
        lv_coord_t sb = lv_obj_get_scroll_bottom(obj_act);
        lv_coord_t sl = lv_obj_get_scroll_left(obj_act);
        lv_coord_t sr = lv_obj_get_scroll_right(obj_act);

        /*If this object is scrollable into the current scroll direction then save it as a candidate.
         *It's important only to be scrollable on the current axis (hor/ver) because if the scroll
         *is propagated to this object it can show at least elastic scroll effect.
         *But if not hor/ver scrollable do not scroll it at all (so it's not a good candidate)*/
        if((st > 0 || sb > 0)  &&
           ((up_en    && proc->types.pointer.scroll_sum.y >=   scroll_limit) ||
            (down_en  && proc->types.pointer.scroll_sum.y <= - scroll_limit))) {
            obj_candidate = obj_act;
            dir_candidate = LV_DIR_VER;
        }

        if((sl > 0 || sr > 0)  &&
           ((left_en   && proc->types.pointer.scroll_sum.x >=   scroll_limit) ||
            (right_en  && proc->types.pointer.scroll_sum.x <= - scroll_limit))) {
            obj_candidate = obj_act;
            dir_candidate = LV_DIR_HOR;
        }

        if(st <= 0) up_en = false;
        if(sb <= 0) down_en = false;
        if(sl <= 0) left_en = false;
        if(sr <= 0) right_en = false;

        /*If the object really can be scrolled into the current direction the use it.*/
        if((left_en  && proc->types.pointer.scroll_sum.x >=   scroll_limit) ||
           (right_en && proc->types.pointer.scroll_sum.x <= - scroll_limit) ||
           (up_en    && proc->types.pointer.scroll_sum.y >=   scroll_limit) ||
           (down_en  && proc->types.pointer.scroll_sum.y <= - scroll_limit)) {
            proc->types.pointer.scroll_dir = hor_en ? LV_DIR_HOR : LV_DIR_VER;
            break;
        }

        /*If this object don't want to chain the scroll to the parent stop searching*/
        if(lv_obj_has_flag(obj_act, LV_OBJ_FLAG_SCROLL_CHAIN_HOR) == false && hor_en) break;
        if(lv_obj_has_flag(obj_act, LV_OBJ_FLAG_SCROLL_CHAIN_VER) == false && ver_en) break;

        /*Try the parent*/
        #if LV_GDX_PATCH_GET_PARENT_DIRECTLY
        obj_act = obj_act->parent;
        #else
        /*optimized*/obj_act = lv_obj_get_parent(obj_act);
        #endif
    }

    /*Use the last candidate*/
    if(obj_candidate) {
        proc->types.pointer.scroll_dir = dir_candidate;
        proc->types.pointer.scroll_obj = obj_candidate;
        proc->types.pointer.scroll_sum.x = 0;
        proc->types.pointer.scroll_sum.y = 0;
    }

    return obj_candidate;
}
#endif // LV_GDX_PATCH_CUSTOM_SCROLL

static void init_scroll_limits(_lv_indev_proc_t * proc)
{
    lv_obj_t * obj = proc->types.pointer.scroll_obj;
    /*If there no STOP allow scrolling anywhere*/
    if(lv_obj_has_flag(obj, LV_OBJ_FLAG_SCROLL_ONE) == false) {
        lv_area_set(&proc->types.pointer.scroll_area, LV_COORD_MIN, LV_COORD_MIN, LV_COORD_MAX, LV_COORD_MAX);
    }
    /*With STOP limit the scrolling to the perv and next snap point*/
    else {
        switch(lv_obj_get_scroll_snap_y(obj)) {
            case LV_SCROLL_SNAP_START:
                proc->types.pointer.scroll_area.y1 = find_snap_point_y(obj, obj->coords.y1 + 1, LV_COORD_MAX, 0);
                proc->types.pointer.scroll_area.y2 = find_snap_point_y(obj, LV_COORD_MIN, obj->coords.y1 - 1, 0);
                break;
            case LV_SCROLL_SNAP_END:
                proc->types.pointer.scroll_area.y1 = find_snap_point_y(obj, obj->coords.y2, LV_COORD_MAX, 0);
                proc->types.pointer.scroll_area.y2 = find_snap_point_y(obj, LV_COORD_MIN, obj->coords.y2, 0);
                break;
            case LV_SCROLL_SNAP_CENTER: {
                    lv_coord_t y_mid = obj->coords.y1 + lv_area_get_height(&obj->coords) / 2;
                    proc->types.pointer.scroll_area.y1 = find_snap_point_y(obj, y_mid + 1, LV_COORD_MAX, 0);
                    proc->types.pointer.scroll_area.y2 = find_snap_point_y(obj, LV_COORD_MIN, y_mid - 1, 0);
                    break;
                }
            default:
                proc->types.pointer.scroll_area.y1 = LV_COORD_MIN;
                proc->types.pointer.scroll_area.y2 = LV_COORD_MAX;
                break;
        }

        switch(lv_obj_get_scroll_snap_x(obj)) {
            case LV_SCROLL_SNAP_START:
                proc->types.pointer.scroll_area.x1 = find_snap_point_x(obj, obj->coords.x1, LV_COORD_MAX, 0);
                proc->types.pointer.scroll_area.x2 = find_snap_point_x(obj, LV_COORD_MIN, obj->coords.x1, 0);
                break;
            case LV_SCROLL_SNAP_END:
                proc->types.pointer.scroll_area.x1 = find_snap_point_x(obj, obj->coords.x2, LV_COORD_MAX, 0);
                proc->types.pointer.scroll_area.x2 = find_snap_point_x(obj, LV_COORD_MIN, obj->coords.x2, 0);
                break;
            case LV_SCROLL_SNAP_CENTER: {
                    lv_coord_t x_mid = obj->coords.x1 + lv_area_get_width(&obj->coords) / 2;
                    proc->types.pointer.scroll_area.x1 = find_snap_point_x(obj, x_mid + 1, LV_COORD_MAX, 0);
                    proc->types.pointer.scroll_area.x2 = find_snap_point_x(obj, LV_COORD_MIN, x_mid - 1, 0);
                    break;
                }
            default:
                proc->types.pointer.scroll_area.x1 = LV_COORD_MIN;
                proc->types.pointer.scroll_area.x2 = LV_COORD_MAX;
                break;
        }
    }

    /*Allow scrolling on the edges. It will be reverted to the edge due to snapping anyway*/
    if(proc->types.pointer.scroll_area.x1 == 0) proc->types.pointer.scroll_area.x1 = LV_COORD_MIN;
    if(proc->types.pointer.scroll_area.x2 == 0) proc->types.pointer.scroll_area.x2 = LV_COORD_MAX;
    if(proc->types.pointer.scroll_area.y1 == 0) proc->types.pointer.scroll_area.y1 = LV_COORD_MIN;
    if(proc->types.pointer.scroll_area.y2 == 0) proc->types.pointer.scroll_area.y2 = LV_COORD_MAX;
}

/**
 * Search for snap point in the `min` - `max` range.
 * @param obj the object on which snap point should be found
 * @param min ignore snap points smaller than this. (Absolute coordinate)
 * @param max ignore snap points greater than this. (Absolute coordinate)
 * @param ofs offset to snap points. Useful the get a snap point in an imagined case
 *            what if children are already moved by this value
 * @return the distance of the snap point.
 */
static lv_coord_t find_snap_point_x(const lv_obj_t * obj, lv_coord_t min, lv_coord_t max, lv_coord_t ofs)
{
    lv_scroll_snap_t align = lv_obj_get_scroll_snap_x(obj);
    if(align == LV_SCROLL_SNAP_NONE) return 0;

    lv_coord_t dist = LV_COORD_MAX;

    lv_coord_t pad_left = lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
    lv_coord_t pad_right = lv_obj_get_style_pad_right(obj, LV_PART_MAIN);

    uint32_t i;
    uint32_t child_cnt = lv_obj_get_child_cnt(obj);
    for(i = 0; i < child_cnt; i++) {
        lv_obj_t * child = obj->spec_attr->children[i];
        if(lv_obj_has_flag_any(child, LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_FLOATING)) continue;
        if(lv_obj_has_flag(child, LV_OBJ_FLAG_SNAPPABLE)) {
            lv_coord_t x_child = 0;
            lv_coord_t x_parent = 0;
            switch(align) {
                case LV_SCROLL_SNAP_START:
                    x_child = child->coords.x1;
                    x_parent = obj->coords.x1 + pad_left;
                    break;
                case LV_SCROLL_SNAP_END:
                    x_child = child->coords.x2;
                    x_parent = obj->coords.x2 - pad_right;
                    break;
                case LV_SCROLL_SNAP_CENTER:
                    x_child = child->coords.x1 + lv_area_get_width(&child->coords) / 2;
                    x_parent = obj->coords.x1 + pad_left + (lv_area_get_width(&obj->coords) - pad_left - pad_right) / 2;
                    break;
                default:
                    continue;
            }

            x_child += ofs;
            if(x_child >= min && x_child <= max) {
                lv_coord_t x = x_child -  x_parent;
                if(LV_ABS(x) < LV_ABS(dist)) dist = x;
            }
        }
    }

    return dist == LV_COORD_MAX ? 0 : -dist;
}

/**
 * Search for snap point in the `min` - `max` range.
 * @param obj the object on which snap point should be found
 * @param min ignore snap points smaller than this. (Absolute coordinate)
 * @param max ignore snap points greater than this. (Absolute coordinate)
 * @param ofs offset to snap points. Useful to get a snap point in an imagined case
 *            what if children are already moved by this value
 * @return the distance of the snap point.
 */
static lv_coord_t find_snap_point_y(const lv_obj_t * obj, lv_coord_t min, lv_coord_t max, lv_coord_t ofs)
{
    lv_scroll_snap_t align = lv_obj_get_scroll_snap_y(obj);
    if(align == LV_SCROLL_SNAP_NONE) return 0;

    lv_coord_t dist = LV_COORD_MAX;

    lv_coord_t pad_top = lv_obj_get_style_pad_top(obj, LV_PART_MAIN);
    lv_coord_t pad_bottom = lv_obj_get_style_pad_bottom(obj, LV_PART_MAIN);

    uint32_t i;
    uint32_t child_cnt = lv_obj_get_child_cnt(obj);
    for(i = 0; i < child_cnt; i++) {
        lv_obj_t * child = obj->spec_attr->children[i];
        if(lv_obj_has_flag_any(child, LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_FLOATING)) continue;
        if(lv_obj_has_flag(child, LV_OBJ_FLAG_SNAPPABLE)) {
            lv_coord_t y_child = 0;
            lv_coord_t y_parent = 0;
            switch(align) {
                case LV_SCROLL_SNAP_START:
                    y_child = child->coords.y1;
                    y_parent = obj->coords.y1 + pad_top;
                    break;
                case LV_SCROLL_SNAP_END:
                    y_child = child->coords.y2;
                    y_parent = obj->coords.y2 - pad_bottom;
                    break;
                case LV_SCROLL_SNAP_CENTER:
                    y_child = child->coords.y1 + lv_area_get_height(&child->coords) / 2;
                    y_parent = obj->coords.y1 + pad_top + (lv_area_get_height(&obj->coords) - pad_top - pad_bottom) / 2;
                    break;
                default:
                    continue;
            }

            y_child += ofs;
            if(y_child >= min && y_child <= max) {
                lv_coord_t y = y_child -  y_parent;
                if(LV_ABS(y) < LV_ABS(dist)) dist = y;
            }
        }
    }

    return dist == LV_COORD_MAX ? 0 : -dist;
}

static void scroll_limit_diff(_lv_indev_proc_t * proc, lv_coord_t * diff_x, lv_coord_t * diff_y)
{
    if(diff_y) {
        if(proc->types.pointer.scroll_sum.y + *diff_y < proc->types.pointer.scroll_area.y1) {
            *diff_y = proc->types.pointer.scroll_area.y1 - proc->types.pointer.scroll_sum.y;
        }

        if(proc->types.pointer.scroll_sum.y + *diff_y > proc->types.pointer.scroll_area.y2) {
            *diff_y = proc->types.pointer.scroll_area.y2 - proc->types.pointer.scroll_sum.y;
        }
    }

    if(diff_x) {
        if(proc->types.pointer.scroll_sum.x + *diff_x < proc->types.pointer.scroll_area.x1) {
            *diff_x = proc->types.pointer.scroll_area.x1 - proc->types.pointer.scroll_sum.x;
        }

        if(proc->types.pointer.scroll_sum.x + *diff_x > proc->types.pointer.scroll_area.x2) {
            *diff_x = proc->types.pointer.scroll_area.x2 - proc->types.pointer.scroll_sum.x;
        }
    }
}



static lv_coord_t scroll_throw_predict_y(_lv_indev_proc_t * proc)
{
    lv_coord_t y = proc->types.pointer.scroll_throw_vect.y;
    lv_coord_t move = 0;

    lv_indev_t * indev_act = lv_indev_get_act();
    lv_coord_t scroll_throw = indev_act->driver->scroll_throw;

    while(y) {
        move += y;
        y = y * (100 - scroll_throw) / 100;
    }
    return move;
}


static lv_coord_t scroll_throw_predict_x(_lv_indev_proc_t * proc)
{
    lv_coord_t x = proc->types.pointer.scroll_throw_vect.x;
    lv_coord_t move = 0;

    lv_indev_t * indev_act = lv_indev_get_act();
    lv_coord_t scroll_throw = indev_act->driver->scroll_throw;

    while(x) {
        move += x;
        x = x * (100 - scroll_throw) / 100;
    }
    return move;
}

static lv_coord_t elastic_diff(lv_obj_t * scroll_obj, lv_coord_t diff, lv_coord_t scroll_start, lv_coord_t scroll_end,
                               lv_dir_t dir)
{
    if(lv_obj_has_flag(scroll_obj, LV_OBJ_FLAG_SCROLL_ELASTIC)) {
        /*If there is snapping in the current direction don't use the elastic factor because
         *it's natural that the first and last items are scrolled (snapped) in.*/
        lv_scroll_snap_t snap;
        snap = dir == LV_DIR_HOR ? lv_obj_get_scroll_snap_x(scroll_obj) : lv_obj_get_scroll_snap_y(scroll_obj);

        lv_obj_t * act_obj = lv_indev_get_obj_act();
        lv_coord_t snap_point = 0;
        lv_coord_t act_obj_point = 0;

        if(dir == LV_DIR_HOR) {
            lv_coord_t pad_left = lv_obj_get_style_pad_left(scroll_obj, LV_PART_MAIN);
            lv_coord_t pad_right = lv_obj_get_style_pad_right(scroll_obj, LV_PART_MAIN);

            switch(snap) {
                case LV_SCROLL_SNAP_CENTER:
                    snap_point = pad_left + (lv_area_get_width(&scroll_obj->coords) - pad_left - pad_right) / 2 + scroll_obj->coords.x1;
                    act_obj_point = lv_area_get_width(&act_obj->coords) / 2 + act_obj->coords.x1;
                    break;
                case LV_SCROLL_SNAP_START:
                    snap_point = scroll_obj->coords.x1 + pad_left;
                    act_obj_point = act_obj->coords.x1;
                    break;
                case LV_SCROLL_SNAP_END:
                    snap_point = scroll_obj->coords.x2 - pad_right;
                    act_obj_point = act_obj->coords.x2;
                    break;
            }
        }
        else {
            lv_coord_t pad_top = lv_obj_get_style_pad_top(scroll_obj, LV_PART_MAIN);
            lv_coord_t pad_bottom = lv_obj_get_style_pad_bottom(scroll_obj, LV_PART_MAIN);

            switch(snap) {
                case LV_SCROLL_SNAP_CENTER:
                    snap_point = pad_top + (lv_area_get_height(&scroll_obj->coords) - pad_top - pad_bottom) / 2 + scroll_obj->coords.y1;
                    act_obj_point = lv_area_get_height(&act_obj->coords) / 2 + act_obj->coords.y1;
                    break;
                case LV_SCROLL_SNAP_START:
                    snap_point = scroll_obj->coords.y1 + pad_top;
                    act_obj_point = act_obj->coords.y1;
                    break;
                case LV_SCROLL_SNAP_END:
                    snap_point = scroll_obj->coords.y2 - pad_bottom;
                    act_obj_point = act_obj->coords.y2;
                    break;
            }
        }

        if(scroll_end < 0) {
            if(snap != LV_SCROLL_SNAP_NONE && act_obj_point > snap_point) return diff;

            /*Rounding*/
            if(diff < 0) diff -= ELASTIC_SLOWNESS_FACTOR / 2;
            if(diff > 0) diff += ELASTIC_SLOWNESS_FACTOR / 2;
            return diff / ELASTIC_SLOWNESS_FACTOR;
        }
        else if(scroll_start < 0) {
            if(snap != LV_SCROLL_SNAP_NONE && act_obj_point < snap_point) return diff;

            /*Rounding*/
            if(diff < 0) diff -= ELASTIC_SLOWNESS_FACTOR / 2;
            if(diff > 0) diff += ELASTIC_SLOWNESS_FACTOR / 2;
            return diff / ELASTIC_SLOWNESS_FACTOR;
        }
    }
    else {
        /*Scroll back to the boundary if required*/
        if(scroll_end + diff < 0) diff = - scroll_end;
        if(scroll_start - diff < 0) diff = scroll_start;
    }

    return diff;
}


