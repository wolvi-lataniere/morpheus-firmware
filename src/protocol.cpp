/**
 * @file protocol.cpp
 * @author Aurelien VALADE (aurelien.valade@balena.io)
 * @brief Morpheus project UART protocol utilities module
 * @version 0.1
 * @date 2022-07-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <zephyr/zephyr.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include "protocol.h"


typedef enum __morpheus_frame_decoding_state_machine_state{
  MFDSS_IDLE,
  MFDSS_HEADER2,
  MFDSS_HEADER_OK,
  MFDSS_READING,
  MFDSS_WAIT_CSUM
} MFDS_STATE;

/* queue to store up to 3 messages (aligned to 4-byte boundary) */
K_MSGQ_DEFINE(uart_msgq, MORPHEUS_FRAME_SIZE, 4, 1);
K_MSGQ_DEFINE(uart_sendmsgq, MORPHEUS_FRAME_SIZE, 4, 1);

/* receive buffer used in UART ISR callback */
static uint8_t rx_buf[MORPHEUS_FRAME_SIZE];
static int rx_buf_pos;
static uint8_t tx_int_buf[MORPHEUS_FRAME_SIZE];
static const struct device *uart_dev = NULL;

/*
 * Read characters from UART until line end is detected. Afterwards push the
 * data to the message queue.
 */
static void morpheus_protocol_serial_cb(const struct device *dev, void *user_data)
{
	uint8_t c;
	static MFDS_STATE state = MFDSS_IDLE;
	static uint8_t csum;
	static uint8_t size;
	// uint32_t dtr;

	if (!uart_irq_update(dev)) {
		return;
	}

	// uart_line_ctrl_get(uart, UART_LINE_CTRL_DTR, &dtr);
	// if (!dtr) return;

	while (uart_irq_rx_ready(dev)) {

		uart_fifo_read(dev, &c, 1);

		switch (state)
		{
		case MFDSS_IDLE:
			if (c == 0x55) {
				state = MFDSS_HEADER2;
			}
			break;
		
		case MFDSS_HEADER2:
			if (c == 0xAA) {
				state = MFDSS_HEADER_OK;
				csum = 0;
				rx_buf_pos = 0;
			}
			else if (c != 0x55) {
				state = MFDSS_IDLE;
			}
			break;

		case MFDSS_HEADER_OK:
		    state = MFDSS_READING;
			size = c;
		    rx_buf[rx_buf_pos++] = c;
			csum = c;
			break;
		
		case MFDSS_READING:
		    rx_buf[rx_buf_pos++] = c;
			csum += c;
			if ((rx_buf_pos + 3) >= size) {
				state = MFDSS_WAIT_CSUM;
			}
			break;

		case MFDSS_WAIT_CSUM:
		    if (csum == c) 
                k_msgq_put(&uart_msgq, &rx_buf, K_NO_WAIT);
				
			state = MFDSS_IDLE;
			break;

		default:
			break;
		}
	}


	if (uart_irq_tx_ready(dev)) {
		if (k_msgq_get(&uart_sendmsgq, &tx_int_buf, K_NO_WAIT) == 0)
		{
			uart_fifo_fill(dev, tx_int_buf, tx_int_buf[2]);
		}
		else
		   uart_irq_tx_disable(dev);   
	}
}

/**
 * @brief Requests to send a frame on the UART bus
 * 
 * @param buffer Buffer containing the frame to be sent
 * @param len lenght of the frame to send
 */
void morpheus_protocol_send_frame(const uint8_t* buffer, uint8_t len) {
	static uint8_t send_buffer[MORPHEUS_FRAME_SIZE];
	send_buffer[0] = 0x55;
	send_buffer[1] = 0xAA;
	send_buffer[2] = len+4;
	uint8_t csum = len+4;
	int i;

    if (uart_dev == NULL)
        return;

	for (i=0; i<len; i++) {
		csum += buffer[i];
		send_buffer[3+i] = buffer[i];
	}
	send_buffer[3+len] = csum;

	uint32_t dtr;
	uart_line_ctrl_get(uart_dev, UART_LINE_CTRL_DTR, &dtr);
	if (!dtr) return;

	k_msgq_put(&uart_sendmsgq, &send_buffer, K_NO_WAIT);
	uart_irq_tx_enable(uart_dev);
}

/**
 * @brief Initialize the uart bus to communicate with the master
 * 
 * @param dev pointer to the UART device to communicate through
 * @return int 1 if success, 0 otherwise.
 */
int morpheus_protocol_init(const struct device *dev)
{
	if (!device_is_ready(dev)) {
		printk("UART device not found!");
		return 0;
	}

    uart_dev = dev;

    /* configure interrupt and callback to receive data */
	uart_irq_callback_user_data_set(dev, morpheus_protocol_serial_cb, NULL);
	uart_irq_rx_enable(dev);

    return 1;
}

int morpheus_protocol_get_frame(char *buffer)
{
    return k_msgq_get(&uart_msgq, buffer, K_FOREVER);
}
