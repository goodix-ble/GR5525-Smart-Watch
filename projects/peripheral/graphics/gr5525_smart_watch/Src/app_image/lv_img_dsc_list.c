#include "lv_img_dsc_list.h"
#include <stdint.h>
#include "gr55xx.h"

#ifndef USE_EXTERNAL_RESOURCES
#define USE_EXTERNAL_RESOURCES 1
#endif // USE_EXTERNAL_RESOURCES

#if USE_EXTERNAL_RESOURCES
#define BINARY_RESOURCES (const uint8_t *)(0x00800000)
#else
extern const uint8_t BINARY_RESOURCES[];
#endif // USE_EXTERNAL_RESOURCES

const lv_img_dsc_t wd_img_10_power_off = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 48,
    .header.h = 48,
    .data_size = 4608,
    .data = BINARY_RESOURCES + OFFSET_10_POWER_OFF,
};

const lv_img_dsc_t wd_img_1_scan_device = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 48,
    .header.h = 48,
    .data_size = 4608,
    .data = BINARY_RESOURCES + OFFSET_1_SCAN_DEVICE,
};

const lv_img_dsc_t wd_img_2_bond_device = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 48,
    .header.h = 48,
    .data_size = 4608,
    .data = BINARY_RESOURCES + OFFSET_2_BOND_DEVICE,
};

const lv_img_dsc_t wd_img_3_reconnecton_control = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 48,
    .header.h = 48,
    .data_size = 4608,
    .data = BINARY_RESOURCES + OFFSET_3_RECONNECTON_CONTROL,
};

const lv_img_dsc_t wd_img_4_reconnecton_times = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 48,
    .header.h = 48,
    .data_size = 4608,
    .data = BINARY_RESOURCES + OFFSET_4_RECONNECTON_TIMES,
};

const lv_img_dsc_t wd_img_5_enter_aniff = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 48,
    .header.h = 48,
    .data_size = 4608,
    .data = BINARY_RESOURCES + OFFSET_5_ENTER_ANIFF,
};

const lv_img_dsc_t wd_img_6_aniff_period = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 48,
    .header.h = 48,
    .data_size = 4608,
    .data = BINARY_RESOURCES + OFFSET_6_ANIFF_PERIOD,
};

const lv_img_dsc_t wd_img_7_version = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 48,
    .header.h = 48,
    .data_size = 4608,
    .data = BINARY_RESOURCES + OFFSET_7_VERSION,
};

const lv_img_dsc_t wd_img_8_restore_setting = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 48,
    .header.h = 48,
    .data_size = 4608,
    .data = BINARY_RESOURCES + OFFSET_8_RESTORE_SETTING,
};

const lv_img_dsc_t wd_img_9_ota = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 48,
    .header.h = 48,
    .data_size = 4608,
    .data = BINARY_RESOURCES + OFFSET_9_OTA,
};

const lv_img_dsc_t wd_img_app_01_workout = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 80,
    .header.h = 80,
    .data_size = 12800,
    .data = BINARY_RESOURCES + OFFSET_APP_01_WORKOUT,
};

const lv_img_dsc_t wd_img_app_02_activity = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 80,
    .header.h = 80,
    .data_size = 12800,
    .data = BINARY_RESOURCES + OFFSET_APP_02_ACTIVITY,
};

const lv_img_dsc_t wd_img_app_03_hr = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 80,
    .header.h = 80,
    .data_size = 12800,
    .data = BINARY_RESOURCES + OFFSET_APP_03_HR,
};

const lv_img_dsc_t wd_img_app_04_stopwatch = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 80,
    .header.h = 80,
    .data_size = 12800,
    .data = BINARY_RESOURCES + OFFSET_APP_04_STOPWATCH,
};

const lv_img_dsc_t wd_img_app_05_spo2 = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 80,
    .header.h = 80,
    .data_size = 12800,
    .data = BINARY_RESOURCES + OFFSET_APP_05_SPO2,
};

const lv_img_dsc_t wd_img_app_06_events = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 80,
    .header.h = 80,
    .data_size = 12800,
    .data = BINARY_RESOURCES + OFFSET_APP_06_EVENTS,
};

const lv_img_dsc_t wd_img_app_07_settings = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 80,
    .header.h = 80,
    .data_size = 12800,
    .data = BINARY_RESOURCES + OFFSET_APP_07_SETTINGS,
};

const lv_img_dsc_t wd_img_black_clock_face = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 360,
    .header.h = 360,
    .data_size = 259200,
    .data = BINARY_RESOURCES + OFFSET_BLACK_CLOCK_FACE,
};

const lv_img_dsc_t wd_img_black_clock_step = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 26,
    .header.h = 30,
    .data_size = 1560,
    .data = BINARY_RESOURCES + OFFSET_BLACK_CLOCK_STEP,
};

const lv_img_dsc_t wd_img_black_clock_sun = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 23,
    .header.h = 23,
    .data_size = 1058,
    .data = BINARY_RESOURCES + OFFSET_BLACK_CLOCK_SUN,
};

const lv_img_dsc_t wd_img_black_clock_thumbnail = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 214,
    .header.h = 214,
    .data_size = 91592,
    .data = BINARY_RESOURCES + OFFSET_BLACK_CLOCK_THUMBNAIL,
};

const lv_img_dsc_t wd_img_bt_device_management = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 48,
    .header.h = 48,
    .data_size = 4608,
    .data = BINARY_RESOURCES + OFFSET_BT_DEVICE_MANAGEMENT,
};

const lv_img_dsc_t wd_img_bt_device_music = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 48,
    .header.h = 48,
    .data_size = 4608,
    .data = BINARY_RESOURCES + OFFSET_BT_DEVICE_MUSIC,
};

const lv_img_dsc_t wd_img_bt_device_phonecall = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 48,
    .header.h = 48,
    .data_size = 4608,
    .data = BINARY_RESOURCES + OFFSET_BT_DEVICE_PHONECALL,
};

const lv_img_dsc_t wd_img_bt_icon = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 72,
    .header.h = 72,
    .data_size = 10368,
    .data = BINARY_RESOURCES + OFFSET_BT_ICON,
};

const lv_img_dsc_t wd_img_calls = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 48,
    .header.h = 48,
    .data_size = 4608,
    .data = BINARY_RESOURCES + OFFSET_CALLS,
};

const lv_img_dsc_t wd_img_call_accept_large = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 80,
    .header.h = 80,
    .data_size = 12800,
    .data = BINARY_RESOURCES + OFFSET_CALL_ACCEPT_LARGE,
};

const lv_img_dsc_t wd_img_call_dial = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 60,
    .header.h = 60,
    .data_size = 7200,
    .data = BINARY_RESOURCES + OFFSET_CALL_DIAL,
};

