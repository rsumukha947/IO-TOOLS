/*******************************************************************
 *                Sumukha IO-TOOLs OPEN-SOURCE                      *
 *******************************************************************/

#ifndef COMMON_H
#define COMMON_H

#define __STDC_WANT_LIB_EXT1__ 1

/* Standard C Library Includes */
#include <assert.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Platform-Specific Includes */
#ifdef _WIN32
#include <windows.h>
#elif __linux__
#include <malloc.h>
#endif

/* ========================================================================
 * Assertions and Basic Macros
 * ======================================================================== */

#define ASSERT(expr)                    assert(expr)

/* ========================================================================
 * Size Constants (Bytes)
 * ======================================================================== */

#define BYTES_128                       128
#define BYTES_256                       256
#define BYTES_512                       512
#define BYTES_1K                        1024
#define BYTES_2K                        2048
#define BYTES_4K                        4096
#define BYTES_8K                        8192
#define BYTES_16K                       16384

#define MAX_FILE_NAME_LEN               128

/* ========================================================================
 * Bit Size Constants
 * ======================================================================== */

#define BIT_1                           1
#define BIT_8                           8
#define BIT_16                          16
#define BIT_32                          32
#define BIT_64                          64

/* ========================================================================
 * Bit Shift Macros - Right Shift
 * ======================================================================== */

#define BIT_8_RIGHT_SHIFT(x)            ((x) >> 8)
#define BIT_16_RIGHT_SHIFT(x)           ((x) >> 16)
#define BIT_32_RIGHT_SHIFT(x)           (ASSERT(sizeof(x) >= 8), (x) >> 32)

/* ========================================================================
 * Bit Shift Macros - Left Shift
 * ======================================================================== */

#define BIT_8_LEFT_SHIFT(x)             ((x) << 8)
#define BIT_16_LEFT_SHIFT(x)            ((x) << 16)
#define BIT_32_LEFT_SHIFT(x)            (ASSERT(sizeof(x) >= 8), (x) << 32)

/* ========================================================================
 * Unit Conversion Macros - Shift Values
 * ======================================================================== */

#define KB_SHIFT                        10
#define MB_SHIFT                        20
#define GB_SHIFT                        30

/* ========================================================================
 * Unit Conversion Macros - Conversions
 * ======================================================================== */

#define KB(x)                           ((x) << (KB_SHIFT))
#define MB(x)                           ((x) << (MB_SHIFT))
#define GB(x)                           ((x) << (GB_SHIFT))

/* ========================================================================
 * Endianness Conversion Macros - Little Endian to Big Endian
 * ======================================================================== */

#define LIL_END_TO_BIG_END_16(x)        \
    ((BIT_8_RIGHT_SHIFT(x) & 0x00FFU) | (BIT_8_LEFT_SHIFT(x) & 0xFF00U))

#define LIL_END_TO_BIG_END_24(x)        \
    (((BIT_16_LEFT_SHIFT(x)) & 0xFF0000U) | ((x) & 0x00FF00U) | \
     (BIT_16_RIGHT_SHIFT(x) & 0x0000FFU))

#define LIL_END_TO_BIG_END_32(x)        \
    ((LIL_END_TO_BIG_END_16(BIT_16_RIGHT_SHIFT(x)) & 0x0000FFFFU) | \
     (BIT_16_LEFT_SHIFT(LIL_END_TO_BIG_END_16(x)) & 0xFFFF0000U))

#define LIL_END_TO_BIG_END_64(x)        \
    ((LIL_END_TO_BIG_END_32(BIT_32_RIGHT_SHIFT(x)) & 0x00000000FFFFFFFFUL) | \
     (BIT_32_LEFT_SHIFT(LIL_END_TO_BIG_END_32(x)) & 0xFFFFFFFF00000000UL))

/* ========================================================================
 * Endianness Conversion Macros - Big Endian to Little Endian
 * ======================================================================== */

#define BIG_END_TO_LIL_END_16(x)        LIL_END_TO_BIG_END_16(x)
#define BIG_END_TO_LIL_END_24(x)        LIL_END_TO_BIG_END_24(x)
#define BIG_END_TO_LIL_END_32(x)        LIL_END_TO_BIG_END_32(x)
#define BIG_END_TO_LIL_END_64(x)        LIL_END_TO_BIG_END_64(x)

