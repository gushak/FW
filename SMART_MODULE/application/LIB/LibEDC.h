/*
 * Copyright (C) 2010 Swift Navigation Inc.
 * Contact: Fergus Noble <fergus@swift-nav.com>
 *
 * This source is subject to the license found in the file 'LICENSE' which must
 * be be distributed together with this source. All other rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 */

#ifndef LIB_EDC_H
#define LIB_EDC_H

#include "mbed.h"
#include "Pipe.h"

uint32_t crc24q(const uint8_t *buf, uint32_t len, uint32_t crc);
uint32_t crc24q(Pipe<char>* pipe, uint32_t len, uint32_t crc);
uint32_t crc24q_bits(uint32_t crc, const uint8_t *buf, uint32_t n_bits, bool invert);

#endif /* LIBSWIFTNAV_EDC_H */
