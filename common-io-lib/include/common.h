/*******************************************************************
 *                Sumukha IO-TOOLs OPEN-SOURCE                      *
 *******************************************************************/

#ifndef COMMON_H
#define COMMON_H

#ifdef __cplusplus
    extern "C" {
#endif

#include "platform.h"

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
