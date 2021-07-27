#ifndef DLOGGER_H
#define DLOGGER_H


/*
    This is the main header for DLogger library.


    Author: Kamil Kielbasa
    Email: kamilkielbasa64@gmail.com
    License: GPL3


    Main features:
    - auto file generation with date and time in name.
    - logging into file with information about source file, line number and function.
    - adding new line for log message if user forget. 
    - multithread safe but require pthread library (dependency from C11).
    - functionlike macro for logging could be use in the same way like any printf.
    - different level of logging available for user.
*/


#include "dlogger_priv.h"


/*
 * Available levels of logging in DLogger library. Remember that logging level set in constuctor will save only these messages which level 
 * is smaller or equal (all level <= level set in constructor)
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
#define DLOGGER_LEVEL_FATAL    dlogger_level_fatal
#define DLOGGER_LEVEL_CRITICAL dlogger_level_critical
#define DLOGGER_LEVEL_ERROR    dlogger_level_error
#define DLOGGER_LEVEL_WARNING  dlogger_level_warning
#define DLOGGER_LEVEL_INFO     dlogger_level_info
#define DLOGGER_LEVEL_DEBUG    dlogger_level_debug
#define DLOGGER_LEVEL_MAX      dlogger_level_max


/* 
 * This function initialize DLogger. Should be called only once and before any DLogger functions.
 *
 * @param[in] level - level of logging message into file. Functionlike macros only with level of message <= @level will be saved. 
 * 
 * @return 0 on succes, non-zero value on failure.
 */
int dlogger_init(Dlogger_levelE level);


/* 
 * This function deinitialize DLogger. Should be called after DLogger init and logging functions.
 *
 * @param[in] - void
 * 
 * @return - void
 */
void dlogger_deinit(void);


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


#endif /* DLOGGER_H */
