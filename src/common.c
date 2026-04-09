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
bool LIBC_CALL_CONVENTION get_local_time(struct tm* local_time, long long unsigned int* micro_seconds) {

#ifdef _WIN32

    FILETIME ft;
    SYSTEMTIME st;

    GetSystemTimePreciseAsFileTime(&ft);
    if (!FileTimeToSystemTime(&ft, &st)) {
        printf("%s, Failed to convert FilTime to SystemTime: error=%lu\n", __func__,  GetLastError());
        return false;
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
        return false;
    }
    if (NULL == localtime_r(&ts.tv_sec, local_time)) {
        printf("%s: %s Failed to convert time using localtime_r: errno=%d\n", __func__, ERR, errno);
        return false;
    }
    *micro_seconds = (ts.tv_nsec / 1000); // Convert nanoseconds to microseconds
    local_time->tm_year += 1900; // Adjust year to be since 1900
    local_time->tm_mon += 1; // Adjust month to be 1-12 instead of 0-11

#endif

    return true;
}

void LIBC_CALL_CONVENTION log_err_dump_init(log_err_dump_t* log_err_dump, char* log_file, char* err_file, char *dump_file, char* tool_name) {

    SAFE_MEMCLEAR(log_err_dump, sizeof(log_err_dump_t));

    if (NULL != tool_name) {
        SAFE_SNPRINTF(log_err_dump->tool_name, MAX_FILE_NAME_LEN, tool_name);
    } else {
        SAFE_SNPRINTF(log_err_dump->tool_name, MAX_FILE_NAME_LEN, "Unknown Tool");
    }
    if (NULL == log_file) {
        printf("[Timer not started yet]: %s: %s Log file is NULL, defaulting to stdout.\n", __func__, WARN);
        log_err_dump->log_file_p = DEFAULT_LOG_FILE;
    } else {
        if (NULL == (log_err_dump->log_file_p = fopen(log_file, "a"))) {
            printf("[Timer not started yet]: %s: %s Unable to open log file errno=%d, defaulting to stdout.\n", __func__, WARN, errno);
            log_err_dump->log_file_p = DEFAULT_LOG_FILE;
        } else {
            SAFE_SNPRINTF(log_err_dump->log_file, MAX_FILE_NAME_LEN, log_file);
        }
    }
    if (NULL == err_file) {
        printf("[Timer not started yet]: %s: %s Error file is NULL, defaulting to stderr.\n", __func__, WARN);
        log_err_dump->err_file_p = DEFAULT_ERROR_LOG_FILE;
    } else {
        if (NULL == (log_err_dump->err_file_p = fopen(err_file, "a"))) {
            printf("[Timer not started yet]: %s: %s Unable to open error file errno=%d, defaulting to stderr.\n", __func__, WARN, errno);
            log_err_dump->err_file_p = DEFAULT_ERROR_LOG_FILE;
        } else {
            SAFE_SNPRINTF(log_err_dump->err_file, MAX_FILE_NAME_LEN, err_file);
        }
    }
    if (NULL == dump_file) {
        printf("[Timer not started yet]: %s: %s Dump file is NULL, defaulting to stdout.\n", __func__, WARN);
        log_err_dump->dump_file_p = DEFAULT_DUMP_FILE;
    } else {
        if(NULL == (log_err_dump->dump_file_p = fopen(dump_file, "a"))) {
            printf("[Timer not started yet]: %s: %s Unable to open dump file errno=%d, defaulting to stdout.\n", __func__, ERR, errno);
            log_err_dump->dump_file_p = DEFAULT_DUMP_FILE;
        } else {
            SAFE_SNPRINTF(log_err_dump->dump_file, MAX_FILE_NAME_LEN, dump_file);
        }
    }
 }

