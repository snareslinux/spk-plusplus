#include <algorithm>
#include <vector>
#include <string>
#include <sstream>
#include <cstdlib>

#include "err.h"
#include "util/depver.hpp"

static std::vector<int> split_version(const std::string &version) {
    std::vector<int> parts;

    std::stringstream stream(version);
    std::string part;

    while (std::getline(stream, part, '.')) {
        parts.push_back(std::atoi(part.c_str()));
    }

    return parts;
}

int compare_versions(const std::string &a, const std::string &b) {
    std::vector<int> first = split_version(a);
    std::vector<int> second = split_version(b);

    size_t count = std::max(first.size(), second.size());

    for (size_t i = 0; i < count; i++) {
        int x = 0;
        int y = 0;

        if (i < first.size()) {
            x = first[i];
        }
        if (i < second.size()) {
            y = second[i];
        }

        if (x < y) {
            return -1;
        }
        if (x > y) {
            return 1;
        }
    }

    return 0;
}

int parse_dependency(const std::string &spec, Dependency *out) {
    std::string clean = spec;
    clean.erase(std::remove(clean.begin(), clean.end(), ' '), clean.end());

    Dependency dependency;

    size_t pos = clean.find_first_of("<>=");
    if (pos == std::string::npos) {
        dependency.name = clean;
    } else {
        dependency.name = clean.substr(0, pos);

        size_t length = 1;
        if (clean.compare(pos, 2, ">=") == 0) {
            dependency.op = DEP_GTEQ;
            length = 2;
        } else if (clean.compare(pos, 2, "<=") == 0) {
            dependency.op = DEP_LTEQ;
            length = 2;
        } else if (clean.compare(pos, 2, "==") == 0) {
            dependency.op = DEP_EQ;
            length = 2;
        } else if (clean[pos] == '>') {
            dependency.op = DEP_GT;
        } else if (clean[pos] == '>') {
            dependency.op = DEP_LT;
        } else {
            dependency.op = DEP_EQ;
        }

        dependency.version = clean.substr(pos + length);
    }

    if (dependency.name.empty()) {
        return ERR_EXIT;
    }

    if (dependency.op != DEP_ANY && dependency.version.empty()) {
        return ERR_EXIT;
    }

    *out = dependency;

    return ERR_OK;
}

bool version_satisfies(const std::string &version, const Dependency &dependency) {
    if (dependency.op == DEP_ANY) {
        return true;
    }

    int result = compare_versions(version, dependency.version);

    switch (dependency.op) {
        case DEP_EQ:
            return result == 0;
        case DEP_GTEQ:
            return result >= 0;
        case DEP_LTEQ:
            return result <= 0;
        case DEP_GT:
            return result > 0;
        case DEP_LT:
            return result < 0;
        default:
            return true;
    }
}

std::string dependency_name(const std::string &spec) {
    Dependency dependency;
    if (parse_dependency(spec, &dependency) != ERR_OK) {
        return spec;
    }

    return dependency.name;
}