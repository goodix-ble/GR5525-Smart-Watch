/**
 * @file lv_virtual_list.c
 *
 * 虚拟列表，在滚动时即时获取数据并渲染。
 * 用于显示大量的数据。例如数据条目超过50条的情况。
 * 只支持垂直方向上的滚动。
 *
 * holder是一个开发者自定义的结构体，里面的字段主要用于引用对应数据项视图中控件。也可以缓存数据，避免重复设置控件中的内容。
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_virtual_list.h"

#if LV_USE_VIRTUAL_LIST && LV_GDX_PATCH_CUSTOM_SCROLL

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &lv_virtual_list_class

/**********************
 *      TYPEDEFS
 **********************/

typedef struct
{
    lv_obj_t *obj;
    uint32_t data_item_idx;
    int32_t item_space_top;
    uint8_t *p_data;
} _holder_header_t;

typedef struct
{
    lv_obj_t obj;
    lv_virtual_list_create_item_widget_cb p_create_cb;
    lv_virtual_list_update_item_widget_cb p_config_cb;
    lv_virtual_list_item_clicked_cb p_clicked_cb;

    void *user_data; // 携带额外的数据，这样可以避免使用全局变量来串联页面和该控件的回调函数

    uint8_t *p_holder_data_pool; // 存储holder数据的内存池
    _holder_header_t *p_holder_list;
    uint16_t holder_data_size;     // holder的数据域的大小
    uint16_t holder_list_size;     // holder的个数
    uint16_t holder_list_capacity; // holder列表的容量

    uint32_t data_item_cnt; // 需要显示的数据的个数

    int32_t scroll_top;             // 顶部滚动到哪里了
    int32_t data_item_total_height; // 避免频繁计算展示所有数据项需要的高度

    lv_coord_t data_item_height; // 数据项的高度，通过固定等高的方式，避免经常去获取高度
    lv_coord_t space_top;        // 顶部留白的空间
    lv_coord_t space_bottom;     // 底部留白的空间
    lv_coord_t space_item;       // 列表项之间的间隔
    bool has_setup;              // false表示还没有准备好待绘制的数据项
    bool is_animating;           // 如果正在执行动画，就立即停止动画
} lv_virtual_list_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_virtual_list_constructor_cb(const lv_obj_class_t *class_p, lv_obj_t *obj);
static void lv_virtual_list_destructor_cb(const lv_obj_class_t *class_p, lv_obj_t *obj);
static void lv_virtual_list_event(const lv_obj_class_t *class_p, lv_event_t *evt);
static void _scroll_by(lv_virtual_list_t *p_this, int32_t delta_y);
// static void _setup_item_view_firstly(lv_virtual_list_t *p_this);
static void _update_item_view(lv_virtual_list_t *p_this);
static void _reload_item_view(lv_virtual_list_t *p_this);
static bool _holder_ensure_capacity(lv_virtual_list_t *p_this, uint32_t required_cnt);
static bool _holder_delete_at(lv_virtual_list_t *p_this, uint32_t idx);
static _holder_header_t *_holder_alloc(lv_virtual_list_t *p_this);
static void _holder_dispose(lv_virtual_list_t *p_this);
static void _scroll_end(lv_virtual_list_t *p_this, int32_t throw_y);
static void _scroll_anim_cb(void *obj, int32_t v);
static void _deliver_click_event(lv_virtual_list_t *p_this, lv_event_t *evt);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_virtual_list_class = {
    .constructor_cb = lv_virtual_list_constructor_cb,
    .destructor_cb = lv_virtual_list_destructor_cb,
    .event_cb = lv_virtual_list_event,
    .width_def = LV_SIZE_CONTENT,
    .height_def = LV_SIZE_CONTENT,
    .instance_size = sizeof(lv_virtual_list_t),
    .base_class = &lv_obj_class,
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t *lv_virtual_list_create(lv_obj_t *parent,
                                 lv_coord_t item_height,
                                 uint16_t widget_holder_size,
                                 lv_virtual_list_create_item_widget_cb p_create_cb,
                                 lv_virtual_list_update_item_widget_cb p_config_cb)
{
    LV_LOG_INFO("begin");
    lv_obj_t *obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);

    lv_virtual_list_t *p_this = (lv_virtual_list_t *)obj;
    p_this->data_item_height = item_height;
    p_this->holder_data_size = widget_holder_size;
    p_this->p_create_cb = p_create_cb;
    p_this->p_config_cb = p_config_cb;

    p_this->holder_list_capacity = 0;
    p_this->p_holder_list = NULL;
    p_this->holder_list_size = 0;
    p_this->has_setup = false;

    return obj;
}

