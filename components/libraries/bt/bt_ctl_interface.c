#include "gr_includes.h"
#include "hal_flash.h"
#include "custom_config.h"
#include "bt_ctl_uart.h"
#include "bt_ctl_call_interface.h"
#include "bt_ctl_audio_interface.h"
#include "app_log.h"
#include "app_uart.h"
#include "bt_sync_tim.h"

uint16_t bt_test_mode_al_cnt = 0;
uint16_t bt_test_mode_l_cnt = 0;//long packet
uint16_t bt_test_mode_s_cnt = 0;//short packet
uint16_t bt_test_mode_w_cnt = 0;////wrong/other packet

void (*advertising_start_func_p)(void) = NULL;

extern uint8_t app_bt_calc_checksum(uint8_t *p_buf, uint16_t len);
extern void app_bt_ctl_cmd_confirm(bt_rx_info *rx_info);

extern bool bt_sniff_state;

app_bt_ack_handler_st app_bt_ack_handler =
{
    .func_cb = app_bt_ctl_cmd_confirm,
#if APP_LOG_ENABLE
    .log_info[OPCODE_A2DP_STATUS_SET]           = "A2DP_STATUS_SET",
    .log_info[OPCODE_PLAY_STATUS_SET]            = "PLAY_STATUS_SET",
    .log_info[OPCODE_PLAY_NEXT]                       ="PLAY_NEXT",
    .log_info[OPCODE_PLAY_PREV]                       ="PLAY_PREV",
    .log_info[OPCODE_PLAY_REWIND]                  = "PLAY_REWIND",
    .log_info[OPCODE_PLAY_FORWARD]               = "PLAY_FORWARD",
    .log_info[OPCODE_AUDIO_VOL_SET]               = "AUDIO_VOL_SET",
    .log_info[OPCODE_AUDIO_VOL_GET]               = "AUDIO_VOL_GET",

    .log_info[OPCODE_VOL_GAIN_SET]                = "VOL_GAIN_SET" ,
    .log_info[OPCODE_VOL_GAIN_GET]                = "VOL_GAIN_GET" ,
    .log_info[OPCODE_AUDIO_INFO_GET]            = "AUDIO_INFO_GET" ,
    .log_info[OPCODE_AUDIO_WORDS_GET]        = "AUDIO_WORDS_GET" ,

    /* phone call control opcode */
    .log_info[OPCODE_CALL_ADDR_BOOK_GET]   = "CALL_ADDR_BOOK_GET" ,
    .log_info[OPCODE_CALL_ACCEPT]                  =           "CALL_ACCEPT" ,
    .log_info[OPCODE_CALL_END]                        =           "CALL_END" ,
    .log_info[OPCODE_CALL_REJECT]                   =           "CALL_REJECT" ,
    .log_info[OPCODE_CALL_REDIAL]                   =           "CALL_REDIAL" ,
    .log_info[OPCODE_CALL_VOL_SET]                =           "CALL_VOL_SET" ,
    .log_info[OPCODE_CALL_VOL_GET]                =           "CALL_VOL_GET" ,
    .log_info[OPCODE_CALL_VOL_GAIN_SET]       =           "CALL_VOL_GAIN_SET" ,
    .log_info[OPCODE_CALL_VOL_GAIN_GET]      =           "CALL_VOL_GAIN_GET" ,
    .log_info[OPCODE_CALL_VOL_SWITCH]         =           "CALL_VOL_SWITCH" ,
    .log_info[OPCODE_CALL_HOLD_FIR_ACCEPT_SEC]  =           "CALL_HOLD_FIR_ACCEPT_SEC" ,
    .log_info[OPCODE_CALL_END_FIR_ACCEPT_SEC]   =           "CALL_END_FIR_ACCEPT_SEC" ,
    .log_info[OPCODE_CALL_HOLD_FIR_REJECT_SEC]  =           "CALL_HOLD_FIR_REJECT_SEC" ,
    .log_info[OPCODE_CALL_LINE_SWITCH]        =           "CALL_LINE_SWITCH" ,
    .log_info[OPCODE_CALL_NUMBER_GET]         =           "CALL_NUMBER_GET" ,
    .log_info[OPCODE_CALL_DIAL_NUMBER]        =           "CALL_DIAL_NUMBER" ,
    .log_info[OPCODE_CALL_RING_STATE_SET]   =           "CALL_RING_STATE_SET" ,
    .log_info[OPCODE_CALL_RING_STATE_GET]   =           "CALL_RING_STATE_GET" ,
    .log_info[OPCODE_CALL_DTMF_SET]              =           "CALL_DTMF_SET" ,

    /* bt common control opcode */
    .log_info[OPCODE_BT_RESET]                        =           "BT_RESET" ,
    .log_info[OPCODE_BT_INQUIRY_SCAN]          =           "BT_INQUIY_SCAN" ,
    .log_info[OPCODE_BT_CONNECT_ABORT]      =           "BT_CONNECT_ABORT" ,
    .log_info[OPCODE_BT_PAGE_SCAN]               =           "BT_PAGE_SCAN" ,
    .log_info[OPCODE_BT_PAIRING_BEGIN]         =           "BT_PAIRING_BEGIN" ,
    .log_info[OPCODE_BT_QUICK_CONNECT]      =           "BT_QUICK_CONNECT" ,
    .log_info[OPCODE_BT_DISCONNECT]             =           "BT_DISCONNECT" ,
    .log_info[OPCODE_BT_SWITCH_PHONE]         =           "BT_SWITCH_PHONE" ,
    .log_info[OPCODE_BT_PAIRING_INFO_STORE]     =           "BT_PAIRING_INFO_STORE" ,
    .log_info[OPCODE_BT_PAIRING_INFO_CLEAN]     =           "BT_PAIRING_INFO_CLEAN" ,
    .log_info[OPCODE_BT_QUICK_CON_MODE_SET]     =           "BT_QUICK_CON_MODE_SET" ,
    .log_info[OPCODE_BT_BD_ADDR_GET]           =                "BT_BD_ADDR_GET" ,
    .log_info[OPCODE_BT_DEVICE_NAME_SET]    =           "BT_DEVICE_NAME_SET" ,
    .log_info[OPCODE_BT_DEVICE_NAME_GET]    =               "BT_DEVICE_NAME_GET" ,

    .log_info[OPCODE_BT_THRPUT_BEGIN]           =           "BT_THROUGHPUT_BEGIN" ,
    .log_info[OPCODE_BT_THRPUT_DATA]             =           "BT_THROUGHPUT_DATA" ,
    .log_info[OPCODE_BT_THRPUT_END]               =           "BT_THROUGHPUT_END" ,

    .log_info[OPCODE_BT_FW_VERSION_GET]      =           "BT_FW_VERSION_GET" ,
    .log_info[OPCODE_BT_OTA_BEGIN]                 =           "BT_OTA_BEGIN" ,

    .log_info[OPCODE_BT_SIRI_WAKE]                  =           "BT_SIRI_WAKE" ,
    .log_info[OPCODE_BT_SWITCH_NOTICE]         =           "BT_SWITCH_NOTICE" ,
    .log_info[OPCODE_BT_MIC_MUTE]                   =           "BT_MIC_MUTE" ,
    .log_info[OPCODE_VOL_P]                               =           "VOL_P" ,
    .log_info[OPCODE_VOL_M]                              =           "VOL_M" ,
    .log_info[OPCODE_VOL_MUTE]                        =           "VOL_MUTE" ,
    .log_info[OPCODE_BT_MIC_GAIN_SET]           =           "BT_MIC_GAIN_SET" ,
    .log_info[OPCODE_BT_MIC_GAIN_GET]           =          "BT_MIC_GAIN_GET" ,
    .log_info[OPCODE_BT_MIC_TEST_START]        =           "BT_MIC_TEST_START" ,
    .log_info[OPCODE_BT_MIC_TEST_STOP]         =           "BT_MIC_TEST_STOP" ,
    .log_info[OPCODE_BT_MIC_AUTO_TEST]         =           "BT_MIC_AUTO_TEST" ,
    .log_info[OPCODE_BT_CUSTOM_NOTICE_PLAY]       =           "BT_CUSTOM_NOTICE_PLAY" ,
    .log_info[OPCODE_BT_CUSTOM_NOTICE_PAUSE]    =           "BT_CUSTOM_NOTICE_PAUSE" ,

    .log_info[OPCODE_BT_FACTORY_RESET]          =           "BT_FACTORY_RESET" ,
    .log_info[OPCODE_BT_POWERDOWN]              =           "BT_POWERDOWN" ,
    .log_info[OPCODE_BT_WAKEUP]                     =           "BT_WAKEUP" ,
    .log_info[OPCODE_BT_PAIR_DEV_LIST_GET]  =           "BT_PAIR_DEV_LIST_GET" ,
    .log_info[OPCODE_BT_CALL_OFFSET_SET]     =           "BT_CALL_OFFSET_SET" ,
    .log_info[OPCODE_BT_CALL_OFFSET_GET]     =           "BT_CALL_OFFSET_GET" ,
    .log_info[OPCODE_BT_LOG_CTRL]                  =           "BT_LOG_CTRL" ,
    .log_info[OPCODE_BT_BD_ADDR_SET]           =           "BT_BD_ADDR_SET" ,
    .log_info[OPCODE_BT_INQ_PAGE_GET]         =           "BT_INQ_PAGE_GET" ,
    .log_info[OPCODE_BT_CONN_STATE_GET]     =           "BT_CONN_STATE_GET" ,
    .log_info[OPCODE_BT_BATT_LV_SYNC]     =           "BT_BATT_LV_SYNC" ,
    .log_info[OPCODE_CUR_VOL_GET]     =           "CUR_VOL_GET" ,
    .log_info[OPCODE_VOL_MUTE_GET]     =           "VOL_MUTE_GET" ,
    .log_info[OPCODE_MIC_MUTE_GET]     =           "MIC_MUTE_GET" ,
    .log_info[OPCODE_DRAIN_VIBRA_START]     =           "DRAIN_VIBRA_START" ,
    .log_info[OPCODE_DRAIN_VIBRA_STOP]     =           "DRAIN_VIBRA_STOP" ,
    .log_info[OPCODE_SEARCH_DEV_START]     =           "SEARCH_DEV_START" ,
    .log_info[OPCODE_SEARCH_DEV_STOP]     =           "SEARCH_DEV_STOP" ,
    .log_info[OPCODE_BT_SIRI_SHUTDOWN]     =           "SIRI_SHUTDOWN" ,
    .log_info[OPCODE_BT_SIRI_STATE_GET]     =           "SIRI_STATE_GET" ,
    .log_info[OPCODE_BT_NOTICE_LVL_SET]     =           "NOTICE_LVL_SET" ,
    .log_info[OPCODE_BT_NOTICE_LVL_GET]     =           "NOTICE_LVL_GET" ,
#endif
};

