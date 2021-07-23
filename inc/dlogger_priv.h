#ifndef DLOGGER_PRIV_H
#define DLOGGER_PRIV_H


/*
    This is the private header for DLogger library. Do not include it directly!

    Author: Kamil Kielbasa
    Email: kamilkielbasa64@gmail.com
    License: GPL3
*/


void __attribute__(( __format__ (__printf__, 4, 5)) ) __dlogger_print(const char* file_p, 
                                                                      const char* func_p, 
                                                                      int line, 
                                                                      const char * const restrict format_p, 
                                                                      ...);


#endif /* DLOGGER_PRIV_H */
