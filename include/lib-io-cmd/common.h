/*******************************************************************
*                Sumukha IO-TOOLs OPEN-SOURCE                      *
********************************************************************/

#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define BYTES_128                       128
#define BYTES_256                       256
#define BYTES_512                       512
#define BYTES_1K                        1024
#define BYTES_2K                        2048
#define BYTES_4K                        4096
#define BYTES_8K                        8192
#define BYTES_16K                       16384

#define BIT_1                           1
#define BIT_8                           8
#define BIT_16                          16
#define BIT_32                          32
#define BIT_64                          64

#define BIT_8_RIGHT_SHIFT(x)            (x >> 8)
#define BIT_16_RIGHT_SHIFT(x)           (x >> 16)
#define BIT_32_RIGHT_SHIFT(x)           (x >> 32)
#define BIT_64_RIGHT_SHIFT(x)           (x >> 64)

#define BIT_8_LEFT_SHIFT(x)             (x << 8)
#define BIT_16_LEFT_SHIFT(x)            (x << 16)
#define BIT_32_LEFT_SHIFT(x)            (x << 32)
#define BIT_64_LEFT_SHIFT(x)            (x << 64)

#define KB_SHIFT                        10
#define MB_SHIFT                        20
#define GB_SHIFT                        30

#define KB(x)                           ((x) << (KB_SHIFT))
#define MB(x)                           ((x) << (MB_SHIFT))
#define GB(x)                           ((x) << (GB_SHIFT))

#define LIL_END_TO_BIG_END_16(x)        ((BIT_8_RIGHT_SHIFT(x) & 0xFF) | (BIT_8_LEFT_SHIFT(x) & 0xFF))
#define LIL_END_TO_BIG_END_24(x)        (((BIT_16_LEFT_SHIFT(x)) & 0xFF) | (BIT_16_RIGHT_SHIFT(x) & 0xFF))
#define LIL_END_TO_BIG_END_32(x)        ((LIL_END_TO_BIG_END_16(BIT_16_RIGHT_SHIFT(x)) & 0xFFFF) | (LIL_END_TO_BIG_END_16(BIT_16_LEFT_SHIFT(x)) & 0xFFFF))
#define LIL_END_TO_BIG_END_64(x)        ((LIL_END_TO_BIG_END_32(BIT_32_RIGHT_SHIFT(x)) & 0xFFFFFFFF) | (LIL_END_TO_BIG_END_32(BIT_32_LEFT_SHIFT(x)) & 0xFFFFFFFF))

#define BIG_END_TO_LIL_END_16(x)        LIL_END_TO_BIG_END_16(x)
#define BIG_END_TO_LIL_END_24(x)        LIL_END_TO_BIG_END_24(x)
#define BIG_END_TO_LIL_END_32(x)        LIL_END_TO_BIG_END_32(x)
#define BIG_END_TO_LIL_END_64(x)        LIL_END_TO_BIG_END_64(x)

typedef enum {
    TE_FAIL = 0,
    TE_PASS = 1
} status_t;

#ifdef _WIN32

#define pack_structure(structure, struct_name)  {\
    __pragma(pack(push, 1))                      \
    typedef struct {                             \
            structure                            \
    } struct_name;                               \
    __pragma(pack(pop))                          \
    }

#define safe_fprintf fprintf_s
#endif

#ifdef __linux__

#define pack_structure(structure, struct_name)  {\
    typedef struct {                             \
            structure                            \
    } __attribute__((packed))  struct_name;      \
    }
#define safe_fprintf fprintf
#endif

extern status_t get_local_time (struct tm* local_time, const char* raw_time);
extern void log_info (void* log_file, uint8_t log_level, const char* format, ...);
extern void log_error (void* err_file, const char* format, ...);

#endif
