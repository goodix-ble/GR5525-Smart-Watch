/**
 ****************************************************************************************
 *
 * @file bt_api_impl.c
 *
 * @brief Classic Bluetooth Command API Implementation
 *
 ****************************************************************************************
 * @attention
  #####Copyright (c) 2023 GOODIX
  All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
  * Neither the name of GOODIX nor the names of its contributors may be used
    to endorse or promote products derived from this software without
    specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************************
 */

/*
 * INCLUDE FILES
 *****************************************************************************************
 */
#include "bt_api.h"
#include <stdio.h>
#include <string.h>

void bt_api_device_reset(void)
{
    bt_api_send_command_queue(OPCODE_BT_RESET, 0, NULL);
}

void bt_api_device_inquiry_start(void)
{
    bool inquiry_start = true;
    bt_api_send_command_queue(OPCODE_BT_INQUIRY, 1, (uint8_t *)&inquiry_start);
}

void bt_api_device_inquiry_stop(void)
{
    bool inquiry_start = false;
    bt_api_send_command_queue(OPCODE_BT_INQUIRY, 1, (uint8_t *)&inquiry_start);
}

void bt_api_device_inquiry_scan_start(void)
{
    bool inquiry_scan_start = true;
    bt_api_send_command_queue(OPCODE_BT_INQUIRY_SCAN, 1, (uint8_t *)&inquiry_scan_start);
}

void bt_api_device_inquiry_scan_stop(void)
{
    bool inquiry_scan_start = false;
    bt_api_send_command_queue(OPCODE_BT_INQUIRY_SCAN, 1, (uint8_t *)&inquiry_scan_start);
}

void bt_api_connect(uint8_t remote_addr[6])
{
    bt_api_send_command_queue(OPCODE_BT_CONNECT, 6, remote_addr);
}

void bt_api_connect_abort(void)
{
    bt_api_send_command_queue(OPCODE_BT_CONNECT_ABORT, 0, NULL);
}

void bt_api_device_page_scan_start(void)
{
    bool page_scan_start = true;
    bt_api_send_command_queue(OPCODE_BT_PAGE_SCAN, 1, (uint8_t *)&page_scan_start);
}

void bt_api_device_page_scan_stop(void)
{
    bool page_scan_start = false;
    bt_api_send_command_queue(OPCODE_BT_PAGE_SCAN, 1, (uint8_t *)&page_scan_start);
}

void bt_api_quick_reconnect(void)
{
    bt_api_send_command_queue(OPCODE_BT_QUICK_CONNECT, 0, NULL);
}

void bt_api_disconnect(void)
{
    bt_api_send_command_queue(OPCODE_BT_DISCONNECT, 0, NULL);
}

void bt_api_pairing_info_store(uint8_t remote_addr[6], uint8_t link_key[16])
{
    uint8_t pairing_info[22] = {
        0,
    };

    memcpy(&pairing_info[0], remote_addr, 6);
    memcpy(&pairing_info[6], link_key, 16);
    bt_api_send_command_queue(OPCODE_BT_PAIRING_INFO_STORE, 22, pairing_info);
}

void bt_api_pairing_info_clean(uint8_t remote_addr[6])
{
    bt_api_send_command_queue(OPCODE_BT_PAIRING_INFO_CLEAN, 6, remote_addr);
}

void bt_api_enable_quick_connect(bool enable)
{
    bt_api_send_command_queue(OPCODE_BT_QUICK_CON_MODE_SET, 1, (uint8_t *)&enable);
}

void bt_api_addr_get(void)
{
    bt_api_send_command_queue(OPCODE_BT_BD_ADDR_GET, 0, NULL);
}

void bt_api_device_name_set(uint8_t *device_name, uint8_t length)
{
    bt_api_send_command_queue(OPCODE_BT_DEVICE_NAME_SET, length, device_name);
}

void bt_api_device_name_get(void)
{
    bt_api_send_command_queue(OPCODE_BT_DEVICE_NAME_GET, 0, NULL);
}

void bt_api_software_version_get(void)
{
    bt_api_send_command_queue(OPCODE_BT_FW_VERSION_GET, 0, NULL);
}

void bt_api_siri_wake_up(void)
{
    bt_api_send_command_queue(OPCODE_BT_SIRI_WAKE, 0, NULL);
}

void bt_api_mic_mute(bool mute)
{
    bt_api_send_command_queue(OPCODE_BT_MIC_MUTE, 1, (uint8_t *)&mute);
}

