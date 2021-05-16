#ifndef PORTABLE_ENDIANNESS_H
#define PORTABLE_ENDIANNESS_H

// SPDX-License-Identifier: Unlicense

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* LOAD */
uint16_t load16_from_LE(const unsigned char* src);
uint32_t load32_from_LE(const unsigned char* src);
uint64_t load64_from_LE(const unsigned char* src);

uint16_t load16_from_BE(const unsigned char* src);
uint32_t load32_from_BE(const unsigned char* src);
uint64_t load64_from_BE(const unsigned char* src);

/* STORE */
void store16_to_LE(const uint16_t val, unsigned char* dest);
void store32_to_LE(const uint32_t val, unsigned char* dest);
void store64_to_LE(const uint64_t val, unsigned char* dest);

void store16_to_BE(const uint16_t val, unsigned char* dest);
void store32_to_BE(const uint32_t val, unsigned char* dest);
void store64_to_BE(const uint64_t val, unsigned char* dest);

#ifdef __cplusplus
}
#endif

#endif /* PORTABLE_ENDIANNESS_H */
