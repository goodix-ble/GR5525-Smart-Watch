#include "app_bt.h"
#include "lv_general_options_page_generator.h"
#include "lv_img_dsc_list.h"
#include "lv_layout_router.h"

static lv_obj_t *s_factory_reset_layout = NULL;

static void on_bt_api_factory_reset_cplt_power_on(bt_api_msg_t *msg)
{
    lv_general_options_page_status_text_set(s_factory_reset_layout, "Factory reset done.");
    bt_api_deregister_indication_callback(on_bt_api_factory_reset_cplt_power_on);
}

static void on_button_clicked(lv_event_t *event)
{
    int32_t yes = (int32_t)event->user_data;

    if (yes == 1)
    {
        // OK
        lv_general_options_page_show_status(s_factory_reset_layout);
        bt_api_register_indication_callback(on_bt_api_factory_reset_cplt_power_on, IND_POWER_ON_STATE);
        bt_api_device_factory_reset();
    }
    else
    {
        // Cancel
        lv_layout_router_back(event->current_target);
    }
}

static void on_layout_deleted(lv_event_t *event)
{
    s_factory_reset_layout = NULL;
}

lv_obj_t *lv_bt_factory_reset_layout_create(lv_obj_t *parent)
{
    general_option_page_dsc_t dsc = {
        .title = "Factory Reset",
        .tip_text = "Do you really want to do factory reset?",
        .status_text = "Reseting...",
        .callback = on_button_clicked,
    };

    s_factory_reset_layout = lv_general_options_page_create(parent, &dsc);
    lv_obj_add_event_cb(s_factory_reset_layout, on_layout_deleted, LV_EVENT_DELETE, NULL);
    return s_factory_reset_layout;
}
