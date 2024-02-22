/**
 ****************************************************************************************
 *
 * @file bt_api.h
 *
 * @brief Classic Bluetooth API
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

#ifndef __BT_API_H__
#define __BT_API_H__

#include "bt_common.h"
#include "cmsis_compiler.h"
#include <stdbool.h>
#include <stdint.h>

#define IND_ALL  IND_MESSAGE_MAX
#define RESP_ALL OPCODE_BT_MAX

typedef uint16_t bt_api_cmd_t;
typedef uint8_t bt_api_opcode_t; // use value from bt_opcode_t
typedef uint8_t bt_api_resp_t; // use value from bt_resp_t
typedef uint16_t bt_api_ind_type_t; // use value from bt_ind_type_t

#define BT_API_STATE_OFF 0
#define BT_API_STATE_ON  1
typedef uint8_t bt_api_state_t;

typedef struct
{
    uint8_t addr[6];
} bt_api_addr_t;

typedef char bt_api_device_name_t;

typedef struct
{
    uint32_t minor : 4;
    uint32_t major : 4;
    uint32_t patch : 4;
    uint32_t build : 4;
    char place_c;
    uint8_t app_v2:4;
    uint8_t app_v1:4;
} bt_api_fw_version_t;

#define BT_API_CONN_STATE_CONNECTED    1
#define BT_API_CONN_STATE_DISCONNECTED 0
typedef uint8_t bt_api_conn_state_t;

typedef struct
{
    bt_api_conn_state_t a2dp;
    bt_api_conn_state_t hfp;
} bt_api_ind_profile_state_t;

typedef struct
{
    bt_api_addr_t bd_addr;
    uint8_t cod[3];
    int8_t rssi;
    bt_api_device_name_t device_name;
} bt_api_ind_inquiry_info_t;

typedef struct
{
    bt_api_addr_t bd_addr;
    bt_api_conn_state_t status;
} bt_api_ind_connection_state_t;

typedef uint8_t bt_api_ind_vol_level_info_t;

typedef struct
{
    uint8_t analog;
    uint8_t digital;
} bt_api_ind_mic_gain_info_t;

typedef uint8_t bt_api_ind_addr_book_info_t;

#define BT_API_CALL_LINE_STATE_INCOMING      0
#define BT_API_CALL_LINE_STATE_ACCEPT        1
#define BT_API_CALL_LINE_STATE_HOLD          2
#define BT_API_CALL_LINE_STATE_END           3
#define BT_API_CALL_LINE_STATE_DIAL          4
#define BT_API_CALL_LINE_STATE_DIAL_CANCELED 5
typedef uint8_t bt_api_ind_call_line_state_state_t;

typedef struct
{
    bt_api_ind_call_line_state_state_t state;
    char phone_number;
} bt_api_ind_call_line_state_t;

typedef uint8_t bt_api_ind_call_ring_state_t;

#define VOL_SITE_PHONE 0
#define VOL_SITE_BT    1
typedef uint8_t bt_api_ind_vol_site_info_t;

typedef char bt_api_ind_audio_song_info_t;

typedef char bt_api_ind_audio_song_word_info_t;

#define AUDIO_STATE_PLAYING 1
#define AUDIO_STATE_PAUSED  2
typedef uint8_t bt_api_ind_audio_state_t;

typedef struct
{
    uint8_t bonding_flag;
    bt_api_addr_t dev_addr;
    uint8_t dev_cod[3];
    bt_api_device_name_t dev_name;
} bt_api_ind_pair_dev_info_t;

typedef uint8_t bt_api_call_offset_t;

typedef struct
{
    bt_api_state_t inquiry;
    bt_api_state_t inquiry_scan;
    bt_api_state_t page;
    bt_api_state_t page_scan;
} bt_api_inq_page_state_t;

typedef uint8_t bt_api_hfp_vol_info_t;

typedef uint8_t bt_api_audio_vol_info_t;

#define BT_API_HFP_GAIN_LEVEL_MAX 17
typedef uint8_t bt_api_hfp_gain_info_t[3 * BT_API_HFP_GAIN_LEVEL_MAX];

#define BT_API_AUDIO_GAIN_LEVEL_MAX 17
typedef uint8_t bt_api_audio_gain_info_t[3 * BT_API_AUDIO_GAIN_LEVEL_MAX];

#define BT_API_MUTE_STATE_UNMUTED 0
#define BT_API_MUTE_STATE_MUTED   1
typedef uint8_t bt_api_mute_state_t;

#define BT_API_LOCAL_VOICE_STATE_PLAYING 1
#define BT_API_LOCAL_VOICE_STATE_STOPPED 2
typedef uint8_t bt_api_local_voice_state_t;

#define BT_API_SIRI_STATE_ACTIVE   1
#define BT_API_SIRI_STATE_INACTIVE 2
typedef uint8_t bt_api_siri_state_t;

typedef uint8_t bt_api_notice_lvl_state_t;

typedef union
{
    bt_api_ind_profile_state_t profile_state;
    bt_api_ind_inquiry_info_t inquiry_info;
    bt_api_ind_connection_state_t connection_state;
    bt_api_addr_t bt_bd_addr;
    bt_api_device_name_t bt_dev_name;
    bt_api_fw_version_t fw_version_info;
    bt_api_ind_vol_level_info_t vol_level_info;
    bt_api_ind_mic_gain_info_t mic_gain_info;
    bt_api_ind_addr_book_info_t addr_book_info;
    bt_api_ind_call_line_state_t call_line_state;
    bt_api_ind_call_ring_state_t call_ring_state;
    bt_api_ind_vol_site_info_t vol_site_info;
    bt_api_ind_audio_song_info_t audio_song_info;
    bt_api_ind_audio_song_word_info_t audio_song_word_info;
    bt_api_ind_audio_state_t audio_state;
    bt_api_state_t power_on_state;
    bt_api_ind_pair_dev_info_t pair_dev_info;
    bt_api_call_offset_t call_offset;
    bt_api_state_t log_state;
    bt_api_inq_page_state_t inq_page_state;
    bt_api_hfp_vol_info_t hfp_vol_info;
    bt_api_audio_vol_info_t audio_vol_info;
    bt_api_hfp_gain_info_t hfp_gain_info;
    bt_api_audio_gain_info_t audio_gain_info;
    bt_api_mute_state_t vol_mute_info;
    bt_api_mute_state_t mic_mute_info;
    bt_api_local_voice_state_t local_voice_state;
    bt_api_siri_state_t siri_state;
    bt_api_notice_lvl_state_t notice_lvl_state;
} bt_api_indication_t;

typedef struct __PACKED
{
    bt_api_cmd_t cmd;
    bt_api_opcode_t opcode;
    uint16_t length;
} bt_api_rx_header_t;

typedef struct
{
    bt_api_cmd_t cmd;
    bt_api_opcode_t opcode;
    uint16_t length;
    union
    {
        bt_api_resp_t *response;
        bt_api_indication_t *indication;
        uint8_t *data;
    } data;
} bt_api_msg_t;

typedef enum {
    THREEWAY_HOLD_FIRST_ACCEPT_SECOND,
    THREEWAY_END_FIRST_ACCEPT_SECOND,
    THREEWAY_HOLD_FIRST_REJECT_SECOND,
    THREEWAY_CALL_ACT_MAX,
} bt_api_threeway_call_act_t;

/**
 * @brief Init BT API
 */
