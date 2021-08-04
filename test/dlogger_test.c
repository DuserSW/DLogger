#include <dlogger.h>
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
 * [FATAL]    [12:40:27.734058] [TID 7705] [test/dlogger_test.c:24	example_default] Message 1
 * [CRITICAL] [12:40:27.734198] [TID 7705] [test/dlogger_test.c:25	example_default] Message 2
 * [ERROR]    [12:40:27.734244] [TID 7705] [test/dlogger_test.c:26	example_default] Message 3
 * [WARNING]  [12:40:27.734273] [TID 7705] [test/dlogger_test.c:27	example_default] Message 4
 * [INFO]     [12:40:27.734300] [TID 7705] [test/dlogger_test.c:28	example_default] Message 5
 * [DEBUG]    [12:40:27.734325] [TID 7705] [test/dlogger_test.c:29	example_default] Message 6
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
 * [FATAL]    [13:32:37.984017] [test/dlogger_test.c:62	example_only_stdout] Message 1
 * [CRITICAL] [13:32:37.984309] [test/dlogger_test.c:63	example_only_stdout] Message 2
 * [ERROR]    [13:32:37.984342] [test/dlogger_test.c:64	example_only_stdout] Message 3
 * [WARNING]  [13:32:37.984375] [test/dlogger_test.c:65	example_only_stdout] Message 4
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
 * [FATAL]    [14:23:45.625824] [TID 9723] [test/dlogger_test.c:102 example_file_and_stderr] Message 1
 * [CRITICAL] [14:23:45.625942] [TID 9723] [test/dlogger_test.c:103 example_file_and_stderr] Message 2
 * [ERROR]    [14:23:45.626012] [TID 9723] [test/dlogger_test.c:104 example_file_and_stderr] Message 3
 * [WARNING]  [14:23:45.626069] [TID 9723] [test/dlogger_test.c:105 example_file_and_stderr] Message 4
 * [INFO]     [14:23:45.626099] [TID 9723] [test/dlogger_test.c:106 example_file_and_stderr] Message 5
 * [DEBUG]    [14:23:45.626176] [TID 9723] [test/dlogger_test.c:107 example_file_and_stderr] Message 6
 *
 *
 * Contents of stderr:
 * [FATAL]    [13:32:37.984017] [test/dlogger_test.c:62	example_file_and_stderr] Message 1
 * [CRITICAL] [13:32:37.984309] [test/dlogger_test.c:63	example_file_and_stderr] Message 2
 * [ERROR]    [13:32:37.984342] [test/dlogger_test.c:64	example_file_and_stderr] Message 3
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
 * [FATAL]    [14:23:45.625824] [TID 9723] [test/dlogger_test.c:102 example_file_and_stderr] Message 1
 * [CRITICAL] [14:23:45.625942] [TID 9723] [test/dlogger_test.c:103 example_file_and_stderr] Message 2
 * [ERROR]    [14:23:45.626012] [TID 9723] [test/dlogger_test.c:104 example_file_and_stderr] Message 3
 * [WARNING]  [14:23:45.626069] [TID 9723] [test/dlogger_test.c:105 example_file_and_stderr] Message 4
 * [INFO]     [14:23:45.626099] [TID 9723] [test/dlogger_test.c:106 example_file_and_stderr] Message 5
 * [DEBUG]    [14:23:45.626176] [TID 9723] [test/dlogger_test.c:107 example_file_and_stderr] Message 6
 *
 *
 * Contents of stdout:
 * [FATAL]    [13:42:11.579405] [TID 14905] [test/dlogger_test.c:167 example_all_descriptors] Message 1
 * [CRITICAL] [13:42:11.579538] [TID 14905] [test/dlogger_test.c:168 example_all_descriptors] Message 2
 * [ERROR]    [13:42:11.579655] [TID 14905] [test/dlogger_test.c:169 example_all_descriptors] Message 3
 *
 *
 * Contents of stderr:
 * [FATAL]    [13:44:21.512584] [TID 14925] [test/dlogger_test.c:167 example_all_descriptors] Message 1
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
