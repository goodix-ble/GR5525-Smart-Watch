#include "gr_soc.h"

#ifndef DRIVER_TEST
#include "gr_includes.h"
#endif

#include "hal_flash.h"
#include "platform_sdk.h"
#include "pmu_calibration.h"
#include "patch_tab.h"
#include "app_pwr_mgmt.h"

#define PUYA_FLASH_HP_CMD              (0xA3)
#define PUYA_FLASH_HP_END_DUMMY        (2)
#define FALSH_HP_MODE                  XQSPI_HP_MODE_DIS
#define FLASH_HP_CMD                   PUYA_FLASH_HP_CMD
#define FLASH_HP_END_DUMMY             PUYA_FLASH_HP_END_DUMMY
#define SOFTWARE_REG_WAKEUP_FLAG_POS   (8)

/******************************************************************************/

uint32_t SystemCoreClock = CLK_64M;  /* System Core Clock Frequency as 64Mhz     */

__ALIGNED(0x400) FuncVector_t FuncVector_table[MAX_NUMS_IRQn + NVIC_USER_IRQ_OFFSET] = {
    0,
    Reset_Handler,
    NMI_Handler,
    HardFault_Handler,
    MemManage_Handler,
    BusFault_Handler,
    UsageFault_Handler,
    0,
    0,
    0,
    0,
    SVC_Handler,
    DebugMon_Handler,
    0,
    PendSV_Handler,
    SysTick_Handler,
};

void soc_register_nvic(IRQn_Type indx, uint32_t func)
{
    FuncVector_table[indx + 16] = (FuncVector_t)func;
}

static void nvds_setup(void)
{
#ifndef ATE_TEST_ENABLE
    nvds_retention_size(4);

#ifdef NVDS_START_ADDR
    uint8_t err_code = nvds_init(NVDS_START_ADDR, NVDS_NUM_SECTOR);
#else
    uint8_t err_code = nvds_init(0, NVDS_NUM_SECTOR);
#endif

    switch(err_code)
    {
        case NVDS_FAIL:
        case NVDS_STORAGE_ACCESS_FAILED:
            {
                extern uint32_t nvds_get_start_addr(void);
                uint32_t start_addr  = nvds_get_start_addr();
                uint32_t sector_size = hal_flash_sector_size();
                if (hal_flash_erase(start_addr, NVDS_NUM_SECTOR * sector_size))
                {
                    err_code = nvds_init(start_addr, NVDS_NUM_SECTOR);
                    if (NVDS_SUCCESS == err_code)
                    {
                        break;
                    }
                }
                /* Flash fault, cannot startup.
                 * TODO: Output log via UART or Dump an error code to flash. */
                while(1);
            }
        case NVDS_SUCCESS:
            break;
        default:
            /* Illegal NVDS Parameters.
             * Please check the start address and number of sectors. */
            while(1);
    }
    #endif //ATE_TEST_ENABLE
}

void first_class_task(void)
{
    exflash_hp_init_t hp_init;

    platform_xqspi_env_init();
    if (!hal_flash_init())
    {
        /* Flash fault, cannot startup.
         * TODO: Output log via UART or Dump an error code to flash. */
        while(1);
    }

    hp_init.xqspi_hp_enable    = FALSH_HP_MODE;
    hp_init.xqspi_hp_cmd       = FLASH_HP_CMD;
    hp_init.xqspi_hp_end_dummy = FLASH_HP_END_DUMMY;
    platform_flash_enable_quad(&hp_init);

    /* nvds module init process. */
    nvds_setup();

    /* IO leakage protect configuration. */
    system_io_leakage_protect();

    /* platform init process. */
    platform_sdk_init();
}

void second_class_task(void)
{
#if CFG_LPCLK_INTERNAL_EN
    platform_clock_init((mcu_clock_type_t)SYSTEM_CLOCK, (sdk_clock_type_t)RNG_OSC_CLK2, CFG_LF_ACCURACY_PPM, 0);
#else
    platform_clock_init((mcu_clock_type_t)SYSTEM_CLOCK, RTC_OSC_CLK, CFG_LF_ACCURACY_PPM, 0);
#endif

#if PMU_CALIBRATION_ENABLE && !defined(DRIVER_TEST)
    /* Enable auto pmu calibration function. */
    if(!CHECK_IS_ON_FPGA())
    {
        system_pmu_calibration_init(30000);
    }
#endif

    system_pmu_init((mcu_clock_type_t)SYSTEM_CLOCK);
    SystemCoreSetClock((mcu_clock_type_t)SYSTEM_CLOCK);
    SetSerialClock(SERIAL_N96M_CLK);

    // recover the default setting by temperature, should be called in the end
    if(!CHECK_IS_ON_FPGA())
    {
        pmu_calibration_handler(NULL);
    }

    /* Init peripheral sleep management */
    app_pwr_mgmt_init();
}

