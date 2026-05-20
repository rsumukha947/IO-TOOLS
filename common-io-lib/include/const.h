#ifndef CONST_H
#define CONST_H

#ifdef __cplusplus
    extern "C" {
#endif

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

#ifdef __cplusplus
    }
#endif

#endif /* CONST_H */