void lv_virtual_list_update_data(lv_obj_t *obj, void *user_data, uint32_t data_item_cnt)
{
    lv_virtual_list_t *p_this = (lv_virtual_list_t *)obj;
    p_this->user_data = user_data;
    p_this->data_item_cnt = data_item_cnt;
    if (data_item_cnt > 0)
    {
        p_this->data_item_total_height = data_item_cnt * p_this->data_item_height + (data_item_cnt - 1) * p_this->space_item;
    }
    else
    {
        p_this->data_item_total_height = 0;
    }

    p_this->has_setup = false; // postpone call
    lv_obj_invalidate(obj);
}

void lv_virtual_list_set_padding_space(lv_obj_t *obj, lv_coord_t top, lv_coord_t bottom, lv_coord_t gap_between_items)
{
    lv_virtual_list_t *p_this = (lv_virtual_list_t *)obj;
    p_this->space_top = top;
    p_this->space_bottom = bottom;
    p_this->space_item = gap_between_items;
}

void lv_virtual_list_set_item_clicked_cb(lv_obj_t *obj, lv_virtual_list_item_clicked_cb click_cb)
{
    lv_virtual_list_t *p_this = (lv_virtual_list_t *)obj;
    p_this->p_clicked_cb = click_cb;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_virtual_list_constructor_cb(const lv_obj_class_t *class_p, lv_obj_t *obj)
{
    LV_UNUSED(class_p);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CUSTOM);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
}

static void lv_virtual_list_destructor_cb(const lv_obj_class_t *class_p, lv_obj_t *obj)
{
    LV_UNUSED(class_p);
    // 释放分配的Holder
    lv_virtual_list_t *p_this = (lv_virtual_list_t *)obj;
    _holder_dispose(p_this);
}

