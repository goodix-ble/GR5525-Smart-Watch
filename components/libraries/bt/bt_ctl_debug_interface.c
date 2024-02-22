/*
 * INCLUDE FILES
 *****************************************************************************************
 */
#include <stdio.h>
#include <string.h>
#include "board_SK.h"
#include "bt_ctl_debug_interface.h"
#include "app_log.h"

/*
 * DEFINES
 *****************************************************************************************
 */
#define DEBUG_CMD_FRAME_HEADER_L             0x44     /* CMD header low byte. */
#define DEUBG_CMD_FRAME_HEADER_H             0x47     /* CMD header high byte. */
#define DEUBG_UART_LINK_CHECK                0xFD     /* Uart link check cmd. */
#define DEBUG_UART_LINK_CHECK_RESPONSE_LEN   13u      /* Uart link check response len. */
#define DEBUG_EXIT_DEBUG_MODE                0xF1     /* Exit debug mode cmd. */
#define DEBUG_EXIT_DEBUG_MODE_RESPONSE_LEN   9u       /* Exit debug mode response len. */

#define DEBUG_EVENT_SUCCESS                  0x01     /* Event success. */
#define DEBUG_EVENT_FAIL                     0x02     /* Event success. */

#define DEBUG_SDK_MAJOR                      1u       /* Sdk major version. */
#define DEBUG_SDK_MINOR                      0u       /* Sdk minor version. */
#define DEBUU_SDK_BUILD_M                    7u       /* Sdk build time. */
#define DEBUU_SDK_BUILD_D                    12u      /* Sdk build time. */

#define DEBUG_CMD_CHECK_FAIL                 0xFF     /* Uart cmd check fail. */
/*
 * LOCAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */
static app_uart_params_t debug_uart_param;
static uint8_t *debug_uart_tx_buffer = NULL;
static uint8_t *debug_uart_rx_buffer = NULL;
static uint16_t debug_uart_tx_bufsize = 0;
static uint16_t debug_uart_rx_bufsize = 0;

static bt_uart_transmit_sync_t s_bt_transmit_func = NULL;

extern void handle_uart_ring_buffer_data(void);
/*
 * GLOBAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */
int32_t bt_debug_mode_ctl         __attribute__((at(BT_DEBUG_MODE_CTL_ADDR)));

volatile BT_DEBUG_STATUS_T bt_debug_state = BT_DEBUG_IDLE;
/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */
static uint16_t deubg_get_checksum(uint8_t *pFram, uint16_t frameSize)
{
    if(NULL == pFram || 0 == frameSize)
    {
        return DEBUG_CMD_CHECK_FAIL;
    }

    uint16_t i = 0;
    uint16_t checksum = 0;

    for(i = 2; i < frameSize - 2; i++)
    {
        checksum += pFram[i];
    }

    return checksum;
}

static uint32_t debug_cmd_check(uint8_t *pBuffer, uint16_t bufferSize)
{
    uint16_t b_checksum = 0;

    if(NULL == pBuffer || 0 == bufferSize)
    {
        return DEBUG_CMD_CHECK_FAIL;
    }

    if(DEUBG_CMD_FRAME_HEADER_H != pBuffer[1] || DEBUG_CMD_FRAME_HEADER_L != pBuffer[0])
    {
        return DEBUG_CMD_CHECK_FAIL;
    }

    switch (pBuffer[2])
    {
        case DEUBG_UART_LINK_CHECK:
            b_checksum |= (pBuffer[6] & 0xff);
            b_checksum |= ((pBuffer[7] << 8) & 0xff00);
            if(deubg_get_checksum(pBuffer, bufferSize) == b_checksum)
            {
                return DEUBG_UART_LINK_CHECK;
            }
            break;
        case DEBUG_EXIT_DEBUG_MODE:
            b_checksum |= (pBuffer[6] & 0xff);
            b_checksum |= ((pBuffer[7] << 8) & 0xff00);
            if(deubg_get_checksum(pBuffer, bufferSize) == b_checksum)
            {
                return DEBUG_EXIT_DEBUG_MODE;
            }
            break;
        default:
            break;
    }

    return DEBUG_CMD_CHECK_FAIL;
}

static void debug_uart_data_tx_sync_send(uint8_t *p_data, uint16_t length)
{
    app_uart_transmit_sync(DEBUG_UART_ID, p_data, length, 100);
}

