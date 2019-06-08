#pragma once

#include <stdint.h>
#include <stdio.h>

uint16_t make_crc16(const void *pbuffer, size_t size);

void append_crc16(uint8_t *data, uint16_t data_len);

uint32_t make_crc32(const void* pbuffer, size_t size);