static void lv_virtual_list_event(const lv_obj_class_t *class_p, lv_event_t *evt)
{
    lv_obj_t *obj = evt->current_target;
    lv_event_code_t code = evt->code;
    lv_virtual_list_t *p_this = (lv_virtual_list_t *)obj;

    if (code == LV_EVENT_PROBE_CUSTOM_SCROLL)
    {
        lv_point_t *scroll_distance = (lv_point_t *)lv_event_get_param(evt);
        if (scroll_distance->x != 0)
        {
            scroll_distance->x = 0; // 不需要消耗水平反向上的滚动
        }
    }
    else if (code == LV_EVENT_PRESSED)
    {
        if (p_this->is_animating)
        {
            if (lv_anim_del(obj, _scroll_anim_cb))
            {
                p_this->is_animating = false;
                // TODO 如果卡在钳位上了怎么办？速度太快了，很难点中。
            }
        }
    }
    else if (code == LV_EVENT_SCROLL_BEGIN)
    {
    }
    else if (code == LV_EVENT_SCROLL_END)
    {
        _lv_indev_proc_t *proc = (_lv_indev_proc_t *)lv_event_get_param(evt);
        // int delta_distance = (int)proc->types.pointer.scroll_sum.y;
        _scroll_end(p_this, proc->types.pointer.scroll_throw_vect.y);
    }
    else if (code == LV_EVENT_SCROLL)
    {
        if (!p_this->is_animating)
        {
            _lv_indev_proc_t *proc = (_lv_indev_proc_t *)lv_event_get_param(evt);
            int32_t delta_y = (int32_t)proc->types.pointer.vect.y;
            // 根据滚动位置计算弹性，要实现弹性效果，还需要与动画效果联动才行。
            if (p_this->scroll_top > p_this->space_top)
            {
                delta_y /= 8;
            }
            else
            {
                int32_t scroll_bottom = p_this->scroll_top + p_this->data_item_total_height + p_this->space_bottom;
                int32_t height = lv_area_get_height(&p_this->obj.coords);
                if (scroll_bottom < height)
                {
                    delta_y /= 8;
                }
            }
            _scroll_by(p_this, delta_y);
        }
    }
    else
    {
        if (!p_this->has_setup && code == LV_EVENT_DRAW_MAIN)
        {
            //_setup_item_view_firstly(p_this);
            _reload_item_view(p_this);
            p_this->has_setup = true;
        }
        else
        {
            if (p_this->p_clicked_cb != NULL)
            {
                // if (code == LV_EVENT_SHORT_CLICKED || code == LV_EVENT_LONG_PRESSED || code == LV_EVENT_LONG_PRESSED_REPEAT || code == LV_EVENT_CLICKED)
                if (code >= LV_EVENT_SHORT_CLICKED && code <= LV_EVENT_CLICKED) // 4..7
                {
                    // 有动画时不处理点击事件
                    if (!p_this->is_animating)
                    {
                        // 终止冒泡
                        evt->stop_bubbling = true;
                        _deliver_click_event(p_this, evt);
                    }
                    return;
                }
            }
        }

        lv_obj_class.event_cb(class_p, evt);
    }
}

static void _scroll_by(lv_virtual_list_t *p_this, int32_t delta_y)
{
    lv_coord_t data_item_space = p_this->space_item + p_this->data_item_height;
    _holder_header_t *p_holder_list = p_this->p_holder_list;
    int32_t height = (int32_t)lv_area_get_height(&p_this->obj.coords);
    int32_t pos_y;

    if (delta_y != 0)
    {
        p_this->scroll_top += delta_y;
        for (int i = 0; i < p_this->holder_list_size; i++)
        {
            _holder_header_t *p_holder_header = p_holder_list + i;
            // 先移动所有子控件
            pos_y = p_holder_header->item_space_top + delta_y;
            // 保存最新的坐标
            p_holder_header->item_space_top = pos_y;
            lv_obj_set_y(p_holder_header->obj, pos_y);
            // 再判断是否隐藏
            if (pos_y > height || (pos_y + data_item_space) < 0)
            {
                // printf("-----> recycle: %2d, index:%4d, top:%03d\n", i, p_holder_header->data_item_idx, pos_y);
                //  已超出显示区域
                //  回收
                _holder_delete_at(p_this, i);
                i--; // 因为处于遍历中，为了避开遍历冲突
            }
        }
    }
    else
    {
        // 仅判断是否有需要隐藏的
        for (int i = 0; i < p_this->holder_list_size; i++)
        {
            _holder_header_t *p_holder_header = p_holder_list + i;
            pos_y = p_holder_header->item_space_top;
            if (pos_y > height || (pos_y + data_item_space) < 0)
            {
                // printf("---$$> recycle: %2d, index:%4d\n", i, p_holder_header->data_item_idx);
                //  已超出显示区域
                //  回收
                _holder_delete_at(p_this, i);
                i--; // 因为处于遍历中，为了避开遍历冲突
            }
        }
    }

    _update_item_view(p_this);

    lv_obj_invalidate(&p_this->obj);
}

static void _scroll_anim_cb(void *obj, int32_t v)
{
    lv_virtual_list_t *p_this = (lv_virtual_list_t *)obj;
    int32_t delta = v - p_this->scroll_top;
    _scroll_by(p_this, delta);
}

