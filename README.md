## DLogger (Duser Logger)
DLogger is simple framework that allows you to log all necessary informations from point of programmer view. Framework is very useful for debugging process and for appliaction traces in case of undefined behaviour. Library is lightweight and use static memory for all logging operations to keep better performance (some logging functionlike macros could be on critical path). If you want to run your application in realease version you can easily turn off all traces from DLogger. For more details please read below documentation. If there is lack of informations or features that you need, please feel free to contant with me. I will try to help!

## Platform
- Supported only Linux platform. Tested on Ubuntu 20.04 LTS.

## Requirements
- Makefile.
- Compiler: clang or gcc. At least standard C11 and GNU dialect.
- Pthread library.

## How to build
There is six available options in Makefile:
````
all - build DLogger library with unit tests as examples.
lib - build only DLogger library.
test - build only DLogger unit tests.
install - build DLogger library and copy necessary files for specified directory.
clean - remove all files related with compilation process.
help - this option will print all available option in Makefile.
````

## How to import
Let's assume that your project where you want to use DLogger has following structure. External directory is a place where you keep libraries needed by your application.
````
├── external
├── inc
├── src
├── tests
├── app
````

There is two available options to install library. Both do the same!
````
by using: make install
by using script: install_dlogger.sh
````

#### Then you need to do following steps:
````
1. Download DLogger repository and go into them.
2. Install library in one way:
    * $make install P=/home/$user/project/external
    * $scripts/install_dlogger.sh /home/$user/project/external
3. You need to link two things: library and header files. Add below options to compilation process.
    * -I/home/$user/project/external/dlogger/inc -L/home/$user/project/external/dlogger -ldlogger -lpthread
4. If you need better backtrace in logs or for debugging, you can pass flag -rdynamic. 
   (Please remember that binary size will increase)
5. In the source file where you want to use DLogger include main header: #include <dlogger/dlogger.h>
6. Now you can use DLogger library. Please read features, functions documentation and understand examples.
````

## Features
- multithread safe but require pthread library (dependency from C11).
- auto file generation with date and time in name.
- available to logging for different descriptors (uniq file, stdout, stderr) in the same time.
- adding new line for log message if user forget. 
- different level of logging available for user.
- available to add timestamp and thread id into logs.
- for fatal level problems the backtrace will be save. Use flag -rdynamic to compilation to get full backtrace.
- functionlike macro for logging could be use in the same way like any printf.
- turn off all (with/without FATAL) log functionslike macros for release version.

### Level of logging:
````
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
````

### Available descriptors:
````
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
````

### Additional options:
````
/*
 * Available additional options for DLogger. These options could be compine with bitwise OR (|) operator.
 *
 * DLOGGER_OPTION_MARK_TIMESTAMP - save for each log timestamp which contain hourse, minuts, seconds and microseconds.
 *
 * DLOGGER_OPTION_MARK_THREADID  - save for each log thread id. Very useful information for multi-thread code.
 */
#define DLOGGER_OPTION_MARK_TIMESTAMP DLOGGER_PRIV_OPTION_MARK_TIMESTAMP
#define DLOGGER_OPTION_MARK_THREADID  DLOGGER_PRIV_OPTION_MARK_THREADID
````

### Turn-off all traces:
````
/* 
 * This define works in the same way like NDEBUG introduced for macro assert from assert.h. If you want to 
 * compile your application to release version, use this define to turn-off functionlike macros for logging. 
 *
 * Please remember that dlogger_log_fatal will be still working. If you want to turn-off fatal functionlike macro 
 * as well, please define additionally DLOGGER_SILENT_FATAL. Then all logging functionlike macros will be turn off.
 */
#define NDEBUG
#define DLOGGER_SILENT_FATAL 
````

## Example of usage