void bt_api_init(void);

/**
 * @brief Deinit BT API
 *
 * @note This will power down BT module. Please re-init before use any BT API
 *
 * @param force If set true, BT API will not wait for any queued command and do power down immediately
 */
void bt_api_deinit(bool force);

/**
 * @brief Process Incoming and Outgoing message related to BT
 */
void bt_api_msg_handler(void);

/**
 * @brief Push BT command into send queue.
 *
 * @note The command will be copied and push into a queue. Once BT module is ready,
 * the copied command will be sent and freed. The real transmission happens in bt_api_msg_handler()
 *
 * @param[in] opcode BT command opcode.
 * @param[in] length Data length, 0 if no data is needed
 * @param[in] data Data buffer, NULL if no data is needed
 *
 * @return true Command is pushed into the queue.
 * @return false Send queue full, the command is not pushed into the queue
 */
bool bt_api_send_command_queue(bt_api_opcode_t opcode, uint16_t length, uint8_t *data);

/**
 * @brief BT message arrived callback
 * @note This function should not be modified. When the callback is needed,
 * implement this function in user's file.
 *
 * @param[out] msg Received BT message;
 */
void bt_api_on_msg_arrived(const bt_api_msg_t *msg);

// Commands API

/**
 *****************************************************************************************
 * @brief Function for reset bt device
 *****************************************************************************************
 */