static void _scroll_anim_ready_cb(lv_anim_t *a)
{
    lv_virtual_list_t *p_this = (lv_virtual_list_t *)a->var;
    p_this->is_animating = false;
}

static void _scroll_end(lv_virtual_list_t *p_this, int32_t throw_y)
{
    int32_t scroll_top = p_this->scroll_top;
    int32_t scroll_bottom = scroll_top + p_this->data_item_total_height + p_this->space_bottom;
    int32_t height = (int32_t)lv_area_get_height(&p_this->obj.coords);

    // 计算抛射
    int32_t scroll_top_final = scroll_top;
    int32_t scroll_bottom_final = scroll_bottom;
    if (throw_y != 0)
    {
        lv_indev_t *indev_act = lv_indev_get_act();
        int32_t scroll_throw = (int32_t)indev_act->driver->scroll_throw;
        while (throw_y != 0)
        {
            scroll_top_final += throw_y;
            throw_y = throw_y * (100 - scroll_throw) / 100;
        }

        scroll_bottom_final = scroll_top_final + p_this->data_item_total_height + p_this->space_bottom;
    }
    // 钳位
    if (scroll_bottom_final < height)
    {
        // 只需要向下回弹归位
        scroll_top_final = height - p_this->data_item_total_height - p_this->space_bottom;
    }
    if (scroll_top_final > p_this->space_top)
    {
        // 只需要向上回弹归位
        scroll_top_final = p_this->space_top;
    }

    if (scroll_top != scroll_top_final)
    {
        uint32_t t = lv_anim_speed_to_time((lv_disp_get_hor_res(lv_disp_get_default()) * 2) >> 2, scroll_top, scroll_top_final);
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, p_this);
        lv_anim_set_time(&a, t);
        lv_anim_set_values(&a, scroll_top, scroll_top_final);
        lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
        lv_anim_set_exec_cb(&a, _scroll_anim_cb);
        lv_anim_set_ready_cb(&a, _scroll_anim_ready_cb);
        p_this->is_animating = true;
        lv_anim_start(&a);
    }
}

