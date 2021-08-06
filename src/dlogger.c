#include <dlogger/dlogger.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <execinfo.h>
#include <stdbool.h>
#include <threads.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>


#define DLOGGER_MAX_NR_OF_FD (3ULL)


struct DLogger_user_optionsS
{
    bool is_filled : 1;   /* Are we able to use this these options for this file descriptor? */

    int file_descriptor;  /* Available file descriptors: stdout, stderr, unique file. */

    DLogger_levelE level; /* Level of logging. */

    bool timestamp : 1;   /* Timestamp should be collected for messages? */
    bool threadid : 1;    /* Thread ID should be collected for messages? */
};


typedef struct DLogger_dataS
{
    struct
    {
        mtx_t mutex;  /* main mutex to provide library thread safe. */
        bool is_init; /* is library initialized or not?             */
    };

    struct
    {
        /* user options for logging */
        DLogger_user_optionsS user_options[DLOGGER_MAX_NR_OF_FD];
    };
} DLogger_dataS;


static DLogger_dataS dlogger_priv_data;


/* 
 * This function generate timestamp and save into @buffer. 
 * There is one not available option: write date + microseconds, without hours, minuts, seconds. All other options are available.
 *
 * @param[in]     buffer_index   - current buffer index where new data could be written.
 * @param[in]     buffer_size    - size of buffer.
 * @param[in/out] buffer         - pointer to first element of buffer.
 * @param[in]     with_date      - timestamp should contain data?
 * @param[in]     with_h_min_sec - timestamp should contain hours, minuts and seconds?
 * @param[in]     with_usec      - timestamp should contain microseconds?
 * 
 * @return - number of bytes written into @buffer.
 */
static size_t __dlogger_write_timestamp(size_t buffer_index, size_t buffer_size, char buffer[static 1],
                                        bool with_date, bool with_h_min_sec, bool with_usec);


/* 
 * This function save into @buffer proper string related with level of logging from functionlike macro.
 *
 * @param[in]     buffer_index - current buffer index where new data could be written.
 * @param[in]     buffer_size  - size of buffer.
 * @param[in/out] buffer       - pointer to first element of buffer.
 * @param[in]     level        - current level of logging.
 * 
 * @return - number of bytes written into @buffer.
 */
static size_t __dlogger_write_level(size_t buffer_index, size_t buffer_size, char buffer[static 1], DLogger_levelE level);


/* 
 * This function save into @buffer filename, line and function where any logger functionlike macro has been called.
 *
 * @param[in]     buffer_index - current buffer index where new data could be written.
 * @param[in]     buffer_size  - size of buffer.
 * @param[in/out] buffer       - pointer to first element of buffer.
 * @param[in]     file_p       - pointer to string with a filename of file.
 * @param[in]     line         - number of line.
 * @param[in]     func_p       - pointer to string with a name of function.
 * 
 * @return - number of bytes written into @buffer.
 */
static size_t __dlogger_write_file_line_func(size_t buffer_index, size_t buffer_size, char buffer[static 1],
                                             const char *restrict file_p, int line, const char *restrict func_p);


/* 
 * This function save into @buffer thread id.
 *
 * @param[in]     buffer_index - current buffer index where new data could be written.
 * @param[in]     buffer_size  - size of buffer.
 * @param[in/out] buffer       - pointer to first element of buffer.
 * 
 * @return - number of bytes written into @buffer.
 */
static size_t __dlogger_write_thread_id(size_t buffer_index, size_t buffer_size, char buffer[static 1]);


/* 
 * This function save into @buffer backtrace from application.
 *
 * @param[in]     buffer_index - current buffer index where new data could be written.
 * @param[in]     buffer_size  - size of buffer.
 * @param[in/out] buffer       - pointer to first element of buffer.
 * 
 * @return - number of bytes written into @buffer.
 */
static size_t __dlogger_write_backtrace(size_t buffer_index, size_t buffer_size, char buffer[static 1]);