### Default usage:
````
/* 
 * By default, if you do not set any options by creating and setting DLogger_user_optionsS object framework 
 * will save all log messages into uniq generated file with with max level DLOGGER_LEVEL_MAX, timestamp
 * and thread id. Please note that if you forget to add new line, framework will do it automatically.
 *
 *
 * Contents of log file:
 * [FATAL]    [15:19:19.526156] [TID 17839] [test/dlogger_test.c:31 example_default] Message 1
 * Backtrace:
 * ./test.out(+0x1f6d) [0x56534b5b5f6d]
 * ./test.out(+0x1398) [0x56534b5b5398]
 * /lib/x86_64-linux-gnu/libc.so.6(__libc_start_main+0xf3) [0x7f7ffebab0b3]
 * ./test.out(+0x14ae) [0x56534b5b54ae]
 * [CRITICAL] [15:19:19.526611] [TID 17839] [test/dlogger_test.c:32 example_default] Message 2
 * [ERROR]    [15:19:19.526649] [TID 17839] [test/dlogger_test.c:33 example_default] Message 3
 * [WARNING]  [15:19:19.526674] [TID 17839] [test/dlogger_test.c:34 example_default] Message 4
 * [INFO]     [15:19:19.526698] [TID 17839] [test/dlogger_test.c:35 example_default] Message 5
 * [DEBUG]    [15:19:19.526722] [TID 17839] [test/dlogger_test.c:36 example_default] Message 6
 */
static void example_default(void)
{
    dlogger_create(NULL);

    register size_t counter = 0;

    dlogger_log_fatal("Message %zu\n", ++counter);
    dlogger_log_critical("Message %zu", ++counter); 
    dlogger_log_error("Message %zu", ++counter);
    dlogger_log_warning("Message %zu", ++counter);
    dlogger_log_info("Message %zu", ++counter);
    dlogger_log_debug("Message %zu", ++counter);

    dlogger_destroy(); 
}
````

### Using only stdout:
````
/* 
 * In this example we want to print all logs to stdout. After initialization we set options
 * that we will print all logs only into stdout with level >= DLOGGER_LEVEL_WARNING and with
 * timestamp. Thread id wasn't necessary. 
 *
 * Please note that if you set any options, uniq file won't be generated.
 *
 *
 * Contents of stdout:
 * [FATAL]    [15:20:59.977256] [test/dlogger_test.c:74 example_only_stdout] Message 1
 * Backtrace:
 * ./test.out(+0x1f9d) [0x5560a3efef9d]
 * ./test.out(+0x13bd) [0x5560a3efe3bd]
 * /lib/x86_64-linux-gnu/libc.so.6(__libc_start_main+0xf3) [0x7f706e7170b3]
 * ./test.out(+0x14de) [0x5560a3efe4de]
 * [CRITICAL] [15:20:59.978115] [test/dlogger_test.c:75 example_only_stdout] Message 2
 * [ERROR]    [15:20:59.978182] [test/dlogger_test.c:76 example_only_stdout] Message 3
 * [WARNING]  [15:20:59.978238] [test/dlogger_test.c:77 example_only_stdout] Message 4
 */
static void example_only_stdout(void)
{
    DLogger_user_optionsS* user_options_p = dlogger_create_user_options();
    dlogger_set_user_options(user_options_p,
                             DLOGGER_OPTION_WRITE_TO_STDOUT,
                             DLOGGER_LEVEL_WARNING,
                             DLOGGER_OPTION_MARK_TIMESTAMP);

    dlogger_create(user_options_p);
    dlogger_destroy_user_options(user_options_p);

    register size_t counter = 0;

    dlogger_log_fatal("Message %zu\n", ++counter);
    dlogger_log_critical("Message %zu", ++counter); 
    dlogger_log_error("Message %zu", ++counter);
    dlogger_log_warning("Message %zu", ++counter);
    dlogger_log_info("Message %zu", ++counter);
    dlogger_log_debug("Message %zu", ++counter);

    dlogger_destroy(); 
}
````