/* ========================================================================
 * Default File Stream Definitions
 * ======================================================================== */

#define DEFAULT_LOG_FILE                stdout
#define DEFAULT_ERROR_LOG_FILE          stderr
#define DEFAULT_INPUT_FILE              stdin
#define DEFAULT_OUTPUT_FILE             stdout
#define DEFAULT_DUMP_FILE               DEFAULT_LOG_FILE
#define DEFAULT_LOG_LEVEL               4

/* ========================================================================
 * Log message type Definitions
 * ======================================================================== */

#define INFO                            "INFO:"
#define ERR                             "ERROR:"
#define DUMP                            "DUMP:"
#define WARN                            "WARN:"
#define DEBUG                           "DEBUG:"

/* ========================================================================
 * Type Definitions
 * ======================================================================== */

typedef enum {
    TE_FAIL                             = 0,
    TE_PASS                             = 1
} status_t;

typedef struct {
    FILE*   log_file_p;
    FILE*   err_file_p;
    FILE*   dump_file_p;
    char    log_file[MAX_FILE_NAME_LEN];
    char    err_file[MAX_FILE_NAME_LEN];
    char    tool_name[MAX_FILE_NAME_LEN];
    char    dump_file[MAX_FILE_NAME_LEN];
    uint8_t log_level;
    bool    use_dump;
} log_err_dump_t;

/* ========================================================================
 * Platform-Specific Helper Macros
 * ======================================================================== */

#ifdef _WIN32

#define pack_structure(structure, struct_name)      \
    {                                               \
        __pragma(pack(push, 1))                     \
        typedef struct {                            \
            structure                               \
        } struct_name;                              \
        __pragma(pack(pop))                         \
    }

#define safe_fprintf(stream, format, ...)           \
    fprintf_s(stream, format, ##__VA_ARGS__)

#define safe_memcpy(dest, destsz, src, count)       \
    memcpy_s(dest, destsz, src, count)

#define safe_memclear(dest, count)                  \
    SecureZeroMemory(dest, count)

#define safe_snprintf(dest, destsz, fmt, ...)       \
    _snprintf_s(dest, destsz, destsz, fmt, ##__VA_ARGS__)

#define alligned_mem_alloc(size, alignment)         \
    _aligned_malloc(size, alignment)

#define alligned_mem_free(ptr)                      \
    _aligned_free(ptr)

#endif /* _WIN32 */

#ifdef __linux__

#define pack_structure(structure, struct_name)      \
    {                                               \
        typedef struct {                            \
            structure                               \
        } __attribute__((packed)) struct_name;      \
    }

#define safe_fprintf(stream, format, ...)           \
    fprintf(stream, format, ##__VA_ARGS__)

#define safe_memcpy(dest, destsz, src, count)       \
    memcpy(dest, src, count)

#define safe_memclear(dest, count)                  \
    explicit_bzero(dest, count)

#define safe_snprintf(dest, destsz, fmt, ...)       \
    snprintf(dest, destsz, fmt, ##__VA_ARGS__)

#define alligned_mem_alloc(size, alignment)         \
    aligned_alloc(alignment, size)

#define alligned_mem_free(ptr)                      \
    free(ptr)

#endif /* __linux__ */

/* ========================================================================
 * Function Declarations
 * ======================================================================== */

status_t get_local_time(struct tm* local_time,
                        long long unsigned int* micro_seconds);

void log_err_dump_init(log_err_dump_t* log_err_dump,
                       char* log_file,
                       char* err_file,
                       char* dump_file,
                       char* tool_name);

void log_info(log_err_dump_t* log,
              uint8_t log_level,
              const char* format,
              ...);

void log_error(log_err_dump_t* err,
               const char* format,
               ...);

void log_err_dump_close(log_err_dump_t* log_err_dump);

void dump_buffer(log_err_dump_t* dump,
                 void* buffer,
                 size_t size);

bool aligned_buffer_alloc(size_t size,
                           size_t alignment,
                           void* ptr);

void aligned_buffer_free(void* ptr);

#endif /* COMMON_H */
