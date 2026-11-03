#include <filesystem>
#include <string>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <openssl/sha.h>

#include "util/util.hpp"

std::string get_sha256(const std::filesystem::path &path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        return "";
    }

    SHA256_CTX context;
    SHA256_Init(&context);

    char buffer[8192];
    while (file.read(buffer, sizeof(buffer)) || file.gcount()) {
        SHA256_Update(&context, buffer, file.gcount());
    }

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_Final(hash, &context);

    std::ostringstream result;

    for (unsigned char byte : hash) {
        result << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }

    return result.str();
}

std::string get_architecture(void) {
#ifdef __i386__
    return "i386";
#elif defined(__x86_64__)
    return "x86_64";
#elif defined(__arm__)
    return "arm";
#elif defined(__aarch64__)
    return "aarch64";
#elif defined(__riscv)
    return "riscv-V";
#elif defined(__powerpc__)
    return "powerpc";
#else
    return "unknown";
#endif
}