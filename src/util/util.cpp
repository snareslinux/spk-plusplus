#include <filesystem>
#include <string>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <openssl/evp.h>

#include "util/util.hpp"

std::string get_sha256(const std::filesystem::path &path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        return "";
    }

    EVP_MD_CTX *context = EVP_MD_CTX_new();
    if (!context) {
        return "";
    }

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_len = 0;

    if (EVP_DigestInit_ex(context, EVP_sha256(), nullptr) != 1) {
        EVP_MD_CTX_free(context);
        return "";
    }

    char buffer[8192];

    while (file.read(buffer, sizeof(buffer)) || file.gcount()) {
        if (EVP_DigestUpdate(context, buffer, file.gcount()) != 1) {
            EVP_MD_CTX_free(context);
            return "";
        }
        
    }

    if (file.bad()) {
        EVP_MD_CTX_free(context);
        return "";
    }
    
    if (EVP_DigestFinal_ex(context, hash, &hash_len) != 1) {
        EVP_MD_CTX_free(context);
        return "";
    }

    EVP_MD_CTX_free(context);

    std::ostringstream result;
    for (unsigned int i = 0; i < hash_len; i++) {
        result << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
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