void LIBC_CALL_CONVENTION log_info(log_err_dump_t* log, uint8_t log_level, const char* format, ...) {

    va_list         args;
    long long unsigned int  micro_seconds = 0;
    struct tm               local_time = {0};
    FILE* file_p = (log->use_dump ? log->dump_file_p : log->log_file_p);

    if (get_local_time((struct tm*)&local_time, &micro_seconds)) {
        SAFE_FPRINTF(file_p, "{%s}[%02d-%02d-%04d %02d:%02d:%02d.%06llu]: ", log->tool_name, local_time.tm_mday, local_time.tm_mon, local_time.tm_year, local_time.tm_hour, local_time.tm_min, local_time.tm_sec, micro_seconds);
    } else {
        SAFE_FPRINTF(file_p, "{%s}[Unable to fetch local time]: ", log->tool_name);
    }
    va_start(args, format);
    SAFE_FPRINTF(file_p, "%d: ", log_level);
    vfprintf(file_p, format, args);
    SAFE_FPRINTF(file_p, "\n");
    va_end(args);
    fflush(file_p);

}

void LIBC_CALL_CONVENTION log_error(log_err_dump_t* err, const char* format, ...) {

    va_list args;
    long long unsigned int  micro_seconds = 0;
    struct tm               local_time = {0};

    if (get_local_time((struct tm*)&local_time, &micro_seconds)) {
        SAFE_FPRINTF(err->err_file_p, "{%s}[%02d-%02d-%04d %02d:%02d:%02d.%06llu]: ", err->tool_name, local_time.tm_mday, local_time.tm_mon, local_time.tm_year, local_time.tm_hour, local_time.tm_min, local_time.tm_sec, micro_seconds);
    } else {
        SAFE_FPRINTF(err->err_file_p, "{%s}[Unable to fetch local time]: ", err->tool_name);
    }
    va_start(args, format);
    vfprintf(err->err_file_p, format, args);
    SAFE_FPRINTF(err->err_file_p, "\n");
    va_end(args);
    fflush(err->err_file_p);

}

void LIBC_CALL_CONVENTION dump_buffer(log_err_dump_t* dump, void* buffer, size_t size) {

    uint32_t i = 0; uint8_t j = 0; uint64_t low = 0, high = 0;
    char str[BIT_16 + 1] = {0}; uint8_t* buffer_p = (uint8_t*)buffer;
    dump->use_dump = true;

    if (NULL == buffer || 0 == size) {
        log_error(dump, "%s: %s Invalid value for (addr)buffer=%p and size=%zu", __func__, ERR, buffer, size);
        return;
    }

    log_info(dump, 0, "Address \t\t\t\t\t\t\t\tData(Hex) \t\t\t\t\t\t\tASCII\n");
    while (size > i) {

        low = high = 0;
        SAFE_MEMCLEAR(str, sizeof(str));

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
            SAFE_MEMCPY(str, sizeof(str), (buffer_p + idx), diff_len);
            log_info(dump, 0, "%s 0x%016llx \t0x%016llx%016llx \t%s", DUMP, (uint32_t)(i), low, high, str);
            dump->use_dump = false;
            return;
        } else {
            for (j = 0; j < BIT_8; j++) {
                low = BIT_8_LEFT_SHIFT(low) | buffer_p[i + j];
                high = BIT_8_LEFT_SHIFT(high) | buffer_p[i + j + BIT_8];
            }
            SAFE_MEMCPY(str, sizeof(str), (buffer_p + i), BIT_16);
        }

        log_info(dump, 0, "%s 0x%016llx \t0x%016llx%016llx \t%s", DUMP, (uint32_t)(i), low, high, str);
        i += 16;
    }
    dump->use_dump = false;
}