const lv_img_dsc_t wd_img_call_hang_up = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 220,
    .header.h = 60,
    .data_size = 26400,
    .data = BINARY_RESOURCES + OFFSET_CALL_HANG_UP,
};

const lv_img_dsc_t wd_img_call_mute = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 60,
    .header.h = 60,
    .data_size = 7200,
    .data = BINARY_RESOURCES + OFFSET_CALL_MUTE,
};

const lv_img_dsc_t wd_img_call_mute_disabled = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 60,
    .header.h = 60,
    .data_size = 7200,
    .data = BINARY_RESOURCES + OFFSET_CALL_MUTE_DISABLED,
};

const lv_img_dsc_t wd_img_call_mute_large = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 80,
    .header.h = 80,
    .data_size = 12800,
    .data = BINARY_RESOURCES + OFFSET_CALL_MUTE_LARGE,
};

const lv_img_dsc_t wd_img_call_mute_large_disabled = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 80,
    .header.h = 80,
    .data_size = 12800,
    .data = BINARY_RESOURCES + OFFSET_CALL_MUTE_LARGE_DISABLED,
};

const lv_img_dsc_t wd_img_call_reject_large = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 80,
    .header.h = 80,
    .data_size = 12800,
    .data = BINARY_RESOURCES + OFFSET_CALL_REJECT_LARGE,
};

const lv_img_dsc_t wd_img_call_volume = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 60,
    .header.h = 60,
    .data_size = 7200,
    .data = BINARY_RESOURCES + OFFSET_CALL_VOLUME,
};

const lv_img_dsc_t wd_img_calories_icon = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 28,
    .header.h = 28,
    .data_size = 1568,
    .data = BINARY_RESOURCES + OFFSET_CALORIES_ICON,
};

const lv_img_dsc_t wd_img_chevron_right = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 12,
    .header.h = 20,
    .data_size = 480,
    .data = BINARY_RESOURCES + OFFSET_CHEVRON_RIGHT,
};

const lv_img_dsc_t wd_img_connect_icon_off = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 26,
    .header.h = 14,
    .data_size = 728,
    .data = BINARY_RESOURCES + OFFSET_CONNECT_ICON_OFF,
};

const lv_img_dsc_t wd_img_connect_icon_on = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 26,
    .header.h = 14,
    .data_size = 728,
    .data = BINARY_RESOURCES + OFFSET_CONNECT_ICON_ON,
};

const lv_img_dsc_t wd_img_contacts = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 48,
    .header.h = 48,
    .data_size = 4608,
    .data = BINARY_RESOURCES + OFFSET_CONTACTS,
};

const lv_img_dsc_t wd_img_dial = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 48,
    .header.h = 48,
    .data_size = 4608,
    .data = BINARY_RESOURCES + OFFSET_DIAL,
};

const lv_img_dsc_t wd_img_dial_backspace = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 46,
    .header.h = 30,
    .data_size = 2760,
    .data = BINARY_RESOURCES + OFFSET_DIAL_BACKSPACE,
};

const lv_img_dsc_t wd_img_dial_number_0 = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 74,
    .header.h = 56,
    .data_size = 8288,
    .data = BINARY_RESOURCES + OFFSET_DIAL_NUMBER_0,
};

const lv_img_dsc_t wd_img_dial_number_1 = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 74,
    .header.h = 56,
    .data_size = 8288,
    .data = BINARY_RESOURCES + OFFSET_DIAL_NUMBER_1,
};

const lv_img_dsc_t wd_img_dial_number_2 = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 74,
    .header.h = 56,
    .data_size = 8288,
    .data = BINARY_RESOURCES + OFFSET_DIAL_NUMBER_2,
};

const lv_img_dsc_t wd_img_dial_number_3 = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 74,
    .header.h = 56,
    .data_size = 8288,
    .data = BINARY_RESOURCES + OFFSET_DIAL_NUMBER_3,
};

const lv_img_dsc_t wd_img_dial_number_4 = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 74,
    .header.h = 56,
    .data_size = 8288,
    .data = BINARY_RESOURCES + OFFSET_DIAL_NUMBER_4,
};

const lv_img_dsc_t wd_img_dial_number_5 = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 74,
    .header.h = 56,
    .data_size = 8288,
    .data = BINARY_RESOURCES + OFFSET_DIAL_NUMBER_5,
};

const lv_img_dsc_t wd_img_dial_number_6 = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 74,
    .header.h = 56,
    .data_size = 8288,
    .data = BINARY_RESOURCES + OFFSET_DIAL_NUMBER_6,
};

const lv_img_dsc_t wd_img_dial_number_7 = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 74,
    .header.h = 56,
    .data_size = 8288,
    .data = BINARY_RESOURCES + OFFSET_DIAL_NUMBER_7,
};

const lv_img_dsc_t wd_img_dial_number_8 = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 74,
    .header.h = 56,
    .data_size = 8288,
    .data = BINARY_RESOURCES + OFFSET_DIAL_NUMBER_8,
};

const lv_img_dsc_t wd_img_dial_number_9 = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 74,
    .header.h = 56,
    .data_size = 8288,
    .data = BINARY_RESOURCES + OFFSET_DIAL_NUMBER_9,
};

const lv_img_dsc_t wd_img_digital2_consume_small = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 16,
    .header.h = 20,
    .data_size = 640,
    .data = BINARY_RESOURCES + OFFSET_DIGITAL2_CONSUME_SMALL,
};

const lv_img_dsc_t wd_img_digital2_exercise_small = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 18,
    .header.h = 20,
    .data_size = 720,
    .data = BINARY_RESOURCES + OFFSET_DIGITAL2_EXERCISE_SMALL,
};

const lv_img_dsc_t wd_img_digital2_heart_small = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 22,
    .header.h = 18,
    .data_size = 792,
    .data = BINARY_RESOURCES + OFFSET_DIGITAL2_HEART_SMALL,
};

const lv_img_dsc_t wd_img_digital_clock_thumbnail = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 214,
    .header.h = 214,
    .data_size = 91592,
    .data = BINARY_RESOURCES + OFFSET_DIGITAL_CLOCK_THUMBNAIL,
};

const lv_img_dsc_t wd_img_distance_icon = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 28,
    .header.h = 28,
    .data_size = 1568,
    .data = BINARY_RESOURCES + OFFSET_DISTANCE_ICON,
};

const lv_img_dsc_t wd_img_heartrate_background = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 270,
    .header.h = 144,
    .data_size = 77760,
    .data = BINARY_RESOURCES + OFFSET_HEARTRATE_BACKGROUND,
};

const lv_img_dsc_t wd_img_heartrate_lower = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 20,
    .header.h = 16,
    .data_size = 640,
    .data = BINARY_RESOURCES + OFFSET_HEARTRATE_LOWER,
};

const lv_img_dsc_t wd_img_heartrate_upper = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 20,
    .header.h = 16,
    .data_size = 640,
    .data = BINARY_RESOURCES + OFFSET_HEARTRATE_UPPER,
};

