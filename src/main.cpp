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
#include "protocol.h"
#include "morpheus-states.h"

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   1000

/* The devicetree node identifier for the "led0" alias. */
#define MORPHEUS_USER_NODE DT_PATH(zephyr_user)

/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
static const struct device *uart = DEVICE_DT_GET_ONE(zephyr_cdc_acm_uart);


int main(void)
{
	int ret;
	char tx_buf[MORPHEUS_FRAME_SIZE];

    if (usb_enable(NULL) != 0){
		printk("Failed to enable USB");
		return 0;
	}
	
	if (!morpheus_protocol_init(uart))
		return 0;

	morpheus_state_init();

	while ( morpheus_protocol_get_frame(tx_buf)== 0) {
		
		void* params;
		Instructions inst = INST_SLEEPTIME;
		int size = tx_buf[0];

		if (parse_instruction_frame(tx_buf+1, size, &inst, &params) < 0)
		{
			s_fb_getversion_params version{.major = 0, .minor = 1, .patch = 99};
			ret = build_feedback_getversion_frame(tx_buf, &size, &version);
			morpheus_protocol_send_frame((const uint8_t*)tx_buf, size);
			continue;
		}
		
		switch (inst)
		{
		case INST_GETVERSION:
			{
				s_fb_getversion_params version{.major = 0, .minor = 1, .patch = 1};
				ret = build_feedback_getversion_frame(tx_buf, &size, &version);
				morpheus_protocol_send_frame((const uint8_t*)tx_buf, size);
				break;
			}

		case INST_SLEEPPIN:
			{
            	uint16_t delay = ((s_inst_sleeppin_params*) params)->pre_sleep_time;
				bool wake_state = ((s_inst_sleeppin_params*) params)->wake_pin_active_state;
				s_fb_sleeppin_params fb {.success = true};
				ret = build_feedback_sleeppin_frame(tx_buf, &size, &fb);
				morpheus_protocol_send_frame((const uint8_t*)tx_buf, size);

				morpheus_sleep_pin(delay, wake_state);
				break;
			}

		case INST_SLEEPTIME:
			{
            	uint16_t delay = ((s_inst_sleeptime_params*) params)->pre_sleep_time;
				uint32_t sleep_time = ((s_inst_sleeptime_params*) params)->duration;
				s_fb_sleeptime_params fb {.feedback = 1};
				ret = build_feedback_sleeptime_frame(tx_buf, &size, &fb);
				morpheus_protocol_send_frame((const uint8_t*)tx_buf, size);

				morpheus_sleep_time(delay, sleep_time);
				break;
			}
		
		default:
			{
				s_fb_getversion_params version{.major = 0, .minor = 1, .patch = 98};
				ret = build_feedback_getversion_frame(tx_buf, &size, &version);
				morpheus_protocol_send_frame((const uint8_t*)tx_buf, size);
				break;
			}
			break;
		}

		k_free(params);

	}
}