static void app_bt_data_send(uint8_t opcode, uint16_t len, uint8_t *data)
{
    uint16_t i;
    uint8_t *ret_data = sys_malloc((BT_CTL_DATA_LEN_MIN + len) * sizeof(uint8_t));

    if (!(((len > 0) && (data != NULL)) || ((len == 0) && (data == NULL))))
    {
        APP_LOG_DEBUG("Invalid Command:length = %d, data_ptr=0x%04x", len, (uint32_t)data);
        return ;
    }

    // header
    ret_data[0] = BT_CTL_CMD_HEAD&0xff;
    ret_data[1] = BT_CTL_CMD_HEAD>>8;
    // opcode
    ret_data[2] = opcode;
    // length
    ret_data[3] = len&0xff;
    ret_data[4] = len>>8;

    if (len)
    {
        // data
        for (i = 0; i < len; i++)
        {
            ret_data[BT_CTL_DATA_INDEX+i] = data[i];
        }
    }

    // sum
#if SUM_CALCULATE
    ret_data[BT_CTL_DATA_INDEX+len] = app_bt_calc_checksum(ret_data, (BT_CTL_DATA_LEN_MIN + len - BT_CTL_CHECKSUM_LEN));
#endif

    watch_uart_data_tx_send(ret_data, (BT_CTL_DATA_LEN_MIN +len));
    sys_free(ret_data);
}

