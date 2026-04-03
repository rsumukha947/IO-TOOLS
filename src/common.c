/*******************************************************************
*                Sumukha IO-TOOLs OPEN-SOURCE                      *
********************************************************************/

#include "common.h"

/**
 * @brief Get the local time object
 * 
 * @param[in, out] local_time
 * @param [in] raw_time 
 * @return status_t
 */
status_t get_local_time(struct tm* local_time, long long unsigned int* micro_seconds) {

#ifdef _WIN32

    FILETIME ft;
    SYSTEMTIME st;

    GetSystemTimePreciseAsFileTime(&ft);
    if (!FileTimeToSystemTime(&ft, &st)) {
        printf("%s, Failed to convert FilTime to SystemTime: error=%lu\n", __func__,  GetLastError());
        return TE_FAIL;
    }

    ULARGE_INTEGER uli;
    uli.LowPart = ft.dwLowDateTime;
    uli.HighPart = ft.dwHighDateTime;

    long long unsigned int microseconds = (uli.QuadPart / 10) % 1000000;

    local_time->tm_year = st.wYear;
    local_time->tm_mon = st.wMonth;
    local_time->tm_mday = st.wDay;
    local_time->tm_hour = st.wHour;
    local_time->tm_min = st.wMinute;
    local_time->tm_sec = st.wSecond;
    local_time->tm_isdst = -1; // Not considering daylight saving time
    local_time->tm_yday = 0; // Not calculating day of the year
    local_time->tm_wday = 0; // Not calculating day of the week
    *micro_seconds = microseconds;

#elif __linux__

    struct timespec ts;
    if (-1 == clock_gettime(CLOCK_REALTIME, &ts)) {
        printf("%s: %s Failed to get time using clock_gettime: errno=%d\n", __func__, ERR, errno);
        return TE_FAIL;
    }
    if (NULL == localtime_r(&ts.tv_sec, local_time)) {
        printf("%s: %s Failed to convert time using localtime_r: errno=%d\n", __func__, ERR, errno);
        return TE_FAIL;
    }
    *micro_seconds = (ts.tv_nsec / 1000); // Convert nanoseconds to microseconds
    local_time->tm_year += 1900; // Adjust year to be since 1900
    local_time->tm_mon += 1; // Adjust month to be 1-12 instead of 0-11

#endif

    return TE_PASS;
}

void log_err_dump_init(log_err_dump_t* log_err_dump, char* log_file, char* err_file, char *dump_file, char* tool_name) {

    safe_memclear(log_err_dump, sizeof(log_err_dump_t));

    if(NULL != tool_name) {
        safe_snprintf(log_err_dump->tool_name, MAX_FILE_NAME_LEN, tool_name);
    } else {
        safe_snprintf(log_err_dump->tool_name, MAX_FILE_NAME_LEN, "Unknown Tool");
    }
    if(NULL == log_file) {
        printf("[Timer not started yet]: %s: %s Log file is NULL, defaulting to stdout.\n", __func__, WARN);
        log_err_dump->log_file_p = DEFAULT_LOG_FILE;
    } else {
        if (NULL == (log_err_dump->log_file_p = fopen(log_file, "a"))) {
            printf("[Timer not started yet]: %s: %s Unable to open log file errno=%d, defaulting to stdout.\n", __func__, WARN, errno);
            log_err_dump->log_file_p = DEFAULT_LOG_FILE;
        } else {
            safe_snprintf(log_err_dump->log_file, MAX_FILE_NAME_LEN, log_file);
        }
    }
    if(NULL == err_file) {
        printf("[Timer not started yet]: %s: %s Error file is NULL, defaulting to stderr.\n", __func__, WARN);
        log_err_dump->err_file_p = DEFAULT_ERROR_LOG_FILE;
    } else {
        if (NULL == (log_err_dump->err_file_p = fopen(err_file, "a"))) {
            printf("[Timer not started yet]: %s: %s Unable to open error file errno=%d, defaulting to stderr.\n", __func__, WARN, errno);
            log_err_dump->err_file_p = DEFAULT_ERROR_LOG_FILE;
        } else {
            safe_snprintf(log_err_dump->err_file, MAX_FILE_NAME_LEN, err_file);
        }
    }
    if(NULL == dump_file) {
        printf("[Timer not started yet]: %s: %s Dump file is NULL, defaulting to stdout.\n", __func__, WARN);
        log_err_dump->dump_file_p = DEFAULT_DUMP_FILE;
    } else {
        if(NULL == (log_err_dump->dump_file_p = fopen(dump_file, "a"))) {
            printf("[Timer not started yet]: %s: %s Unable to open dump file errno=%d, defaulting to stdout.\n", __func__, ERR, errno);
            log_err_dump->dump_file_p = DEFAULT_DUMP_FILE;
        } else {
            safe_snprintf(log_err_dump->dump_file, MAX_FILE_NAME_LEN, dump_file);
        }
    }
 }