static void uart_cmd_response(uint32_t cmd)
{
    switch (cmd)
    {
        case DEUBG_UART_LINK_CHECK:
        {
            uint8_t pbuffer[DEBUG_UART_LINK_CHECK_RESPONSE_LEN] = {0};
            pbuffer[0] = DEBUG_CMD_FRAME_HEADER_L;
            pbuffer[1] = DEUBG_CMD_FRAME_HEADER_H;
            pbuffer[2] = DEUBG_UART_LINK_CHECK;
            pbuffer[3] = 0x0;
            pbuffer[4] = 0x5;  //data_len
            pbuffer[5] = 0x0;
            pbuffer[6] = DEBUG_EVENT_SUCCESS;
            pbuffer[7] = DEBUG_SDK_MAJOR;
            pbuffer[8] = DEBUG_SDK_MINOR;
            pbuffer[9] = DEBUU_SDK_BUILD_D;
            pbuffer[10] = DEBUU_SDK_BUILD_M;
            uint16_t pbffer_checksum = deubg_get_checksum(pbuffer, DEBUG_UART_LINK_CHECK_RESPONSE_LEN);
            pbuffer[11] = pbffer_checksum;
            pbuffer[12] = pbffer_checksum >> 8;
            debug_uart_data_tx_sync_send(pbuffer, DEBUG_UART_LINK_CHECK_RESPONSE_LEN);
            break;
        }
        case DEBUG_EXIT_DEBUG_MODE:
        {
            uint8_t pbuffer[DEBUG_EXIT_DEBUG_MODE_RESPONSE_LEN] = {0};
            pbuffer[0] = DEBUG_CMD_FRAME_HEADER_L;
            pbuffer[1] = DEUBG_CMD_FRAME_HEADER_H;
            pbuffer[2] = DEBUG_EXIT_DEBUG_MODE;
            pbuffer[3] = 0x0;
            pbuffer[4] = 0x1;  //data_len
            pbuffer[5] = 0x0;
            pbuffer[6] = DEBUG_EVENT_SUCCESS;
            uint16_t pbffer_checksum = deubg_get_checksum(pbuffer, DEBUG_EXIT_DEBUG_MODE_RESPONSE_LEN);
            pbuffer[7] = pbffer_checksum;
            pbuffer[8] = pbffer_checksum >> 8;
            debug_uart_data_tx_sync_send(pbuffer, DEBUG_EXIT_DEBUG_MODE_RESPONSE_LEN);
            break;
        }
        default:
            break;
    }
}

static void app_uart_debug_evt_handler(app_uart_evt_t *p_evt)
{
    switch(p_evt->type)
    {
        case APP_UART_EVT_RX_DATA:
        {
            uint32_t DEUBG_UART_CMD = debug_cmd_check(debug_uart_rx_buffer, p_evt->data.size);
            if(DEUBG_UART_LINK_CHECK == DEUBG_UART_CMD)
            {
                uart_cmd_response(DEUBG_UART_LINK_CHECK);
            }
            else if(DEBUG_EXIT_DEBUG_MODE == DEUBG_UART_CMD)
            {
                bt_debug_mode_ctl = BT_DEBUG_DISABLE;
            }
            else
            {
                bt_debug_state = BT_DEBUG_PROCESS;
                if (s_bt_transmit_func)
                {
                    s_bt_transmit_func(debug_uart_rx_buffer, p_evt->data.size);
                }
            }
            memset(debug_uart_rx_buffer, 0, debug_uart_rx_bufsize);
            app_uart_receive_async(DEBUG_UART_ID, debug_uart_rx_buffer, debug_uart_rx_bufsize);
            break;
        }
        case APP_UART_EVT_ERROR:
            memset(debug_uart_rx_buffer, 0, debug_uart_rx_bufsize);
            app_uart_receive_async(DEBUG_UART_ID, debug_uart_rx_buffer, debug_uart_rx_bufsize);
            break;
        default:
            break;
    }
}

