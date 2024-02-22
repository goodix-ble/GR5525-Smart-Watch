#include "gr_includes.h"
#include "hal_flash.h"
#include "custom_config.h"
#include "bt_ctl_call_interface.h"
#include "bt_ctl_audio_interface.h"
#include "bt_ctl_ind_parse.h"
#include "app_log.h"
#include "app_uart.h"

extern void (*advertising_start_func_p)(void) ;
extern uint16_t bt_test_mode_l_cnt;
extern uint16_t bt_test_mode_s_cnt;

void app_bt_profile_state(bt_rx_info *rx_info);
void app_bt_connect_state(bt_rx_info *rx_info);
void app_bt_bd_addr(bt_rx_info *rx_info);
void app_bt_device_name(bt_rx_info *rx_info);
void app_bt_fw_version_info(bt_rx_info *rx_info);
void app_bt_vol_level_info(bt_rx_info *rx_info);
void app_bt_mic_gain_info(bt_rx_info *rx_info);
void app_bt_addr_book_info(bt_rx_info *rx_info);
void app_bt_call_line_state(bt_rx_info *rx_info);
void app_bt_vol_site_info(bt_rx_info *rx_info);
void app_bt_audio_song_info(bt_rx_info *rx_info);
void app_bt_audio_song_word_info(bt_rx_info *rx_info);
void app_bt_audio_state(bt_rx_info *rx_info);
void app_bt_power_on_state(bt_rx_info *rx_info);
void app_bt_pair_device_info(bt_rx_info *rx_info);
void app_bt_call_offset_info(bt_rx_info *rx_info);
void app_bt_log_state(bt_rx_info *rx_info);
void app_bt_inquiry_page_state(bt_rx_info *rx_info);
void app_bt_sniff_state(bt_rx_info *rx_info);
void app_bt_hfp_vol_level_info(bt_rx_info *rx_info);
void app_bt_audio_vol_lvl_info(bt_rx_info *rx_info);
void app_bt_hfp_gain_info(bt_rx_info *rx_info);
void app_bt_audio_gain_info(bt_rx_info *rx_info);
void app_bt_vol_mute_state(bt_rx_info *rx_info);
void app_bt_mic_mute_state(bt_rx_info *rx_info);
void app_bt_local_voice_state(bt_rx_info *rx_info);
void app_bt_siri_state(bt_rx_info *rx_info);
void app_bt_notice_vol_level_info(bt_rx_info *rx_info);

bool bt_sniff_state = false;