const lv_img_dsc_t wd_img_heart_icon_50 = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 50,
    .header.h = 50,
    .data_size = 5000,
    .data = BINARY_RESOURCES + OFFSET_HEART_ICON_50,
};

const lv_img_dsc_t wd_img_heart_icon_70 = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 70,
    .header.h = 70,
    .data_size = 9800,
    .data = BINARY_RESOURCES + OFFSET_HEART_ICON_70,
};

const lv_img_dsc_t wd_img_incoming_call_reminder = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 32,
    .header.h = 32,
    .data_size = 2048,
    .data = BINARY_RESOURCES + OFFSET_INCOMING_CALL_REMINDER,
};

const lv_img_dsc_t wd_img_music_backward = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 36,
    .header.h = 32,
    .data_size = 2304,
    .data = BINARY_RESOURCES + OFFSET_MUSIC_BACKWARD,
};

const lv_img_dsc_t wd_img_music_forward = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 36,
    .header.h = 32,
    .data_size = 2304,
    .data = BINARY_RESOURCES + OFFSET_MUSIC_FORWARD,
};

const lv_img_dsc_t wd_img_music_pause = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 80,
    .header.h = 80,
    .data_size = 12800,
    .data = BINARY_RESOURCES + OFFSET_MUSIC_PAUSE,
};

const lv_img_dsc_t wd_img_music_play = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 80,
    .header.h = 80,
    .data_size = 12800,
    .data = BINARY_RESOURCES + OFFSET_MUSIC_PLAY,
};

const lv_img_dsc_t wd_img_notification_del_all = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 200,
    .header.h = 48,
    .data_size = 19200,
    .data = BINARY_RESOURCES + OFFSET_NOTIFICATION_DEL_ALL,
};

const lv_img_dsc_t wd_img_notification_del_msg_btn = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 60,
    .header.h = 92,
    .data_size = 11040,
    .data = BINARY_RESOURCES + OFFSET_NOTIFICATION_DEL_MSG_BTN,
};

const lv_img_dsc_t wd_img_notification_msg_wechat = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 48,
    .header.h = 48,
    .data_size = 4608,
    .data = BINARY_RESOURCES + OFFSET_NOTIFICATION_MSG_WECHAT,
};

const lv_img_dsc_t wd_img_notification_title = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 110,
    .header.h = 20,
    .data_size = 4400,
    .data = BINARY_RESOURCES + OFFSET_NOTIFICATION_TITLE,
};

const lv_img_dsc_t wd_img_quick_elc_100 = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 26,
    .header.h = 12,
    .data_size = 624,
    .data = BINARY_RESOURCES + OFFSET_QUICK_ELC_100,
};

const lv_img_dsc_t wd_img_quick_elc_20 = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 26,
    .header.h = 12,
    .data_size = 624,
    .data = BINARY_RESOURCES + OFFSET_QUICK_ELC_20,
};

const lv_img_dsc_t wd_img_quick_elc_40 = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 26,
    .header.h = 12,
    .data_size = 624,
    .data = BINARY_RESOURCES + OFFSET_QUICK_ELC_40,
};

const lv_img_dsc_t wd_img_quick_elc_5 = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 26,
    .header.h = 12,
    .data_size = 624,
    .data = BINARY_RESOURCES + OFFSET_QUICK_ELC_5,
};

const lv_img_dsc_t wd_img_quick_elc_60 = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 26,
    .header.h = 12,
    .data_size = 624,
    .data = BINARY_RESOURCES + OFFSET_QUICK_ELC_60,
};

const lv_img_dsc_t wd_img_quick_elc_80 = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 26,
    .header.h = 12,
    .data_size = 624,
    .data = BINARY_RESOURCES + OFFSET_QUICK_ELC_80,
};

const lv_img_dsc_t wd_img_quick_tab_about = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 80,
    .header.h = 80,
    .data_size = 12800,
    .data = BINARY_RESOURCES + OFFSET_QUICK_TAB_ABOUT,
};

const lv_img_dsc_t wd_img_quick_tab_brightness = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 80,
    .header.h = 80,
    .data_size = 12800,
    .data = BINARY_RESOURCES + OFFSET_QUICK_TAB_BRIGHTNESS,
};

const lv_img_dsc_t wd_img_quick_tab_dont_off = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 80,
    .header.h = 80,
    .data_size = 12800,
    .data = BINARY_RESOURCES + OFFSET_QUICK_TAB_DONT_OFF,
};

const lv_img_dsc_t wd_img_quick_tab_dont_on = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 80,
    .header.h = 80,
    .data_size = 12800,
    .data = BINARY_RESOURCES + OFFSET_QUICK_TAB_DONT_ON,
};

const lv_img_dsc_t wd_img_quick_tab_flashlight = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 80,
    .header.h = 80,
    .data_size = 12800,
    .data = BINARY_RESOURCES + OFFSET_QUICK_TAB_FLASHLIGHT,
};

const lv_img_dsc_t wd_img_quick_tab_setting = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 80,
    .header.h = 80,
    .data_size = 12800,
    .data = BINARY_RESOURCES + OFFSET_QUICK_TAB_SETTING,
};

const lv_img_dsc_t wd_img_quick_tab_taiwan_off = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 80,
    .header.h = 80,
    .data_size = 12800,
    .data = BINARY_RESOURCES + OFFSET_QUICK_TAB_TAIWAN_OFF,
};

const lv_img_dsc_t wd_img_quick_tab_taiwan_on = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 80,
    .header.h = 80,
    .data_size = 12800,
    .data = BINARY_RESOURCES + OFFSET_QUICK_TAB_TAIWAN_ON,
};

const lv_img_dsc_t wd_img_step_icon = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 28,
    .header.h = 28,
    .data_size = 1568,
    .data = BINARY_RESOURCES + OFFSET_STEP_ICON,
};

const lv_img_dsc_t wd_img_vivid_clock_hr = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 16,
    .header.h = 16,
    .data_size = 512,
    .data = BINARY_RESOURCES + OFFSET_VIVID_CLOCK_HR,
};

const lv_img_dsc_t wd_img_vivid_clock_thumbnail = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 214,
    .header.h = 214,
    .data_size = 91592,
    .data = BINARY_RESOURCES + OFFSET_VIVID_CLOCK_THUMBNAIL,
};

const lv_img_dsc_t wd_img_volume_control_background = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 220,
    .header.h = 50,
    .data_size = 22000,
    .data = BINARY_RESOURCES + OFFSET_VOLUME_CONTROL_BACKGROUND,
};

const lv_img_dsc_t wd_img_volume_control_indicator = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 132,
    .header.h = 4,
    .data_size = 1056,
    .data = BINARY_RESOURCES + OFFSET_VOLUME_CONTROL_INDICATOR,
};

