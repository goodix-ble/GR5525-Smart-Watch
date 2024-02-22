#ifndef __BT_CTL_DEBUG_INTERFACE_H__
#define __BT_CTL_DEBUG_INTERFACE_H__

// Bt debug flag
#define BT_DEBUG_ENABLE        0xFB
#define BT_DEBUG_POLLING       0xCF
#define BT_DEBUG_DISABLE       0xBF
#define BT_DEBUG_INVALID       0x00
#define BT_DEBUG_MODE_CTL_ADDR 0x20020000

/******* DEBUG UART IO CONFIG *******/
#define DEBUG_UART_ID         APP_UART_ID
#define DEBUG_UART_BAUDRATE   115200
#define DEBUG_UART_TX_IO_TYPE APP_UART_TX_IO_TYPE
#define DEBUG_UART_RX_IO_TYPE APP_UART_RX_IO_TYPE
#define DEBUG_UART_TX_PIN     APP_UART_TX_PIN
#define DEBUG_UART_RX_PIN     APP_UART_RX_PIN
#define DEBUG_UART_TX_PINMUX  APP_UART_TX_PINMUX
#define DEBUG_UART_RX_PINMUX  APP_UART_RX_PINMUX
#define DEBUG_UART_TX_PULL    APP_UART_TX_PULL
#define DEBUG_UART_RX_PULL    APP_UART_RX_PULL

#define DEBUG_TX_BUFF_SIZE    0x100
#define DEBUG_RX_BUFF_SIZE    0x100

typedef enum
{
    BT_DEBUG_PROCESS,
    BT_DEBUG_PROCESS_RSP,
    BT_DEBUG_IDLE,
}BT_DEBUG_STATUS_T;

typedef void (*bt_uart_transmit_sync_t)(uint8_t *buf, uint16_t length);

void app_bt_debug_reponse_handle(uint8_t *buf, uint16_t len);

void bt_debug_exit(void);

void bt_debug_task(void);

void bt_debug_set_bt_transmit_func(bt_uart_transmit_sync_t transmit_func);

void bt_debug_set_buffer(uint8_t *tx_buf, uint16_t tx_bufsize, uint8_t *rx_buf, uint16_t rx_bufsize);

bool bt_debug_mode_is_active(void);

void bt_debug_on_enter_debug_mode(void);

void bt_debug_on_exit_debug_mode(void);

void bt_debug_open_to_receive(void);

#endif // __BT_CTL_DEBUG_INTERFACE_H__
