#include <stdint.h>

#ifndef SPK_H
#define SPK_H

#define SPK_MAGIC 0x53504B01

struct spk_header {
    uint32_t magic;

    uint64_t metadata_offset;
    uint64_t metadata_size;

    uint64_t data_offset;
    uint64_t data_size;
} __attribute__((packed));

struct metadata {
    char name[64];
    char version[16];
    char arch[16];
    char dependencies[1024];
} __attribute__((packed));

#endif