const lv_img_dsc_t wd_img_volume_down = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 12,
    .header.h = 12,
    .data_size = 288,
    .data = BINARY_RESOURCES + OFFSET_VOLUME_DOWN,
};

const lv_img_dsc_t wd_img_volume_down_disabled = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 12,
    .header.h = 12,
    .data_size = 288,
    .data = BINARY_RESOURCES + OFFSET_VOLUME_DOWN_DISABLED,
};

const lv_img_dsc_t wd_img_volume_up = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 12,
    .header.h = 12,
    .data_size = 288,
    .data = BINARY_RESOURCES + OFFSET_VOLUME_UP,
};

const lv_img_dsc_t wd_img_volume_up_disabled = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 12,
    .header.h = 12,
    .data_size = 288,
    .data = BINARY_RESOURCES + OFFSET_VOLUME_UP_DISABLED,
};

const lv_img_dsc_t wd_img_watchface_bg = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 360,
    .header.h = 360,
    .data_size = 259200,
    .data = BINARY_RESOURCES + OFFSET_WATCHFACE_BG,
};

const lv_img_dsc_t wd_img_watchface_icon_battery = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 19,
    .header.h = 19,
    .data_size = 722,
    .data = BINARY_RESOURCES + OFFSET_WATCHFACE_ICON_BATTERY,
};

const lv_img_dsc_t wd_img_watchface_icon_energy = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 18,
    .header.h = 16,
    .data_size = 576,
    .data = BINARY_RESOURCES + OFFSET_WATCHFACE_ICON_ENERGY,
};

const lv_img_dsc_t wd_img_watchface_icon_hr = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 18,
    .header.h = 18,
    .data_size = 648,
    .data = BINARY_RESOURCES + OFFSET_WATCHFACE_ICON_HR,
};

const lv_img_dsc_t wd_img_watchface_icon_step = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 22,
    .header.h = 19,
    .data_size = 836,
    .data = BINARY_RESOURCES + OFFSET_WATCHFACE_ICON_STEP,
};

const lv_img_dsc_t wd_img_weather_clear = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 40,
    .header.h = 40,
    .data_size = 3200,
    .data = BINARY_RESOURCES + OFFSET_WEATHER_CLEAR,
};

const lv_img_dsc_t wd_img_weather_moonrise_icon = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 20,
    .header.h = 20,
    .data_size = 800,
    .data = BINARY_RESOURCES + OFFSET_WEATHER_MOONRISE_ICON,
};

const lv_img_dsc_t wd_img_weather_moonset_icon = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 20,
    .header.h = 20,
    .data_size = 800,
    .data = BINARY_RESOURCES + OFFSET_WEATHER_MOONSET_ICON,
};

const lv_img_dsc_t wd_img_weather_moon_icon = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 20,
    .header.h = 20,
    .data_size = 800,
    .data = BINARY_RESOURCES + OFFSET_WEATHER_MOON_ICON,
};

const lv_img_dsc_t wd_img_weather_sunrise_icon = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 20,
    .header.h = 20,
    .data_size = 800,
    .data = BINARY_RESOURCES + OFFSET_WEATHER_SUNRISE_ICON,
};

const lv_img_dsc_t wd_img_weather_sunset_icon = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 20,
    .header.h = 20,
    .data_size = 800,
    .data = BINARY_RESOURCES + OFFSET_WEATHER_SUNSET_ICON,
};

const lv_img_dsc_t wd_img_weather_sun_icon = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 20,
    .header.h = 20,
    .data_size = 800,
    .data = BINARY_RESOURCES + OFFSET_WEATHER_SUN_ICON,
};

const lv_img_dsc_t wd_img_weather_thunderstorm = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 40,
    .header.h = 40,
    .data_size = 3200,
    .data = BINARY_RESOURCES + OFFSET_WEATHER_THUNDERSTORM,
};

const lv_img_dsc_t wd_img_weather_typhoon = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 40,
    .header.h = 40,
    .data_size = 3200,
    .data = BINARY_RESOURCES + OFFSET_WEATHER_TYPHOON,
};

const lv_img_dsc_t wd_img_wf_num_0 = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 10,
    .header.h = 14,
    .data_size = 280,
    .data = BINARY_RESOURCES + OFFSET_WF_NUM_0,
};

const lv_img_dsc_t wd_img_wf_num_1 = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 10,
    .header.h = 14,
    .data_size = 280,
    .data = BINARY_RESOURCES + OFFSET_WF_NUM_1,
};

const lv_img_dsc_t wd_img_wf_num_2 = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 10,
    .header.h = 14,
    .data_size = 280,
    .data = BINARY_RESOURCES + OFFSET_WF_NUM_2,
};

const lv_img_dsc_t wd_img_wf_num_3 = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 10,
    .header.h = 14,
    .data_size = 280,
    .data = BINARY_RESOURCES + OFFSET_WF_NUM_3,
};

const lv_img_dsc_t wd_img_wf_num_4 = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 10,
    .header.h = 14,
    .data_size = 280,
    .data = BINARY_RESOURCES + OFFSET_WF_NUM_4,
};

const lv_img_dsc_t wd_img_wf_num_5 = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 10,
    .header.h = 14,
    .data_size = 280,
    .data = BINARY_RESOURCES + OFFSET_WF_NUM_5,
};

const lv_img_dsc_t wd_img_wf_num_6 = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 10,
    .header.h = 14,
    .data_size = 280,
    .data = BINARY_RESOURCES + OFFSET_WF_NUM_6,
};

const lv_img_dsc_t wd_img_wf_num_7 = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 10,
    .header.h = 14,
    .data_size = 280,
    .data = BINARY_RESOURCES + OFFSET_WF_NUM_7,
};

const lv_img_dsc_t wd_img_wf_num_8 = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 10,
    .header.h = 14,
    .data_size = 280,
    .data = BINARY_RESOURCES + OFFSET_WF_NUM_8,
};

const lv_img_dsc_t wd_img_wf_num_9 = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .header.w = 10,
    .header.h = 14,
    .data_size = 280,
    .data = BINARY_RESOURCES + OFFSET_WF_NUM_9,
};

const lv_img_dsc_t wd_img_watchface_center_point = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
    .header.w = 18,
    .header.h = 18,
    .data_size = 972,
    .data = BINARY_RESOURCES + OFFSET_WATCHFACE_CENTER_POINT,
};

const lv_gximg_dsc_t wd_gximg_watchface_hand_hour_00 = {
    .header.always_zero = 0,
    .header.w = 96,
    .header.h = 12,
    .header.cf = LV_IMG_CF_GDX_GXIMG,
    .center_x = 0,
    .center_y = 6,
    .start_y = 0,
    .total_lines = 12,
    .data_size = 3520,
    .line_table = 1,
    .data = BINARY_RESOURCES + OFFSET_GXIMG_WATCHFACE_HAND_HOUR_00
};

