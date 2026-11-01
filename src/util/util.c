#define _GNU_SOURCE

#include "util/util.h"

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <errno.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <memory.h>
#include <curl/curl.h>

size_t copy_file(FILE *src, FILE *dest, uint64_t total) {
    char buffer[64 * 1024];

    uint64_t remaining = total;
    size_t total_read = 0;

    while (remaining > 0) {
        size_t amount = remaining < sizeof(buffer) ? remaining : sizeof(buffer);
        size_t bytes = fread(buffer, 1, amount, src);

        if (!bytes) {
            break;
        }

        if (fwrite(buffer, 1, bytes, dest) != bytes) {
            break;
        }

        remaining -= bytes;
        total_read += bytes;
    }

    return total_read;
}

int mkdir_p(const char *dir, mode_t mode) {
    char tmp[PATH_MAX] = "";
    char *p = 0;
    size_t len = 0;

    snprintf(tmp, sizeof(tmp), "%s", dir);
    len = strlen(tmp);

    if (len > 0 && tmp[len - 1] == '/') {
        tmp[len - 1] = '\0';
    }
    
    for (p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            if (mkdir(tmp, mode) < 0 && errno != EEXIST) {
                return 1;
            }
            *p = '/';
        }
    }

    if (mkdir(tmp, mode) < 0 && errno != EEXIST) {
        return 1;
    }

    return 0;
}

void clean_string(char *buffer) {
    int i = 0;
    while (buffer[i]) {
        if (buffer[i] == '\n') {
            memmove(&buffer[i], &buffer[i + 1], strlen(&buffer[i + 1]) + 1);
            i--;
        }
        i++;
    }
}

int download_to_file(const char *url, FILE *file) {
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);

        res = curl_easy_perform(curl);

        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            return 1;
        }
    }

    return 0;
}