static void _update_item_view(lv_virtual_list_t *p_this)
{
    int32_t distance = -p_this->scroll_top;
    int32_t data_item_height = (int32_t)p_this->data_item_height;
    lv_coord_t data_item_space = data_item_height + p_this->space_item;
    int32_t first_visible_item_idx = 0;
    if (distance > 0)
    {
        first_visible_item_idx = distance / data_item_space;
    }

    // 判断一个视图内可以显示多少个数据项
    int32_t content_height = (int32_t)lv_obj_get_height(&p_this->obj);
    lv_coord_t content_width = lv_obj_get_width(&p_this->obj);
    if (content_height < 1)
    {
        content_height = 1;
    }

    int32_t visible_item_cnt = (content_height + data_item_space - 1) / data_item_space;
    uint32_t data_item_cnt = p_this->data_item_cnt;

    if (visible_item_cnt > data_item_cnt)
    {
        visible_item_cnt = data_item_cnt;
    }

    // 准备足量的holder
    if (p_this->holder_list_capacity < visible_item_cnt)
    {
        _holder_ensure_capacity(p_this, visible_item_cnt);
    }

    // 统计出已经处于显示状态的项的序号区间
    _holder_header_t *p_holder_list = p_this->p_holder_list;
    uint32_t max_idx = 0;
    uint32_t min_idx = 0xFFFFFFFF;
    bool update_all = false;
    if (p_this->holder_list_size == 0)
    {
        update_all = true;
    }
    else
    {
        for (int i = 0; i < p_this->holder_list_size; i++)
        {
            _holder_header_t *p_holder_header = p_holder_list + i;
            if (p_holder_header->data_item_idx < min_idx)
            {
                min_idx = p_holder_header->data_item_idx;
            }
            if (p_holder_header->data_item_idx > max_idx)
            {
                max_idx = p_holder_header->data_item_idx;
            }
        }
    }

    // 开始依次绑定
    uint32_t idx = first_visible_item_idx;
    lv_coord_t pos_y = (lv_coord_t)(p_this->scroll_top + (int32_t)(first_visible_item_idx * (int32_t)data_item_space));
    lv_coord_t bottom_y = (lv_coord_t)content_height;
    uint32_t child_cnt = lv_obj_get_child_cnt(&p_this->obj);
    // printf("scroll_top:%4d, idx:%2d, min:%2d, max:%2d\n", p_this->scroll_top, idx, min_idx, max_idx);
    //  不再使用visible_item_cnt，要以是否还能放得下可显示的数据项为准
    for (uint32_t i = 0; pos_y < bottom_y && idx < data_item_cnt; i++)
    {
        // 如果已经处于显示状态了，就跳过绑定
        // 没有处于显示状态的数据项才进行绑定
        if (idx < min_idx || idx > max_idx || update_all)
        {
            // 先获取holder，
            _holder_header_t *p_holder_header = _holder_alloc(p_this);
            p_holder_header->data_item_idx = idx;
            // 因为是复用的，所以，每个holder都会有对应的obj
            if (p_holder_header->obj == NULL)
            {
                p_holder_header->obj = p_this->p_create_cb(&p_this->obj, p_this->user_data, p_holder_header->p_data);
                // 因为马上就要用到了，就不为新创建的控件设置坐标了。
                lv_obj_set_height(p_holder_header->obj, data_item_height); // 不限制宽度
            }

            // 配置view的位置和宽高
            lv_obj_set_pos(p_holder_header->obj, 0, pos_y);
            p_holder_header->item_space_top = pos_y;
            // printf(">>  obj:0x%08X index:%2d, pos_y: %d\n", p_holder_header->obj, p_holder_header->data_item_idx, (lv_coord_t)pos_y);

            // 最后绑定数据
            p_this->p_config_cb(&p_this->obj, p_this->user_data, p_holder_header->p_data, idx, p_holder_header->obj, true);
        }

        idx++;
        pos_y += data_item_space;
    }
}