/*
 * This function will add into @buffer additional newline if user forget.
 *
 * @param[in]     buffer_index - current buffer index where new data could be written.
 * @param[in]     buffer_size  - size of buffer.
 * @param[in/out] buffer       - pointer to first element of buffer.
 * 
 * @return - void.
 */
static void __dlogger_add_newline(size_t buffer_index, size_t buffer_size, char buffer[static 1]);


/*
 * This function will parse user options by using compund literals.
 *
 * @param[in] descriptor_to_write - which descriptor options @level_of_logging and @additional_options will be written.
 * @param[in] level_of_logging    - level of logging for above @descriptor_to_write.
 * @param[in] additional_options  - additional options available in DLogger.
 *
 * @return - parsed input into DLogger_user_optionsS.
 */
static inline DLogger_user_optionsS __dlogger_parse_user_option(DLogger_options_writeE descriptor_to_write,
                                                                DLogger_levelE level_of_logging,
                                                                DLogger_options_markE additional_options);


static size_t __dlogger_write_timestamp(const size_t buffer_index, const size_t buffer_size, char buffer[const static 1],
                                        const bool with_date, const bool with_h_min_sec, const bool with_usec)
{
    if (buffer_index >= buffer_size)
    {
        perror("DLogger: end of internal buffer");
        return 0;
    }

    struct timeval timeval_now = {0};
    register const int ret = gettimeofday(&timeval_now, NULL);

    if (ret == -1)
    {
        perror("DLogger: error with function gettimeofday");
        return 0;
    }

    const struct tm *const restrict datetime_now_p = localtime(&timeval_now.tv_sec);

    if (datetime_now_p == NULL)
    {
        perror("DLogger: error with function localtime");
        return 0;
    }

    register size_t written_bytes = buffer_index;

    if (with_date == true)
    {
        register const char *const restrict fmt_date_p = "%Y:%m:%d";
        written_bytes += strftime(&buffer[written_bytes], buffer_size - written_bytes, fmt_date_p, datetime_now_p);
    }

    if (with_h_min_sec == true)
    {
        register const char *const restrict fmt_h_min_sec_p = (with_date == true) ? "-%H:%M:%S" : "%H:%M:%S";
        written_bytes += strftime(&buffer[written_bytes], buffer_size - written_bytes, fmt_h_min_sec_p, datetime_now_p);

        if (with_usec == true)
        {
            register const char *const restrict fmt_usec_p = ".%ld";
            written_bytes += (size_t)snprintf(&buffer[written_bytes], buffer_size - written_bytes, fmt_usec_p, (long)timeval_now.tv_usec);
        }
    }

    return written_bytes - buffer_index;
}


static size_t __dlogger_write_level(const size_t buffer_index, const size_t buffer_size,
                                    char buffer[const static 1], const DLogger_levelE level)
{
    if (buffer_index >= buffer_size)
    {
        perror("DLogger: end of internal buffer");
        return 0;
    }

    /* 
     * This aligment with spaces is necessary to start saving logger messages with proper offset. This is why we need to calculate 
     * different between the longest level which is "CRITICAL" and the current level. 
     *
     * Example:
     * [FATAL]    log message1
     * [CRITICAL] log message2
     * [ERROR]    log message3
     * [WARNING]  log message4
     */
    register const size_t length_level_critical = strlen(dlogger_priv_level_strings[DLOGGER_LEVEL_CRITICAL]) + 1;
    register const size_t length_level_current = strlen(dlogger_priv_level_strings[level]) + 1;

    /* "+2" means - additional space after square bracket and for null-terminated character. */
    register const size_t size = length_level_critical - length_level_current + 2;
    char spaces_to_fill[size];

    register const char space = ' ';
    memset(&spaces_to_fill[0], space, sizeof(spaces_to_fill));
    spaces_to_fill[size - 1] = '\0';

    register const char *const restrict fmt_p = "[%s]%s";

    return (size_t)snprintf(&buffer[buffer_index], buffer_size - buffer_index,
                            fmt_p, dlogger_priv_level_strings[level], &spaces_to_fill[0]);
}