void bt_api_mic_gain_set(uint8_t analog, uint8_t digital)
{
    uint8_t data[2] = {analog, digital};
    bt_api_send_command_queue(OPCODE_BT_MIC_GAIN_SET, 2, data);
}

void bt_api_volp_action(void)
{
    bt_api_send_command_queue(OPCODE_VOL_P, 0, NULL);
}

void bt_api_volm_action(void)
{
    bt_api_send_command_queue(OPCODE_VOL_M, 0, NULL);
}

void bt_api_mic_gain_get(void)
{
    bt_api_send_command_queue(OPCODE_BT_MIC_GAIN_GET, 0, NULL);
}

void bt_api_vol_mute(bool mute)
{
    bt_api_send_command_queue(OPCODE_VOL_MUTE, 1, (uint8_t *)&mute);
}

void bt_api_mic_test_start(void)
{
    bt_api_send_command_queue(OPCODE_BT_MIC_TEST_START, 0, NULL);
}

void bt_api_mic_test_stop(void)
{
    bt_api_send_command_queue(OPCODE_BT_MIC_TEST_STOP, 0, NULL);
}

void bt_api_device_factory_reset(void)
{
    bt_api_send_command_queue(OPCODE_BT_FACTORY_RESET, 0, NULL);
}

void bt_api_enter_ultra_sleep(void)
{
    bt_api_send_command_queue(OPCODE_BT_POWERDOWN, 0, NULL);
}

void bt_api_pair_list_get(void)
{
    bt_api_send_command_queue(OPCODE_BT_PAIR_DEV_LIST_GET, 0, NULL);
}

void bt_api_call_offset_set(uint8_t value)
{
    bt_api_send_command_queue(OPCODE_BT_CALL_OFFSET_SET, 1, &value);
}

void bt_api_call_offset_get(void)
{
    bt_api_send_command_queue(OPCODE_BT_CALL_OFFSET_GET, 0, NULL);
}

void bt_api_log_ctrl_set(uint8_t value)
{
    bt_api_send_command_queue(OPCODE_BT_LOG_CTRL, 1, &value);
}

void bt_api_addr_set(bt_api_addr_t *bd_addr)
{
    // valid after restart
    bt_api_send_command_queue(OPCODE_BT_BD_ADDR_SET, 6, bd_addr->addr);
}

void bt_api_device_inquiry_page_get(void)
{
    bt_api_send_command_queue(OPCODE_BT_INQ_PAGE_GET, 0, NULL);
}

void bt_api_conn_state_get(void)
{
    bt_api_send_command_queue(OPCODE_BT_CONN_STATE_GET, 0, NULL);
}

void bt_api_call_address_book_get(void)
{
    bt_api_send_command_queue(OPCODE_CALL_ADDR_BOOK_GET, 0, NULL);
}

void bt_api_call_accept(void)
{
    bt_api_send_command_queue(OPCODE_CALL_ACCEPT, 0, NULL);
}

void bt_api_call_dtmf_set(uint8_t *buf, uint8_t length)
{
    bt_api_send_command_queue(OPCODE_CALL_DTMF_SET, length, buf);
}

void bt_api_call_end(void)
{
    bt_api_send_command_queue(OPCODE_CALL_END, 0, NULL);
}

void bt_api_call_reject(void)
{
    bt_api_send_command_queue(OPCODE_CALL_REJECT, 0, NULL);
}

void bt_api_call_redial(void)
{
    bt_api_send_command_queue(OPCODE_CALL_REDIAL, 0, NULL);
}

void bt_api_call_vol_switch(void)
{
    bt_api_send_command_queue(OPCODE_CALL_VOL_SWITCH, 0, NULL);
}

void bt_api_three_way_call_ctl(bt_api_threeway_call_act_t act)
{
    if (act < THREEWAY_CALL_ACT_MAX)
    {
        bt_api_send_command_queue(act + OPCODE_CALL_HOLD_FIR_ACCEPT_SEC, 0, NULL);
    }
}

void bt_api_three_way_call_switch(void)
{
    bt_api_send_command_queue(OPCODE_CALL_LINE_SWITCH, 0, NULL);
}

void bt_api_calling_number_get(void)
{
    bt_api_send_command_queue(OPCODE_CALL_NUMBER_GET, 0, NULL);
}

void bt_api_call_vol_level_set(uint8_t vol_level)
{
    bt_api_send_command_queue(OPCODE_CALL_VOL_SET, 1, &vol_level);
}

void bt_api_call_vol_level_get(void)
{
    bt_api_send_command_queue(OPCODE_CALL_VOL_GET, 0, NULL);
}