void app_bt_ack_handle(bt_rx_info *g_bt_rx_info_p)
{
    if (g_bt_rx_info_p->opcode >= OPCODE_BT_MAX)
    {
        APP_LOG_DEBUG("wrong opcode:0x%02x\r\n", g_bt_rx_info_p->opcode);
        return;
    }

    APP_LOG_INFO("%s, length = %d", app_bt_ack_handler.log_info[g_bt_rx_info_p->opcode], g_bt_rx_info_p->length);

    if (app_bt_ack_handler.func_cb != NULL)
    {
        if (g_bt_rx_info_p->length == 1)
        {
            app_bt_ack_handler.func_cb(g_bt_rx_info_p);
        }
        else
        {
             APP_LOG_DEBUG("this ack length err %d", g_bt_rx_info_p->length);
        }
    }
    else
    {
        APP_LOG_DEBUG("not handle this ack !!!");
    }
}

__WEAK void app_bt_ctl_cmd_confirm(bt_rx_info *rx_info)
{
    switch(rx_info->data[0])
    {
        case OPCODE_BT_NO_ERR:
                APP_LOG_DEBUG("cmd  %s !", "OPCODE_BT_NO_ERR");
            break;
        case OPCODE_BT_INVALID_PARAM:
                APP_LOG_DEBUG("cmd  %s !", "OPCODE_BT_INVALID_PARAM");
            break;
        case OPCODE_BT_A2DP_NO_CONN_STATE:
                APP_LOG_DEBUG("cmd  %s !", "OPCODE_BT_A2DP_NO_CONN_STATE");
            break;
        case OPCODE_BT_HFP_NO_CONN_STATE:
                APP_LOG_DEBUG("cmd  %s !", "OPCODE_BT_HFP_NO_CONN_STATE");
            break;
        case OPCODE_BT_NO_CALL_LINE:
                APP_LOG_DEBUG("cmd  %s !", "OPCODE_BT_NO_CALL_LINE");
            break;
        case OPCODE_BT_NO_CONN_STATE:
                APP_LOG_DEBUG("cmd  %s !", "OPCODE_BT_NO_CONN_STATE");
            break;
        case OPCODE_BT_PHONE_BOOK_ACCESS_DENIED:
                APP_LOG_DEBUG("cmd  %s !", "OPCODE_BT_PHONE_BOOK_ACCESS_DENIED");
            break;
        case OPCODE_BT_ADDRESS_INEXISTENCE:
                APP_LOG_DEBUG("cmd  %s !", "OPCODE_BT_ADDRESS_INEXISTENCE");
            break;
        case OPCODE_BT_OP_CONDITION_NOT_MET:
                APP_LOG_DEBUG("cmd  %s !", "OPCODE_BT_OP_CONDITION_NOT_MET");
            break;
        case OPCODE_BT_IS_CONNECTTED:
                APP_LOG_DEBUG("cmd  %s !", "OPCODE_BT_IS_CONNECTTED");
            break;
        case OPCODE_BT_NO_PAIR_RECORD:
                APP_LOG_DEBUG("cmd  %s !", "OPCODE_BT_NO_PAIR_RECORD");
            break;
        case OPCODE_BT_EXIST_CALL_LINE:
                APP_LOG_DEBUG("cmd  %s !", "OPCODE_BT_EXIST_CALL_LINE");
            break;
        default:
            APP_LOG_DEBUG("cmd unknow err code %d !", rx_info->data[0]);
            break;
    }
}

