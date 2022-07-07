/**
 * @file main.cpp
 * @author Aurelien VALADE (aurelien.valade@balena.io)
 * @brief Morpheus is a companion application to be run on co-processor micro-controller, disgned to manage SingleBoard Computers deep-sleep capabilities.
 * @version 0.1
 * @date 2022-06-22
 * 
 * @copyright Copyright (c) 2022 Balena io
 * 
 */


#include <zephyr/zephyr.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/usb/usb_device.h>
#include <string.h>
#include "generated.h"

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   1000
#define MSG_SIZE 255

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)



/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct device *uart = DEVICE_DT_GET_ONE(zephyr_cdc_acm_uart);

typedef enum __morpheus_frame_decoding_state_machine_state{
  MFDSS_IDLE,
  MFDSS_HEADER2,
  MFDSS_HEADER_OK,
  MFDSS_READING,
  MFDSS_WAIT_CSUM
} MFDS_STATE;

/* queue to store up to 3 messages (aligned to 4-byte boundary) */
K_MSGQ_DEFINE(uart_msgq, MSG_SIZE, 3, 1);
K_MSGQ_DEFINE(uart_sendmsgq, MSG_SIZE, 3, 1);

/* receive buffer used in UART ISR callback */
static uint8_t rx_buf[MSG_SIZE];
static int rx_buf_pos;
static uint8_t tx_int_buf[MSG_SIZE];

/*
 * Read characters from UART until line end is detected. Afterwards push the
 * data to the message queue.
 */
void serial_cb(const struct device *dev, void *user_data)
{
	uint8_t c;
	static MFDS_STATE state = MFDSS_IDLE;
	static uint8_t csum;
	static uint8_t size;
	// uint32_t dtr;

	if (!uart_irq_update(uart)) {
		return;
	}

	// uart_line_ctrl_get(uart, UART_LINE_CTRL_DTR, &dtr);
	// if (!dtr) return;

	while (uart_irq_rx_ready(uart)) {

		uart_fifo_read(uart, &c, 1);

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


	if (uart_irq_tx_ready(uart)) {
		if (k_msgq_get(&uart_sendmsgq, &tx_int_buf, K_NO_WAIT) == 0)
		   uart_fifo_fill(uart, tx_int_buf, tx_int_buf[2]);
		else
		   uart_irq_tx_disable(uart);   
	}
}


void send_frame(const uint8_t* buffer, uint8_t len) {
	static uint8_t send_buffer[256];
	send_buffer[0] = 0x55;
	send_buffer[1] = 0xAA;
	send_buffer[2] = len+4;
	uint8_t csum = len+4;
	int i;

	for (i=0; i<len; i++) {
		csum += buffer[i];
		send_buffer[3+i] = buffer[i];
	}
	send_buffer[3+len] = csum;

	k_msgq_put(&uart_sendmsgq, &send_buffer, K_NO_WAIT);
	uart_irq_tx_enable(uart);
}

int main(void)
{
	int ret;
	char tx_buf[MSG_SIZE];

	if (!device_is_ready(led.port)) {
		return 0;
	}

    if (usb_enable(NULL) != 0){
		printk("Failed to enable USB");
		return 0;
	}
	

	if (!device_is_ready(uart)) {
		printk("UART device not found!");
		return 0;
	}

	/* configure interrupt and callback to receive data */
	uart_irq_callback_user_data_set(uart, serial_cb, NULL);
	uart_irq_rx_enable(uart);

	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return 0;
	}

	while (k_msgq_get(&uart_msgq, &tx_buf, K_FOREVER) == 0) {
		
		void* params;
		Instructions inst = INST_SLEEPTIME;
		int size = tx_buf[0];

		if (parse_instruction_frame(tx_buf+1, size, &inst, &params) < 0)
		{
			s_fb_getversion_params version{.major = 0, .minor = 1, .patch = 99};
			ret = build_feedback_getversion_frame(tx_buf, &size, &version);
			send_frame((const uint8_t*)tx_buf, size);
			continue;
		}
		
		switch (inst)
		{
		case INST_GETVERSION:
			{
				s_fb_getversion_params version{.major = 0, .minor = 1, .patch = 0};
				ret = build_feedback_getversion_frame(tx_buf, &size, &version);
				send_frame((const uint8_t*)tx_buf, size);
				break;
			}
		
		default:
			{
				s_fb_getversion_params version{.major = 0, .minor = 1, .patch = 98};
				ret = build_feedback_getversion_frame(tx_buf, &size, &version);
				send_frame((const uint8_t*)tx_buf, size);
				break;
			}
			break;
		}

		k_free(params);

		ret = gpio_pin_toggle_dt(&led);
		if (ret < 0) {
			return 0;
		}
	}
}
