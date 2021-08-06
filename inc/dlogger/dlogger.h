#ifndef DLOGGER_H
#define DLOGGER_H


/*
    This is the main header for DLogger library.


    Author: Kamil Kielbasa
    Email: kamilkielbasa64@gmail.com
    License: GPL3


    Main features:
    - multithread safe but require pthread library (dependency from C11).
    - auto file generation with date and time in name.
    - available to logging for different descriptors (uniq file, stdout, stderr) in the same time.
    - adding new line for log message if user forget. 
    - different level of logging available for user.
    - available to add timestamp and thread id into logs.
    - for fatal level problems the backtrace will be save. Use flag -rdynamic to compilation to get full backtrace.
    - functionlike macro for logging could be use in the same way like any printf.
*/


#include "dlogger_priv.h"


/*
 * Available levels of logging in DLogger library. Remember that logging level set in constuctor will save only these messages which level 
 * is smaller or equal (all level <= level set in constructor).
 *
 * DLOGGER_LEVEL_FATAL    - Fatal level error will occurs only if application needs to be immediately close. This kind of errors will not 
 *                          allow application to works correctly.
 * 
 * DLOGGER_LEVEL_CRITICAL - Critical level errors will occurs if we have hardware error or system error. Application doesn't need to be 
 *                          close but we can try few more times process some part of code. Good example of critical error might be problem 
 *                          with create or open file.
 * 
 * DLOGGER_LEVEL_ERROR    - Error level is kind of error that is controled by user layer of appliaction and you can handle this or close 
 *                          appliaction. Good examples might be: 
 *                            1) pass NULL pointer to function. 
 *                            2) cannot allocate memory on heap by using malloc.
 * 
 * DLOGGER_LEVEL_WARNING  - Warning level tell us about that something goes wrong but application might be still executed. It could be 
 *                          information about potencial bug on current or upper layer. Good examples might be:
 *                            1) Overflow of variables.
 *                            2) Unaligned memory.
 * 
 * DLOGGER_LEVEL_INFO     - Information level should be use for imporant informations from the point of user. Good examples might be:
 *                            1) Start or end of threads execution.
 *                            2) Using external framework.
 * 
 * DLOGGER_LEVEL_DEBUG    - Debug level should be use for programmer to debug code. You can use it as you want. It depends of your needs.
 * 
 * DLOGGER_LEVEL_MAX      - Turn on all levels of saving messages.
 */
#define DLOGGER_LEVEL_FATAL    DLOGGER_PRIV_LEVEL_FATAL
#define DLOGGER_LEVEL_CRITICAL DLOGGER_PRIV_LEVEL_CRITICAL
#define DLOGGER_LEVEL_ERROR    DLOGGER_PRIV_LEVEL_ERROR
#define DLOGGER_LEVEL_WARNING  DLOGGER_PRIV_LEVEL_WARNING
#define DLOGGER_LEVEL_INFO     DLOGGER_PRIV_LEVEL_INFO
#define DLOGGER_LEVEL_DEBUG    DLOGGER_PRIV_LEVEL_DEBUG
#define DLOGGER_LEVEL_MAX      DLOGGER_PRIV_LEVEL_MAX


/*
 * Available descriptors to save logs. DLogger is able to write for all descriptors in the same time.
 *
 * DLOGGER_OPTION_WRITE_TO_FILE   - write to uniq generated file basen on timestamp with date, time (with microseconds).
 *
 * DLOGGER_OPTION_WRITE_TO_STDOUT - write to standard output stream.
 *
 * DLOGGER_OPTION_WRITE_TO_STDERR - write to standard error stream.
 */
#define DLOGGER_OPTION_WRITE_TO_FILE   DLOGGER_PRIV_OPTION_WRITE_TO_FILE
#define DLOGGER_OPTION_WRITE_TO_STDOUT DLOGGER_PRIV_OPTION_WRITE_TO_STDOUT
#define DLOGGER_OPTION_WRITE_TO_STDERR DLOGGER_PRIV_OPTION_WRITE_TO_STDERR