void app_bt_device_reset(void)
{
    app_bt_data_send(OPCODE_BT_RESET, 0, NULL);
}

void app_bt_device_inquiry_page_get(void)
{
    app_bt_data_send(OPCODE_BT_INQ_PAGE_GET, 0, NULL);
}

void app_bt_device_inquiry_scan_start(void)
{
    bool inquiry_scan_start = true;
    app_bt_data_send(OPCODE_BT_INQUIRY_SCAN, 1, (uint8_t *)&inquiry_scan_start);
}

void app_bt_device_inquiry_scan_stop(void)
{
    bool inquiry_scan_start = false;
    app_bt_data_send(OPCODE_BT_INQUIRY_SCAN, 1, (uint8_t *)&inquiry_scan_start);
}

void app_bt_conn_state_get(void)
{
    app_bt_data_send(OPCODE_BT_CONN_STATE_GET, 0, NULL);
}

void app_bt_connect_abort(void)
{
    app_bt_data_send(OPCODE_BT_CONNECT_ABORT, 0, NULL);
}

void app_bt_device_page_scan_start(void)
{
    bool page_scan_start = true;
    app_bt_data_send(OPCODE_BT_PAGE_SCAN, 1, (uint8_t *)&page_scan_start);
}

void app_bt_device_page_scan_stop(void)
{
    bool page_scan_start = false;
    app_bt_data_send(OPCODE_BT_PAGE_SCAN, 1, (uint8_t *)&page_scan_start);
}

