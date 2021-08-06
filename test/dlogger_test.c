#include <dlogger/dlogger.h>
#include <stddef.h>


static void example_default(void);
static void example_only_stdout(void);
static void example_file_and_stderr(void);
static void example_all_descriptors(void);


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


int main(void)
{
    example_default();
    example_only_stdout();
    example_file_and_stderr();
    example_all_descriptors();

    return 0;
}