void bt_api_call_vol_gain_set(uint8_t vol_level, uint8_t ana_gain, uint8_t dig_gain)
{
    uint8_t data[3] = {vol_level, ana_gain, dig_gain};
    bt_api_send_command_queue(OPCODE_CALL_VOL_GAIN_SET, 3, data);
}

void bt_api_call_vol_gain_get(void)
{
    bt_api_send_command_queue(OPCODE_CALL_VOL_GAIN_GET, 0, NULL);
}

void bt_api_place_call_number(uint8_t *number, uint8_t lenth)
{
    bt_api_send_command_queue(OPCODE_CALL_DIAL_NUMBER, lenth, number);
}

void bt_api_a2dp_status_set(void)
{
    bt_api_send_command_queue(OPCODE_A2DP_STATUS_SET, 0, NULL);
}

void bt_api_avrcp_status_set(void)
{
    bt_api_send_command_queue(OPCODE_PLAY_STATUS_SET, 0, NULL);
}

void bt_api_avrcp_play_next(void)
{
    bt_api_send_command_queue(OPCODE_PLAY_NEXT, 0, NULL);
}

void bt_api_avrcp_play_prev(void)
{
    bt_api_send_command_queue(OPCODE_PLAY_PREV, 0, NULL);
}

void bt_api_avrcp_play_rewind(void)
{
    bt_api_send_command_queue(OPCODE_PLAY_REWIND, 0, NULL);
}

void bt_api_avrcp_play_forward(void)
{
    bt_api_send_command_queue(OPCODE_PLAY_FORWARD, 0, NULL);
}

void bt_api_vol_level_set(uint8_t vol_level)
{
    bt_api_send_command_queue(OPCODE_AUDIO_VOL_SET, 1, &vol_level);
}

void bt_api_vol_level_get(void)
{
    bt_api_send_command_queue(OPCODE_AUDIO_VOL_GET, 0, NULL);
}

void bt_api_vol_gain_set(uint8_t vol_level, uint8_t ana_gain, uint8_t dig_gain)
{
    uint8_t data[3] = {vol_level, ana_gain, dig_gain};
    bt_api_send_command_queue(OPCODE_VOL_GAIN_SET, 3, data);
}

void bt_api_vol_gain_get(void)
{
    bt_api_send_command_queue(OPCODE_VOL_GAIN_GET, 0, NULL);
}

void bt_api_audio_info_get(void)
{
    bt_api_send_command_queue(OPCODE_AUDIO_INFO_GET, 0, NULL);
}

void bt_api_batt_lvl_info_sync(uint8_t bat_lvl)
{
    bat_lvl = 10 * (bat_lvl / 10);
    bt_api_send_command_queue(OPCODE_BT_BATT_LV_SYNC, 1, &bat_lvl);
}

void bt_api_cur_vol_get(void)
{
    bt_api_send_command_queue(OPCODE_CUR_VOL_GET, 0, NULL);
}

void bt_api_vol_mute_get(void)
{
    bt_api_send_command_queue(OPCODE_VOL_MUTE_GET, 0, NULL);
}

void bt_api_mic_mute_get(void)
{
    bt_api_send_command_queue(OPCODE_MIC_MUTE_GET, 0, NULL);
}

void bt_api_drain_vibra_start(void)
{
    bt_api_send_command_queue(OPCODE_DRAIN_VIBRA_START, 0, NULL);
}

void bt_api_drain_vibra_stop(void)
{
    bt_api_send_command_queue(OPCODE_DRAIN_VIBRA_STOP, 0, NULL);
}

void bt_api_search_dev_start(void)
{
    bt_api_send_command_queue(OPCODE_SEARCH_DEV_START, 0, NULL);
}

void bt_api_search_dev_stop(void)
{
    bt_api_send_command_queue(OPCODE_SEARCH_DEV_STOP, 0, NULL);
}

void bt_api_notice_level_set(uint8_t vol_level)
{
    bt_api_send_command_queue(OPCODE_BT_NOTICE_LVL_SET, 1, &vol_level);
}

void bt_api_notice_level_get(void)
{
    bt_api_send_command_queue(OPCODE_BT_NOTICE_LVL_GET, 0, NULL);
}

void bt_api_siri_exit(void)
{
    bt_api_send_command_queue(OPCODE_BT_SIRI_SHUTDOWN, 0, NULL);
}

void bt_api_siri_state_get(void)
{
    bt_api_send_command_queue(OPCODE_BT_SIRI_STATE_GET, 0, NULL);
}
