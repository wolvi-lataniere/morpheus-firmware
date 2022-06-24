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
#include <string.h>

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   1000

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)
#define UART0_NODE DT_CHOSEN(zephyr_shell_uart)


/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct device *uart = DEVICE_DT_GET(UART0_NODE);

/*
 * Print a null-terminated string character by character to the UART interface
 */
void print_uart(char *buf)
{
	int msg_len = strlen(buf);

	for (int i = 0; i < msg_len; i++) {
		uart_poll_out(uart, buf[i]);
	}
}

void main(void)
{
	int ret;
	struct uart_config uart_cfg = {
       .baudrate = 115200,
	   .parity = UART_CFG_PARITY_NONE,
	   .stop_bits = UART_CFG_STOP_BITS_1,
	   .data_bits = UART_CFG_DATA_BITS_8,
	   .flow_ctrl = UART_CFG_FLOW_CTRL_NONE
	};

	if (!device_is_ready(led.port)) {
		return;
	}

	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return;
	}

	// ret = uart_configure(uart, &uart_cfg);
	// if (ret < 0) {
	// 	// return;
	// }

	while (1) {
		ret = gpio_pin_toggle_dt(&led);
		if (ret < 0) {
			return;
		}
		print_uart("hello\r\n");
		k_msleep(SLEEP_TIME_MS);
	}
}