app_bt_evt_handler_st app_bt_event_handler[IND_MESSAGE_MAX] =
{
    [IND_PROFILE_STATE]        = {app_bt_profile_state,        2},
    [IND_CONNECTION_STATE]     = {app_bt_connect_state,        7},
    [IND_BT_BD_ADDR]           = {app_bt_bd_addr,              6},
    [IND_BT_DEV_NAME]          = {app_bt_device_name,          BT_MAX_RX_LEN},
    [IND_FW_VERSION_INFO]      = {app_bt_fw_version_info,      BT_MAX_RX_LEN},
    [IND_VOL_LEVEL_INFO]       = {app_bt_vol_level_info,       2},
    [IND_MIC_GAIN_INFO]        = {app_bt_mic_gain_info,        2},
    [IND_ADDR_BOOK_INFO]       = {app_bt_addr_book_info,       BT_MAX_RX_LEN},
    [IND_CALL_LINE_STATE]      = {app_bt_call_line_state,      BT_MAX_RX_LEN},
    [IND_VOL_SITE_INFO]        = {app_bt_vol_site_info,        1},
    [IND_AUDIO_SONG_INFO]      = {app_bt_audio_song_info,      BT_MAX_RX_LEN},
    [IND_AUDIO_SONG_WORD_INFO] = {app_bt_audio_song_word_info, BT_MAX_RX_LEN},
    [IND_AUDIO_STATE]          = {app_bt_audio_state,          BT_MAX_RX_LEN},
    [IND_POWER_ON_STATE]       = {app_bt_power_on_state,       1},
    [IND_PAIR_DEV_INFO]        = {app_bt_pair_device_info,     BT_MAX_RX_LEN},
    [IND_BT_CALL_OFFSET]       = {app_bt_call_offset_info,     1},
    [IND_BT_LOG_STATE]         = {app_bt_log_state,            1},
    [IND_BT_INQ_PAGE_STATE]    = {app_bt_inquiry_page_state,   4},
    [IND_BT_SNIFF_STATE]       = {app_bt_sniff_state,          1},
    [IND_HFP_VOL_INFO]         = {app_bt_hfp_vol_level_info,   1},
    [IND_AUDIO_VOL_INFO]       = {app_bt_audio_vol_lvl_info,   1},
    [IND_HFP_GAIN_INFO]        = {app_bt_hfp_gain_info,        51},
    [IND_AUDIO_GAIN_INFO]      = {app_bt_audio_gain_info,      51},
    [IND_VOL_MUTE_INFO]        = {app_bt_vol_mute_state,       1},
    [IND_MIC_MUTE_INFO]        = {app_bt_mic_mute_state,       1},
    [IND_LOCAL_VOICE_STATE]    = {app_bt_local_voice_state,    1},
    [IND_SIRI_STATE]           = {app_bt_siri_state,    1},
    [IND_NOTICE_LVL_STATE]           = {app_bt_notice_vol_level_info,    1},
#if APP_LOG_ENABLE
    [IND_PROFILE_STATE].log_info                    = "PROFILE_STATE",
    [IND_CONNECTION_STATE].log_info                 = "CONNECTION_STATE",
    [IND_BT_BD_ADDR].log_info                       = "BT_BD_ADDR",
    [IND_BT_DEV_NAME].log_info                      = "BT_DEV_NAME",
    [IND_FW_VERSION_INFO].log_info                  = "FW_VERSION_INFO",
    [IND_VOL_LEVEL_INFO].log_info                   = "VOL_LEVEL_INFO",
    [IND_MIC_GAIN_INFO].log_info                    = "MIC_GAIN_INFO",
    [IND_ADDR_BOOK_INFO].log_info                   = "ADDR_BOOK_INFO",
    [IND_CALL_LINE_STATE].log_info                  = "CALL_LINE_STATE",
    [IND_VOL_SITE_INFO].log_info                    = "VOL_SITE_INFO",
    [IND_AUDIO_SONG_INFO].log_info                  ="AUDIO_SONG_INFO",
    [IND_AUDIO_SONG_WORD_INFO].log_info             = "AUDIO_SONG_WORD_INFO",
    [IND_AUDIO_STATE].log_info                      = "AUDIO_STATE",
    [IND_POWER_ON_STATE].log_info                   = "POWER_ON_STATE",
    [IND_PAIR_DEV_INFO].log_info                    = "PAIR_DEV_INFO",
    [IND_BT_CALL_OFFSET].log_info                   = "BT_CALL_OFFSET",
    [IND_BT_LOG_STATE].log_info                     = "BT_LOG_STATE",
    [IND_BT_INQ_PAGE_STATE].log_info                = "BT_INQ_PAGE_STATE",
    [IND_BT_SNIFF_STATE].log_info                   = "BT_INQ_SNIFF_STATE",
    [IND_HFP_VOL_INFO].log_info                     = "HFP_VOL_INFO",
    [IND_AUDIO_VOL_INFO].log_info                   = "AUDIO_VOL_INFO",
    [IND_HFP_GAIN_INFO].log_info                    = "HFP_GAIN_INFO",
    [IND_AUDIO_GAIN_INFO].log_info                  = "AUDIO_GAIN_INFO",
    [IND_VOL_MUTE_INFO].log_info                    = "VOL_MUTE_INFO",
    [IND_MIC_MUTE_INFO].log_info                    = "MIC_MUTE_INFO",
    [IND_LOCAL_VOICE_STATE].log_info                = "LOCAL_VOICE_STATE",
    [IND_SIRI_STATE].log_info                       = "SIRI_STATE",
    [IND_NOTICE_LVL_STATE].log_info                       = "NOTICE_LVL_STATE",
#endif
};

__WEAK void app_bt_profile_state(bt_rx_info *rx_info)
{
    APP_LOG_DEBUG("%s:", __func__);
    APP_LOG_DEBUG("A2DP %s, HFP %s.", rx_info->data[0]?"connected state":"unconnected state", rx_info->data[1]?"connected state":"unconnected state");
}