static size_t __dlogger_write_file_line_func(const size_t buffer_index, const size_t buffer_size, char buffer[const static 1],
                                             const char *const restrict file_p, const int line, const char *const restrict func_p)
{
    if (buffer_index >= buffer_size)
    {
        perror("DLogger: end of internal buffer");
        return 0;
    }

    register const char* const restrict fmt_p = "[%s:%d %s] ";

    return (size_t)snprintf(&buffer[buffer_index], buffer_size - buffer_index, fmt_p, file_p, line, func_p);
}


static size_t __dlogger_write_thread_id(const size_t buffer_index, const size_t buffer_size, char buffer[const static 1])
{
    if (buffer_index >= buffer_size)
    {
        perror("DLogger: end of internal buffer");
        return 0;
    }

    register const pid_t thread_id = (pid_t)syscall(__NR_gettid);


    register const char* const restrict fmt_p = "[TID %ld] ";

    return (size_t)snprintf(&buffer[buffer_index], buffer_size - buffer_index, fmt_p, (long)thread_id);
}


static size_t __dlogger_write_backtrace(const size_t buffer_index, const size_t buffer_size, char buffer[const static 1])
{
    if (buffer_index >= buffer_size)
    {
        perror("DLogger: end of internal buffer");
        return 0;
    }

    /* 
     * If we save backtrace_size as: register const int we get clang warning: "variable length array folded to constant array as an extension"
     * Simple workaround is just use enum or define.
     */
    enum { backtrace_size = 128 };
    void* backtrace_buffer[backtrace_size];

    register const int number_of_frames = backtrace(&backtrace_buffer[0], backtrace_size);

    char** backtrace_strings_pp = backtrace_symbols(&backtrace_buffer[0], number_of_frames);

    if (backtrace_strings_pp == NULL)
    {
        perror("DLogger: error with backtrace symbols");
        return 0;
    }

    register size_t bytes_written = buffer_index;

    bytes_written += (size_t)snprintf(&buffer[bytes_written], buffer_size - bytes_written, "Backtrace:\n");

    for (size_t i = 0; i < (size_t)number_of_frames; ++i)
    {
        bytes_written += (size_t)snprintf(&buffer[bytes_written], buffer_size - bytes_written, "%s\n", backtrace_strings_pp[i]);
    }

    free(backtrace_strings_pp);

    return bytes_written - buffer_index;
}


static void __dlogger_add_newline(const size_t buffer_index, const size_t buffer_size, char buffer[const static 1])
{
    if (buffer_index >= buffer_size)
    {
        perror("DLogger: end of internal buffer");
        return;
    }

    if (buffer[buffer_index - 1] != '\n')
    {
        buffer[buffer_index] = '\n';
        buffer[buffer_index + 1] = '\0';
    }
}


static inline DLogger_user_optionsS __dlogger_parse_user_option(const DLogger_options_writeE descriptor_to_write,
                                                                const DLogger_levelE level_of_logging,
                                                                const DLogger_options_markE additional_options)
{
    const int fd[] = 
    {
        [DLOGGER_OPTION_WRITE_TO_FILE] = -1,
        [DLOGGER_OPTION_WRITE_TO_STDERR] = 2,
        [DLOGGER_OPTION_WRITE_TO_STDOUT] = 1,
    };

    return (DLogger_user_optionsS){
        .is_filled = true,
        .file_descriptor = fd[descriptor_to_write],
        .level = level_of_logging,
        .timestamp = additional_options & DLOGGER_OPTION_MARK_TIMESTAMP,
        .threadid = additional_options & DLOGGER_OPTION_MARK_THREADID,
    };
}


