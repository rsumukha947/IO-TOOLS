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
#include <semaphore.h>
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

#define SAFE_STRTOK(str, delim, context_p)          \
    strtok_s(str, delim, context_p)

#define SAFE_STRNCAT(dest, destsz, src, count)      \
    strncat_s(dest, destsz, src, count)

#define MUTEX_T                                     \
    HANDLE

#define MUTEX_CREATE(security_attr, initial_owner, name) \
    CreateMutexA(security_attr, initial_owner, name)

#define MUTEX_LOCK(mutex)                           \
    WaitForSingleObject(mutex, INFINITE)

#define MUTEX_UNLOCK(mutex)                         \
    ReleaseMutex(mutex)

#define MUTEX_DESTROY(mutex)                        \
    CloseHandle(mutex)

#define SEMAPHORE_T                                 \
    HANDLE

#define SEMAPHORE_CREATE(security_attr, init_count, max_count, name) \
    CreateSemaphoreA(security_attr, init_count, max_count, name)

#define SEMAPHORE_WAIT(semaphore)                   \
    WaitForSingleObject(semaphore, INFINITE)

#define SEMAPHORE_RELEASE(semaphore, release_count, prev_count) \
    ReleaseSemaphore(semaphore, release_count, prev_count)

#define SEMAPHORE_DESTROY(semaphore)                \
    CloseHandle(semaphore)

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

#define SAFE_STRTOK(str, delim, context_p)          \
    strtok_r(str, delim, context_p)

#define SAFE_STRNCAT(dest, destsz, src, count)      \
    strncat(dest, destsz, src, count)

#define MUTEX_T                                     \
    pthread_mutex_t

#define MUTEX_CREATE(mutex_addr, attr)              \
        pthread_mutex_init(mutex_addr, attr)

#define MUTEX_LOCK(mutex_addr)                      \
        pthread_mutex_lock(mutex_addr)

#define MUTEX_UNLOCK(mutex_addr)                    \
        pthread_mutex_unlock(mutex_addr)

#define MUTEX_DESTROY(mutex_addr)                   \
        pthread_mutex_destroy(mutex_addr)
#define SEMAPHORE_T                                 \
    sem_t

#define SEMAPHORE_CREATE(sem_addr, init_count, max_count) \
    sem_init(sem_addr, 0, init_count)

#define SEMAPHORE_WAIT(semaphore)                   \
    sem_wait(semaphore)

#define SEMAPHORE_RELEASE(semaphore, release_count, prev_count) \
    sem_post(semaphore)

#define SEMAPHORE_DESTROY(semaphore)                \
    sem_destroy(semaphore)
#endif /* __linux__ */

/* ========================================================================
 * Assertions and Basic Macros
 * ======================================================================== */

#define ASSERT(expr)                    assert(expr)

/* ========================================================================
 *  Platform specific structures for various operations
 * ======================================================================== */

 typedef struct mutex {

    MUTEX_T                 mutex;
#ifdef WIN32
    LPSECURITY_ATTRIBUTES   lpmutex_security_attr;
    BOOL                    initial_onwer;
    LPCSTR                  mutex_name;
#elif defined(__linux__)
    pthread_mutexattr_t     mutex_attr;
#endif

 } mutex_t;

 /* ========================================================================
 *  Platform specific functions for various operations
 * ======================================================================== */

 
#ifdef __cplusplus
    }
#endif

#endif /* PLATFORM_H */