const lv_gximg_dsc_t wd_gximg_watchface_hand_hour_06 = {
    .header.always_zero = 0,
    .header.w = 98,
    .header.h = 22,
    .header.cf = LV_IMG_CF_GDX_GXIMG,
    .center_x = 1,
    .center_y = 16,
    .start_y = 0,
    .total_lines = 22,
    .data_size = 3632,
    .line_table = 1,
    .data = BINARY_RESOURCES + OFFSET_GXIMG_WATCHFACE_HAND_HOUR_06
};

const lv_gximg_dsc_t wd_gximg_watchface_hand_hour_12 = {
    .header.always_zero = 0,
    .header.w = 98,
    .header.h = 32,
    .header.cf = LV_IMG_CF_GDX_GXIMG,
    .center_x = 2,
    .center_y = 26,
    .start_y = 1,
    .total_lines = 32,
    .data_size = 3716,
    .line_table = 1,
    .data = BINARY_RESOURCES + OFFSET_GXIMG_WATCHFACE_HAND_HOUR_12
};

const lv_gximg_dsc_t wd_gximg_watchface_hand_hour_18 = {
    .header.always_zero = 0,
    .header.w = 96,
    .header.h = 42,
    .header.cf = LV_IMG_CF_GDX_GXIMG,
    .center_x = 2,
    .center_y = 36,
    .start_y = 1,
    .total_lines = 41,
    .data_size = 3812,
    .line_table = 1,
    .data = BINARY_RESOURCES + OFFSET_GXIMG_WATCHFACE_HAND_HOUR_18
};

const lv_gximg_dsc_t wd_gximg_watchface_hand_hour_24 = {
    .header.always_zero = 0,
    .header.w = 94,
    .header.h = 52,
    .header.cf = LV_IMG_CF_GDX_GXIMG,
    .center_x = 3,
    .center_y = 46,
    .start_y = 2,
    .total_lines = 50,
    .data_size = 3876,
    .line_table = 1,
    .data = BINARY_RESOURCES + OFFSET_GXIMG_WATCHFACE_HAND_HOUR_24
};

const lv_gximg_dsc_t wd_gximg_watchface_hand_hour_30 = {
    .header.always_zero = 0,
    .header.w = 90,
    .header.h = 60,
    .header.cf = LV_IMG_CF_GDX_GXIMG,
    .center_x = 3,
    .center_y = 54,
    .start_y = 2,
    .total_lines = 58,
    .data_size = 3904,
    .line_table = 1,
    .data = BINARY_RESOURCES + OFFSET_GXIMG_WATCHFACE_HAND_HOUR_30
};

const lv_gximg_dsc_t wd_gximg_watchface_hand_hour_36 = {
    .header.always_zero = 0,
    .header.w = 86,
    .header.h = 68,
    .header.cf = LV_IMG_CF_GDX_GXIMG,
    .center_x = 4,
    .center_y = 62,
    .start_y = 2,
    .total_lines = 66,
    .data_size = 3980,
    .line_table = 1,
    .data = BINARY_RESOURCES + OFFSET_GXIMG_WATCHFACE_HAND_HOUR_36
};

const lv_gximg_dsc_t wd_gximg_watchface_hand_hour_42 = {
    .header.always_zero = 0,
    .header.w = 80,
    .header.h = 74,
    .header.cf = LV_IMG_CF_GDX_GXIMG,
    .center_x = 4,
    .center_y = 69,
    .start_y = 2,
    .total_lines = 72,
    .data_size = 4116,
    .line_table = 1,
    .data = BINARY_RESOURCES + OFFSET_GXIMG_WATCHFACE_HAND_HOUR_42
};

const lv_gximg_dsc_t wd_gximg_watchface_hand_hour_48 = {
    .header.always_zero = 0,
    .header.w = 74,
    .header.h = 80,
    .header.cf = LV_IMG_CF_GDX_GXIMG,
    .center_x = 5,
    .center_y = 76,
    .start_y = 2,
    .total_lines = 78,
    .data_size = 4064,
    .line_table = 1,
    .data = BINARY_RESOURCES + OFFSET_GXIMG_WATCHFACE_HAND_HOUR_48
};

const lv_gximg_dsc_t wd_gximg_watchface_hand_hour_54 = {
    .header.always_zero = 0,
    .header.w = 68,
    .header.h = 86,
    .header.cf = LV_IMG_CF_GDX_GXIMG,
    .center_x = 6,
    .center_y = 82,
    .start_y = 2,
    .total_lines = 84,
    .data_size = 4284,
    .line_table = 1,
    .data = BINARY_RESOURCES + OFFSET_GXIMG_WATCHFACE_HAND_HOUR_54
};

const lv_gximg_dsc_t wd_gximg_watchface_hand_hour_60 = {
    .header.always_zero = 0,
    .header.w = 60,
    .header.h = 90,
    .header.cf = LV_IMG_CF_GDX_GXIMG,
    .center_x = 6,
    .center_y = 87,
    .start_y = 2,
    .total_lines = 88,
    .data_size = 4256,
    .line_table = 1,
    .data = BINARY_RESOURCES + OFFSET_GXIMG_WATCHFACE_HAND_HOUR_60
};

const lv_gximg_dsc_t wd_gximg_watchface_hand_hour_66 = {
    .header.always_zero = 0,
    .header.w = 52,
    .header.h = 94,
    .header.cf = LV_IMG_CF_GDX_GXIMG,
    .center_x = 6,
    .center_y = 91,
    .start_y = 2,
    .total_lines = 92,
    .data_size = 4244,
    .line_table = 1,
    .data = BINARY_RESOURCES + OFFSET_GXIMG_WATCHFACE_HAND_HOUR_66
};

const lv_gximg_dsc_t wd_gximg_watchface_hand_hour_72 = {
    .header.always_zero = 0,
    .header.w = 42,
    .header.h = 96,
    .header.cf = LV_IMG_CF_GDX_GXIMG,
    .center_x = 6,
    .center_y = 94,
    .start_y = 1,
    .total_lines = 95,
    .data_size = 4232,
    .line_table = 1,
    .data = BINARY_RESOURCES + OFFSET_GXIMG_WATCHFACE_HAND_HOUR_72
};

const lv_gximg_dsc_t wd_gximg_watchface_hand_hour_78 = {
    .header.always_zero = 0,
    .header.w = 32,
    .header.h = 98,
    .header.cf = LV_IMG_CF_GDX_GXIMG,
    .center_x = 6,
    .center_y = 96,
    .start_y = 2,
    .total_lines = 97,
    .data_size = 4212,
    .line_table = 1,
    .data = BINARY_RESOURCES + OFFSET_GXIMG_WATCHFACE_HAND_HOUR_78
};

