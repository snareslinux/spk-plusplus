#include <stdbool.h>

#ifndef UI_H
#define UI_H

#ifdef __cplusplus
extern "C" {
#endif

void ui_init(void);

int ui_create_task(const char *fmt, ...);
void ui_update_task(int task, const char *fmt, ...);
void ui_end_task(int task, const char *fmt, ...);

int ui_create_bar(const char *fmt, ...);
void ui_update_bar(int bar, int progress, const char *fmt, ...);

bool ui_confirm(bool assume_yes, bool def, const char *fmt, ...);

void ui_message(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif