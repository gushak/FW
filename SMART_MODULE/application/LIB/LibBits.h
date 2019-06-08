/*
 * Copyright (C) 2013, 2016 Swift Navigation Inc.
 * Contact: Fergus Noble <fergus@swift-nav.com>
 *
 * This source is subject to the license found in the file 'LICENSE' which must
 * be be distributed together with this source. All other rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 */

#ifndef LIB_BITS_H
#define LIB_BITS_H
#include "mbed.h"

uint8_t parity(uint32_t x);
uint32_t getbitu(const uint8_t *buff, uint32_t pos, uint8_t len);
int32_t getbits(const uint8_t *buff, uint32_t pos, uint8_t len);
void setbitu(uint8_t *buff, uint32_t pos, uint32_t len, uint32_t data);
void setbits(uint8_t *buff, uint32_t pos, uint32_t len, int32_t data);
void bitcopy(void *dst, uint32_t dst_index,
	const void *src, uint32_t src_index, uint32_t count);
void bitshl(void *buf, uint32_t size, uint32_t shift);
uint8_t count_bits_u64(uint64_t v, uint8_t bv);
uint8_t count_bits_u32(uint32_t v, uint8_t bv);
uint8_t count_bits_u16(uint16_t v, uint8_t bv);
uint8_t count_bits_u8(uint8_t v, uint8_t bv);

#endif /* LIBSWIFTNAV_BITS_H */
