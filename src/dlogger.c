#include <dlogger.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <stdbool.h>
#include <threads.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <stddef.h>
#include <stdio.h>
#include <fcntl.h>


typedef struct DLogger_data
{
    mtx_t mutex;            /* main mutex to provide library thread safe. */
    bool is_init;           /* is library initialized or not?             */
    int log_descriptor;     /* file descriptor to write log messages.     */
    Dlogger_levelE level;   /* level of logging                           */
} DLogger_data;


static DLogger_data dlogger_priv_data;


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
static size_t __dlogger_write_level(size_t buffer_index, size_t buffer_size, char buffer[static 1], Dlogger_levelE level);


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
                                             const char* restrict file_p, int line, const char* restrict func_p);

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

    const struct tm* const restrict datetime_now_p = localtime(&timeval_now.tv_sec);

    if (datetime_now_p == NULL)
    {
        perror("DLogger: error with function localtime");
        return 0;
    }

    register size_t written_bytes = buffer_index;

    if (with_date == true)
    {
        register const char* const restrict fmt_date_p = "%Y:%b:%d";
        written_bytes += strftime(&buffer[written_bytes], buffer_size - written_bytes, fmt_date_p, datetime_now_p);
    }

    if (with_h_min_sec == true)
    {
        register const char* const restrict fmt_h_min_sec_p = (with_date == true) ? "-%H:%M:%S" : "%H:%M:%S";
        written_bytes += strftime(&buffer[written_bytes], buffer_size - written_bytes, fmt_h_min_sec_p, datetime_now_p);

        if (with_usec == true)
        {
            register const char* const restrict fmt_usec_p = ".%ld";
            written_bytes += (size_t)snprintf(&buffer[written_bytes], buffer_size - written_bytes, fmt_usec_p, (long)timeval_now.tv_usec);
        }
    }

    return written_bytes - buffer_index;
}


static size_t __dlogger_write_level(const size_t buffer_index, const size_t buffer_size, 
                                    char buffer[const static 1], const Dlogger_levelE level)
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

    register const char* const restrict fmt_p = "[%s]%s";

    return (size_t)snprintf(&buffer[buffer_index], buffer_size - buffer_index, 
                            fmt_p, dlogger_priv_level_strings[level], &spaces_to_fill[0]);
}


static size_t __dlogger_write_file_line_func(const size_t buffer_index, const size_t buffer_size, char buffer[const static 1], 
                                             const char* const restrict file_p, const int line, const char* const restrict func_p)
{
    if (buffer_index >= buffer_size)
    {
        perror("DLogger: end of internal buffer");
        return 0;
    }

    register const char* const restrict fmt_p = "%s:%d\t%s\t";

    return (size_t)snprintf(&buffer[buffer_index], buffer_size - buffer_index, fmt_p, file_p, line, func_p);
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


int dlogger_init(const Dlogger_levelE level)
{
    if (dlogger_priv_data.is_init == true)
    {
        perror("DLogger: dlogger cannot be initialized more than once");
        return 1;
    }

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
        filename_buffer_index += __dlogger_write_timestamp(filename_buffer_index, sizeof(filename_buffer), 
                                                           &filename_buffer[0], with_date, with_h_min_sec, with_usec);
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

        register int fd = -1;
        register const mode_t mode = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH;
        
        fd = creat(&filename_buffer[0], mode);

        if (fd == -1)
        {
            perror("DLogger: cannot create file");
            return 1;
        }

        register const int flags = O_WRONLY | O_TRUNC;
        fd = open(&filename_buffer[0], flags);

        if (fd == -1)
        {
            perror("DLogger: cannot open file");
            return 1;
        }

        dlogger_priv_data.log_descriptor = fd;
        tries = max_tries;

    } while (tries < max_tries);
    
    if (mtx_init(&dlogger_priv_data.mutex, mtx_plain) != thrd_success)
    {
        perror("DLogger: mutex cannot be initialized");
        dlogger_priv_data.log_descriptor = -1;
        return 1;
    }
        
    dlogger_priv_data.level = level;
    dlogger_priv_data.is_init = true;

    return 0;
}


void dlogger_deinit(void)
{
    if (dlogger_priv_data.is_init == false)
    {
        perror("DLogger: first initialize DLogger");
        return;
    }

    mtx_destroy(&dlogger_priv_data.mutex);

    if (close(dlogger_priv_data.log_descriptor) == -1)
    {
        perror("DLogger: cannot close log descriptor");
    }

    memset(&dlogger_priv_data, 0, sizeof(dlogger_priv_data));
}


void __attribute__(( __format__ (__printf__, 5, 6)) ) __dlogger_print(const char* file_p, 
                                                                      int line,
                                                                      const char* func_p, 
                                                                      const Dlogger_levelE level,
                                                                      const char * const restrict format_p,
                                                                      ...)
{
    if (dlogger_priv_data.is_init == false)
    {
        perror("DLogger: first initialize DLogger");
        return;
    }

    if (dlogger_priv_data.level < level)
    {
        return;
    }

    if (mtx_lock(&dlogger_priv_data.mutex) != thrd_success)
    {
        perror("DLogger: cannot lock mutex");
        return;
    }

    static char buffer[1 << 15] = {0};
    register size_t buffer_index = 0;

    buffer_index += __dlogger_write_file_line_func(buffer_index, sizeof(buffer), &buffer[0], file_p, line, func_p);
    buffer_index += __dlogger_write_level(buffer_index, sizeof(buffer), &buffer[0], level);

    /* Not moved to another functions because it will be triumph of form over content with passing format and variadic arguments. */
    va_list args;
    va_start(args, format_p);

    buffer_index += (size_t)vsnprintf(&buffer[buffer_index], sizeof(buffer) - buffer_index, format_p, args);

    va_end(args);

    __dlogger_add_newline(buffer_index, sizeof(buffer), &buffer[0]);

    dprintf(dlogger_priv_data.log_descriptor, "%s", &buffer[0]);

    mtx_unlock(&dlogger_priv_data.mutex);
}