const lv_gximg_dsc_t wd_gximg_watchface_hand_hour_84 = {
    .header.always_zero = 0,
    .header.w = 22,
    .header.h = 98,
    .header.cf = LV_IMG_CF_GDX_GXIMG,
    .center_x = 6,
    .center_y = 97,
    .start_y = 1,
    .total_lines = 97,
    .data_size = 4224,
    .line_table = 1,
    .data = BINARY_RESOURCES + OFFSET_GXIMG_WATCHFACE_HAND_HOUR_84
};

const lv_gximg_dsc_t wd_gximg_watchface_hand_hour_90 = {
    .header.always_zero = 0,
    .header.w = 12,
    .header.h = 96,
    .header.cf = LV_IMG_CF_GDX_GXIMG,
    .center_x = 6,
    .center_y = 96,
    .start_y = 0,
    .total_lines = 96,
    .data_size = 4196,
    .line_table = 1,
    .data = BINARY_RESOURCES + OFFSET_GXIMG_WATCHFACE_HAND_HOUR_90
};

const lv_gximg_dsc_t wd_gximg_watchface_hand_min_00 = {
    .header.always_zero = 0,
    .header.w = 128,
    .header.h = 12,
    .header.cf = LV_IMG_CF_GDX_GXIMG,
    .center_x = 0,
    .center_y = 6,
    .start_y = 0,
    .total_lines = 12,
    .data_size = 4672,
    .line_table = 1,
    .data = BINARY_RESOURCES + OFFSET_GXIMG_WATCHFACE_HAND_MIN_00
};

const lv_gximg_dsc_t wd_gximg_watchface_hand_min_06 = {
    .header.always_zero = 0,
    .header.w = 130,
    .header.h = 26,
    .header.cf = LV_IMG_CF_GDX_GXIMG,
    .center_x = 1,
    .center_y = 20,
    .start_y = 1,
    .total_lines = 25,
    .data_size = 4804,
    .line_table = 1,
    .data = BINARY_RESOURCES + OFFSET_GXIMG_WATCHFACE_HAND_MIN_06
};

const lv_gximg_dsc_t wd_gximg_watchface_hand_min_12 = {
    .header.always_zero = 0,
    .header.w = 128,
    .header.h = 40,
    .header.cf = LV_IMG_CF_GDX_GXIMG,
    .center_x = 1,
    .center_y = 33,
    .start_y = 1,
    .total_lines = 39,
    .data_size = 4936,
    .line_table = 1,
    .data = BINARY_RESOURCES + OFFSET_GXIMG_WATCHFACE_HAND_MIN_12
};

const lv_gximg_dsc_t wd_gximg_watchface_hand_min_18 = {
    .header.always_zero = 0,
    .header.w = 126,
    .header.h = 52,
    .header.cf = LV_IMG_CF_GDX_GXIMG,
    .center_x = 2,
    .center_y = 46,
    .start_y = 1,
    .total_lines = 50,
    .data_size = 5056,
    .line_table = 1,
    .data = BINARY_RESOURCES + OFFSET_GXIMG_WATCHFACE_HAND_MIN_18
};

const lv_gximg_dsc_t wd_gximg_watchface_hand_min_24 = {
    .header.always_zero = 0,
    .header.w = 122,
    .header.h = 64,
    .header.cf = LV_IMG_CF_GDX_GXIMG,
    .center_x = 3,
    .center_y = 58,
    .start_y = 2,
    .total_lines = 62,
    .data_size = 5116,
    .line_table = 1,
    .data = BINARY_RESOURCES + OFFSET_GXIMG_WATCHFACE_HAND_MIN_24
};

const lv_gximg_dsc_t wd_gximg_watchface_hand_min_30 = {
    .header.always_zero = 0,
    .header.w = 118,
    .header.h = 76,
    .header.cf = LV_IMG_CF_GDX_GXIMG,
    .center_x = 4,
    .center_y = 70,
    .start_y = 2,
    .total_lines = 74,
    .data_size = 5176,
    .line_table = 1,
    .data = BINARY_RESOURCES + OFFSET_GXIMG_WATCHFACE_HAND_MIN_30
};

const lv_gximg_dsc_t wd_gximg_watchface_hand_min_36 = {
    .header.always_zero = 0,
    .header.w = 112,
    .header.h = 86,
    .header.cf = LV_IMG_CF_GDX_GXIMG,
    .center_x = 4,
    .center_y = 81,
    .start_y = 2,
    .total_lines = 84,
    .data_size = 5288,
    .line_table = 1,
    .data = BINARY_RESOURCES + OFFSET_GXIMG_WATCHFACE_HAND_MIN_36
};

const lv_gximg_dsc_t wd_gximg_watchface_hand_min_42 = {
    .header.always_zero = 0,
    .header.w = 104,
    .header.h = 96,
    .header.cf = LV_IMG_CF_GDX_GXIMG,
    .center_x = 4,
    .center_y = 91,
    .start_y = 2,
    .total_lines = 94,
    .data_size = 5500,
    .line_table = 1,
    .data = BINARY_RESOURCES + OFFSET_GXIMG_WATCHFACE_HAND_MIN_42
};

const lv_gximg_dsc_t wd_gximg_watchface_hand_min_48 = {
    .header.always_zero = 0,
    .header.w = 96,
    .header.h = 104,
    .header.cf = LV_IMG_CF_GDX_GXIMG,
    .center_x = 5,
    .center_y = 100,
    .start_y = 2,
    .total_lines = 102,
    .data_size = 5424,
    .line_table = 1,
    .data = BINARY_RESOURCES + OFFSET_GXIMG_WATCHFACE_HAND_MIN_48
};

const lv_gximg_dsc_t wd_gximg_watchface_hand_min_54 = {
    .header.always_zero = 0,
    .header.w = 86,
    .header.h = 112,
    .header.cf = LV_IMG_CF_GDX_GXIMG,
    .center_x = 5,
    .center_y = 108,
    .start_y = 2,
    .total_lines = 110,
    .data_size = 5728,
    .line_table = 1,
    .data = BINARY_RESOURCES + OFFSET_GXIMG_WATCHFACE_HAND_MIN_54
};

const lv_gximg_dsc_t wd_gximg_watchface_hand_min_60 = {
    .header.always_zero = 0,
    .header.w = 76,
    .header.h = 118,
    .header.cf = LV_IMG_CF_GDX_GXIMG,
    .center_x = 6,
    .center_y = 114,
    .start_y = 2,
    .total_lines = 116,
    .data_size = 5692,
    .line_table = 1,
    .data = BINARY_RESOURCES + OFFSET_GXIMG_WATCHFACE_HAND_MIN_60
};

