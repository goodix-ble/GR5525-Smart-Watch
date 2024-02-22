#ifndef __DISPLAY_FLS_360P_QSPI_DRV_H__
#define __DISPLAY_FLS_360P_QSPI_DRV_H__

#include "app_qspi.h"


#define FLS_DISPLAY_RES_360         0x01        /* Resolution : 390x390x16bit */
#define FLS_DISPLAY_TE_ENABLED      1
#define FLS_DISPLAY_TE_DBG_SIGNAL   (FLS_DISPLAY_TE_ENABLED && 1)

uint32_t display_fls_amo139_init(app_qspi_id_t id, uint32_t clock_prescaler, app_qspi_evt_handler_t evt_handler);

void display_fls_amo139_deinit(app_qspi_id_t id);

void display_fls_amo139_set_show_area(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2);

#if FLS_DISPLAY_TE_ENABLED
void display_fls_amo139_wait_te_signal(void);
void display_fls_amo139_set_te_enable(bool enable);
#else
#define display_fls_amo139_wait_te_signal(...)
#define display_fls_amo139_set_te_enable(...)
#endif // FLS_DISPLAY_TE_ENABLED

#endif /*__DISPLAY_FLS_360P_QSPI_DRV_H__*/
