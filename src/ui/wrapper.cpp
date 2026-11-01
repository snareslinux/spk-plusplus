#include <cstdarg>

#include "ui/ui.h"
#include "ui/ui.hpp"

extern "C" void ui_init(void) {
    ui.init();
}

extern "C" int ui_create_task(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int res = ui.vcreate_task(fmt, args);
    va_end(args);
    return res;
}

extern "C" void ui_update_task(int task, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    ui.vupdate_task(task, fmt, args);
    va_end(args);
}

extern "C" void ui_end_task(int task, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    ui.vend_task(task, fmt, args);
    va_end(args);
}

extern "C" int ui_create_bar(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int res = ui.vcreate_bar(fmt, args);
    va_end(args);
    return res;
}

extern "C" void ui_update_bar(int bar, int progress, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    ui.vupdate_bar(bar, progress, fmt, args);
    va_end(args);
}

extern "C" bool ui_confirm(bool assume_yes, bool def, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    bool res = ui.vconfirm(assume_yes, def, fmt, args);
    va_end(args);
    return res;
}

extern "C" void ui_message(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    ui.vmessage(fmt, args);
    va_end(args);
}