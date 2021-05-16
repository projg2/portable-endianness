// SPDX-License-Identifier: Unlicense

#include <portable-endianness.h>

/* inspired by
 * https://commandcenter.blogspot.com/2012/04/byte-order-fallacy.html
 */

/* LOAD */
uint16_t load16_from_LE(const unsigned char* src)
{
    return (uint16_t)(src[0] << 0) | (uint16_t)(src[1] << 8);
}

uint32_t load32_from_LE(const unsigned char* src)
{
    return ((uint32_t)src[0] << 0) | ((uint32_t)src[1] << 8) | ((uint32_t)src[2] << 16) |
           ((uint32_t)src[3] << 24);
}

uint64_t load64_from_LE(const unsigned char* src)
{
    return ((uint64_t)src[0] << 0) | ((uint64_t)src[1] << 8) | ((uint64_t)src[2] << 16) |
           ((uint64_t)src[3] << 24) | ((uint64_t)src[4] << 32) | ((uint64_t)src[5] << 40) |
           ((uint64_t)src[6] << 48) | ((uint64_t)src[7] << 56);
}

uint16_t load16_from_BE(const unsigned char* src)
{
    return (uint16_t)(src[0] << 8) | (uint16_t)(src[1] << 0);
}

uint32_t load32_from_BE(const unsigned char* src)
{
    return ((uint32_t)src[0] << 24) | ((uint32_t)src[1] << 16) | ((uint32_t)src[2] << 8) |
           ((uint32_t)src[3] << 0);
}

uint64_t load64_from_BE(const unsigned char* src)
{
    return ((uint64_t)src[0] << 56) | ((uint64_t)src[1] << 48) | ((uint64_t)src[2] << 40) |
           ((uint64_t)src[3] << 32) | ((uint64_t)src[4] << 24) | ((uint64_t)src[5] << 16) |
           ((uint64_t)src[6] << 8) | ((uint64_t)src[7] << 0);
}

/* STORE */
void store16_to_LE(const uint16_t val, unsigned char* dest)
{
    dest[0] = (val >> 0);
    dest[1] = (val >> 8);
}

void store32_to_LE(const uint32_t val, unsigned char* dest)
{
    dest[0] = (val >> 0);
    dest[1] = (val >> 8);
    dest[2] = (val >> 16);
    dest[3] = (val >> 24);
}

void store64_to_LE(const uint64_t val, unsigned char* dest)
{
    dest[0] = (val >> 0);
    dest[1] = (val >> 8);
    dest[2] = (val >> 16);
    dest[3] = (val >> 24);
    dest[4] = (val >> 32);
    dest[5] = (val >> 40);
    dest[6] = (val >> 48);
    dest[7] = (val >> 56);
}

void store16_to_BE(const uint16_t val, unsigned char* dest)
{
    dest[0] = (val >> 8);
    dest[1] = (val >> 0);
}

void store32_to_BE(const uint32_t val, unsigned char* dest)
{
    dest[0] = (val >> 24);
    dest[1] = (val >> 16);
    dest[2] = (val >> 8);
    dest[3] = (val >> 0);
}

void store64_to_BE(const uint64_t val, unsigned char* dest)
{
    dest[0] = (val >> 56);
    dest[1] = (val >> 48);
    dest[2] = (val >> 40);
    dest[3] = (val >> 32);
    dest[4] = (val >> 24);
    dest[5] = (val >> 16);
    dest[6] = (val >> 8);
    dest[7] = (val >> 0);
}
