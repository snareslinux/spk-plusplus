#include <iostream>
#include <cstring>
#include <string>
#include <vector>

#include "err.h"
#include "commands/search/search.h"
#include "repository/repository.hpp"
#include "ui/color.hpp"

int search_for_keyword(const char *key) {
    std::vector<RepoPackage> available;
    if (get_available_packages(&available) != ERR_OK) {
        return ERR_EXIT;
    }

    for (const auto &package : available) {
        std::string long_name = package.name + "-" + package.version;

        if (long_name.contains(key)) {
            size_t pos = long_name.find(key);
            if (pos == std::string::npos) {
                return ERR_EXIT;
            }

            std::string start = long_name.substr(0, pos);
            std::string middle = long_name.substr(pos, std::strlen(key));
            std::string end = long_name.substr(pos + std::strlen(key));

            std::cout
                << start
                << BLUE + BOLD << middle
                << RESET << end
                << " "
                << package.repository_name
                << std::endl;
        }
    }

    return ERR_OK;
}