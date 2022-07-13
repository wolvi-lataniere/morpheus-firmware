/**
 * @file protocol.h
 * @author Aurelien VALADE (aurelien.valade@balena.io)
 * @brief Morpheus project UART protocol utilities module header file
 * @version 0.1
 * @date 2022-07-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef __MORPHEUS_PROTOCOL_H__
#define __MORPHEUS_PROTOCOL_H__

#include <zephyr/zephyr.h>
#include <zephyr/device.h>

#define MORPHEUS_FRAME_SIZE 255

int morpheus_protocol_init(const struct device *dev);
void morpheus_protocol_send_frame(const uint8_t* buffer, uint8_t len);
int morpheus_protocol_get_frame(char *buffer);

#endif