void bt_api_device_reset(void);

/**
 *****************************************************************************************
 * @brief Function for start inquiry
 *****************************************************************************************
 */
void bt_api_device_inquiry_start(void);

/**
 *****************************************************************************************
 * @brief Function for stop inquiry
 *****************************************************************************************
 */
void bt_api_device_inquiry_stop(void);

/**
 *****************************************************************************************
 * @brief Function for start inquiry scan.
 *****************************************************************************************
 */
void bt_api_device_inquiry_scan_start(void);

/**
 *****************************************************************************************
 * @brief Function for stop inquiry scan.
 *****************************************************************************************
 */
void bt_api_device_inquiry_scan_stop(void);

/**
 *****************************************************************************************
 * @brief Function for connect addr device.
 *
 * @param[in] bd_addr: addr to be connected.
 *
 ****************************************************************************************
 */
void bt_api_connect(uint8_t remote_addr[6]);

/**
 *****************************************************************************************
 * @brief Function for abort connect to addr device. if have been connected or have not connect action, do nothing.
 *
 ****************************************************************************************
 */
void bt_api_connect_abort(void);

/**
 *****************************************************************************************
 * @brief Function for start page scan
 *****************************************************************************************
 */
void bt_api_device_page_scan_start(void);

/**
 *****************************************************************************************
 * @brief Function for stop page scan
 *****************************************************************************************
 */
void bt_api_device_page_scan_stop(void);

/**
 *****************************************************************************************
 * @brief Function for re-connect the last pairing device or last store link key device
 *****************************************************************************************
 */
void bt_api_quick_reconnect(void);

/**
 *****************************************************************************************
 * @brief Function for disconnect current connection device.
 *****************************************************************************************
 */
void bt_api_disconnect(void);

/**
 *****************************************************************************************
 * @brief Function for store remote addr and link key
 *
 * @param[in] remote_addr: remote addr to store
 * @param[in] link_key: remote pairing link key.
 *
 *****************************************************************************************
 */
void bt_api_pairing_info_store(uint8_t remote_addr[6], uint8_t link_key[16]);

/**
 *****************************************************************************************
 * @brief Function for clean pairing bonding info.
 *
 * @param[in] remote_addr: remote addr to store
 *
 *****************************************************************************************
 */
void bt_api_pairing_info_clean(uint8_t remote_addr[6]);

/**
 *****************************************************************************************
 * @brief Function for control re-connect procedure.
 *
 * @param[in] enable: true is open re-connect, false is close re-connect.
 *
 *****************************************************************************************
 */
void bt_api_enable_quick_connect(bool enable);

/**
 *****************************************************************************************
 * @brief Function for get bt address, will report by addr ind message
 *****************************************************************************************
 */
void bt_api_addr_get(void);

