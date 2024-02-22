/**
 * @file lv_img_gximg.c
 * 
 * 专门绘制GX IMG格式的图片，主要用于显示表盘指针。
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_img_gximg.h"


#if LV_GDX_PATCH_GX_IMG > 0u
#include "lv_port_gximg.h"
#endif // LV_GDX_PATCH_GX_IMG > 0u

/*********************
 *      DEFINES
 *********************/
typedef struct {
    lv_obj_t obj;
    const void * src;       /*Image source: Pointer to an array or a file or a symbol*/
    const lv_gximg_dsc_t *p_cur_img; /*current showing image*/
    lv_area_t img_area;/*draw current showing image to this area, relative coords*/
    lv_point_t pivot;       /*rotation center of the image*/
    uint16_t angle;         /*rotation angle of the image*/
    uint16_t rsc_idx;       /*gximg index is corresponding to angle*/
    bool flip_x;
    bool flip_y;
} lv_img_gximg_t;

#define MY_CLASS &lv_img_gximg_class

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_img_gximg_event(const lv_obj_class_t *class_p, lv_event_t *evt);
static void _udpate_angle(lv_img_gximg_t * p_this);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_img_gximg_class = {
    .constructor_cb = NULL,
    .destructor_cb = NULL,
    .event_cb = lv_img_gximg_event,
    .width_def = 2,
    .height_def = 2,
    .instance_size = sizeof(lv_img_gximg_t),
    // .base_class = &lv_img_class,
    .base_class = &lv_obj_class,
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t *lv_img_gximg_create(lv_obj_t *parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t *obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

void lv_img_gximg_set_pivot_and_src(lv_obj_t *obj, lv_coord_t center_x, lv_coord_t center_y, const void* gximg_src)
{
    lv_img_gximg_t * p_this = (lv_img_gximg_t *)obj;

    // 中心点的相对坐标
    p_this->pivot.x = center_x;
    p_this->pivot.y = center_y;

    if (p_this->src != NULL)
    {
        lv_area_t tmp = p_this->img_area;
        tmp.x1 += obj->coords.x1;
        tmp.y1 += obj->coords.y1;
        tmp.x2 += obj->coords.x1;
        tmp.y2 += obj->coords.y1;
        lv_obj_invalidate_area(obj, &tmp);
    }

    p_this->src = gximg_src;

    if (gximg_src != NULL) {
        lv_gximg_bundle_dsc_t *p_bundle = (lv_gximg_bundle_dsc_t *)gximg_src;
        const lv_gximg_dsc_t *p_gximg = p_bundle->group[0];
        if (p_bundle->header.cf == LV_IMG_CF_GDX_GXIMG_BUNDLE) {
            lv_img_gximg_set_angle(obj, 0);
        }
        else {
            p_gximg = (lv_gximg_dsc_t *)gximg_src;
            // 中心点对其后计算图片的贴图区域
            lv_area_t blit_area;
            blit_area.x1 = center_x - p_gximg->center_x;
            blit_area.y1 = center_y - p_gximg->center_y;
            blit_area.x2 = blit_area.x1 + p_gximg->header.w - 1;
            blit_area.y2 = blit_area.y1 + p_gximg->header.h - 1;
            p_this->img_area = blit_area;
            p_this->p_cur_img = p_gximg;
            // 其实不需要去更新，统一更新
            // blit_area.x1 += obj->coords.x1;
            // blit_area.y1 += obj->coords.y1;
            // blit_area.x2 += obj->coords.x1;
            // blit_area.y2 += obj->coords.y1;
            // lv_obj_invalidate_area(obj, &blit_area);
        }
    }
}

void lv_img_gximg_set_angle(lv_obj_t * obj, int16_t angle)
{
    lv_img_gximg_t * p_this = (lv_img_gximg_t *)obj;
    lv_gximg_bundle_dsc_t *bundle = (lv_gximg_bundle_dsc_t *)p_this->src;

    if (bundle == NULL || bundle->header.cf != LV_IMG_CF_GDX_GXIMG_BUNDLE) {
        p_this->angle = angle;
        return;
    }

    if(angle < 0 || angle >= 3600) angle = angle % 3600;
    if(angle == p_this->angle) return;

    // 设置旧区域为脏区
    lv_area_t tmp = p_this->img_area;
    tmp.x1 += obj->coords.x1;
    tmp.y1 += obj->coords.y1;
    tmp.x2 += obj->coords.x1;
    tmp.y2 += obj->coords.y1;
    lv_obj_invalidate_area(obj, &tmp);

    p_this->angle = angle;
    _udpate_angle(p_this);

    // 设置新图片所占用的区域为脏区
    tmp = p_this->img_area;
    tmp.x1 += obj->coords.x1;
    tmp.y1 += obj->coords.y1;
    tmp.x2 += obj->coords.x1;
    tmp.y2 += obj->coords.y1;
    lv_obj_invalidate_area(obj, &tmp);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void _udpate_angle(lv_img_gximg_t * p_this)
{
    uint16_t angle = (3600 - p_this->angle) % 3600;
    uint8_t quadrant = angle / 900 + 1;
    uint8_t rsc_idx = (angle % 900) / 60;
    angle = rsc_idx * 60;

    bool flip_x = false;
    bool flip_y = false;

    switch (quadrant)
    {
        case 1:
            break;

        case 2:
            rsc_idx = 15 - rsc_idx;
            flip_x = true;
            break;

        case 3:
            flip_x = true;
            flip_y = true;
            break;

        case 4:
            rsc_idx = 15 - rsc_idx;
            flip_y = true;
            break;
    }

    if (angle == 900 || angle == 2700)
    {
        rsc_idx = 15;
        flip_x = false;
    }

    lv_gximg_bundle_dsc_t *bundle = (lv_gximg_bundle_dsc_t *)p_this->src;
    const lv_gximg_dsc_t *p_rsc = bundle->group[rsc_idx];
    p_this->p_cur_img = p_rsc;
    // 计算变换后的中心点
    lv_coord_t px = p_rsc->center_x;
    lv_coord_t py = p_rsc->center_y;
    if (flip_x) {
        px = p_rsc->header.w - px;
    }
    if (flip_y) {
        py = p_rsc->header.h - py;
    }
    // 根据中心点计算需要将图片偏移多少距离后，才能将图片中的中心点与控件的旋转中心点对齐
    lv_coord_t px_offset = p_this->pivot.x - px;
    lv_coord_t py_offset = p_this->pivot.y - py;
    // 得到图片在控件中占用的区域。绘制时还需要转换为屏幕上的区域
    lv_area_t * p_img_area = &p_this->img_area;
    p_img_area->x1 = px_offset;
    p_img_area->y1 = py_offset;
    p_img_area->x2 = px_offset + p_rsc->header.w - 1;
    p_img_area->y2 = py_offset + p_rsc->header.h - 1;
    p_this->rsc_idx = rsc_idx;
    p_this->flip_x = flip_x;
    p_this->flip_y = flip_y;
    // 并且检查是否需要更新extDrawSize，避免其他控件设置脏区时，弄脏了该控件，但有认为脏区不在控件内，从而导致了漏刷新。
    lv_coord_t w = lv_obj_get_width(&p_this->obj);
    lv_coord_t h = lv_obj_get_height(&p_this->obj);
    lv_coord_t old_size = _lv_obj_get_ext_draw_size(&p_this->obj);
    lv_coord_t new_size = 0;
    lv_coord_t delta_size;
    delta_size = -px_offset;
    if (new_size < delta_size) {
        new_size = delta_size; // x1
    }
    delta_size = -py_offset;
    if (new_size < delta_size) {
        new_size = delta_size; // y1
    }
    delta_size = p_img_area->x2 - w;
    if (new_size < delta_size) {
        new_size = delta_size; // x2
    }
    delta_size = p_img_area->y2 - h;
    if (new_size < delta_size) {
        new_size = delta_size; // y2
    }
    // 保存新边沿
    if(p_this->obj.spec_attr) {
        p_this->obj.spec_attr->ext_draw_size = new_size;
    }
    else if(new_size != 0) {
        lv_obj_allocate_spec_attr(&p_this->obj);
        p_this->obj.spec_attr->ext_draw_size = new_size;
    }
    // 这里不着急刷新，后续会刷新
    // if(s_new != s_old) lv_obj_invalidate(obj);
}

static void lv_img_gximg_event(const lv_obj_class_t *class_p, lv_event_t *evt)
{
    lv_obj_t * obj = evt->current_target;
    lv_event_code_t code = evt->code;
    //class_p->base_class->event_cb(class_p->base_class, evt);
#if LV_GDX_PATCH_GX_IMG > 0u
    if (code == LV_EVENT_DRAW_MAIN) {
        lv_draw_ctx_t * draw_ctx = lv_event_get_draw_ctx(evt);

        lv_img_gximg_t * p_this = (lv_img_gximg_t *)obj;
        const lv_gximg_dsc_t *p_gximg = p_this->p_cur_img;
        if (p_gximg) {
            lv_area_t tmp_result;
            lv_area_t tmp = p_this->img_area;
            tmp.x1 += obj->coords.x1;
            tmp.y1 += obj->coords.y1;
            tmp.x2 += obj->coords.x1;
            tmp.y2 += obj->coords.y1;
            if (_lv_area_intersect(&tmp_result, draw_ctx->clip_area, &tmp))
            {
                lv_port_gximg_draw(draw_ctx, NULL, &tmp, p_gximg, p_this->flip_x, p_this->flip_y);
            }
            return;
        }
    }
    else if (code == LV_EVENT_REFR_EXT_DRAW_SIZE) {
        lv_coord_t * s = lv_event_get_param(evt);
        lv_img_gximg_t * p_this = (lv_img_gximg_t *)obj;

        lv_coord_t w = lv_obj_get_width(&p_this->obj);
        lv_coord_t h = lv_obj_get_height(&p_this->obj);
        lv_coord_t new_size = 0;
        lv_coord_t delta_size;
        delta_size = -p_this->img_area.x1;
        if (new_size < delta_size) {
            new_size = delta_size; // x1
        }
        delta_size = -p_this->img_area.y1;
        if (new_size < delta_size) {
            new_size = delta_size; // y1
        }
        delta_size = p_this->img_area.x2 - w;
        if (new_size < delta_size) {
            new_size = delta_size; // x2
        }
        delta_size = p_this->img_area.y2 - h;
        if (new_size < delta_size) {
            new_size = delta_size; // y2
        }
        *s = new_size;
        return;
    }
#else
    lv_obj_class.event_cb(class_p, evt);
#endif
}
