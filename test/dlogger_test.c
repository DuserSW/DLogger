#include <dlogger.h>

int main(void)
{
    dlogger_init(DLOGGER_LEVEL_MAX);

    dlogger_log_fatal("FATAL!");
    dlogger_log_critical("CRITICAL!"); 
    dlogger_log_error("ERROR!");
    dlogger_log_warning("WARNING!");
    dlogger_log_info("INFO!");     
    dlogger_log_debug("DEBUG!");

    dlogger_deinit();
}