const lv_gximg_dsc_t wd_gximg_watchface_hand_min_66 = {
    .header.always_zero = 0,
    .header.w = 64,
    .header.h = 122,
    .header.cf = LV_IMG_CF_GDX_GXIMG,
    .center_x = 6,
    .center_y = 119,
    .start_y = 2,
    .total_lines = 122,
    .data_size = 5660,
    .line_table = 1,
    .data = BINARY_RESOURCES + OFFSET_GXIMG_WATCHFACE_HAND_MIN_66
};

const lv_gximg_dsc_t wd_gximg_watchface_hand_min_72 = {
    .header.always_zero = 0,
    .header.w = 52,
    .header.h = 126,
    .header.cf = LV_IMG_CF_GDX_GXIMG,
    .center_x = 6,
    .center_y = 124,
    .start_y = 1,
    .total_lines = 125,
    .data_size = 5640,
    .line_table = 1,
    .data = BINARY_RESOURCES + OFFSET_GXIMG_WATCHFACE_HAND_MIN_72
};

const lv_gximg_dsc_t wd_gximg_watchface_hand_min_78 = {
    .header.always_zero = 0,
    .header.w = 40,
    .header.h = 128,
    .header.cf = LV_IMG_CF_GDX_GXIMG,
    .center_x = 7,
    .center_y = 127,
    .start_y = 1,
    .total_lines = 128,
    .data_size = 5636,
    .line_table = 1,
    .data = BINARY_RESOURCES + OFFSET_GXIMG_WATCHFACE_HAND_MIN_78
};

const lv_gximg_dsc_t wd_gximg_watchface_hand_min_84 = {
    .header.always_zero = 0,
    .header.w = 26,
    .header.h = 130,
    .header.cf = LV_IMG_CF_GDX_GXIMG,
    .center_x = 6,
    .center_y = 129,
    .start_y = 1,
    .total_lines = 129,
    .data_size = 5632,
    .line_table = 1,
    .data = BINARY_RESOURCES + OFFSET_GXIMG_WATCHFACE_HAND_MIN_84
};

const lv_gximg_dsc_t wd_gximg_watchface_hand_min_90 = {
    .header.always_zero = 0,
    .header.w = 12,
    .header.h = 128,
    .header.cf = LV_IMG_CF_GDX_GXIMG,
    .center_x = 6,
    .center_y = 128,
    .start_y = 0,
    .total_lines = 128,
    .data_size = 5592,
    .line_table = 1,
    .data = BINARY_RESOURCES + OFFSET_GXIMG_WATCHFACE_HAND_MIN_90
};

const lv_gximg_dsc_t wd_gximg_watchface_hand_second_00 = {
    .header.always_zero = 0,
    .header.w = 177,
    .header.h = 5,
    .header.cf = LV_IMG_CF_GDX_GXIMG,
    .center_x = 25,
    .center_y = 2,
    .start_y = 0,
    .total_lines = 4,
    .data_size = 1212,
    .line_table = 1,
    .data = BINARY_RESOURCES + OFFSET_GXIMG_WATCHFACE_HAND_SECOND_00
};

const lv_gximg_dsc_t wd_gximg_watchface_hand_second_06 = {
    .header.always_zero = 0,
    .header.w = 177,
    .header.h = 25,
    .header.cf = LV_IMG_CF_GDX_GXIMG,
    .center_x = 25,
    .center_y = 19,
    .start_y = 1,
    .total_lines = 24,
    .data_size = 1892,
    .line_table = 1,
    .data = BINARY_RESOURCES + OFFSET_GXIMG_WATCHFACE_HAND_SECOND_06
};

const lv_gximg_dsc_t wd_gximg_watchface_hand_second_12 = {
    .header.always_zero = 0,
    .header.w = 175,
    .header.h = 43,
    .header.cf = LV_IMG_CF_GDX_GXIMG,
    .center_x = 25,
    .center_y = 35,
    .start_y = 1,
    .total_lines = 42,
    .data_size = 2092,
    .line_table = 1,
    .data = BINARY_RESOURCES + OFFSET_GXIMG_WATCHFACE_HAND_SECOND_12
};

const lv_gximg_dsc_t wd_gximg_watchface_hand_second_18 = {
    .header.always_zero = 0,
    .header.w = 171,
    .header.h = 61,
    .header.cf = LV_IMG_CF_GDX_GXIMG,
    .center_x = 25,
    .center_y = 50,
    .start_y = 1,
    .total_lines = 60,
    .data_size = 2232,
    .line_table = 1,
    .data = BINARY_RESOURCES + OFFSET_GXIMG_WATCHFACE_HAND_SECOND_18
};

const lv_gximg_dsc_t wd_gximg_watchface_hand_second_24 = {
    .header.always_zero = 0,
    .header.w = 165,
    .header.h = 77,
    .header.cf = LV_IMG_CF_GDX_GXIMG,
    .center_x = 24,
    .center_y = 64,
    .start_y = 1,
    .total_lines = 76,
    .data_size = 2416,
    .line_table = 1,
    .data = BINARY_RESOURCES + OFFSET_GXIMG_WATCHFACE_HAND_SECOND_24
};

const lv_gximg_dsc_t wd_gximg_watchface_hand_second_30 = {
    .header.always_zero = 0,
    .header.w = 157,
    .header.h = 93,
    .header.cf = LV_IMG_CF_GDX_GXIMG,
    .center_x = 24,
    .center_y = 78,
    .start_y = 1,
    .total_lines = 92,
    .data_size = 2580,
    .line_table = 1,
    .data = BINARY_RESOURCES + OFFSET_GXIMG_WATCHFACE_HAND_SECOND_30
};

const lv_gximg_dsc_t wd_gximg_watchface_hand_second_36 = {
    .header.always_zero = 0,
    .header.w = 147,
    .header.h = 109,
    .header.cf = LV_IMG_CF_GDX_GXIMG,
    .center_x = 22,
    .center_y = 92,
    .start_y = 1,
    .total_lines = 108,
    .data_size = 2648,
    .line_table = 1,
    .data = BINARY_RESOURCES + OFFSET_GXIMG_WATCHFACE_HAND_SECOND_36
};

const lv_gximg_dsc_t wd_gximg_watchface_hand_second_42 = {
    .header.always_zero = 0,
    .header.w = 135,
    .header.h = 123,
    .header.cf = LV_IMG_CF_GDX_GXIMG,
    .center_x = 20,
    .center_y = 104,
    .start_y = 1,
    .total_lines = 122,
    .data_size = 2732,
    .line_table = 1,
    .data = BINARY_RESOURCES + OFFSET_GXIMG_WATCHFACE_HAND_SECOND_42
};

const lv_gximg_dsc_t wd_gximg_watchface_hand_second_48 = {
    .header.always_zero = 0,
    .header.w = 123,
    .header.h = 135,
    .header.cf = LV_IMG_CF_GDX_GXIMG,
    .center_x = 19,
    .center_y = 115,
    .start_y = 1,
    .total_lines = 134,
    .data_size = 2772,
    .line_table = 1,
    .data = BINARY_RESOURCES + OFFSET_GXIMG_WATCHFACE_HAND_SECOND_48
};

