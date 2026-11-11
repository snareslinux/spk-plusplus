#ifndef DEPVER_HPP
#define DEPVER_HPP

#include <string>
#include <vector>

enum dep_op {
    DEP_ANY,
    DEP_EQ,
    DEP_GTEQ,
    DEP_LTEQ,
    DEP_GT,
    DEP_LT,
};

struct Dependency {
    std::string name;
    int op = DEP_ANY;
    std::string version;
};

int compare_versions(const std::string &a, const std::string &b);
int parse_dependency(const std::string &spec, Dependency *out);
bool version_satisfies(const std::string &version, const Dependency &dependency);
std::string dependency_name(const std::string &spec);

#endif