### Using uniq file and stderr:
````
/* 
 * In this example is typical for application development. We want to save all logs into uniq 
 * file and print to stderr only important issues.
 *
 * 
 * Contants of uniq file:
 * [FATAL]    [15:22:28.24314] [TID 18031] [test/dlogger_test.c:126 example_file_and_stderr] Message 1
 * Backtrace:
 * ./test.out(+0x1fad) [0x557137bd4fad]
 * ./test.out(+0x13d1) [0x557137bd43d1]
 * /lib/x86_64-linux-gnu/libc.so.6(__libc_start_main+0xf3) [0x7fe4c80eb0b3]
 * ./test.out(+0x14ee) [0x557137bd44ee]
 * [CRITICAL] [15:22:28.25116] [TID 18031] [test/dlogger_test.c:127 example_file_and_stderr] Message 2
 * [ERROR]    [15:22:28.25214] [TID 18031] [test/dlogger_test.c:128 example_file_and_stderr] Message 3
 * [WARNING]  [15:22:28.25299] [TID 18031] [test/dlogger_test.c:129 example_file_and_stderr] Message 4
 * [INFO]     [15:22:28.25343] [TID 18031] [test/dlogger_test.c:130 example_file_and_stderr] Message 5
 * [DEBUG]    [15:22:28.25382] [TID 18031] [test/dlogger_test.c:131 example_file_and_stderr] Message 6
 *
 *
 * Contents of stderr:
 * [FATAL]    [15:22:28.24900] [TID 18031] [test/dlogger_test.c:126 example_file_and_stderr] Message 1
 * Backtrace:
 * ./test.out(+0x1fad) [0x557137bd4fad]
 * ./test.out(+0x13d1) [0x557137bd43d1]
 * /lib/x86_64-linux-gnu/libc.so.6(__libc_start_main+0xf3) [0x7fe4c80eb0b3]
 * ./test.out(+0x14ee) [0x557137bd44ee]
 * [CRITICAL] [15:22:28.25170] [TID 18031] [test/dlogger_test.c:127 example_file_and_stderr] Message 2
 * [ERROR]    [15:22:28.25255] [TID 18031] [test/dlogger_test.c:128 example_file_and_stderr] Message 3
 */
static void example_file_and_stderr(void)
{
    DLogger_user_optionsS* user_options_p = dlogger_create_user_options();
    dlogger_set_user_options(user_options_p,
                             DLOGGER_OPTION_WRITE_TO_FILE, 
                             DLOGGER_LEVEL_MAX, 
                             DLOGGER_OPTION_MARK_TIMESTAMP | DLOGGER_OPTION_MARK_THREADID);
    dlogger_set_user_options(user_options_p,
                             DLOGGER_OPTION_WRITE_TO_STDERR, 
                             DLOGGER_LEVEL_ERROR, 
                             DLOGGER_OPTION_MARK_TIMESTAMP | DLOGGER_OPTION_MARK_THREADID);

    dlogger_create(user_options_p);
    dlogger_destroy_user_options(user_options_p);

    register size_t counter = 0;

    dlogger_log_fatal("Message %zu\n", ++counter);
    dlogger_log_critical("Message %zu", ++counter); 
    dlogger_log_error("Message %zu", ++counter);
    dlogger_log_warning("Message %zu", ++counter);
    dlogger_log_info("Message %zu", ++counter);
    dlogger_log_debug("Message %zu", ++counter);

    dlogger_destroy();
}
````