/**
 *****************************************************************************************
 * @brief Function for set bt device name
 *
 * @param[in] device_name: device name buffer ptr.
 * @param[in] length: device name length.
 *
 *****************************************************************************************
 */
void bt_api_device_name_set(uint8_t *device_name, uint8_t length);

/**
 *****************************************************************************************
 * @brief Function for get bt device name
 *****************************************************************************************
 */
void bt_api_device_name_get(void);

/**
 *****************************************************************************************
 * @brief Function for get software version
 *****************************************************************************************
 */
void bt_api_software_version_get(void);

/**
 *****************************************************************************************
 * @brief Function for siri wake up.
 *****************************************************************************************
 */
void bt_api_siri_wake_up(void);

/**
 *****************************************************************************************
 * @brief Function for siri exit.
 *****************************************************************************************
 */
void bt_api_siri_exit(void);

/**
 *****************************************************************************************
 * @brief Function to get siri state
 *****************************************************************************************
 */
void bt_api_siri_state_get(void);

/**
 *****************************************************************************************
 * @brief Function for mute the mic
 *
 * @param[mute] mute: enable mute(true), or not (false)
 *
 *****************************************************************************************
 */
void bt_api_mic_mute(bool mute);

/**
 *****************************************************************************************
 * @brief Function for volume up.
 *****************************************************************************************
 */
void bt_api_volp_action(void);

/**
 *****************************************************************************************
 * @brief Function for volume down.
 *****************************************************************************************
 */
void bt_api_volm_action(void);

/**
 *****************************************************************************************
 * @brief Function for get mic gain.
 *****************************************************************************************
 */
void bt_api_mic_gain_get(void);

/**
 *****************************************************************************************
 * @brief Function for volume mute.
 *
 * @param[mute] mute: enable mute(true), or not (false)
 *
 *****************************************************************************************
 */
void bt_api_vol_mute(bool mute);

/**
 * @brief Function for setting mic gain
 *
 * @param[in] analog Analog gain, value should be ranged in 0 to 13
 * @param[in] digital Digital gain, value should be ranged in 0 to 45
 */
void bt_api_mic_gain_set(uint8_t analog, uint8_t digital);

/**
 *****************************************************************************************
 * @brief Function for start mic test
 *****************************************************************************************
 */
void bt_api_mic_test_start(void);

/**
 *****************************************************************************************
 * @brief Function for stop mic test
 *****************************************************************************************
 */
void bt_api_mic_test_stop(void);

/**
 *****************************************************************************************
 * @brief Function for factory reset bt device
 *****************************************************************************************
 */
void bt_api_device_factory_reset(void);

/**
 *****************************************************************************************
 * @brief Function for BT to enter ultra deepsleep.
 *
 * @note This function will automatically deinit BT module for lowest power consumption.
 *       Please re-init BT module by using bt_api_init() before any operation after this.
 *****************************************************************************************
 */
void bt_api_enter_ultra_sleep(void);

/**
 *****************************************************************************************
 * @brief Function for get pairing binding list.
 *****************************************************************************************
 */
void bt_api_pair_list_get(void);

/**
 *****************************************************************************************
 * @brief Function to set frequency offset.
 *
 * @param[in] value: xo offset value
 *
 *****************************************************************************************
 */
void bt_api_call_offset_set(uint8_t value);

/**
 *****************************************************************************************
 * @brief Function for get frequency offset.
 *****************************************************************************************
 */
void bt_api_call_offset_get(void);

/**
 *****************************************************************************************
 * @brief Function for enable/disable BT device log.
 *****************************************************************************************
 */
void bt_api_log_ctrl_set(uint8_t value);

/**
 *****************************************************************************************
 * @brief Function for set bt address, valid after restart
 *
 * @param[in] bd_addr: BT address buffer ptr.
 *
 *****************************************************************************************
 */
void bt_api_addr_set(bt_api_addr_t *bd_addr);