/*
 * Available additional options for DLogger. These options could be compine with bitwise OR (|) operator.
 *
 * DLOGGER_OPTION_MARK_TIMESTAMP - save for each log timestamp which contain hourse, minuts, seconds and microseconds.
 *
 * DLOGGER_OPTION_MARK_THREADID  - save for each log thread id. Very useful information for multi-thread code.
 */
#define DLOGGER_OPTION_MARK_TIMESTAMP DLOGGER_PRIV_OPTION_MARK_TIMESTAMP
#define DLOGGER_OPTION_MARK_THREADID  DLOGGER_PRIV_OPTION_MARK_THREADID


/* Structure which contain options set by user by dedicated API. */
typedef struct DLogger_user_optionsS DLogger_user_optionsS;


/* 
 * This function create DLogger user options. Should be called only once and before any DLogger functions.
 *
 * @param[in] - void.
 * 
 * @return pointer to DLogger_user_optionsS if success, otherwise NULL.
 */
DLogger_user_optionsS* dlogger_create_user_options(void);


/* 
 * This function destroy DLogger user options. Should be called after creating of DLogger to free memory 
 * becuase in run-time we cannot change any option of DLogger.
 *
 * @param[in] user_options_p - pointer to options specified by user.
 * 
 * @return - void.
 */
void dlogger_destroy_user_options(DLogger_user_optionsS* user_options_p);


/* 
 * This function allows user to specify options for DLogger.
 *
 * @param[in] user_options_p      - pointer to options specified by user.
 * @param[in] descriptor_to_write - which descriptor options @level_of_logging and @additional_options will be written.
 * @param[in] level_of_logging    - level of logging for above @descriptor_to_write.
 * @param[in] additional_options  - additional options available in DLogger.
 * 
 * @return - void.
 */
void dlogger_set_user_options(DLogger_user_optionsS* user_options_p, 
                              DLogger_options_writeE descriptor_to_write, 
                              DLogger_levelE level_of_logging, 
                              DLogger_options_markE additional_options);


/*
 * This function create and initialize DLogger. Should be called only once and before any DLogger functions.
 *
 * @param[in] user_options_p - pointer to options specified by user.
 * 
 * @return 0 on succes, non-zero value on failure.
 */
int dlogger_create(const DLogger_user_optionsS* user_options_p);


/* 
 * This function destroy DLogger. Should be called after DLogger create and logging functions.
 *
 * @param[in] - void
 * 
 * @return - void
 */
void dlogger_destroy(void);


/* 
 * This define works in the same way like NDEBUG introduced for macro assert from assert.h. If you want to 
 * compile your application to release version, use this define to turn-off functionlike macros for logging. 
 *
 * Please remember that dlogger_log_fatal will be still working. If you want to turn-off fatal functionlike macro 
 * as well, please define additionally DLOGGER_SILENT_FATAL. Then all logging functionlike macros will be turn off.
 */
#ifndef NDEBUG

/* 
 * This functionlike macro is responsible for logging messages into file. Can be use in the same way like printf.
 *
 * @param[in] - variadic arguments.
 * 
 * @return - void
 */
#define dlogger_log_fatal(...)    dlogger_priv_log_fatal(__VA_ARGS__)
#define dlogger_log_critical(...) dlogger_priv_log_critical(__VA_ARGS__)
#define dlogger_log_error(...)    dlogger_priv_log_error(__VA_ARGS__)
#define dlogger_log_warning(...)  dlogger_priv_log_warning(__VA_ARGS__)
#define dlogger_log_info(...)     dlogger_priv_log_info(__VA_ARGS__)
#define dlogger_log_debug(...)    dlogger_priv_log_debug(__VA_ARGS__)

#else

#ifndef DLOGGER_SILENT_FATAL 

#define dlogger_log_fatal(...)    dlogger_priv_log_fatal(__VA_ARGS__)

#else

#define dlogger_log_fatal(...)

#endif /* DLOGGER_SILENT_FATAL */

#define dlogger_log_critical(...)
#define dlogger_log_error(...)
#define dlogger_log_warning(...)
#define dlogger_log_info(...)
#define dlogger_log_debug(...)

#endif /* NDEBUG */

#endif /* DLOGGER_H */
