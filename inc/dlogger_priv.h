#ifndef DLOGGER_PRIV_H
#define DLOGGER_PRIV_H


/*
    This is the private header for DLogger library. Do not include it directly!

    Author: Kamil Kielbasa
    Email: kamilkielbasa64@gmail.com
    License: GPL3
*/


typedef enum Dlogger_levelE
{
    dlogger_level_fatal,
    dlogger_level_critical,
    dlogger_level_error,
    dlogger_level_warning, 
    dlogger_level_info,
    dlogger_level_debug,
    dlogger_level_max = dlogger_level_debug,
} Dlogger_levelE;


static const char* const dlogger_priv_level_strings[] = { 
                                                          [dlogger_level_fatal]     = "FATAL", 
                                                          [dlogger_level_critical]  = "CRITICAL", 
                                                          [dlogger_level_error]     = "ERROR", 
                                                          [dlogger_level_warning]   = "WARNING", 
                                                          [dlogger_level_info]      = "INFO", 
                                                          [dlogger_level_debug]     = "DEBUG",
                                                        };


void __attribute__(( __format__ (__printf__, 5, 6)) ) __dlogger_print(const char* file_p, 
                                                                      int line,
                                                                      const char* func_p, 
                                                                      const Dlogger_levelE level,
                                                                      const char * const restrict format_p,
                                                                      ...);


#define dlogger_priv_log_general(level, ...) __dlogger_print(__FILE__, __LINE__, __func__, level, __VA_ARGS__)

#define dlogger_priv_log_fatal(...)    dlogger_priv_log_general(dlogger_level_fatal, __VA_ARGS__)
#define dlogger_priv_log_critical(...) dlogger_priv_log_general(dlogger_level_critical, __VA_ARGS__)
#define dlogger_priv_log_error(...)    dlogger_priv_log_general(dlogger_level_error, __VA_ARGS__)
#define dlogger_priv_log_warning(...)  dlogger_priv_log_general(dlogger_level_warning, __VA_ARGS__)
#define dlogger_priv_log_info(...)     dlogger_priv_log_general(dlogger_level_info, __VA_ARGS__)
#define dlogger_priv_log_debug(...)    dlogger_priv_log_general(dlogger_level_debug, __VA_ARGS__)

#endif /* DLOGGER_PRIV_H */
