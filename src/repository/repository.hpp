#ifndef REPOSITORY_HPP
#define REPOSITORY_HPP

#include <string>
#include <vector>

struct RepoPackage {
    std::string repository_name;
    std::string repository_location;

    std::string name;
    std::string version;
    std::string arch;
    std::string filename;
    std::string sha256;
    std::vector<std::string> dependencies;
};

struct Repository {
    std::string name;
    std::string location;
};

//int update_repositories(void);
int get_available_packages(std::vector<RepoPackage> *packages);
int find_package(std::vector<RepoPackage> &available, std::string name, RepoPackage *out);

#endif