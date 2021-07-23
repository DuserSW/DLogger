#include <dlogger.h>
#include <sys/stat.h>
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
    mtx_t mutex;        /* main mutex to provide library thread safe. */
    bool is_init;       /* is library initialized or not?             */
    int log_descriptor; /* file descriptor to write log messages.     */
} DLogger_data;


static DLogger_data dlooger_priv_data;


/* 
 * This function generate timestamp and save into @file_name. 
 * Name of log file is following: years:months:days-hours:minuts:seconds.log.
 *
 * @param[in]     len       - length of @file_name array.
 * @param[in/out] file_name - array which collect characters for create log file.
 * 
 * @return - void
 */
static void __dlogger_write_timestamp(size_t len, char file_name[static len])
{
    const struct tm* restrict const dt_p = localtime(&(time_t){time(NULL)});

    if (dt_p == NULL)
    {
        perror("DLogger: cannot get date!");
        return;
    }

    strftime(&file_name[0], len, "%Y:%b:%d-%H:%M:%S.log", dt_p);
}


int dlogger_init(void)
{
    if (dlooger_priv_data.is_init == true)
    {
        perror("DLogger: dlogger cannot be initialized more than once");
        return 1;
    }

    register size_t tries = 0;
    register const size_t max_tries = 10;
    
    do
    {
        char file_name[64] = {0};
        __dlogger_write_timestamp(sizeof(file_name), &file_name[0]);

        /* somehow this file name exist, we need to try more times */
        if (stat(&file_name[0], &((struct stat){0})) != -1)
        {
            fprintf(stderr, "DLogger: file name: %s already exist. We will try more time %zu/%zu\n", &file_name[0], tries, max_tries);

            tries++;
            sleep(1);
            continue;
        }

        register int fd = -1;
        register const mode_t mode = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH;
        
        fd = creat(&file_name[0], mode);

        if (fd == -1)
        {
            perror("DLogger: cannot create file");
            return 1;
        }

        register const int flags = O_WRONLY | O_TRUNC;
        fd = open(&file_name[0], flags);

        if (fd == -1)
        {
            perror("DLogger: cannot open file");
            return 1;
        }

        dlooger_priv_data.log_descriptor = fd;
        tries = max_tries;

    } while (tries < max_tries);
    
    if (mtx_init(&dlooger_priv_data.mutex, mtx_plain) != thrd_success)
    {
        perror("DLogger: mutex cannot be initialized");
        dlooger_priv_data.log_descriptor = -1;
        return 1;
    }
        
    dlooger_priv_data.is_init = true;
    return 0;
}


void dlogger_deinit(void)
{
    if (dlooger_priv_data.is_init == false)
    {
        perror("DLogger: first initialize DLogger");
        return;
    }

    mtx_destroy(&dlooger_priv_data.mutex);

    if (close(dlooger_priv_data.log_descriptor) == -1)
    {
        perror("DLogger: cannot close log descriptor");
    }

    memset(&dlooger_priv_data, 0, sizeof(dlooger_priv_data));
}


void __attribute__(( __format__ (__printf__, 4, 5)) ) __dlogger_print(const char* file_p, 
                                                                      const char* func_p, 
                                                                      int line, 
                                                                      const char * const restrict format_p,
                                                                      ...)
{
    if (dlooger_priv_data.is_init == false)
    {
        perror("DLogger: first initialize DLogger");
        return;
    }

    if (mtx_lock(&dlooger_priv_data.mutex) != thrd_success)
    {
        perror("DLogger: cannot lock mutex");
        return;
    }

    va_list args;
    va_start(args, format_p);

    static char buffer[1 << 10] = {0};
    register size_t written_bytes = 0;

    written_bytes += (size_t)snprintf(&buffer[written_bytes], sizeof(buffer) - written_bytes, "%s:%s:%d\t", file_p, func_p, line);

    written_bytes += (size_t)vsnprintf(&buffer[written_bytes], sizeof(buffer) - written_bytes, format_p, args);

    va_end(args);

    if (buffer[written_bytes - 1] != '\n' && written_bytes - 1 < sizeof(buffer))
    {
        buffer[written_bytes++] = '\n';
        buffer[written_bytes] = '\0';
    }

    dprintf(dlooger_priv_data.log_descriptor, "%s", &buffer[0]);

    mtx_unlock(&dlooger_priv_data.mutex);
}
