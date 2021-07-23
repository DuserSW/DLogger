#ifndef DLOGGER_H
#define DLOGGER_H


/*
    This is the main header for DLogger library.


    Author: Kamil Kielbasa
    Email: kamilkielbasa64@gmail.com
    License: GPL3


    Main features:
    - auto file generation with date and time in name.
    - logging into file with information about source file, function and line number.
    - adding new line for log message if user forget. 
    - multithread safe but require pthread library (dependency from C11).
    - functionlike macro for logging could be use in the same way like any printf.
*/


#include "dlogger_priv.h"


/* 
 * This function initialize DLogger. Should be called only once and before any DLogger functions.
 *
 * @param[in] - void
 * 
 * @return 0 on succes, non-zero value on failure.
 */
int dlogger_init(void);


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
#define dlogger_log(...) __dlogger_print(__FILE__, __func__, __LINE__, __VA_ARGS__)


#endif /* DLOGGER_H */
