#ifndef PLATFORM_H
#define PLATFORM_H

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
#include <pthread.h>
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

#define ATTR_ALLIGNED_PACKED_STRUCT(struct_name, structure)  \
        __pragma(pack(push, 1))                     \
        typedef ATTR_ALIGN(64) struct {             \
            structure                               \
        } struct_name;                              \
        __pragma(pack(pop))

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

#define MUTEX_CREATE(security_attr, initial_owner, name) \
    CreateMutexA(security_attr, initial_owner, name)

#define MUTEX_LOCK(mutex)                          \
    WaitForSingleObject(mutex, INFINITE)

#define MUTEX_UNLOCK(mutex)                        \
    ReleaseMutex(mutex)

#define MUTEX_DESTROY(mutex)                       \
    CloseHandle(mutex)
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

#define ATTR_ALLIGNED_PACKED_STRUCT(struct_name, structure)  \
        typedef struct {                            \
            structure                               \
        } struct_name __attribute__((packed, aligned(64)));

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

#define 
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

#ifdef __cplusplus
    }
#endif

#endif /* PLATFORM_H */