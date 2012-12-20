#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdint.h>

#if TARGET32
	typedef uint32_t address;
#elif TARGET64
	typedef uint64_t address;
#else
#error "Unknown Target"
#endif

#endif