void app_bt_quick_reconnect(void)
{
    app_bt_data_send(OPCODE_BT_QUICK_CONNECT, 0, NULL);
}

void app_bt_disconnect(void)
{
    app_bt_data_send(OPCODE_BT_DISCONNECT, 0, NULL);
}

void app_bt_pairing_info_store(uint8_t remote_addr[6], uint8_t link_key[16])
{
    uint8_t pairing_info[22] = {0,};

    memcpy(&pairing_info[0], remote_addr, 6);
    memcpy(&pairing_info[6], link_key, 16);
    app_bt_data_send(OPCODE_BT_PAIRING_INFO_STORE, 22, pairing_info);
}

void app_bt_pairing_info_clean(uint8_t remote_addr[6])
{
    app_bt_data_send(OPCODE_BT_PAIRING_INFO_CLEAN, 6, remote_addr);
}

void app_bt_enable_quick_connect(bool enable)
{
    app_bt_data_send(OPCODE_BT_QUICK_CON_MODE_SET, 1, (uint8_t *)&enable);
}

void app_bt_addr_get(void)
{
    app_bt_data_send(OPCODE_BT_BD_ADDR_GET, 0, NULL);
}

void app_bt_addr_set(uint8_t bdaddr[6])
{
    //valid after restart
    app_bt_data_send(OPCODE_BT_BD_ADDR_SET, 6, bdaddr);
}

void app_bt_device_name_set(uint8_t *device_name, uint8_t lenth)
{
    app_bt_data_send(OPCODE_BT_DEVICE_NAME_SET, lenth, device_name);
}

void app_bt_device_name_get(void)
{
    app_bt_data_send(OPCODE_BT_DEVICE_NAME_GET, 0, NULL);
}

void app_bt_software_version_get(void)
{
    app_bt_data_send(OPCODE_BT_FW_VERSION_GET, 0, NULL);
}

void app_bt_siri_wake_up(void)
{
    app_bt_data_send(OPCODE_BT_SIRI_WAKE, 0, NULL);
}

