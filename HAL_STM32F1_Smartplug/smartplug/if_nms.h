#pragma once

#include "smartplug_def.h"
#include "NMS_format.h"

void nms_interface_init();

bool_t nms_set_mtu(uint16_t mtu);

void nms_send_command(uint16_t cmd_stat);

void nms_send_packet(uint16_t cmd_stat, const uint8_t* payload, uint16_t payload_len, uint16_t payload_limit);

bool_t nms_has_tx_packet();

bool_t nms_send_tx_packet();

void nms_clear_tx_packet();

void nms_dump(NMS_Direction_t packet_dir, const NMS_PacketStruct_t* packet, uint16_t packet_len);