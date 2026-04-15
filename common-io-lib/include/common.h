/*******************************************************************
 *                Sumukha IO-TOOLs OPEN-SOURCE                      *
 *******************************************************************/

#ifndef COMMON_H
#define COMMON_H

#ifdef __cplusplus
    extern "C" {
#endif

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
#define _LARGEFILE64_SOURCE     /* To use lseek64 explicitly */
#define _FILE_OFFSET_BITS 64
#include <unistd.h>
#include <malloc.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif

/* ========================================================================
 * Platform-Specific Helper Macros
 * ======================================================================== */

#ifdef _WIN32

#define ATTR_EXPORT                                 \
    __declspec(dllexport)

#define ATTR_IMPORT                                 \
    __declspec(dllimport)

#define ATTR_ALIGN(n)                               \
    __declspec(align(n))

#define NO_RETURN                                   \
    __declspec(noreturn)

#define ATTR_INLINE                                 \
    __forceinline

#define ATTR_DEPRECATED(msg)                        \
    __declspec(deprecated(msg))

#define LIBC_CALL_CONVENTION                        \
    __cdecl

#define ATTR_PACKED_STRUCT(struct_name, structure)  \
    {                                               \
        __pragma(pack(push, 1))                     \
        typedef struct {                            \
            structure                               \
        } struct_name;                              \
        __pragma(pack(pop))                         \
    }

#define SAFE_FPRINTF(stream, format, ...)           \
    fprintf_s(stream, format, ##__VA_ARGS__)

#define SAFE_MEMCPY(dest, destsz, src, count)       \
    memcpy_s(dest, destsz, src, count)

#define SAFE_MEMCLEAR(dest, count)                  \
    SecureZeroMemory(dest, count)

#define SAFE_SNPRINTF(dest, destsz, fmt, ...)       \
    _snprintf_s(dest, destsz, destsz, fmt, ##__VA_ARGS__)

#define ALIGNED_MEM_ALLOC(size, alignment)          \
    _aligned_malloc(size, alignment)

#define ALIGNED_MEM_FREE(ptr)                       \
    _aligned_free(ptr)

#define SAFE_STRTOK(str, delim, context)            \
    strtok_s(str, delim, context)

#define SAFE_STRNCAT(dest, destsz, src, count)      \
    strncat_s(dest, destsz, src, count)

#endif /* _WIN32 */

#ifdef __linux__

#define ATTR_EXPORT                                 \
    __attribute__((visibility("default")))

#define ATTR_IMPORT

#define NO_RETURN                                   \
    __attribute__((noreturn))

#define ATTR_INLINE                                 \
    __attribute__((always_inline))

#define ATTR_DEPRECATED(msg)                        \
    __attribute__(deprecated(msg))

#define ATTR_ALIGN(n)                               \
    __attribute__((aligned(n)))

#define LIBC_CALL_CONVENTION

#define ATTR_PACKED_STRUCT(struct_name, structure)  \
    {                                               \
        typedef struct {                            \
            structure                               \
        } struct_name __attribute__((packed));      \
    }

#define SAFE_FPRINTF(stream, format, ...)           \
    fprintf(stream, format, ##__VA_ARGS__)

#define SAFE_MEMCPY(dest, destsz, src, count)       \
    memcpy(dest, src, count)

#define SAFE_MEMCLEAR(dest, count)                  \
    explicit_bzero(dest, count)

#define SAFE_SNPRINTF(dest, destsz, fmt, ...)       \
    snprintf(dest, destsz, fmt, ##__VA_ARGS__)

#define ALIGNED_MEM_ALLOC(size, alignment)          \
    aligned_alloc(alignment, size)

#define ALIGNED_MEM_FREE(ptr)                       \
    free(ptr)

#define SAFE_STRTOK(str, delim, context)            \
    strtok_r(str, delim, &context)

#define SAFE_STRNCAT(dest, destsz, src, count)      \
    strncat(dest, destsz, src, count)

#endif /* __linux__ */

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

#define MAX_FILE_NAME_LEN               BYTES_128
#define MAX_ERROR_LEN                   BYTES_256
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
 * Type Definitions & Global variables
 * ======================================================================== */

typedef enum {

    TE_FAIL                             = 0,
    TE_PASS                             = 1

} status_t;

char error_buffer[MAX_ERROR_LEN];

#ifdef _WIN32

ATTR_IMPORT extern uint8_t          g_log_verbosity;
typedef BY_HANDLE_FILE_INFORMATION  dev_stat;

#elif defined(__linux__)

typedef struct stat                 dev_stat;

#endif

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

ATTR_EXPORT log_err_dump_t g_log_err_dump;

typedef struct {
    uint32_t error_num;
    char error_buffer[MAX_ERROR_LEN];
} error_t;
typedef enum {

    BACK_SLASH = 0,
    FORWARD_SLASH

} slash_type_t;

typedef struct {

#ifdef _WIN32
    HANDLE*                 handle;
    DWORD                   desired_access_mode;
    DWORD                   share_mode;
    LPSECURITY_ATTRIBUTES   security_attributes;
    DWORD                   creation_disposition;
    DWORD                   flags_and_attributes;
    HANDLE                  template_file;
#elif __linux__
    int                     flags;
#endif

} device_open_attr_t;

typedef struct {

#ifdef _WIN32
    HANDLE*         handle_p;
    LARGE_INTEGER   offset;
    PLARGE_INTEGER  new_fp;
    DWORD           whence;
#elif __linux__
    int*            fd_p;
    off_t           offset;
    int             whence;
#endif

} dev_seek_t;

typedef struct {

#ifdef _WIN32
    HANDLE*         handle_p;
#elif defined(__linux__)
    int*            fd_p;
#endif
    dev_stat*     dev_stat_p;

} dev_stat_t;

typedef struct {

    char        devname[MAX_FILE_NAME_LEN];
    uint64_t    device_bytes_size;
    uint32_t    physical_sector_size;
    uint32_t    device_sector_size;

} device_t;

/* ========================================================================
 * Function Declarations
 * ======================================================================== */

ATTR_EXPORT char* LIBC_CALL_CONVENTION print_error();

ATTR_EXPORT bool LIBC_CALL_CONVENTION get_local_time(struct tm* local_time,
                                                     long long unsigned int* micro_seconds);

ATTR_EXPORT void LIBC_CALL_CONVENTION log_err_dump_init(char* log_file,
                                                        char* err_file,
                                                        char* dump_file,
                                                        char* tool_name);

ATTR_EXPORT void LIBC_CALL_CONVENTION log_info(uint8_t log_level,
                                               const char* format,
                                               ...);

ATTR_EXPORT void LIBC_CALL_CONVENTION log_error(log_err_dump_t* err,
                                                const char* format,
                                                ...);

ATTR_EXPORT void LIBC_CALL_CONVENTION log_err_dump_close(log_err_dump_t* log_err_dump);

ATTR_EXPORT void LIBC_CALL_CONVENTION dump_buffer(log_err_dump_t* dump,
                                                  void* buffer,
                                                  size_t size);

ATTR_EXPORT bool LIBC_CALL_CONVENTION aligned_buffer_alloc(size_t size,
                                                           size_t alignment,
                                                           void* ptr);

ATTR_EXPORT void LIBC_CALL_CONVENTION aligned_buffer_free(void* ptr);

ATTR_EXPORT bool LIBC_CALL_CONVENTION get_ascii_devname(const char* devname,
                                                        size_t size,
                                                        char* ascii_devname,
                                                        slash_type_t slash_type);

ATTR_EXPORT bool LIBC_CALL_CONVENTION open_device(char* devname, 
                                                  device_open_attr_t* dev_attr,
                                                  void* handle);

ATTR_EXPORT void LIBC_CALL_CONVENTION close_device(void* handle);

ATTR_EXPORT bool LIBC_CALL_CONVENTION device_seek(dev_stat_t* dev_stat);

ATTR_EXPORT bool LIBC_CALL_CONVENTION device_stat(dev_stat_t* dev_stat_t);

#endif /* COMMON_H */


#ifdef __cplusplus
    }
#endif