static int __dlogger_try_create_unique_file(void)
{
    register int fd = -1;
    register size_t tries = 0;
    register const size_t max_tries = 10;

    do
    {
        char filename_buffer[1 << 6] = {0};
        register size_t filename_buffer_index = 0;

        register const bool with_date = true;
        register const bool with_h_min_sec = true;
        register const bool with_usec = (tries > max_tries / 2) ? true : false;

        /* first generate timestamp, then add file extension */
        filename_buffer_index += __dlogger_write_timestamp(filename_buffer_index, sizeof(filename_buffer), &filename_buffer[0], 
                                                           with_date, with_h_min_sec, with_usec);
        filename_buffer_index += (size_t)snprintf(&filename_buffer[filename_buffer_index], 
                                                  sizeof(filename_buffer) - filename_buffer_index, "%s", ".log");

        /* somehow this file name exist, we need to try more times */
        if (stat(&filename_buffer[0], &((struct stat){0})) != -1)
        {
            fprintf(stderr, "DLogger: file name: %s already exist. We will try more time %zu/%zu\n",
                    &filename_buffer[0], tries, max_tries);

            tries++;
            sleep(1);
            continue;
        }

        register const mode_t mode = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH;

        fd = creat(&filename_buffer[0], mode);

        if (fd == -1)
        {
            perror("DLogger: cannot create file");
            return -1;
        }

        register const int flags = O_WRONLY | O_TRUNC;
        fd = open(&filename_buffer[0], flags);

        if (fd == -1)
        {
            perror("DLogger: cannot open file");
            return -1;
        }

        dlogger_priv_data.user_options[DLOGGER_OPTION_WRITE_TO_FILE].file_descriptor = fd;
        tries = max_tries;

    } while (tries < max_tries);

    return fd;
}


DLogger_user_optionsS* dlogger_create_user_options(void)
{
    DLogger_user_optionsS* user_options_p = calloc(DLOGGER_MAX_NR_OF_FD, sizeof(*user_options_p));

    if (user_options_p == NULL)
    {
        perror("DLogger: calloc error");
        return NULL;
    }

    for (size_t i = 0; i < DLOGGER_MAX_NR_OF_FD; ++i)
    {
        user_options_p[i].file_descriptor = -1;
    }

    return user_options_p;
}


void dlogger_destroy_user_options(DLogger_user_optionsS* user_options_p)
{
    free(user_options_p);
    user_options_p = NULL;
}


void dlogger_set_user_options(DLogger_user_optionsS* const user_options_p,
                              const DLogger_options_writeE descriptor_to_write,
                              const DLogger_levelE level_of_logging,
                              const DLogger_options_markE additional_options)
{
    if (user_options_p == NULL)
    {
        perror("DLogger: pass NULL pointer");
        return;
    }

    if (dlogger_priv_data.is_init == true)
    {
        perror("DLogger: options can be specify before initialization");
        return;
    }

    user_options_p[descriptor_to_write] = __dlogger_parse_user_option(descriptor_to_write, level_of_logging, additional_options);
}


int dlogger_create(const DLogger_user_optionsS* const user_options_p)
{
    if (dlogger_priv_data.is_init == true)
    {
        perror("DLogger: dlogger cannot be initialized more than once");
        return 1;
    }

    register bool create_uniq_file = false;

    if (user_options_p == NULL)
    {
        create_uniq_file = true;
        dlogger_priv_data.user_options[DLOGGER_OPTION_WRITE_TO_FILE] = 
            __dlogger_parse_user_option(DLOGGER_OPTION_WRITE_TO_FILE,
                                        DLOGGER_LEVEL_MAX,
                                        DLOGGER_OPTION_MARK_TIMESTAMP | DLOGGER_OPTION_MARK_THREADID);
    }
    else
    {
        if (user_options_p[DLOGGER_OPTION_WRITE_TO_FILE].is_filled == true)
        {
            create_uniq_file = true;
        }

        if (memcpy(&dlogger_priv_data.user_options[0], user_options_p, sizeof(dlogger_priv_data.user_options)) != &dlogger_priv_data.user_options[0])
        {
            perror("DLogger: memcpy error");
            return -1;
        }
    }

    if (create_uniq_file == true)
    {
        register const int fd = __dlogger_try_create_unique_file();

        if (fd == -1)
        {
            perror("DLogger: cannot create or open log file");
            return -1;
        }

        dlogger_priv_data.user_options[DLOGGER_OPTION_WRITE_TO_FILE].file_descriptor = fd;
    }

    if (mtx_init(&dlogger_priv_data.mutex, mtx_plain) != thrd_success)
    {
        perror("DLogger: mutex cannot be initialized");
        memset(&dlogger_priv_data, 0, sizeof(dlogger_priv_data));
        return -1;
    }

    dlogger_priv_data.is_init = true;

    return 0;
}


