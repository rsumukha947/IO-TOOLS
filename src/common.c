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

void log_common(void* log_file, bool is_error) {

    time_t          raw_time;
    struct tm       local_time;
    raw_time      = time(NULL);
    static FILE*    log_file_p = NULL;

    if (NULL == log_file) {
        printf("[Timer not started yet]: Log file is NULL, defaulting to %s.\n", is_error ? "stderr" : "stdout");
        log_file = is_error ? stderr : stdout;
    }

    if ((NULL == log_file_p) && (stdout != log_file)) {
        if (NULL == (log_file_p = fopen((char *)log_file, "a"))) {
            printf("[Timer not started yet]: Unable to open log file, defaulting to  %s.\n", is_error ? "stderr" : "stdout");
            log_file_p = is_error ? stderr : stdout;
        }
    }

    if (get_local_time((struct tm*)&local_time, &raw_time)) {
        safe_fprintf(log_file, "[%02d-%02d-%04d %02d:%02d:%02d]: ", local_time.tm_mday, (local_time.tm_mon + 1), (local_time.tm_year + 1900), local_time.tm_hour, local_time.tm_min, local_time.tm_sec);
    } else {
        safe_fprintf(log_file, "[Unable to fetch local time]: ");
    }

}

void log_info(void* log_file, uint8_t log_level, const char* format, ...) {

    va_list         args;

    log_common(log_file, false);
    va_start(args, format);
    safe_fprintf(log_file, "%d: ", log_level);
    vfprintf(log_file, format, args);
    safe_fprintf(log_file, "\n");
    va_end(args);
    fflush(log_file);

}

void log_error(void* err_file, const char* format, ...) {

    va_list args;

    log_common(err_file, true);
    va_start(args, format);
    vfprintf(err_file, format, args);
    safe_fprintf(err_file, "\n");
    va_end(args);
    fflush(err_file);

}
