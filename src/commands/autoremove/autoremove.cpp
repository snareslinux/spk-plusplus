#include <fstream>
#include <string>
#include <filesystem>

#include "err.h"
#include "path.h"
#include "commands/autoremove/autoremove.h"

int perform_autoremove(void) {
    std::filesystem::path unowned_path = PATHDIR_INSTALLED_PKGS;
    unowned_path /= "unowned";
    unowned_path /= "unowned.files";
    
    std::ifstream unowned(unowned_path);
    std::string line;
    
    std::filesystem::path temp_path(std::filesystem::temp_directory_path());
    temp_path /= "unowned.files.temp";
    std::ofstream temp(temp_path);
    
    if (unowned && temp) {
        while (!unowned.eof()) {
            std::getline(unowned, line, '\n');
            if (std::filesystem::is_empty(line)) {
                std::filesystem::remove(line);
            } else {
                temp << line << std::endl;
            }
        }
    }
    
    unowned.close();
    std::filesystem::remove(unowned_path);
    std::filesystem::rename(temp_path, unowned_path);
    
    return ERR_OK;
}