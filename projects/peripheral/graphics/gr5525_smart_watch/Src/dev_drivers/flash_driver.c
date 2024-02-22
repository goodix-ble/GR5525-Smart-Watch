#include <stdint.h>
#include "app_qspi.h"
#include "qspi_flash.h"


/*
 * Flash Device Setting
 */
#define Q_NOR_FLASH_QSPI_ID                         APP_QSPI_ID_0
#define Q_NOR_FLASH_CLOCK_PREESCALER                2u
#define Q_NOR_FLASH_PIN_GROUP                       QSPI0_PIN_GROUP_0

void gx_flash_init()
{
    uint8_t flash_id = SPI_FLASH_init(Q_NOR_FLASH_QSPI_ID, Q_NOR_FLASH_CLOCK_PREESCALER, Q_NOR_FLASH_PIN_GROUP);
    if(flash_id != 0x0B)
    {
        printf("Flash init error\r\n");
    }
    app_qspi_mmap_device_t dev = {
        .dev_type    = APP_QSPI_DEVICE_FLASH,
        .rd.flash_rd = FLASH_MMAP_CMD_QREAD_6BH,
    };
    SPI_FLASH_Enable_Quad();
    app_qspi_config_memory_mappped(Q_NOR_FLASH_QSPI_ID, dev);
    app_qspi_mmap_set_endian_mode(Q_NOR_FLASH_QSPI_ID, APP_QSPI_MMAP_ENDIAN_MODE_2);
}