void log_info(log_err_dump_t* log, uint8_t log_level, const char* format, ...) {

    va_list         args;
    long long unsigned int  micro_seconds = 0;
    struct tm               local_time = {0};
    FILE* file_p = (log->use_dump ? log->dump_file_p : log->log_file_p);

    if (get_local_time((struct tm*)&local_time, &micro_seconds)) {
        safe_fprintf(file_p, "{%s}[%02d-%02d-%04d %02d:%02d:%02d.%06llu]: ", log->tool_name, local_time.tm_mday, local_time.tm_mon, local_time.tm_year, local_time.tm_hour, local_time.tm_min, local_time.tm_sec, micro_seconds);
    } else {
        safe_fprintf(file_p, "{%s}[Unable to fetch local time]: ", log->tool_name);
    }
    va_start(args, format);
    safe_fprintf(file_p, "%d: ", log_level);
    vfprintf(file_p, format, args);
    safe_fprintf(file_p, "\n");
    va_end(args);
    fflush(file_p);

}

void log_error(log_err_dump_t* err, const char* format, ...) {

    va_list args;
    long long unsigned int  micro_seconds = 0;
    struct tm               local_time = {0};

    if (get_local_time((struct tm*)&local_time, &micro_seconds)) {
        safe_fprintf(err->err_file_p, "{%s}[%02d-%02d-%04d %02d:%02d:%02d.%06llu]: ", err->tool_name, local_time.tm_mday, local_time.tm_mon, local_time.tm_year, local_time.tm_hour, local_time.tm_min, local_time.tm_sec, micro_seconds);
    } else {
        safe_fprintf(err->err_file_p, "{%s}[Unable to fetch local time]: ", err->tool_name);
    }
    va_start(args, format);
    vfprintf(err->err_file_p, format, args);
    safe_fprintf(err->err_file_p, "\n");
    va_end(args);
    fflush(err->err_file_p);

}

void dump_buffer(log_err_dump_t* dump, void* buffer, size_t size) {

    uint32_t i = 0; uint8_t j = 0; uint64_t low = 0, high = 0;
    char str[BIT_16 + 1] = {0}; uint8_t* buffer_p = (uint8_t*)buffer;
    dump->use_dump = true;
    log_info(dump, 0, "Address \t\t\t\t\t\t\t\tData(Hex) \t\t\t\t\t\t\tASCII\n");
    while (size > i) {

        low = high = 0;
        safe_memclear(str, sizeof(str));

        if (16 > (size - i)) {
            uint8_t diff_len = (uint8_t)(size - i);
            uint8_t low_diff_len = (uint8_t)(diff_len > 8 ? 8 : diff_len);
            uint8_t high_diff_len = (uint8_t)(diff_len > 8 ? diff_len - 8 : 0);
            uint32_t idx = i;

            for (j = 0; j < low_diff_len; j++) {
                low = BIT_8_LEFT_SHIFT(low) | buffer_p[i + j];
            }
            if (high_diff_len) {
                i += low_diff_len;
                for (j = 0; j < high_diff_len; j++) {
                    high = BIT_8_LEFT_SHIFT(high) | buffer_p[i + j];
                }
            }
            safe_memcpy(str, sizeof(str), (buffer_p + idx), diff_len);
            log_info(dump, 0, "%s 0x%016llx \t0x%016llx%016llx \t%s", DUMP, (uint32_t)(i), low, high, str);
            dump->use_dump = false;
            return;
        } else {
            for (j = 0; j < BIT_8; j++) {
                low = BIT_8_LEFT_SHIFT(low) | buffer_p[i + j];
                high = BIT_8_LEFT_SHIFT(high) | buffer_p[i + j + BIT_8];
            }
            safe_memcpy(str, sizeof(str), (buffer_p + i), BIT_16);
        }

        log_info(dump, 0, "%s 0x%016llx \t0x%016llx%016llx \t%s", DUMP, (uint32_t)(i), low, high, str);
        i += 16;
    }
    dump->use_dump = false;
}

void log_err_dump_close(log_err_dump_t* log_err_dump) {

    if (log_err_dump->log_file_p && log_err_dump->log_file_p != DEFAULT_LOG_FILE) {
        fclose(log_err_dump->log_file_p);
    }
    if (log_err_dump->err_file_p && log_err_dump->err_file_p != DEFAULT_ERROR_LOG_FILE) {
        fclose(log_err_dump->err_file_p);
    }
    if (log_err_dump->dump_file_p && log_err_dump->dump_file_p != DEFAULT_DUMP_FILE) {
        fclose(log_err_dump->dump_file_p);
    }
}

bool alligned_buffer_alloc(size_t size, size_t alignment, void* ptr) {

    if ((size == 0 || alignment == 0) && (alignment <= size) && (0 != (size % alignment))) {
        printf("%s: %s Invalid input for size=%zu and alignment=%zu\n", __func__, ERR, size, alignment);
        return false;
    }
    if (NULL == (ptr = alligned_mem_alloc(size, alignment))) {
        printf("%s: %s Memory allocation failed for errno=%d, size=%zu and alignment=%zu\n", __func__, ERR, errno, size, alignment);
        return false;
    }

    return true;
}

void aligned_buffer_free(void* ptr) {
    if (ptr) {
        alligned_mem_free(ptr);
    }
}
