#include "cli.h"
#include "os/os.h"
#include "ui/ui.h"

using namespace os;

bool cmd_state(const String &args)
{
    ui_print_state();
    return true;
}