static void _reload_item_view(lv_virtual_list_t *p_this)
{
    // 钳位scroll top
    int32_t scroll_top = p_this->scroll_top;
    int32_t height = (int32_t)lv_area_get_height(&p_this->obj.coords);
    int32_t scroll_bottom = p_this->scroll_top + p_this->data_item_total_height + p_this->space_bottom;
    if (scroll_bottom < height)
    {
        scroll_top = height - p_this->data_item_total_height; // 因为是突然刷新，所以可以不包含p_this->space_bottom;
    }
    if (scroll_top > p_this->space_top)
    {
        scroll_top = p_this->space_top;
    }
    p_this->scroll_top = scroll_top;

    // 计算要显示的第一个数据项
    lv_coord_t data_item_height = p_this->data_item_height;
    lv_coord_t data_item_space = data_item_height + p_this->space_item;
    uint32_t first_visible_item_idx = 0;
    if (scroll_top < 0)
    {
        first_visible_item_idx = (-scroll_top) / data_item_space;
    }

    // 判断一个视图内可以显示多少个数据项
    lv_coord_t content_height = lv_obj_get_height(&p_this->obj);
    lv_coord_t content_width = lv_obj_get_width(&p_this->obj);
    if (content_height < 1)
    {
        content_height = 1;
    }
    uint32_t visible_item_cnt = (uint32_t)((content_height + data_item_space - 1) / data_item_space);
    uint32_t data_item_cnt = p_this->data_item_cnt;
    if (visible_item_cnt > data_item_cnt)
    {
        // 限制要缓存数量
        visible_item_cnt = data_item_cnt;
    }
    ////////////////////////////////////////////////////////////////////////////
    // 如果要复用已经创建的View，不能直接丢弃，因为有holder
    // 如果holder数量不足，就增加
    // 如果holder过多，就需要删除多余的
    uint32_t holder_list_capacity = (uint32_t)p_this->holder_list_capacity;
    if (holder_list_capacity < visible_item_cnt)
    {
        _holder_ensure_capacity(p_this, visible_item_cnt);
    }
    else if (holder_list_capacity > visible_item_cnt)
    {
        for (uint32_t i = visible_item_cnt; i < holder_list_capacity; i++)
        {
            _holder_header_t *p_holder_header = p_this->p_holder_list + i;
            lv_obj_del(p_holder_header->obj); // 删除多余的子控件
            p_holder_header->obj = NULL;
        }
        p_this->holder_list_capacity = (uint16_t)visible_item_cnt;
    }
    p_this->holder_list_size = visible_item_cnt; // use all holders

    // 根据坐标顺序，重新排序holder，避免闪烁
    _holder_header_t sort_tmp;
    for (uint32_t k = 0; k < visible_item_cnt; k++)
    {
        _holder_header_t *p_base = p_this->p_holder_list + k;
        for (uint32_t i = k + 1; i < visible_item_cnt; i++)
        {
            _holder_header_t *p_cursor = p_this->p_holder_list + i;
            if (p_cursor->obj != NULL)
            {
                // swap
                if (p_base->obj == NULL || p_cursor->item_space_top < p_base->item_space_top)
                {
                    sort_tmp = *p_base;
                    *p_base = *p_cursor;
                    *p_cursor = sort_tmp;
                }
            }
        }
    }

    // 开始依次绑定
    uint32_t idx = first_visible_item_idx;
    lv_coord_t pos_y = (lv_coord_t)(p_this->scroll_top + (int32_t)(first_visible_item_idx * (uint32_t)data_item_space));
    for (uint32_t i = 0; i < visible_item_cnt; i++)
    {
        // 先获取holder
        _holder_header_t *p_holder_header = p_this->p_holder_list + i;
        // 再获取view，一般可以复用子控件
        if (p_holder_header->obj == NULL)
        {
            // 如果没有就通过建造函数创建
            p_holder_header->obj = p_this->p_create_cb(&p_this->obj, p_this->user_data, p_holder_header->p_data);
        }
        p_holder_header->data_item_idx = idx;
        // 配置view的位置和宽高
        lv_obj_set_pos(p_holder_header->obj, 0, pos_y);
        p_holder_header->item_space_top = pos_y;
        lv_obj_set_height(p_holder_header->obj, data_item_height);

        // 最后绑定数据
        p_this->p_config_cb(&p_this->obj, p_this->user_data, p_holder_header->p_data, idx, p_holder_header->obj, true);

        idx++;
        pos_y += data_item_space;
    }
}

static void _deliver_click_event(lv_virtual_list_t *p_this, lv_event_t *evt)
{
    // 找到直接数据项的根控件
    lv_obj_t *cur = evt->target;
    lv_obj_t *parent;
    lv_obj_t *item_widget_root = NULL;
    while (true)
    {
        parent = cur->parent;
        if (parent == NULL)
        {
            break;
        }

        if (parent->class_p == &lv_virtual_list_class)
        {
            item_widget_root = cur;
            break;
        }
        cur = parent;
    }

    if (item_widget_root != NULL)
    {
        // 找到对应控件的位置
        _holder_header_t *p_holder_list = p_this->p_holder_list;
        for (int i = 0; i < p_this->holder_list_size; i++)
        {
            _holder_header_t *p_holder_header = p_holder_list + i;
            if (item_widget_root == p_holder_header->obj)
            {
                // 传递事件给回调
                p_this->p_clicked_cb(&p_this->obj, p_this->user_data, p_holder_header->p_data, p_holder_header->data_item_idx, evt);
            }
        }
    }
}

