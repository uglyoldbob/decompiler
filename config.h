#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <cstdint>

#if TARGET32
	typedef uint32_t address;
#elif TARGET64
	typedef uint64_t address;
#else
#error "Unknown Target"
#endif

void reverse(uint64_t *in, int rbo);
void reverse(uint32_t *in, int rbo);
void reverse(uint16_t *in, int rbo);

#endif