void app_bt_mic_mute(bool mute)
{
    app_bt_data_send(OPCODE_BT_MIC_MUTE, 1, (uint8_t *)&mute);
}

void app_bt_volp_action(void)
{
    app_bt_data_send(OPCODE_VOL_P, 0, NULL);
}

void app_bt_volm_action(void)
{
    app_bt_data_send(OPCODE_VOL_M, 0, NULL);
}

void app_bt_vol_mute(bool mute)
{
    app_bt_data_send(OPCODE_VOL_MUTE, 1, (uint8_t *)&mute);
}

void app_bt_mic_gain_set(uint8_t ana_gain, uint8_t dig_gain)
{
    uint8_t gain_level[2] = {0,0};
    gain_level[0] = ana_gain;
    gain_level[1] = dig_gain;
    app_bt_data_send(OPCODE_BT_MIC_GAIN_SET, 2, gain_level);
}

void app_bt_mic_gain_get(void)
{
    app_bt_data_send(OPCODE_BT_MIC_GAIN_GET, 0, NULL);
}

void app_bt_mic_test_start(void)
{
    app_bt_data_send(OPCODE_BT_MIC_TEST_START, 0, NULL);
}

void app_bt_mic_test_stop(void)
{
    app_bt_data_send(OPCODE_BT_MIC_TEST_STOP, 0, NULL);
}

void app_bt_device_factory_reset(void)
{
    app_bt_data_send(OPCODE_BT_FACTORY_RESET, 0, NULL);
}

void app_bt_powerdown_bt(void)
{
    app_bt_data_send(OPCODE_BT_POWERDOWN, 0, NULL);
    bt_sniff_state = true;
}

void app_bt_wakeup_bt(void)
{
    extern void ble_bt_reset_sync_io(void);
    extern void ble_bt_set_sync_io(void);

    if (bt_sniff_state)
    {
        APP_LOG_DEBUG("app_bt_wakeup_bt, OUTPUT: LOW");
        ble_bt_reset_sync_io();
        delay_ms(30);
        ble_bt_set_sync_io();
    }
    bt_sniff_state = false;
}

void app_bt_pair_list_get(void)
{
    app_bt_data_send(OPCODE_BT_PAIR_DEV_LIST_GET, 0, NULL);
}

void app_bt_freq_offset_set(uint8_t value)
{
    app_bt_data_send(OPCODE_BT_CALL_OFFSET_SET, 1, &value);
}

void app_bt_freq_offset_get(void)
{
    app_bt_data_send(OPCODE_BT_CALL_OFFSET_GET, 0, NULL);
}

void app_bt_log_ctrl_set(uint8_t value)
{
    app_bt_data_send(OPCODE_BT_LOG_CTRL, 1, &value);
}

void app_bt_call_address_book_get(void)
{
    app_bt_data_send(OPCODE_CALL_ADDR_BOOK_GET, 0, NULL);
}

void app_bt_call_accept(void)
{
    app_bt_data_send(OPCODE_CALL_ACCEPT, 0, NULL);
}

void app_bt_call_dtmf_set(uint8_t *buf, uint8_t length)
{
    app_bt_data_send(OPCODE_CALL_DTMF_SET, length, buf);
}

void app_bt_call_end(void)
{
    app_bt_data_send(OPCODE_CALL_END, 0, NULL);
}

void app_bt_call_reject(void)
{
    app_bt_data_send(OPCODE_CALL_REJECT, 0, NULL);
}

void app_bt_call_redial(void)
{
    app_bt_data_send(OPCODE_CALL_REDIAL, 0, NULL);
}

void app_bt_call_vol_level_set(uint8_t vol_level)
{
    app_bt_data_send(OPCODE_CALL_VOL_SET, 1, &vol_level);
}