void dlogger_destroy(void)
{
    if (dlogger_priv_data.is_init == false)
    {
        perror("DLogger: first initialize DLogger");
        return;
    }

    mtx_destroy(&dlogger_priv_data.mutex);

    if (dlogger_priv_data.user_options[DLOGGER_OPTION_WRITE_TO_FILE].is_filled == true)
    {
        if (close(dlogger_priv_data.user_options[DLOGGER_OPTION_WRITE_TO_FILE].file_descriptor) == -1)
        {
            perror("DLogger: cannot close log descriptor");
        }
    }

    memset(&dlogger_priv_data, 0, sizeof(dlogger_priv_data));
}


void __attribute__(( __format__ (__printf__, 5, 6)) ) __dlogger_print(const char* const restrict file_p, 
                                                                      const int line,
                                                                      const char* const restrict func_p, 
                                                                      const DLogger_levelE level,
                                                                      const char* const restrict format_p,
                                                                      ...)
{
    if (dlogger_priv_data.is_init == false)
    {
        perror("DLogger: first initialize DLogger");
        return;
    }

    if (mtx_lock(&dlogger_priv_data.mutex) != thrd_success)
    {
        perror("DLogger: cannot lock mutex");
        return;
    }

    for (DLogger_options_writeE i = DLOGGER_OPTION_WRITE_TO_FILE; i <= DLOGGER_OPTION_WRITE_TO_STDOUT; ++i)
    {
        if (dlogger_priv_data.user_options[i].is_filled == false || 
            dlogger_priv_data.user_options[i].level < level)
        {
            continue;
        }

        static char buffer[1 << 15] = {0};
        register size_t buffer_index = 0;

        buffer_index += __dlogger_write_level(buffer_index, sizeof(buffer), &buffer[0], level);

        if (dlogger_priv_data.user_options[i].timestamp == true)
        {
            register const bool with_date = false;
            register const bool with_h_min_sec = true;
            register const bool with_usec = true;

            buffer[buffer_index++] = '[';
            buffer_index += __dlogger_write_timestamp(buffer_index, sizeof(buffer), &buffer[0], with_date, with_h_min_sec, with_usec);
            buffer[buffer_index++] = ']';
            buffer[buffer_index++] = ' ';
        }
            
        if (dlogger_priv_data.user_options[i].threadid == true)
        {
            buffer_index += __dlogger_write_thread_id(buffer_index, sizeof(buffer), &buffer[0]);      
        }

        buffer_index += __dlogger_write_file_line_func(buffer_index, sizeof(buffer), &buffer[0], file_p, line, func_p);

        /* Not moved to another functions because it will be triumph of form over content with passing format and variadic arguments. */
        va_list args;
        va_start(args, format_p);

        buffer_index += (size_t)vsnprintf(&buffer[buffer_index], sizeof(buffer) - buffer_index, format_p, args);

        va_end(args);

        __dlogger_add_newline(buffer_index, sizeof(buffer), &buffer[0]);

        if (level == DLOGGER_LEVEL_FATAL)
        {
            buffer_index += __dlogger_write_backtrace(buffer_index, sizeof(buffer), &buffer[0]);  
        }

        dprintf(dlogger_priv_data.user_options[i].file_descriptor, "%s", &buffer[0]);
    }

    mtx_unlock(&dlogger_priv_data.mutex);
}
