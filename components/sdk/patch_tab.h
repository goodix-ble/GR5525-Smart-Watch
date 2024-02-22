#ifndef __PATCH_TAB_H_
#define __PATCH_TAB_H_

#include <stdint.h>

#if (CFG_MAX_ADVS || CFG_MAX_SCAN || CFG_MAX_CONNECTIONS || DTM_TEST_ENABLE)

typedef uint16_t task_id_t;
typedef uint16_t msg_id_t;

typedef int (*msg_func_t)(msg_id_t const msgid, void const *param,
    task_id_t const dest_id, task_id_t const src_id);

typedef int (*llm_hci_cmd_hdl_func_t)(void const *param, uint16_t opcode);

typedef int (*gapm_hci_evt_hdl_func_t)(uint16_t opcode, void const *param);

typedef struct
{
    msg_func_t ori_func_addr;
    msg_func_t new_func_addr;
} ke_msg_tab_item_t;

typedef struct
{
    llm_hci_cmd_hdl_func_t ori_func_addr;
    llm_hci_cmd_hdl_func_t new_func_addr;
} llm_hci_cmd_tab_item_t;

typedef struct
{
    gapm_hci_evt_hdl_func_t ori_func_addr;
    gapm_hci_evt_hdl_func_t new_func_addr;
} gapm_hci_evt_tab_item_t;

extern void ble_common_env_init(void);
extern void ble_con_env_init(void);
extern void ble_scan_env_init(void);
extern void ble_adv_env_init(void);
extern void ble_test_evn_init(void);
extern void ble_iso_env_init(void);
extern void ble_car_key_env_init(void);
extern void ble_bt_bredr_env_init(void);
extern void ble_mul_link_env_init(void);

// // gapm task for common
extern int gapm_set_dev_config_cmd_handler_patch(msg_id_t const msgid, void *param,
    task_id_t const dest_id, task_id_t const src_id);

extern int hci_command_llm_handler_patch(msg_id_t const msgid, void const *param,
    task_id_t const dest_id,task_id_t const src_id);

extern int gapm_hci_handler_patch(msg_id_t const msgid, void const* event,
    task_id_t dest_id, task_id_t opcode);

extern int gapc_hci_handler_patch(msg_id_t const msgid, void const* event,
    task_id_t dest_id, task_id_t src_id);

#if CFG_MAX_CONNECTIONS
extern int llc_op_ch_class_en_ind_handler_patch(msg_id_t const msgid, void const *param,
    task_id_t const dest_id, task_id_t const src_id);

extern int llc_op_ch_class_rep_ind_handler_patch(msg_id_t const msgid, void const *param,
    task_id_t const dest_id, task_id_t const src_id);
#endif

#if CFG_CAR_KEY_SUPPORT
// llm task for car key
extern int llm_pub_key_gen_ind_handler_patch(msg_id_t const msgid, void const *param,
    task_id_t const dest_id, task_id_t const src_id);
#endif

#if CFG_MUL_LINK_WITH_SAME_DEV
// gapm hci event for multiple link
extern int hci_le_adv_set_term_evt_handler_patch(uint16_t opcode, void const *p_event);

// gapc task for multiple link
extern int gapc_bond_cfm_handler_patch(msg_id_t const msgid, void *cfm,
    task_id_t const dest_id, task_id_t const src_id);

extern int lld_adv_end_ind_handler_patch(msg_id_t const msgid, void const *param,
    task_id_t const dest_id, task_id_t const src_id);
#endif

#if DTM_TEST_ENABLE
// llm hci cmd handler for test
extern int hci_le_tx_test_v4_cmd_handler_patch(void const *param, uint16_t opcode);
#endif

