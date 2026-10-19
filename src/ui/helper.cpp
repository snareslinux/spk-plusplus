#include <string>

#include "helper.hpp"

char get_next_spin(char curr) {
    switch (curr) {
        case '|':
            return '/';
        
        case '/':
            return '-';

        case '-':
            return '\\';

        case '\\':
            return '|';

        default:
            return '|';
    }
}

std::string get_bar(int progress) {
    int tenth = progress / 10;

    std::string bar = "[";

    char last = 0;
    for (int i = 0; i < 10; i++) {
        if (tenth-- > 0) {
            last = '#';
            bar += "#";
        } else {
            if (last == '#') {
                last = '>';
                bar += ">";
            } else {
                last = '-';
                bar += "-";
            }
        }
    }

    bar += "]";

    return bar;
}