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
status_t get_local_time (struct tm* local_time, long long unsigned int* micro_seconds) {

#ifdef WIN32

    FILETIME ft;
    SYSTEMTIME st;

    GetSystemTimePreciseAsFileTime(&ft);
    if (!FileTimeToSystemTime(&ft, &st)) {
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
        return TE_FAIL;
    }
    if (NULL == localtime_r(&ts.tv_sec, local_time)) {
        return TE_FAIL;
    }
    *micro_seconds = ts.tv_nsec / 1000; // Convert nanoseconds to microseconds
    local_time->tm_year += 1900; // Adjust year to be since 1900
    local_time->tm_mon += 1; // Adjust month to be 1-12 instead of 0-11

#endif

    return TE_PASS;
}

void log_common(void* log_file, char* tool_name, bool is_error) {

    struct tm       local_time = {0};
    static FILE*    log_file_p = NULL;
    static char     log_file_str[MAX_FILE_NAME_LEN] = {0};
    long long unsigned int micro_seconds = 0;

    if (NULL == log_file) {
        printf("{%s}[Timer not started yet]: Log file is NULL, defaulting to %s.\n", tool_name, is_error ? "stderr" : "stdout");
        log_file = is_error ? stderr : stdout;
    }

    if (!log_file_str[0]) {
        safe_snprintf(log_file_str, MAX_FILE_NAME_LEN, (char *)log_file);
    }

    if (((NULL == log_file_p) && (stdout != log_file)) || (0 != strcmp((char *)log_file, log_file_str))) {
        safe_snprintf(log_file_str, MAX_FILE_NAME_LEN, (char *)log_file);
        if (NULL == (log_file_p = fopen((char *)log_file, "a"))) {
            printf("{%s}[Timer not started yet]: Unable to open log file, defaulting to  %s.\n", tool_name, is_error ? "stderr" : "stdout");
            log_file_p = is_error ? stderr : stdout;
        }
    }

    if (get_local_time((struct tm*)&local_time, &micro_seconds)) {
        safe_fprintf(log_file, "{%s}[%02d-%02d-%04d %02d:%02d:%02d.06%lld]: ", tool_name, local_time.tm_mday, local_time.tm_mon, local_time.tm_year, local_time.tm_hour, local_time.tm_min, local_time.tm_sec, micro_seconds);
    } else {
        safe_fprintf(log_file, "{%s}[Unable to fetch local time]: ", tool_name);
    }

}

void log_info(void* log_file, uint8_t log_level, char* tool_name, const char* format, ...) {

    va_list         args;

    log_common(log_file, tool_name, false);
    va_start(args, format);
    safe_fprintf(log_file, "%d: ", log_level);
    vfprintf(log_file, format, args);
    safe_fprintf(log_file, "\n");
    va_end(args);
    fflush(log_file);

}

void log_error(void* err_file, char* tool_name, const char* format, ...) {

    va_list args;

    log_common(err_file, tool_name, true);
    va_start(args, format);
    vfprintf(err_file, format, args);
    safe_fprintf(err_file, "\n");
    va_end(args);
    fflush(err_file);

}

void dump_buffer(void* dump_file, char* tool_name, void* buffer, size_t size) {

    uint32_t i = 0; uint8_t j = 0; uint64_t low = 0, high = 0;
    char str[BIT_16 + 1] = {0}; uint8_t* buffer_p = (uint8_t*)buffer;

    while (size > i) {

        low = high = 0;
        safe_memclear(str, sizeof(str));

        if (16 > (size - i)) {

            uint8_t diff_len = (uint8_t)(size - i);
            uint8_t high_diff_len = (uint8_t)(diff_len > 8 ? diff_len - 8 : 0);

            for (j = 0; j < diff_len; j++) {
                low = BIT_8_LEFT_SHIFT(low) | buffer_p[i + j];
            }
            if (high_diff_len) {
                i += diff_len;
                for (j = 0; j < high_diff_len; j++) {
                    high = BIT_8_LEFT_SHIFT(high) | buffer_p[i + j];
                }
            }
            safe_memcpy(str, sizeof(str), (buffer_p + i), diff_len);
            str[diff_len] = '\0';
            log_info(dump_file, 0, tool_name, "ADDR: 0x%016llx \t DATA: 0x%016llx%016llx \t ASCII: %s", (uint32_t)(i), low, high, str);
            return;

        } else {

            for (j = 0; j < BIT_8; j++) {
                low = BIT_8_LEFT_SHIFT(low) | buffer_p[i + j];
                high = BIT_8_LEFT_SHIFT(high) | buffer_p[i + j + BIT_8];
            }
            safe_memcpy(str, sizeof(str), (buffer_p + i), BIT_16);
            str[16] = '\0';
        }

        log_info(dump_file, 0, tool_name, "ADDR: 0x%016llx \t DATA: 0x%016llx%016llx \t ASCII: %s", (uint32_t)(i), low, high, str);

        i += 16;
    }
}
