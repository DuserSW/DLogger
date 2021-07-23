#include <dlogger.h>

int main(void)
{
    dlogger_init();

    dlogger_log("Hello dlogger!\n");
    dlogger_log("Hello without newline!");
    dlogger_log("New values saved = %d %c!", 0, 'a');

    dlogger_deinit();
}
