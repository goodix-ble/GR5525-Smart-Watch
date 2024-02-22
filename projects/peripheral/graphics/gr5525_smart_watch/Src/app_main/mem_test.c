
#include "scatter_common.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"
#include "grx_sys.h"
//#include "printf.h"
#include "app_io.h"
#include "platform_sdk.h"
#include "app_rtc.h"
#include "app_key.h"
#include "user_periph_setup.h"
#include "bt_ctl_comm_interface.h"



const uint8_t flash_data[4*1024];

extern uint32_t get_fb_addr(void);


void memcopy_test(void) {
    int i = 0 ;
    uint32_t * ram_data = (uint32_t *)get_fb_addr();
    uint32_t start_stamp = HAL_TIMEOUT_GET_TICK();
    for(i = 0; i < 320; i++) {
        memcpy(&ram_data[0], &flash_data[i*4], 32*1024);
    }
    uint32_t end_stamp = HAL_TIMEOUT_GET_TICK();
    printf("start: %d, end:%d, memcpy spends %d .", start_stamp, end_stamp, end_stamp - start_stamp);
}

void memset_test(void) {
    int i = 0 ;
    uint32_t * ram_data = (uint32_t *)get_fb_addr();
    uint32_t start_stamp = HAL_TIMEOUT_GET_TICK();
    for(i = 0; i < 1024; i++) {
        memset(&ram_data[0], i%256, 32*1024);
    }
    uint32_t end_stamp = HAL_TIMEOUT_GET_TICK();
    printf("start: %d, end:%d, memset spends %d .", start_stamp, end_stamp, end_stamp - start_stamp);
}



void soc_mem_test(uint8_t type) {
    switch(type) {
        case 0:
        {
            memcopy_test();
        }
        break;
        case 1:
        {
            memset_test();
        }
        break;
    }
}