void LIBC_CALL_CONVENTION log_err_dump_close(log_err_dump_t* log_err_dump) {

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

bool LIBC_CALL_CONVENTION alligned_buffer_alloc(log_err_dump_t* log_err_dump, size_t size, size_t alignment, void* ptr) {

    if ((size == 0 || alignment == 0) && (alignment <= size) && (0 != (size % alignment))) {
        log_error(log_err_dump, "%s: %s Invalid input for size=%zu and alignment=%zu", __func__, ERR, size, alignment);
        return false;
    }
    if (NULL == (ptr = ALIGNED_MEM_ALLOC(size, alignment))) {
        log_error(log_err_dump, "%s: %s Memory allocation failed for errno=%d, size=%zu and alignment=%zu", __func__, ERR, errno, size, alignment);
        return false;
    }

    return true;
}

void LIBC_CALL_CONVENTION aligned_buffer_free(void* ptr) {

    if (ptr) {
        ALIGNED_MEM_FREE(ptr);
    }
}

bool LIBC_CALL_CONVENTION get_ascii_devname(log_err_dump_t* log_err_dump, const char* devname, size_t size, char* ascii_devname, slash_type_t slash_type) {

    char* context = NULL, *token = NULL;
    char slash = (slash_type == BACK_SLASH) ? '\\' : '/';
    char ascii_name[MAX_FILE_NAME_LEN] = {0};

    if ( NULL == devname || size < 0 || NULL == ascii_devname ) {
        log_error(log_err_dump, "%s: %s Invalid input for devname=%p, size=%zu, ascii_devname=%p", __func__, ERR, devname, size, ascii_devname);
        return false;
    }
    SAFE_MEMCLEAR(ascii_devname, sizeof(ascii_devname));
    if ((NULL != strstr(devname, "\\\\.\\"))) {
        SAFE_SNPRINTF(ascii_name, MAX_FILE_NAME_LEN, "%s%s.%s%s", slash, slash,slash, (devname + strlen("\\\\.\\")));
        goto done;
    }
    if (slash == devname[0]) {
        snprintf(ascii_name, MAX_FILE_NAME_LEN, "%c", slash);
    }
    token = SAFE_STRTOK((char*)devname, &slash, &context);
    if (NULL == token) {
        log_error(log_err_dump, "%s: %s Failed to tokenize devname=%p for delimiter=%c", __func__, ERR, devname, slash);
        return false;
    }
    while (token) {
        token = SAFE_STRTOK(NULL, &slash, &context);
        SAFE_SNPRINTF(ascii_name, MAX_FILE_NAME_LEN, "%s%s%s", ascii_name, token, slash);
    }
    ascii_name[strlen(ascii_name) - 1] = '\0'; // Remove the trailing slash
done:
    log_info(log_err_dump, 4, "%s: %s Device name %s converted to ascii name %s\n", __func__, INFO, token, ascii_name);
    return true;
}

bool LIBC_CALL_CONVENTION open_device(log_err_dump_t *log_err_dump, device_open_t* dev_attr, void* handle) {

    if (NULL == dev_attr->devname || NULL == dev_attr) {
        log_error(log_err_dump, "%s: %s Invalid input for devname=%s and (addr)dev_attr=%p", __func__, ERR, dev_attr->devname, dev_attr);
        return false;
    }

#ifdef _WIN32
    HANDLE* handle_p = (HANDLE*)handle;
    if (INVALID_HANDLE_VALUE == (*handle_p = CreateFileW(dev_attr->devname,
                                                         dev_attr->desired_access_mode,
                                                         dev_attr->share_mode,
                                                         dev_attr->security_attributes,
                                                         dev_attr->creation_disposition,
                                                         dev_attr->flags_and_attributes,
                                                         dev_attr->template_file))) {

        log_error(log_err_dump, "%s: %s Failed to open device %s with errno=%lu", __func__, ERR, dev_attr->devname, GetLastError());
        return false;
    }
#elif __linux__
    int* fd_p = (int*) handle;
    if (-1 == (*fd_p = open(dev_attr->devname, dev_attr->flags))) {
        log_error(log_err_dump, "%s: %s Failed to open device %s with errno=%d", __func__, ERR, dev_attr->devname, errno);
    }
#endif

    log_info(log_err_dump, 4, "%s: %s Device %s opened successfully handle=%p", __func__, INFO, dev_attr->devname, *handle_p);
    return true;
}

void LIBC_CALL_CONVENTION close_device(void* handle) {

#ifdef _WIN32
    if ((HANDLE*) handle) {
        closeHandle((HANDLE*) handle);
#elif __linux__
    if ((int *) handle) {
        close((int*) handle);
#endif
    }

}
