#include "lv_general_list_generator.h"
#include "lv_img_dsc_list.h"
#include "lv_layout_router.h"

#if 0
static const general_list_item_t PHONECALL_LIST_ITEMS[] = {
    {"Contacts", &wd_img_contacts},
    {"Calls", &wd_img_calls},
    {"Dial", &wd_img_dial},
};

static void phonecall_list_event_callback(uint16_t idx, lv_event_t *event)
{
    switch (idx)
    {
        case 0:
            lv_layout_router_show_bt_contacts(event->current_target);
            break;

        case 1:
            // TODO: Go to Calls
            break;

        case 2:
            lv_layout_router_show_bt_dial(event->current_target);
            break;
    }
}

lv_obj_t *lv_bt_phonecall_layout_create(lv_obj_t *parent)
{
    general_list_dsc_t dsc = {
        .title = "Phone Call",
        .items = PHONECALL_LIST_ITEMS,
        .item_num = LIST_SIZE(PHONECALL_LIST_ITEMS),
        .suffix = &wd_img_chevron_right,
        .item_width = 0, // default
        .callback = phonecall_list_event_callback,
    };
    return lv_general_list_create(parent, &dsc);
}
#endif // 0
