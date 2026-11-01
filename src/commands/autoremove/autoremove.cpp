#include <fstream>
#include <string>
#include <filesystem>

#include "err.h"
#include "path.h"
#include "commands/autoremove/autoremove.h"
#include "ui/ui.hpp"

int perform_autoremove(bool assume_yes) {
    if (!ui.confirm(assume_yes, false, "Do you wish to perform a auto remove")) {
        return ERR_OK;
    }

    std::filesystem::path unowned_path = PATHDIR_INSTALLED_PKGS;
    unowned_path /= "unowned";
    unowned_path /= "unowned.files";
    
    std::ifstream unowned(unowned_path);
    std::string line;
    
    std::filesystem::path temp_path(std::filesystem::temp_directory_path());
    temp_path /= "unowned.files.temp";
    std::ofstream temp(temp_path);
    
    if (unowned && temp) {
        while (std::getline(unowned, line, '\n')) {
            if (std::filesystem::is_empty(line)) {
                std::filesystem::remove(line);
            } else {
                temp << line << std::endl;
            }
        }
    }
    
    unowned.close();
    temp.close();
    std::filesystem::copy(temp_path, unowned_path);
    std::filesystem::remove(temp_path);
    
    return ERR_OK;
}