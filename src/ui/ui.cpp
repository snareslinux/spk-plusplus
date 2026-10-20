#include <iostream>
#include <cstdarg>
#include <mutex>
#include <optional>
#include <cstdio>
#include <array>
#include <string>
#include <chrono>
#include <thread>
#include <sys/ioctl.h>
#include <unistd.h>

#include "ui.h"
#include "ui.hpp"
#include "helper.hpp"

UI ui;

int UI::get_terminal_rows(void) {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) != 0) {
        return -1;
    }
    return ws.ws_row;
}

int UI::create_task(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int res = vcreate_task(fmt, args);
    va_end(args);
    return res;
}

void UI::update_task(int task, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vupdate_task(task, fmt, args);
    va_end(args);
}

void UI::end_task(int task, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vend_task(task, fmt, args);
    va_end(args);
}

int UI::create_bar(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int res = vcreate_bar(fmt, args);
    va_end(args);
    return res;
}

void UI::update_bar(int bar, int progress, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vupdate_bar(bar, progress, fmt, args);
    va_end(args);
}

bool UI::confirm(bool assume_yes, bool def, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    bool res = vconfirm(assume_yes, def, fmt, args);
    va_end(args);
    return res;
}

void UI::message(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vmessage(fmt, args);
    va_end(args);
}

bool UI::valid_task(int task) {
    return (
        task >= 0 &&
        task < MAX_TASKS &&
        tasks[task].has_value()
    );
}

bool UI::valid_bar(int bar) {
    return (
        bar >= 0 &&
        bar < MAX_BARS &&
        bars[bar].has_value()
    );
}

int UI::vcreate_task(const char *fmt, va_list args) {
    std::lock_guard lock(mutex);

    for (int i = 0; i < MAX_TASKS; i++) {
        if (!tasks[i]) {

            char text[256];
            std::vsnprintf(text, sizeof(text), fmt, args);

            tasks[i] = Task{text, true};
            return i;
        }
    }
    return -1;
}

int UI::vcreate_bar(const char *fmt, va_list args) {
    std::lock_guard lock(mutex);

    for (int i = 0; i < MAX_BARS; i++) {
        if (!bars[i]) {

            char text[256];
            std::vsnprintf(text, sizeof(text), fmt, args);

            bars[i] = Bar{text, 0};
            return i;
        }
    }
    return -1;
}

void UI::vupdate_task(int task, const char *fmt, va_list args) {
    std::lock_guard lock(mutex);

    if (!valid_task(task)) {
        return;
    }

    char text[256];
    std::vsnprintf(text, sizeof(text), fmt, args);

    tasks[task]->text = text;
}


void UI::vupdate_bar(int bar, int progress, const char *fmt, va_list args) {
    std::lock_guard lock(mutex);

    if (!valid_bar(bar)) {
        return;
    }

    bars[bar]->progress = progress;

    if (!fmt) {
        return;
    }

    char text[256];
    std::vsnprintf(text, sizeof(text), fmt, args);

    bars[bar]->text = text;
}


void UI::vend_task(int task, const char *fmt, va_list args) {
    std::lock_guard lock(mutex);

    if (!valid_task(task)) {
        return;
    }

    tasks[task]->running = false;

    if (!fmt) {
        return;
    }

    char text[256];
    std::vsnprintf(text, sizeof(text), fmt, args);

    tasks[task]->text = text;
}

bool UI::vconfirm(bool assume_yes, bool def, const char *fmt, va_list args) {
    char message[256] = "";
    std::vsnprintf(message, sizeof(message), fmt, args);

    {
        std::lock_guard lock(mutex);
        paused = true;
        ansi.moveup(writable_lines);
        writable_lines = 0;
        
        std::cout << message << (def ? " [Y/n] " : " [y/N] ");
        if (assume_yes) {
            std::cout << "y" << std::endl;
        }
        std::cout << std::flush;
    }

    bool res = def;

    if (assume_yes) {
        res = true;
    } else {
        char line[64];
        if (fgets(line, sizeof(line), stdin)) {
            if (line[0] == 'y' || line[0] == 'Y') {
                res = true;
            } else if (line[0] == 'n' || line[0] == 'N') {
                res = false;
            }
        } else {
            std::cout << std::endl << std::flush;
        }
    }

    std::lock_guard lock(mutex);
    paused = false;
    return res;
}

void UI::vmessage(const char *fmt, va_list args) {
    char message[256];
    std::vsnprintf(message, sizeof(message), fmt, args);

    std::lock_guard lock(mutex);

    ansi.moveup(writable_lines);
    writable_lines = 0;

    std::cout << message << std::endl << std::flush;
}

void UI::init(void) {
    if (thread.joinable()) {
        return;
    }

    thread = std::jthread([this](std::stop_token stop) {
        while (!stop.stop_requested()) {
            redraw();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });
}

void UI::shutdown(void) {
    if (!thread.joinable()) {
        return;
    }

    thread.request_stop();
    thread.join();

    redraw();
}

UI::~UI() {
    shutdown();
};

void UI::redraw(void) {
    std::lock_guard lock(mutex);

    if (paused) {
        return;
    }

    int term_rows = get_terminal_rows();
    if (term_rows < 0) {
        return;
    }

    ansi.moveup(writable_lines);
    writable_lines = 0;

    for (int i = 0; i < MAX_TASKS; i++) {
        if (tasks[i].has_value() && tasks[i]->running == false) {
            std::cout << tasks[i]->text << std::endl;
            tasks[i].reset();
        }
    }

    for (int i = 0; i < MAX_BARS; i++) {
        if (bars[i].has_value() && bars[i]->progress >= 100) {
            std::cout << bars[i]->text << std::endl;
            bars[i].reset();
        }
    }

    char next_spin = get_next_spin(curr_spin);

    for (int i = 0; i < MAX_TASKS; i++) {
        if (tasks[i].has_value()) {
            std::cout << tasks[i]->text << " " << next_spin << std::endl;
            writable_lines++;
        }
    }

    curr_spin = next_spin;

    for (int i = 0; i < MAX_BARS; i++) {
        if (bars[i].has_value()) {
            std::cout << bars[i]->text << " " << get_bar(bars[i]->progress) << std::endl;
            writable_lines++;
        }
    }

    std::cout << std::flush;
}