const lv_gximg_dsc_t wd_gximg_watchface_hand_second_54 = {
    .header.always_zero = 0,
    .header.w = 109,
    .header.h = 147,
    .header.cf = LV_IMG_CF_GDX_GXIMG,
    .center_x = 17,
    .center_y = 125,
    .start_y = 1,
    .total_lines = 146,
    .data_size = 2988,
    .line_table = 1,
    .data = BINARY_RESOURCES + OFFSET_GXIMG_WATCHFACE_HAND_SECOND_54
};

const lv_gximg_dsc_t wd_gximg_watchface_hand_second_60 = {
    .header.always_zero = 0,
    .header.w = 93,
    .header.h = 157,
    .header.cf = LV_IMG_CF_GDX_GXIMG,
    .center_x = 15,
    .center_y = 133,
    .start_y = 1,
    .total_lines = 156,
    .data_size = 3156,
    .line_table = 1,
    .data = BINARY_RESOURCES + OFFSET_GXIMG_WATCHFACE_HAND_SECOND_60
};

const lv_gximg_dsc_t wd_gximg_watchface_hand_second_66 = {
    .header.always_zero = 0,
    .header.w = 77,
    .header.h = 165,
    .header.cf = LV_IMG_CF_GDX_GXIMG,
    .center_x = 13,
    .center_y = 141,
    .start_y = 1,
    .total_lines = 164,
    .data_size = 3312,
    .line_table = 1,
    .data = BINARY_RESOURCES + OFFSET_GXIMG_WATCHFACE_HAND_SECOND_66
};

const lv_gximg_dsc_t wd_gximg_watchface_hand_second_72 = {
    .header.always_zero = 0,
    .header.w = 61,
    .header.h = 171,
    .header.cf = LV_IMG_CF_GDX_GXIMG,
    .center_x = 11,
    .center_y = 146,
    .start_y = 1,
    .total_lines = 170,
    .data_size = 3428,
    .line_table = 1,
    .data = BINARY_RESOURCES + OFFSET_GXIMG_WATCHFACE_HAND_SECOND_72
};

const lv_gximg_dsc_t wd_gximg_watchface_hand_second_78 = {
    .header.always_zero = 0,
    .header.w = 43,
    .header.h = 175,
    .header.cf = LV_IMG_CF_GDX_GXIMG,
    .center_x = 8,
    .center_y = 150,
    .start_y = 1,
    .total_lines = 174,
    .data_size = 3500,
    .line_table = 1,
    .data = BINARY_RESOURCES + OFFSET_GXIMG_WATCHFACE_HAND_SECOND_78
};

const lv_gximg_dsc_t wd_gximg_watchface_hand_second_84 = {
    .header.always_zero = 0,
    .header.w = 25,
    .header.h = 177,
    .header.cf = LV_IMG_CF_GDX_GXIMG,
    .center_x = 6,
    .center_y = 152,
    .start_y = 0,
    .total_lines = 177,
    .data_size = 3564,
    .line_table = 1,
    .data = BINARY_RESOURCES + OFFSET_GXIMG_WATCHFACE_HAND_SECOND_84
};

const lv_gximg_dsc_t wd_gximg_watchface_hand_second_90 = {
    .header.always_zero = 0,
    .header.w = 5,
    .header.h = 177,
    .header.cf = LV_IMG_CF_GDX_GXIMG,
    .center_x = 2,
    .center_y = 152,
    .start_y = 0,
    .total_lines = 177,
    .data_size = 2220,
    .line_table = 1,
    .data = BINARY_RESOURCES + OFFSET_GXIMG_WATCHFACE_HAND_SECOND_90
};


const lv_gximg_bundle_dsc_t wd_gximg_watchface_hand_hour_bundle = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_GDX_GXIMG_BUNDLE,
    .group = {
        &wd_gximg_watchface_hand_hour_00,
        &wd_gximg_watchface_hand_hour_06,
        &wd_gximg_watchface_hand_hour_12,
        &wd_gximg_watchface_hand_hour_18,
        &wd_gximg_watchface_hand_hour_24,
        &wd_gximg_watchface_hand_hour_30,
        &wd_gximg_watchface_hand_hour_36,
        &wd_gximg_watchface_hand_hour_42,
        &wd_gximg_watchface_hand_hour_48,
        &wd_gximg_watchface_hand_hour_54,
        &wd_gximg_watchface_hand_hour_60,
        &wd_gximg_watchface_hand_hour_66,
        &wd_gximg_watchface_hand_hour_72,
        &wd_gximg_watchface_hand_hour_78,
        &wd_gximg_watchface_hand_hour_84,
        &wd_gximg_watchface_hand_hour_90,
    },
};

const lv_gximg_bundle_dsc_t wd_gximg_watchface_hand_min_bundle = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_GDX_GXIMG_BUNDLE,
    .group = {
        &wd_gximg_watchface_hand_min_00,
        &wd_gximg_watchface_hand_min_06,
        &wd_gximg_watchface_hand_min_12,
        &wd_gximg_watchface_hand_min_18,
        &wd_gximg_watchface_hand_min_24,
        &wd_gximg_watchface_hand_min_30,
        &wd_gximg_watchface_hand_min_36,
        &wd_gximg_watchface_hand_min_42,
        &wd_gximg_watchface_hand_min_48,
        &wd_gximg_watchface_hand_min_54,
        &wd_gximg_watchface_hand_min_60,
        &wd_gximg_watchface_hand_min_66,
        &wd_gximg_watchface_hand_min_72,
        &wd_gximg_watchface_hand_min_78,
        &wd_gximg_watchface_hand_min_84,
        &wd_gximg_watchface_hand_min_90,
    },
};

const lv_gximg_bundle_dsc_t wd_gximg_watchface_hand_second_bundle = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_GDX_GXIMG_BUNDLE,
    .group = {
        &wd_gximg_watchface_hand_second_00,
        &wd_gximg_watchface_hand_second_06,
        &wd_gximg_watchface_hand_second_12,
        &wd_gximg_watchface_hand_second_18,
        &wd_gximg_watchface_hand_second_24,
        &wd_gximg_watchface_hand_second_30,
        &wd_gximg_watchface_hand_second_36,
        &wd_gximg_watchface_hand_second_42,
        &wd_gximg_watchface_hand_second_48,
        &wd_gximg_watchface_hand_second_54,
        &wd_gximg_watchface_hand_second_60,
        &wd_gximg_watchface_hand_second_66,
        &wd_gximg_watchface_hand_second_72,
        &wd_gximg_watchface_hand_second_78,
        &wd_gximg_watchface_hand_second_84,
        &wd_gximg_watchface_hand_second_90,
    },
};


