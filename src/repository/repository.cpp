#include <vector>
#include <string>
#include <sstream>
#include <filesystem>
#include <fstream>

#include "err.h"
#include "path.h"
#include "util/util.h"
#include "util/util.hpp"
#include "repository/repository.hpp"
#include "repository/repository.h"

int get_repositories(std::vector<Repository> &out) {
    out.clear();

    if (!std::filesystem::exists(PATH_REPO_CONF)) {
        return ERR_EXIT;
    }

    std::error_code ec;
    std::ifstream config(PATH_REPO_CONF);
    if (!config) {
        return ERR_EXIT;
    }

    std::string line;
    while (std::getline(config, line)) {
        if (line.empty()) {
            continue;
        }

        Repository repo;
        std::istringstream stream(line);

        stream >> repo.name;
        if (repo.name[0] == '#') {
            continue;
        }

        if (!(stream >> repo.location)) {
            return ERR_EXIT;
        }

        if (repo.location.back() != '/') {
            repo.location += "/";
        }
        
        out.push_back(repo);
    }

    return ERR_OK;
}

int update_repositories(void) {
    std::vector<Repository> repositories;
    if (get_repositories(repositories) != ERR_OK) {
        return ERR_EXIT;
    }

    std::error_code ec;
    std::filesystem::create_directories(PATHDIR_REPO_CACHE, ec);
    if (ec) {
        return ERR_EXIT;
    }

    for (const auto &repo : repositories) {
        std::filesystem::path output_path(PATHDIR_REPO_CACHE);
        output_path /= repo.name;

        FILE *output = std::fopen(output_path.c_str(), "wb");
        if (!output) {
            return ERR_EXIT;
        }

        std::string url = repo.location;
        url += get_architecture() + "/index";

        if (download_to_file(url.c_str(), output) != ERR_OK) {
            std::fclose(output);
            std::filesystem::remove(output_path);
            return ERR_EXIT;
        }

        std::fclose(output);
    }

    return ERR_OK;
}

int get_available_packages(std::vector<RepoPackage> *packages) {
    packages->clear();

    std::filesystem::path cache(PATHDIR_REPO_CACHE);

    std::vector<Repository> repositories;
    if (get_repositories(repositories) != ERR_OK) {
        return ERR_EXIT;
    }
    
    for (const auto &repo : repositories) {
        std::filesystem::path index_path = cache / repo.name;
        if (!std::filesystem::exists(index_path) || !std::filesystem::is_regular_file(index_path)) {
            continue;
        }

        std::ifstream index(index_path);
        if (!index) {
            return ERR_EXIT;
        }

        std::string line;
        while (std::getline(index, line)) {
            RepoPackage package;

            std::stringstream stream(line);
            std::vector<std::string> fields;
            std::string field;
            
            while (std::getline(stream, field, '|')) {
                if (field.empty()) {
                    continue;
                }

                fields.push_back(field);
            }

            if (fields.size() < 6 || fields[0].empty()) {
                continue;
            }

            package.repository_name = repo.name;
            package.repository_location = repo.location;

            package.name = fields[0];
            package.version = fields[1];
            package.arch = fields[2];
            package.filename = fields[3];
            //package.size = std::stoi(fields[4]);
            package.sha256 = fields[5];

            if (fields.size() > 6) {
                std::stringstream depstream(fields[6]);
                std::string dependency;
                while (std::getline(depstream, dependency, ',')) {
                    if (dependency.empty()) {
                        continue;
                    }

                    package.dependencies.push_back(dependency);
                }
            }

            packages->push_back(package);
        }
    }

    return ERR_OK;
}

int find_package(std::vector<RepoPackage> &available, std::string name, RepoPackage *out) {
    for (auto &package : available) {
        if (package.name == name) {
            *out = package;
            return ERR_OK;
        }
    }

    return ERR_EXIT;
}