/**
 *****************************************************************************************
 * @brief Function for get inquiry/inquiry scan/page/page scan status
 *****************************************************************************************
 */
void bt_api_device_inquiry_page_get(void);

/**
 *****************************************************************************************
 * @brief Function for abort connect to addr device. if have been connected or have not connect action, do nothing.
 *
 ****************************************************************************************
 */
void bt_api_conn_state_get(void);

/**
 *****************************************************************************************
 * @brief Function for get BT current volume
 *
 *****************************************************************************************
 */
extern void bt_api_cur_vol_get(void);

/**
 *****************************************************************************************
 * @brief Function for get BT volume mute state
 *
 *****************************************************************************************
 */
extern void bt_api_vol_mute_get(void);

/**
 *****************************************************************************************
 * @brief Function for get BT mic mute state
 *
 *****************************************************************************************
 */
extern void bt_api_mic_mute_get(void);

/**
 *****************************************************************************************
 * @brief Function for accept the incoming call.
 *****************************************************************************************
 */
void bt_api_call_address_book_get(void);

/**
 *****************************************************************************************
 * @brief Function for accept the incoming call.
 *****************************************************************************************
 */
void bt_api_call_accept(void);

/**
 *****************************************************************************************
 * @brief Function for set DTMF command
 *
 * @param[in] buf: shall be char : '*', '#', '1', '2', ...
 * @param[in] length: buf length.
 *
 *****************************************************************************************
 */
void bt_api_call_dtmf_set(uint8_t *buf, uint8_t length);

/**
 *****************************************************************************************
 * @brief Function for hang up current call.
 *****************************************************************************************
 */
void bt_api_call_end(void);

/**
 *****************************************************************************************
 * @brief Function for reject the incoming call.
 *****************************************************************************************
 */
void bt_api_call_reject(void);

/**
 *****************************************************************************************
 * @brief Function for re-dail last call number.
 *****************************************************************************************
 */
void bt_api_call_redial(void);

/**
 *****************************************************************************************
 * @brief Function for switch between phone and headphones.
 *****************************************************************************************
 */
void bt_api_call_vol_switch(void);

/**
 *****************************************************************************************
 * @brief Function for three-way calling control
 *
 * @param[in] ctrl: Action on first call and second call.
 *
 *****************************************************************************************
 */
void bt_api_three_way_call_ctl(bt_api_threeway_call_act_t act);

/**
 *****************************************************************************************
 * @brief Function for three-way calling switch calling lines
 *****************************************************************************************
 */
void bt_api_three_way_call_switch(void);

/**
 *****************************************************************************************
 * @brief Function for calling number get
 *****************************************************************************************
 */
void bt_api_calling_number_get(void);

/**
 *****************************************************************************************
 * @brief Function for call volume level set.
 *
 * @param[in] vol_level     : call volume level set
 *
 *****************************************************************************************
 */
void bt_api_call_vol_level_set(uint8_t vol_level);

/**
 *****************************************************************************************
 * @brief Function for call volume level get.
 *****************************************************************************************
 */
void bt_api_call_vol_level_get(void);

/**
 *****************************************************************************************
 * @brief Function for call volume gain set.
 *
 * @param[in] vol_level     : call volume level to set,                 0~16
 * @param[in] ana_gain     : call volume level analog gain to set, 0~7(0db~7db)
 * @param[in] dig_gain      : audio volume level digital to set,        0~44(-44db ~ 0db), 45 = Mute
 *
 *****************************************************************************************
 */
void bt_api_call_vol_gain_set(uint8_t vol_level, uint8_t ana_gain, uint8_t dig_gain);

/**
 *****************************************************************************************
 * @brief Function for call volume gain get.
 *****************************************************************************************
 */
void bt_api_call_vol_gain_get(void);