__WEAK void app_bt_connect_state(bt_rx_info *rx_info)
{
    APP_LOG_DEBUG("%s: connected to device: %02X:%02X:%02X:%02X:%02X:%02X status: %s ",
                        __func__,
                        rx_info->data[5],
                        rx_info->data[4],
                        rx_info->data[3],
                        rx_info->data[2],
                        rx_info->data[1],
                        rx_info->data[0],
                        rx_info->data[6]?"connected":"disconnected");
}
__WEAK void app_bt_bd_addr(bt_rx_info *rx_info)
{
    APP_LOG_DEBUG("BT address report: %02X:%02X:%02X:%02X:%02X:%02X",
                        rx_info->data[5],
                        rx_info->data[4],
                        rx_info->data[3],
                        rx_info->data[2],
                        rx_info->data[1],
                        rx_info->data[0]);

    app_bt_device_name_get();
}

__WEAK void app_bt_device_name(bt_rx_info *rx_info)
{
    uint8_t i = 0;
    APP_LOG_DEBUG("%s:", __func__);
    while(rx_info->length > i)
    {
        APP_LOG_RAW_INFO("%c", rx_info->data[i]);
        i++;
    }
    APP_LOG_RAW_INFO(" \r\n");
}

__WEAK void app_bt_fw_version_info(bt_rx_info *rx_info)
{
    BLE_BT_VERSION_T version_bt;
    APP_LOG_RAW_INFO("%s:", __func__);
    
    memcpy(&version_bt, rx_info->data, 4);

    APP_LOG_RAW_INFO("bt FW Version is %d.%d.%d.%d %c V%d.%d ", version_bt.v1, version_bt.v2, version_bt.v3, version_bt.v4, version_bt.place_c, version_bt.app_v1, version_bt.app_v2);

    APP_LOG_RAW_INFO("\r\n");
}

__WEAK void app_bt_vol_level_info(bt_rx_info *rx_info)
{
    APP_LOG_RAW_INFO("%s: current volume : 0x%02x ", __func__, rx_info->data[1]);
    switch(rx_info->data[0])
    {
        case AUDIO_VOLUME:
            APP_LOG_RAW_INFO("audio volume.\r\n");
            break;
        case HFP_VOLUME:
            APP_LOG_RAW_INFO("hfp volume.\r\n");
            break;
    }
}

__WEAK void app_bt_mic_gain_info(bt_rx_info *rx_info)
{
    APP_LOG_DEBUG("%s: Ana gian 0x%02x, Dig gain is 0x%02x ", __func__, rx_info->data[0], rx_info->data[1]);
}

__WEAK void app_bt_audio_vol_lvl_info(bt_rx_info *rx_info)
{
    APP_LOG_DEBUG("%s: audio volume : 0x%02x ", __func__, rx_info->data[0]);
}

__WEAK void app_bt_audio_gain_info(bt_rx_info *rx_info)
{
    uint8_t level = 0;
    APP_LOG_DEBUG("%s: \r\n", __func__);
    for (level = 0; level <= 16; level ++)
    {
        APP_LOG_DEBUG("\t level %d, \tAna %d, \tDig %d \r\n", rx_info->data[level*3], (int8_t)rx_info->data[level*3 + 1], (int8_t)rx_info->data[level*3 + 2]);
         
    }
}

__WEAK void app_bt_hfp_vol_level_info(bt_rx_info *rx_info)
{
    APP_LOG_DEBUG("%s: hfp volume : 0x%02x ", __func__, rx_info->data[0]);
}

__WEAK void app_bt_hfp_gain_info(bt_rx_info *rx_info)
{
    uint8_t level = 0;
    APP_LOG_DEBUG("%s: \r\n", __func__);
    for (level = 0; level <= 16; level ++)
    {
        APP_LOG_DEBUG("\t level %d, \tAna %d, \tDig %d \r\n", rx_info->data[level*3], (int8_t)rx_info->data[level*3 + 1], (int8_t)rx_info->data[level*3 + 2]);
    }
}

__WEAK void app_bt_vol_mute_state(bt_rx_info *rx_info)
{
    switch(rx_info->data[0])
    {
        case MUTE_STATE:
            APP_LOG_DEBUG("%s: vol mute state", __func__);
            break;
        case UNMUTE_STATE:
            APP_LOG_DEBUG("%s: vol unmute state ", __func__);
            break;
        default:
            break;
    }
}

__WEAK void app_bt_mic_mute_state(bt_rx_info *rx_info)
{
    switch(rx_info->data[0])
    {
        case MUTE_STATE:
            APP_LOG_DEBUG("%s: mic mute state", __func__);
            break;
        case UNMUTE_STATE:
            APP_LOG_DEBUG("%s: mic unmute state ", __func__);
            break;
        default:
            break;
    }
}

