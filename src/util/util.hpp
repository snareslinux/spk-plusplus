#ifndef UTIL_HPP
#define UTIL_HPP

#include <string>
#include <filesystem>

std::string get_sha256(const std::filesystem::path &path);
std::string get_architecture(void);

#endif