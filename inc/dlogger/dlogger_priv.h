#ifndef DLOGGER_PRIV_H
#define DLOGGER_PRIV_H


#include <stdint.h>


/*
    This is the private header for DLogger library. Do not include it directly!

    Author: Kamil Kielbasa
    Email: kamilkielbasa64@gmail.com
    License: GPL3
*/


typedef enum DLogger_levelE
{
    DLOGGER_PRIV_LEVEL_FATAL,
    DLOGGER_PRIV_LEVEL_CRITICAL,
    DLOGGER_PRIV_LEVEL_ERROR,
    DLOGGER_PRIV_LEVEL_WARNING, 
    DLOGGER_PRIV_LEVEL_INFO,
    DLOGGER_PRIV_LEVEL_DEBUG,
    DLOGGER_PRIV_LEVEL_MAX = DLOGGER_PRIV_LEVEL_DEBUG,
} DLogger_levelE;


typedef enum DLogger_options_writeE
{
    DLOGGER_PRIV_OPTION_WRITE_TO_FILE,
    DLOGGER_PRIV_OPTION_WRITE_TO_STDERR,
    DLOGGER_PRIV_OPTION_WRITE_TO_STDOUT,
} DLogger_options_writeE;


typedef uint32_t DLogger_options_markE;
#define DLOGGER_PRIV_OPTION_MARK_TIMESTAMP (1 << 0)
#define DLOGGER_PRIV_OPTION_MARK_THREADID  (1 << 1)


static const char* const dlogger_priv_level_strings[] = { 
                                                          [DLOGGER_PRIV_LEVEL_FATAL]     = "FATAL", 
                                                          [DLOGGER_PRIV_LEVEL_CRITICAL]  = "CRITICAL", 
                                                          [DLOGGER_PRIV_LEVEL_ERROR]     = "ERROR", 
                                                          [DLOGGER_PRIV_LEVEL_WARNING]   = "WARNING", 
                                                          [DLOGGER_PRIV_LEVEL_INFO]      = "INFO", 
                                                          [DLOGGER_PRIV_LEVEL_DEBUG]     = "DEBUG",
                                                        };



void __attribute__(( __format__ (__printf__, 5, 6)) ) __dlogger_print(const char* restrict file_p, 
                                                                      int line,
                                                                      const char* restrict func_p, 
                                                                      DLogger_levelE level,
                                                                      const char * restrict format_p,
                                                                      ...);


#define dlogger_priv_log_general(level, ...) __dlogger_print(__FILE__, __LINE__, __func__, level, __VA_ARGS__)

#define dlogger_priv_log_fatal(...)    dlogger_priv_log_general(DLOGGER_PRIV_LEVEL_FATAL, __VA_ARGS__)
#define dlogger_priv_log_critical(...) dlogger_priv_log_general(DLOGGER_PRIV_LEVEL_CRITICAL, __VA_ARGS__)
#define dlogger_priv_log_error(...)    dlogger_priv_log_general(DLOGGER_PRIV_LEVEL_ERROR, __VA_ARGS__)
#define dlogger_priv_log_warning(...)  dlogger_priv_log_general(DLOGGER_PRIV_LEVEL_WARNING, __VA_ARGS__)
#define dlogger_priv_log_info(...)     dlogger_priv_log_general(DLOGGER_PRIV_LEVEL_INFO, __VA_ARGS__)
#define dlogger_priv_log_debug(...)    dlogger_priv_log_general(DLOGGER_PRIV_LEVEL_DEBUG, __VA_ARGS__)

#endif /* DLOGGER_PRIV_H */
