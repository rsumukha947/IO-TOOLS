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
status_t get_local_time (struct tm* local_time, const char* raw_time) {

#ifdef WIN32
    return (0 == localtime_s(local_time, raw_time) ? TE_PASS : TE_FAIL);
#elif __linux__
    return (NULL == localtime_r(raw_time, local_time) ? TE_FAIL : TE_PASS);
#endif

}

void log_common(void* log_file, char* tool_name, bool is_error) {

    time_t          raw_time;
    struct tm       local_time;
    raw_time      = time(NULL);
    static FILE*    log_file_p = NULL;

    if (NULL == log_file) {
        printf("{%s}[Timer not started yet]: Log file is NULL, defaulting to %s.\n", tool_name, is_error ? "stderr" : "stdout");
        log_file = is_error ? stderr : stdout;
    }

    if ((NULL == log_file_p) && (stdout != log_file)) {
        if (NULL == (log_file_p = fopen((char *)log_file, "a"))) {
            printf("{%s}[Timer not started yet]: Unable to open log file, defaulting to  %s.\n", tool_name, is_error ? "stderr" : "stdout");
            log_file_p = is_error ? stderr : stdout;
        }
    }

    if (get_local_time((struct tm*)&local_time, &raw_time)) {
        safe_fprintf(log_file, "{%s}[%02d-%02d-%04d %02d:%02d:%02d]: ", tool_name, local_time.tm_mday, (local_time.tm_mon + 1), (local_time.tm_year + 1900), local_time.tm_hour, local_time.tm_min, local_time.tm_sec);
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
        memset(str, 0, sizeof(str));

        if (16 > (size - i)) {

            uint8_t diff_len = (uint8_t)(size - i);
            uint8_t high_diff_len = (uint8_t)(diff_len > 8 ? diff_len - 8 : 0);

            for (j = 0; j < diff_len; j++) {
                low = (low << 8) | buffer_p[i + j];
            }
            if (high_diff_len) {
                i += diff_len;
                for (j = 0; j < high_diff_len; j++) {
                    high = (high << 8) | buffer_p[i + j];
                }
            }
            memcpy(str, buffer_p + i, diff_len);
            str[diff_len] = '\0';
            log_info(dump_file, 0, tool_name, "ADDR: 0x%016llx \t DATA: 0x%016llx%016llx \t ASCII: %s", (uint32_t)(i), low, high, str);
            return;

        } else {

            for (j = 0; j < BIT_8; j++) {
                low = (low << 8) | buffer_p[i + j];
                high = (high << 8) | buffer_p[i + j + BIT_8];
            }
            memcpy(str, buffer_p + i, 16);
            str[16] = '\0';
        }

        log_info(dump_file, 0, tool_name, "ADDR: 0x%016llx \t DATA: 0x%016llx%016llx \t ASCII: %s", (uint32_t)(i), low, high, str);

        i += 16;
    }
}