void app_bt_call_vol_level_get(void)
{
    app_bt_data_send(OPCODE_CALL_VOL_GET, 0, NULL);
}

void app_bt_call_vol_gain_set(uint8_t vol_level, uint8_t ana_gain, uint8_t dig_gain)
{
    uint8_t gain_level[3] = {0,0,0};
    gain_level[0] = vol_level;
    gain_level[1] = ana_gain;
    gain_level[2] = dig_gain;
    app_bt_data_send(OPCODE_CALL_VOL_GAIN_SET, 3, gain_level);
}

void app_bt_call_vol_gain_get(void)
{
    app_bt_data_send(OPCODE_CALL_VOL_GAIN_GET, 0, NULL);
}

void app_bt_call_vol_switch(void)
{
    app_bt_data_send(OPCODE_CALL_VOL_SWITCH, 0, NULL);
}

void app_bt_three_way_call_ctl(BT_CALLING_STATUS_T first_state_set, BT_CALLING_STATUS_T second_state_set)
{
    if ((first_state_set == CALL_HOLD_STATE) && (second_state_set == CALL_ACCEPT_STATE))
    {
        app_bt_data_send(OPCODE_CALL_HOLD_FIR_ACCEPT_SEC, 0, NULL);
    }
    else if ((first_state_set == CALL_END_STATE) && (second_state_set == CALL_ACCEPT_STATE))
    {
        app_bt_data_send(OPCODE_CALL_END_FIR_ACCEPT_SEC, 0, NULL);
    }
    else if ((first_state_set == CALL_ACCEPT_STATE) && (second_state_set == CALL_REJECT_STATE))
    {
        app_bt_data_send(OPCODE_CALL_HOLD_FIR_REJECT_SEC, 0, NULL);
    }
    else
    {
        APP_LOG_DEBUG("%s err call cmd : first call %d, second call %d", __func__, first_state_set, second_state_set);
    }
}

void app_bt_three_way_call_switch(void)
{
    app_bt_data_send(OPCODE_CALL_LINE_SWITCH, 0, NULL);
}

void app_bt_calling_number_get(void)
{
    app_bt_data_send(OPCODE_CALL_NUMBER_GET, 0, NULL);
}

void app_bt_place_call_number(uint8_t *number, uint8_t lenth)
{
    app_bt_data_send(OPCODE_CALL_DIAL_NUMBER, lenth, number);
}

void app_bt_a2dp_status_set(void)
{
    app_bt_data_send(OPCODE_A2DP_STATUS_SET, 0, NULL);
}

void app_bt_avrcp_status_set(void)
{
    app_bt_data_send(OPCODE_PLAY_STATUS_SET, 0, NULL);
}

void app_bt_avrcp_play_next(void)
{
    app_bt_data_send(OPCODE_PLAY_NEXT, 0, NULL);
}

void app_bt_avrcp_play_prev(void)
{
    app_bt_data_send(OPCODE_PLAY_PREV, 0, NULL);
}

void app_bt_avrcp_play_rewind(void)
{
    app_bt_data_send(OPCODE_PLAY_REWIND, 0, NULL);
}

void app_bt_avrcp_play_forward(void)
{
    app_bt_data_send(OPCODE_PLAY_FORWARD, 0, NULL);
}

void app_bt_vol_level_set(uint8_t vol_level)
{
    app_bt_data_send(OPCODE_AUDIO_VOL_SET, 1, &vol_level);
}

void app_bt_vol_level_get(void)
{
    app_bt_data_send(OPCODE_AUDIO_VOL_GET, 0, NULL);
}

void app_bt_vol_gain_set(uint8_t vol_level, uint8_t ana_gain, uint8_t dig_gain)
{
    uint8_t gain_level[3] = {0,0,0};
    gain_level[0] = vol_level;
    gain_level[1] = ana_gain;
    gain_level[2] = dig_gain;
    app_bt_data_send(OPCODE_VOL_GAIN_SET, 3, gain_level);
}

