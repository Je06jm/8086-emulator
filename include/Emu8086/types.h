#ifndef EMU8086_TYPES_H
#define EMU8086_TYPES_H

#include <stdint.h>
#include <stdbool.h>

#ifdef EMU_8086_FAST_TYPES
typedef uint_fast8_t nibble_t;
typedef uint_fast8_t byte_t;
typedef uint_fast16_t word_t;
typedef uint_fast32_t address_t;
#elif defined(EMU_8086_SMALL_TYPES)
typedef uint_least8_t nibble_t;
typedef uint_least8_t byte_t;
typedef uint_least16_t word_t;
typedef uint_least32_t address_t;
#else
typedef uint8_t nibble_t;
typedef uint8_t byte_t;
typedef uint16_t word_t;
typedef uint32_t address_t;
#endif

#ifndef NULL
#define NULL
#endif

#endif