__WEAK void app_bt_local_voice_state(bt_rx_info *rx_info)
{
    BT_AUDIO_STATUS_T audio_state = (BT_AUDIO_STATUS_T)rx_info->data[0];
    APP_LOG_DEBUG("%s:", __func__);
    switch (audio_state)
    {
        case AUDIO_PLAY_STATE:
            APP_LOG_DEBUG("local playing state !!!");
            break;
        case AUDIO_PAUSE_STATE:
            APP_LOG_DEBUG("local pause state !!!");
            break;
        default:
            APP_LOG_DEBUG("audio state unknow :%d", rx_info->data[0]);
            break;
    }
}

__WEAK void app_bt_siri_state(bt_rx_info *rx_info)
{
    BT_AUDIO_STATUS_T siri_state = (BT_AUDIO_STATUS_T)rx_info->data[0];
    APP_LOG_DEBUG("%s:", __func__);
    switch (siri_state)
    {
        case SIRI_ON_STATE:
            APP_LOG_DEBUG("siri on state !!!");
            break;
        case SIRI_OFF_STATE:
            APP_LOG_DEBUG("siri off state !!!");
            break;
        default:
            APP_LOG_DEBUG("siri state unknow :%d", rx_info->data[0]);
            break;
    }
}

__WEAK void app_bt_addr_book_info(bt_rx_info *rx_info)
{
    uint8_t i = 0;
    APP_LOG_RAW_INFO("%s:", __func__);
    while(rx_info->length > i)
    {
        APP_LOG_RAW_INFO("%c", rx_info->data[i]);
        i++;
    }
    APP_LOG_RAW_INFO("\r\n");
}

__WEAK void app_bt_call_line_state(bt_rx_info *rx_info)
{
    uint8_t i = 1;

    APP_LOG_DEBUG("%s: %d", __func__, rx_info->data[0]);
    switch(rx_info->data[0])
    {
        case CALL_ACCEPT_STATE:
            APP_LOG_DEBUG("Accept incomming call number :");
            break;
        case CALL_HOLD_STATE:
            APP_LOG_DEBUG("Hold incomming call number :");
            break;
        case CALL_END_STATE:
            APP_LOG_DEBUG("End incomming call number :");
            break;
        case CALL_INCOMING_STATE:
            APP_LOG_DEBUG("Ring incomming call number :");
            break;
        case CALL_DIAL_STATE:
            APP_LOG_DEBUG("phone outcomming call number :");
            break;
        case CALL_DIAL_CANCEL_STATE:
            APP_LOG_DEBUG("phone outcomming call cancel :");
            break;
        default:
            break;
    }

    while(rx_info->length > i)
    {
        APP_LOG_RAW_INFO("%c", rx_info->data[i]);
        i++;
    }
    APP_LOG_RAW_INFO(" \r\n");

}

__WEAK void app_bt_vol_site_info(bt_rx_info *rx_info)
{
    APP_LOG_DEBUG("%s:", __func__);

    switch(rx_info->data[0])
    {
        case VOL_SITE_PHONE:
            APP_LOG_DEBUG("voice in phone site");
            break;
        case VOL_SITE_DEVICE:
            APP_LOG_DEBUG("voice in device site");
            break;
        default:
            break;
    }
}

__WEAK void app_bt_audio_song_info(bt_rx_info *rx_info)
{
    uint8_t i = 0;
    APP_LOG_DEBUG("%s:", __func__);
    while(rx_info->length > i)
    {
        //should be format utf-8
        APP_LOG_RAW_INFO("%c", rx_info->data[i]);
        i++;
    }
    APP_LOG_RAW_INFO("\r\n");
}

__WEAK void app_bt_audio_song_word_info(bt_rx_info *rx_info)
{
    uint8_t i = 0;
    APP_LOG_RAW_INFO("%s: \t\t\t", __func__);
    while(rx_info->length > i)
    {
        APP_LOG_RAW_INFO("%c", rx_info->data[i]);
        i++;
    }
    APP_LOG_RAW_INFO("\r\n");
}

__WEAK void app_bt_audio_state(bt_rx_info *rx_info)
{
    BT_AUDIO_STATUS_T audio_state = (BT_AUDIO_STATUS_T)rx_info->data[0];
    APP_LOG_DEBUG("%s:", __func__);
    switch (audio_state)
    {
        case AUDIO_PLAY_STATE:
            APP_LOG_DEBUG("audio playing state !!!");
            break;
        case AUDIO_PAUSE_STATE:
            APP_LOG_DEBUG("audio pause state !!!");
            break;
        default:
            APP_LOG_DEBUG("audio state unknow :%d", rx_info->data[0]);
            break;
    }
}