static bool _holder_ensure_capacity(lv_virtual_list_t *p_this, uint32_t required_cnt)
{
    if (p_this->holder_list_capacity < required_cnt)
    {
        size_t holder_data_size = p_this->holder_data_size;
        // 先分配出一段连续的空间，确保分配过程的完整性。如果分两次分配，有可能失败一次。
        size_t new_holder_mem_size = required_cnt * sizeof(_holder_header_t) + required_cnt * holder_data_size;
        uint8_t *p_tmp = lv_mem_alloc(new_holder_mem_size);
        if (p_tmp == NULL)
        {
            return false;
        }
        lv_memset_00(p_tmp, new_holder_mem_size);

        // 命名新数据
        _holder_header_t *p_holder_list_new = (_holder_header_t *)p_tmp;
        uint8_t *p_holder_data_pool_new = &p_tmp[required_cnt * sizeof(_holder_header_t)];
        // 命名旧数据
        _holder_header_t *p_holder_list_old = p_this->p_holder_list;
        // uint8_t *p_holder_data_pool_old = p_this->p_holder_data_pool;

        // 为新数据分配好内存池
        uint8_t *pt = p_holder_data_pool_new;
        for (int i = 0; i < required_cnt; i++)
        {
            p_holder_list_new[i].p_data = pt;
            pt += holder_data_size;
        }

        // 拷贝旧数据
        if (p_holder_list_old != NULL)
        {
            // 拷贝
            for (int i = 0; i < p_this->holder_list_size; i++)
            {
                // 先拷贝表头
                p_holder_list_new[i].obj = p_holder_list_old[i].obj;
                p_holder_list_new[i].data_item_idx = p_holder_list_old[i].data_item_idx;
                p_holder_list_new[i].item_space_top = p_holder_list_old[i].item_space_top;
                // 再拷贝数据
                lv_memcpy(p_holder_list_new[i].p_data, p_holder_list_old[i].p_data, holder_data_size);
            }
            // 最后释放旧数据
            lv_mem_free(p_holder_list_old);
        }

        p_this->p_holder_list = p_holder_list_new;
        p_this->p_holder_data_pool = p_holder_data_pool_new;
        p_this->holder_list_capacity = required_cnt;
    }
    return true;
}

static bool _holder_delete_at(lv_virtual_list_t *p_this, uint32_t idx)
{
    if (idx < p_this->holder_list_size)
    {
        if (p_this->holder_list_size > 1)
        {
            // 将要删除的holder调换到最后的位置上
            uint32_t last_idx = p_this->holder_list_size - 1;
            _holder_header_t mid = p_this->p_holder_list[idx];
            p_this->p_holder_list[idx] = p_this->p_holder_list[last_idx];
            p_this->p_holder_list[last_idx] = mid;
        }
        // else
        //{
        //     // 直接标记为删除
        // }
        p_this->holder_list_size--;
        return true;
    }
    return false;
}

static _holder_header_t *_holder_alloc(lv_virtual_list_t *p_this)
{
    uint16_t holder_list_size = p_this->holder_list_size;
    if (holder_list_size >= p_this->holder_list_capacity)
    {
        if (!_holder_ensure_capacity(p_this, holder_list_size + 1))
        {
            return NULL;
        }
    }
    _holder_header_t *pt = &(p_this->p_holder_list[holder_list_size]);
    p_this->holder_list_size++;
    return pt;
}

static void _holder_dispose(lv_virtual_list_t *p_this)
{
    if (p_this->p_holder_list != NULL)
    {
        lv_mem_free(p_this->p_holder_list);
        p_this->p_holder_list = NULL;
        p_this->p_holder_data_pool = NULL;
        p_this->holder_list_size = 0;
        p_this->holder_list_capacity = 0;
    }
}

#endif // LV_USE_VIRTUAL_LIST && LV_GDX_PATCH_CUSTOM_SCROLL