### Using all available descriptors:
````
/* 
 * In this example is typical for application debugging. We want to save all logs into uniq 
 * file and print to stderr only fatal errors, less and equal to stdout.
 *
 * 
 * Contants of uniq file:
 * [FATAL]    [15:24:14.569410] [TID 18094] [test/dlogger_test.c:191 example_all_descriptors] Message 1
 * Backtrace:
 * ./test.out(+0x1fbd) [0x5563641affbd]
 * ./test.out(+0x13e5) [0x5563641af3e5]
 * /lib/x86_64-linux-gnu/libc.so.6(__libc_start_main+0xf3) [0x7fd5e86aa0b3]
 * ./test.out(+0x14fe) [0x5563641af4fe]
 * [CRITICAL] [15:24:14.570618] [TID 18094] [test/dlogger_test.c:192 example_all_descriptors] Message 2
 * [ERROR]    [15:24:14.570738] [TID 18094] [test/dlogger_test.c:193 example_all_descriptors] Message 3
 * [WARNING]  [15:24:14.570835] [TID 18094] [test/dlogger_test.c:194 example_all_descriptors] Message 4
 * [INFO]     [15:24:14.570882] [TID 18094] [test/dlogger_test.c:195 example_all_descriptors] Message 5
 * [DEBUG]    [15:24:14.570938] [TID 18094] [test/dlogger_test.c:196 example_all_descriptors] Message 6
 *
 *
 * Contents of stdout:
 * [FATAL]    [15:24:14.570384] [TID 18094] [test/dlogger_test.c:191 example_all_descriptors] Message 1
 * Backtrace:
 * ./test.out(+0x1fbd) [0x5563641affbd]
 * ./test.out(+0x13e5) [0x5563641af3e5]
 * /lib/x86_64-linux-gnu/libc.so.6(__libc_start_main+0xf3) [0x7fd5e86aa0b3]
 * ./test.out(+0x14fe) [0x5563641af4fe]
 * [CRITICAL] [15:24:14.570682] [TID 18094] [test/dlogger_test.c:192 example_all_descriptors] Message 2
 * [ERROR]    [15:24:14.570784] [TID 18094] [test/dlogger_test.c:193 example_all_descriptors] Message 3
 *
 *
 * Contents of stderr:
 * [FATAL]    [15:24:14.570123] [TID 18094] [test/dlogger_test.c:191 example_all_descriptors] Message 1
 * Backtrace:
 * ./test.out(+0x1fbd) [0x5563641affbd]
 * ./test.out(+0x13e5) [0x5563641af3e5]
 * /lib/x86_64-linux-gnu/libc.so.6(__libc_start_main+0xf3) [0x7fd5e86aa0b3]
 * ./test.out(+0x14fe) [0x5563641af4fe]
 */
static void example_all_descriptors(void)
{
    DLogger_user_optionsS* user_options_p = dlogger_create_user_options();
    dlogger_set_user_options(user_options_p,
                             DLOGGER_OPTION_WRITE_TO_FILE,
                             DLOGGER_LEVEL_MAX,
                             DLOGGER_OPTION_MARK_TIMESTAMP | DLOGGER_OPTION_MARK_THREADID);
    dlogger_set_user_options(user_options_p,
                             DLOGGER_OPTION_WRITE_TO_STDOUT,
                             DLOGGER_LEVEL_ERROR,
                             DLOGGER_OPTION_MARK_TIMESTAMP | DLOGGER_OPTION_MARK_THREADID);
    dlogger_set_user_options(user_options_p,
                             DLOGGER_OPTION_WRITE_TO_STDERR,
                             DLOGGER_LEVEL_FATAL,
                             DLOGGER_OPTION_MARK_TIMESTAMP | DLOGGER_OPTION_MARK_THREADID);

    dlogger_create(user_options_p);
    dlogger_destroy_user_options(user_options_p);

    register size_t counter = 0;

    dlogger_log_fatal("Message %zu\n", ++counter);
    dlogger_log_critical("Message %zu", ++counter); 
    dlogger_log_error("Message %zu", ++counter);
    dlogger_log_warning("Message %zu", ++counter);
    dlogger_log_info("Message %zu", ++counter);
    dlogger_log_debug("Message %zu", ++counter);

    dlogger_destroy();
}
````
## Limitations
For better performance static memory has been used as internal buffer for every log functionlike macro. We cannot use dynamic allocations on heap because it might introduce latency for huge messages (malloc -> system call) in critial path in your application. Please remember that internal buffer contain 2^15 bytes. For bigger messages you can split it for few logging functionlike macros or just increase internal memory buffer as much as need.

````
static char buffer[1 << 15] = {0};
````

## Contact
Email: kamilkielbasa64@gmail.com