void app_bt_vol_gain_get(void)
{
    app_bt_data_send(OPCODE_VOL_GAIN_GET, 0, NULL);
}

void app_bt_audio_info_get(void)
{
    app_bt_data_send(OPCODE_AUDIO_INFO_GET, 0, NULL);
}

void app_bt_batt_lvl_info_sync(uint8_t level)
{
    app_bt_data_send(OPCODE_BT_BATT_LV_SYNC, 1, &level);
}

void app_bt_cur_vol_get(void)
{
    app_bt_data_send(OPCODE_CUR_VOL_GET, 0, NULL);
}

void app_bt_vol_mute_get(void)
{
    app_bt_data_send(OPCODE_VOL_MUTE_GET, 0, NULL);
}

void app_bt_mic_mute_get(void)
{
    app_bt_data_send(OPCODE_MIC_MUTE_GET, 0, NULL);
}

void app_bt_drain_vibra_start(void)
{
    app_bt_data_send(OPCODE_DRAIN_VIBRA_START, 0, NULL);
}

void app_bt_drain_vibra_stop(void)
{
    app_bt_data_send(OPCODE_DRAIN_VIBRA_STOP, 0, NULL);
}

void app_bt_search_dev_start(void)
{
    app_bt_data_send(OPCODE_SEARCH_DEV_START, 0, NULL);
}

void app_bt_search_dev_stop(void)
{
    app_bt_data_send(OPCODE_SEARCH_DEV_STOP, 0, NULL);
}

void app_bt_notice_level_set(uint8_t vol_level)
{
    app_bt_data_send(OPCODE_BT_NOTICE_LVL_SET, 1, &vol_level);
}

void app_bt_notice_level_get(void)
{
    app_bt_data_send(OPCODE_BT_NOTICE_LVL_GET, 0, NULL);
}

void app_bt_siri_exit(void)
{
    app_bt_data_send(OPCODE_BT_SIRI_SHUTDOWN, 0, NULL);
}

void app_bt_siri_state_get(void)
{
    app_bt_data_send(OPCODE_BT_SIRI_STATE_GET, 0, NULL);
}

void app_bt_test_data_ctl(void)
{
    bt_test_mode_l_cnt = 0;
    bt_test_mode_s_cnt = 0;
    bt_test_mode_w_cnt = 0;
    bt_test_mode_al_cnt = 0;
    app_bt_data_send(OPCODE_BT_TEST_CMD, 0, NULL);
}

void app_bt_register_ble_adv_ptr(void (*ptr)(void ))
{
    advertising_start_func_p = ptr;
}

void app_bt_ultra_deep_sleep(void)
{
    uint8_t power_down_code[] = {0xBA, 0xAB, OPCODE_BT_POWERDOWN, 0x00, 0x00, 0x55};
    delay_ms(100);
    watch_uart_data_tx_send(power_down_code, sizeof(power_down_code));
    delay_ms(1000);
    app_uart_abort(WATCH_DEMO_UART_ID);
    app_uart_deinit(WATCH_DEMO_UART_ID);
    ble_bt_sync_io_deinit();
    ble_bt_resetpin_deinit();
}

void app_bt_reset_hw(void)
{
    app_io_init_t io_init;

    io_init.pin  = BT_RESET_GPIO_PIN;
    io_init.mode = APP_IO_MODE_OUTPUT;
    io_init.pull = APP_IO_PULLUP;
    io_init.mux  = APP_IO_MUX;
    app_io_init(APP_IO_TYPE_GPIOB, &io_init);

    app_io_write_pin(BT_RESET_GPIO_TYPE, BT_RESET_GPIO_PIN, APP_IO_PIN_RESET);
    delay_ms(100);
    app_io_write_pin(BT_RESET_GPIO_TYPE, BT_RESET_GPIO_PIN, APP_IO_PIN_SET);
}