/**
 *****************************************************************************************
 * @brief Function for place a call
 *
 * @param[in] number: call number to place, number shall be char : '1', '2', ...
 * @param[in] length: call number length.
 *
 *****************************************************************************************
 */
void bt_api_place_call_number(uint8_t *number, uint8_t lenth);

/**
 *****************************************************************************************
 * @brief Function for re-connect or disconnect a2dp
 *****************************************************************************************
 */
void bt_api_a2dp_status_set(void);

/**
 *****************************************************************************************
 * @brief Function for play start or stop .
 *****************************************************************************************
 */
void bt_api_avrcp_status_set(void);

/**
 *****************************************************************************************
 * @brief Function for play next song.
 *****************************************************************************************
 */
void bt_api_avrcp_play_next(void);

/**
 *****************************************************************************************
 * @brief Function for play prev song.
 *****************************************************************************************
 */
void bt_api_avrcp_play_prev(void);

/**
 *****************************************************************************************
 * @brief Function for play rewind.
 *****************************************************************************************
 */
void bt_api_avrcp_play_rewind(void);

/**
 *****************************************************************************************
 * @brief Function for play forward.
 *****************************************************************************************
 */
void bt_api_avrcp_play_forward(void);

/**
 *****************************************************************************************
 * @brief Function for volume level set.
 *
 * @param[in] vol_level     : volume level set
 *
 *****************************************************************************************
 */
void bt_api_vol_level_set(uint8_t vol_level);

/**
 *****************************************************************************************
 * @brief Function for volume level get.
 *
 * @param[in] vol_level     : volume level get
 *
 *****************************************************************************************
 */
void bt_api_vol_level_get(void);

/**
 *****************************************************************************************
 * @brief Function for volume gain set.
 *
 * @param[in] vol_level     : audio volume level to set,                 0~16
 * @param[in] ana_gain     : audio volume level analog gain to set, 0~7(0db~7db)
 * @param[in] dig_gain      : audio volume level digital to set,        0~44(-44db ~ 0db), 45 = Mute
 *
 *****************************************************************************************
 */
void bt_api_vol_gain_set(uint8_t vol_level, uint8_t ana_gain, uint8_t dig_gain);

/**
 *****************************************************************************************
 * @brief Function for volume gain get.
 *
 * @param[in] vol_level     : volume gain get
 *
 *****************************************************************************************
 */
void bt_api_vol_gain_get(void);

/**
 *****************************************************************************************
 * @brief Function for volume information get, include singer, album, the length of the song, and so on.
 *****************************************************************************************
 */
void bt_api_audio_info_get(void);

/**
 *****************************************************************************************
 * @brief Function for sync batt level to BT module.
 *
 * @param[level] value: 0~100
 *
 *****************************************************************************************
 */
void bt_api_batt_lvl_info_sync(uint8_t bat_lvl);

/**
 *****************************************************************************************
 * @brief Function for start to play drainage vibration audio
 *
 *****************************************************************************************
 */
void bt_api_drain_vibra_start(void);

/**
 *****************************************************************************************
 * @brief Function for stop to play drainage vibration audio
 *
 *****************************************************************************************
 */
void bt_api_drain_vibra_stop(void);

/**
 *****************************************************************************************
 * @brief Function for start to play search device audio
 *
 *****************************************************************************************
 */
void bt_api_search_dev_start(void);

/**
 *****************************************************************************************
 * @brief Function for start to play search device audio
 *
 *****************************************************************************************
 */
void bt_api_search_dev_stop(void);

/**
 *****************************************************************************************
 * @brief Function for notice volume level set.
 *
 * @param[in] vol_level     : notice volume level set:0x0~0x10
 *
 *****************************************************************************************
 */
 void bt_api_notice_level_set(uint8_t vol_level);

/**
 *****************************************************************************************
 * @brief Function to get notice volume level .
 *
 *****************************************************************************************
 */
 void bt_api_notice_level_get(void);

#endif // __BT_API_H__