static void debug_uart_init(void)
{
    app_uart_tx_buf_t uart_buffer;

    uart_buffer.tx_buf       = debug_uart_tx_buffer;
    uart_buffer.tx_buf_size  = debug_uart_tx_bufsize;

    app_uart_deinit(DEBUG_UART_ID);

    debug_uart_param.id                   = DEBUG_UART_ID;
    debug_uart_param.init.baud_rate       = DEBUG_UART_BAUDRATE;
    debug_uart_param.init.data_bits       = UART_DATABITS_8;
    debug_uart_param.init.stop_bits       = UART_STOPBITS_1;
    debug_uart_param.init.parity          = UART_PARITY_NONE;
    debug_uart_param.init.hw_flow_ctrl    = UART_HWCONTROL_NONE;
    debug_uart_param.init.rx_timeout_mode = UART_RECEIVER_TIMEOUT_ENABLE;
    debug_uart_param.pin_cfg.rx.type      = DEBUG_UART_RX_IO_TYPE;
    debug_uart_param.pin_cfg.rx.pin       = DEBUG_UART_RX_PIN;
    debug_uart_param.pin_cfg.rx.mux       = DEBUG_UART_RX_PINMUX;
    debug_uart_param.pin_cfg.rx.pull      = DEBUG_UART_RX_PULL;
    debug_uart_param.pin_cfg.tx.type      = DEBUG_UART_TX_IO_TYPE;
    debug_uart_param.pin_cfg.tx.pin       = DEBUG_UART_TX_PIN;
    debug_uart_param.pin_cfg.tx.mux       = DEBUG_UART_TX_PINMUX;
    debug_uart_param.pin_cfg.tx.pull      = DEBUG_UART_TX_PULL;
    debug_uart_param.dma_cfg.tx_dma_instance = DMA0;
    debug_uart_param.dma_cfg.rx_dma_instance = DMA0;
    debug_uart_param.dma_cfg.tx_dma_channel  = DMA_Channel2;
    debug_uart_param.dma_cfg.rx_dma_channel  = DMA_Channel3;

    uint16_t ret = app_uart_init(&debug_uart_param, app_uart_debug_evt_handler, &uart_buffer);
    if(ret != APP_DRV_SUCCESS)
    {
        return ;
    }

    app_uart_receive_async(DEBUG_UART_ID, debug_uart_rx_buffer, debug_uart_rx_bufsize);
}
/*
 * GLOBAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */
void app_bt_debug_reponse_handle(uint8_t *buf, uint16_t len)
{
    switch(bt_debug_state)
    {
        case BT_DEBUG_PROCESS:
            debug_uart_data_tx_sync_send(buf, len);
            bt_debug_state = BT_DEBUG_IDLE;
            break;
        default:
            break;
    }
}

void bt_debug_exit(void)
{
    uart_cmd_response(DEBUG_EXIT_DEBUG_MODE);

    app_uart_abort_receive(DEBUG_UART_ID);
    app_uart_deinit(DEBUG_UART_ID);

    bt_debug_mode_ctl = BT_DEBUG_INVALID;

    /* Recovery jlink io. */
    gpio_init_t GPIO_jlink;
    GPIO_jlink.pin = GPIO_PIN_0 | GPIO_PIN_1;
    GPIO_jlink.mode = GPIO_MODE_MUX;
    GPIO_jlink.pull = GPIO_PULLDOWN;
    GPIO_jlink.mux = GPIO_MUX_0;
    hal_gpio_init(GPIO0, &GPIO_jlink);

    bt_debug_on_exit_debug_mode();

    /* Avoid interference caused by log recovery. */
    delay_ms(10);

#if (APP_LOG_ENABLE == 1)
    /* If the debug uart is the same as the log uart, restore log. */
    if(DEBUG_UART_ID == APP_UART_ID)
    {
        bsp_log_init();
    }
#endif

}

void bt_debug_task(void)
{
    switch(bt_debug_mode_ctl)
    {
        case BT_DEBUG_POLLING:
            return;
        case BT_DEBUG_DISABLE:
            bt_debug_exit();
            return;
        case BT_DEBUG_ENABLE:
            break;
        default:
            return;
    }

    bt_debug_mode_ctl = BT_DEBUG_POLLING;

    // TODO: assert buf and transmit_func
    bt_debug_on_enter_debug_mode();

#if (APP_LOG_ENABLE == 1)
    /* If the debug uart is the same as the log uart, stop log. */
    if(DEBUG_UART_ID == APP_UART_ID)
    {
        app_log_init(NULL, NULL, NULL);
    }
#endif

    debug_uart_init();

    /* Turn on bt uart to accept */
    bt_debug_open_to_receive();
}

void bt_debug_set_bt_transmit_func(bt_uart_transmit_sync_t transmit_func)
{
    s_bt_transmit_func = transmit_func;
}

void bt_debug_set_buffer(uint8_t *tx_buf, uint16_t tx_bufsize, uint8_t *rx_buf, uint16_t rx_bufsize)
{
    debug_uart_tx_buffer = tx_buf;
    debug_uart_tx_bufsize = tx_bufsize;
    debug_uart_rx_buffer = rx_buf;
    debug_uart_rx_bufsize = rx_bufsize;
}

bool bt_debug_mode_is_active(void)
{
    return (bt_debug_mode_ctl == BT_DEBUG_POLLING) || (bt_debug_mode_ctl == BT_DEBUG_ENABLE);
}

__WEAK void bt_debug_on_enter_debug_mode(void)
{
}

__WEAK void bt_debug_on_exit_debug_mode(void)
{
}

__WEAK void bt_debug_open_to_receive(void)
{
}
