#ifndef __BSP_TP_H_
#define __BSP_TP_H_

#include <stdbool.h>
#include <stdint.h>


void tp_set_sleep_mode(bool sleep_enable);
bool tp_get_data(int16_t *p_x, int16_t *p_y);
void tp_init(void);

#endif