ke_msg_tab_item_t ke_msg_tab[] =
{
    {(msg_func_t)0x00014981, (msg_func_t)gapm_hci_handler_patch},
    {(msg_func_t)0x0000fe45, (msg_func_t)gapc_hci_handler_patch},

    #if (!DTM_TEST_ENABLE)
    {(msg_func_t)0x00015fc5, (msg_func_t)gapm_set_dev_config_cmd_handler_patch},
    {(msg_func_t)0x0001b7cd, (msg_func_t)hci_command_llm_handler_patch},
    #endif

    #if CFG_MAX_CONNECTIONS
    {(msg_func_t)0x0002a83d, (msg_func_t)llc_op_ch_class_en_ind_handler_patch},
    {(msg_func_t)0x0002a8f9, (msg_func_t)llc_op_ch_class_rep_ind_handler_patch},
    #endif

    #if CFG_CAR_KEY_SUPPORT
    // llm task for car key
    {(msg_func_t)0x000422d9, (msg_func_t)llm_pub_key_gen_ind_handler_patch},
    #endif

    #if CFG_MUL_LINK_WITH_SAME_DEV
    {(msg_func_t)0x0000ee99, (msg_func_t)gapc_bond_cfm_handler_patch},
    {(msg_func_t)0x0002d63d, (msg_func_t)lld_adv_end_ind_handler_patch},
    #endif
};

extern int hci_le_add_dev_to_rslv_list_cmd_handler_patch(void const *param, uint16_t opcode);

extern int hci_le_rmv_dev_from_rslv_list_cmd_handler_patch(void const *param, uint16_t opcode);

extern int hci_le_clear_rslv_list_cmd_handler_patch(void const *param, uint16_t opcode);

extern int hci_le_set_addr_resol_en_cmd_handler_patch(void const *param, uint16_t opcode);

extern int hci_le_set_priv_mode_cmd_handler_patch(void const *param, uint16_t opcode);

#if CFG_CAR_KEY_SUPPORT
extern int hci_le_rd_local_p256_public_key_cmd_handler_patch(void const *param, uint16_t opcode);
#endif

llm_hci_cmd_tab_item_t llm_hci_cmd_tab[] =
{
    {NULL, NULL},
    
    #if (!DTM_TEST_ENABLE)
     // hci cmd for common
    {(llm_hci_cmd_hdl_func_t)0x0001c071, (llm_hci_cmd_hdl_func_t)hci_le_add_dev_to_rslv_list_cmd_handler_patch},
    {(llm_hci_cmd_hdl_func_t)0x0001efd5, (llm_hci_cmd_hdl_func_t)hci_le_rmv_dev_from_rslv_list_cmd_handler_patch},
    {(llm_hci_cmd_hdl_func_t)0x0001c4bd, (llm_hci_cmd_hdl_func_t)hci_le_clear_rslv_list_cmd_handler_patch},
    {(llm_hci_cmd_hdl_func_t)0x0001f295, (llm_hci_cmd_hdl_func_t)hci_le_set_addr_resol_en_cmd_handler_patch},
    {(llm_hci_cmd_hdl_func_t)0x00021005, (llm_hci_cmd_hdl_func_t)hci_le_set_priv_mode_cmd_handler_patch},
    #endif

    #if DTM_TEST_ENABLE
    // llm hci cmd for dtm test
    {(llm_hci_cmd_hdl_func_t)0x00021861, (llm_hci_cmd_hdl_func_t)hci_le_tx_test_v4_cmd_handler_patch},
    #endif

    #if CFG_CAR_KEY_SUPPORT
    {(llm_hci_cmd_hdl_func_t)0x0001e4e1, (llm_hci_cmd_hdl_func_t)hci_le_rd_local_p256_public_key_cmd_handler_patch},
    #endif
};

gapm_hci_evt_tab_item_t gapm_hci_evt_tab[] =
{
    {NULL, NULL},

    #if CFG_MUL_LINK_WITH_SAME_DEV
    {(gapm_hci_evt_hdl_func_t)0x0001c2f9, (gapm_hci_evt_hdl_func_t)hci_le_adv_set_term_evt_handler_patch},
    #endif
};

extern void reg_ke_msg_patch_tab(ke_msg_tab_item_t *ke_msg_tab, uint16_t ke_msg_cnt);
extern void reg_gapm_hci_evt_patch_tab(gapm_hci_evt_tab_item_t *gapm_hci_evt_tab, uint16_t gapm_hci_evt_cnt);
extern void reg_llm_hci_cmd_patch_tab(llm_hci_cmd_tab_item_t *hci_cmd_tab, uint16_t hci_cmd_cnt);
#endif

#endif  //(CFG_MAX_ADVS || CFG_MAX_SCAN || CFG_MAX_CONNECTIONS || DTM_TEST_ENABLE)
