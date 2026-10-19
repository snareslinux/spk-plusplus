#include <cstdarg>
#include <string>
#include <array>
#include <optional>
#include <thread>
#include <mutex>

#include "ui.h"

#ifndef UI_HPP
#define UI_HPP

#define MAX_TASKS 12
#define MAX_BARS 12

struct Task {
    std::string text;
    bool running = true;
};

struct Bar {
    std::string text;
    int progress;
};

class UI {
    public:
        void init(void);
        void shutdown(void);
        ~UI();

        void redraw(void);

        int create_task(const char *fmt, ...);
        void update_task(int task, const char *fmt, ...);
        void end_task(int task, const char *fmt, ...);

        int create_bar(const char *fmt, ...);
        void update_bar(int bar, int progress, const char *fmt, ...);

        bool confirm(bool assume_yes, bool default_value, const char *fmt, ...);
    
        int vcreate_task(const char *fmt, va_list args);
        void vupdate_task(int task, const char *fmt, va_list args);
        void vend_task(int task, const char *fmt, va_list args);

        int vcreate_bar(const char *fmt, va_list args);
        void vupdate_bar(int bar, int progress, const char *fmt, va_list args);

        bool vconfirm(bool assume_yes, bool def, const char *fmt, va_list args);

    private:
        bool valid_task(int task);
        bool valid_bar(int bar);

        int get_terminal_rows(void);

        int writable_lines = 0;
        char curr_spin = '|';

        bool paused = false;

        std::jthread thread;
        std::mutex mutex;

        std::array<std::optional<Task>, MAX_TASKS> tasks;
        std::array<std::optional<Bar>, MAX_BARS> bars;
};

extern UI ui;

class ANSI {
    public:
        void move(int line);
        void moveup(int n);
        void clear_line(int line);

    private:
};

extern ANSI ansi;

#endif