static fun_t svc_user_func = NULL;

void svc_func_register(uint8_t svc_num, uint32_t user_func)
{
    svc_user_func = (fun_t)user_func;
}

void svc_user_handler(uint8_t svc_num)
{
    if (svc_user_func)
		svc_user_func();
}


static void patch_init(void)
{
    gr5xx_fpb_init(FPB_MODE_PATCH_AND_DEBUG);
}

void platform_init(void)
{
	patch_init();
    first_class_task();
    second_class_task();
}

void ble_feature_init(void)
{
    #if (CFG_MAX_ADVS)
    adv_func_init();
    #endif

    #if (CFG_MAX_SCAN)
    scan_func_init();
    #endif

    #if (CFG_MASTER_SUPPORT)
    master_func_init();
    #endif

    #if (CFG_SLAVE_SUPPORT)
    slave_func_init();
    #endif

    #if (CFG_LEGACY_PAIR_SUPPORT)
    legacy_pair_func_init();
    #endif

    #if (CFG_SC_PAIR_SUPPORT)
    sc_pair_func_init();
    #endif

    #if (CFG_COC_SUPPORT)
    coc_func_init();
    #endif

    #if (CFG_GATTS_SUPPORT)
    gatts_func_init();
    #endif

    #if (CFG_GATTC_SUPPORT)
    gattc_func_init();
    #endif

    #if (CFG_CONN_AOA_AOD_SUPPORT)
    conn_aoa_aod_func_init();
    #endif

    #if (CFG_CONNLESS_AOA_AOD_SUPPORT)
    connless_aoa_aod_func_init();
    #endif

    #if (CFG_RANGING_SUPPORT)
    ranging_func_init();
    #endif

    #if (CFG_POWER_CONTROL_SUPPORT)
    power_control_func_init();
    #endif
}

uint32_t get_wakeup_flag(void)
{
    return (AON_CTL->SOFTWARE_REG0 & (1 << SOFTWARE_REG_WAKEUP_FLAG_POS));
}

void vector_table_init(void)
{
    __DMB(); // Data Memory Barrier
    FuncVector_table[0] = *(FuncVector_t *)(SCB->VTOR);
    SCB->VTOR = (uint32_t)FuncVector_table; // Set VTOR to the new vector table location
    __DSB(); // Data Synchronization Barrier to ensure all
}

void warm_boot_process(void)
{
    vector_table_init();
    pwr_mgmt_warm_boot();
}

void soc_init(void)
{
    platform_init();
}

__WEAK void sdk_init(void){};

void ble_sdk_patch_env_init(void)
{
    #if (CFG_MAX_ADVS || CFG_MAX_SCAN || CFG_MAX_CONNECTIONS || DTM_TEST_ENABLE)

    // register the msg handler for patch
    uint16_t ke_msg_cnt = sizeof(ke_msg_tab) / sizeof(ke_msg_tab_item_t);
    reg_ke_msg_patch_tab(ke_msg_tab, ke_msg_cnt);

    // register the llm hci cmd handler for patch
    uint16_t llm_hci_cmd_cnt = sizeof(llm_hci_cmd_tab) / sizeof(llm_hci_cmd_tab_item_t);
    reg_llm_hci_cmd_patch_tab(llm_hci_cmd_tab, llm_hci_cmd_cnt);

    // register the gapm hci evt handler for patch
    uint16_t gapm_hci_evt_cnt = sizeof(gapm_hci_evt_tab) / sizeof(gapm_hci_evt_tab_item_t);
    reg_gapm_hci_evt_patch_tab(gapm_hci_evt_tab, gapm_hci_evt_cnt);

    ble_common_env_init();

    #if CFG_MAX_CONNECTIONS
    ble_con_env_init();
    #endif

    #if CFG_MAX_SCAN
    ble_scan_env_init();
    #endif

    #if CFG_MAX_ADVS
    ble_adv_env_init();
    #endif

    #if DTM_TEST_ENABLE
    ble_test_evn_init();
    #endif

    #if CFG_CAR_KEY_SUPPORT
    ble_car_key_env_init();
    #endif

    #if CFG_BT_BREDR
    ble_bt_bredr_env_init();
    #endif

    #if CFG_MUL_LINK_WITH_SAME_DEV
    ble_mul_link_env_init();
    #endif

    #endif  // (CFG_MAX_ADVS || CFG_MAX_SCAN || CFG_MAX_CONNECTIONS || DTM_TEST_ENABLE)
}
