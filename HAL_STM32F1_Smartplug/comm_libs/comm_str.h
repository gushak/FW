#pragma once

#include "comm_def.h"

size_t bin_from_hex_str(uint8_t bin[], size_t bin_size, const char *str);

char* hex_str_from_bin(const uint8_t *bstr, size_t len, char result[], size_t result_size);

void set_network_word(uint8_t* dest, uint16_t val);

void set_network_dword(uint8_t* dest, uint32_t val);

uint16_t get_network_word(const uint8_t* data);

uint32_t get_network_dword(const uint8_t* data);

size_t nstrlen(const char *str, size_t max_len);

int is_plain_str(const char* str, size_t max_len);