__WEAK void app_bt_power_on_state(bt_rx_info *rx_info)
{
    app_bt_addr_get();

    APP_LOG_DEBUG("%s: power on now!!!", __func__);
}

__WEAK void app_bt_pair_device_info(bt_rx_info *rx_info)
{
    uint8_t * dev_name = &rx_info->data[10];
    uint16_t dev_name_len = rx_info->length - 10;

    if(rx_info->length > 0)
    {
        APP_LOG_DEBUG("%s: %s device: %02X:%02X:%02X:%02X:%02X:%02X, cod = 0x%06X",
                                        __func__,
                                        rx_info->data[0] ? "new bonding":"un-binding",
                                        rx_info->data[1], rx_info->data[2], rx_info->data[3], rx_info->data[4], rx_info->data[5],rx_info->data[6],
                                        (((uint32_t)rx_info->data[7]) | (((uint32_t)rx_info->data[8])<<8) | (((uint32_t)rx_info->data[9])<<16)));

        if (dev_name_len)
        {
            APP_LOG_RAW_INFO("device name: ");
            while(dev_name_len)
            {
                APP_LOG_RAW_INFO("%c", *dev_name);
                dev_name_len--;
                dev_name++;
            }
        }
    }
    else
    {
        APP_LOG_DEBUG("no more pair device info !!!");
    }
}

__WEAK void app_bt_notice_vol_level_info(bt_rx_info *rx_info)
{
    APP_LOG_DEBUG("%s: notice volume : 0x%02x ", __func__, rx_info->data[0]);
}

__WEAK void app_bt_call_offset_info(bt_rx_info *rx_info)
{
    APP_LOG_DEBUG("%s, offset = %d ", __func__, rx_info->data[0]);
}

__WEAK void app_bt_log_state(bt_rx_info *rx_info)
{
    APP_LOG_DEBUG("%s, log state = %d ", __func__, rx_info->data[0]);
}

__WEAK void app_bt_inquiry_page_state(bt_rx_info *rx_info)
{
    APP_LOG_DEBUG("%s, inquiry %s , inquiry scan %s , page %s , page scan %s ,", __func__,
                                                                                                            rx_info->data[0]?"on":"off",
                                                                                                            rx_info->data[1]?"on":"off",
                                                                                                            rx_info->data[2]?"on":"off",
                                                                                                            rx_info->data[3]?"on":"off");
}

__WEAK void app_bt_sniff_state(bt_rx_info *rx_info)
{
    APP_LOG_DEBUG("%s : enter sniff mode", __func__, rx_info->data[0]);
    bt_sniff_state = true;
}

void app_bt_ind_handle(bt_rx_info *g_bt_rx_info_p)
{
    // test mode ind message
    if (g_bt_rx_info_p->opcode == IND_BT_TEST_MSG)
    {
        if (g_bt_rx_info_p->length== BLE_BT_TEST_MODE_L_LEN)
        {
            bt_test_mode_l_cnt ++;
        }
        else
        {
            bt_test_mode_s_cnt++;
        }
        return ;
    }

    if (g_bt_rx_info_p->opcode >= IND_MESSAGE_MAX)
    {
        APP_LOG_DEBUG("wrong message:0x%02x\r\n", g_bt_rx_info_p->opcode);
        return;
    }

    APP_LOG_INFO("%s, length = %d", app_bt_event_handler[g_bt_rx_info_p->opcode].log_info, g_bt_rx_info_p->length);

    if (app_bt_event_handler[g_bt_rx_info_p->opcode].func_cb != NULL)
    {
        if (app_bt_event_handler[g_bt_rx_info_p->opcode].max_len >= g_bt_rx_info_p->length)
        {
            app_bt_event_handler[g_bt_rx_info_p->opcode].func_cb(g_bt_rx_info_p);
        }
        else
        {
             APP_LOG_DEBUG("this meeeage length err %d, should not more than %d", g_bt_rx_info_p->length, app_bt_event_handler[g_bt_rx_info_p->opcode].max_len);
        }
    }
    else
    {
        APP_LOG_DEBUG("not handle this event !!!");
    }
}

