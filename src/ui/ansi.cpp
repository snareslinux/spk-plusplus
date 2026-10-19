#include <iostream>

#include "ui.hpp"

ANSI ansi;

void ANSI::move(int line) {
    std::cout << "\033[" << line << ";1H";
}

void ANSI::moveup(int n) {
    if (n > 0) {
        std::cout << "\033[" << n << "A";
    }
    std::cout << "\r\033[J";
}

void ANSI::clear_line(int line) {
    move(line);
    std::cout << "\033[2K";
}