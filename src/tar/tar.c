#define _GNU_SOURCE

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <sys/stat.h>

#include <archive.h>
#include <archive_entry.h>

#include "tar.h"

int write_archive(const char *outname, const char *dirpath) {
    struct archive *a_write;
    struct archive *a_read;
    struct archive_entry *entry = NULL;

    int res = 0;
    
    a_write = archive_write_new();
    if (!a_write) {
        return 1;
    }
    archive_write_add_filter_zstd(a_write);
    archive_write_set_format_ustar(a_write);

    res = archive_write_open_filename(a_write, outname);
    if (res != ARCHIVE_OK) {
        archive_write_free(a_write);
        return 1;
    }

    a_read = archive_read_disk_new();
    if (!a_read) {
        archive_write_close(a_write);
        archive_write_free(a_write);
        return 1;
    }

    archive_read_disk_set_standard_lookup(a_read);

    res = archive_read_disk_open(a_read, dirpath);
    if (res != ARCHIVE_OK) {
        archive_read_free(a_read);
        archive_write_close(a_write);
        archive_write_free(a_write);
        return 1;
    }

    while (1) {
        entry = archive_entry_new();
        res = archive_read_next_header2(a_read, entry);
        if (res == ARCHIVE_EOF) {
            archive_entry_free(entry);
            break;
        }

        const char *current_path = archive_entry_pathname(entry);

        if (!strcmp(current_path, dirpath)) {
            archive_entry_free(entry);
            archive_read_disk_descend(a_read);
            continue;
        }

        const char *rel_path = current_path + strlen(dirpath);
        while (*rel_path == '/') {
            rel_path++;
        }
        archive_entry_set_pathname(entry, rel_path);

        archive_read_disk_descend(a_read);
        
        archive_write_header(a_write, entry);

        if (archive_entry_size(entry) > 0) {
            const void *buffer;
            size_t bytes_read;
            int64_t offset;
            while (archive_read_data_block(a_read, &buffer, &bytes_read, &offset) == ARCHIVE_OK) {
                archive_write_data(a_write, buffer, bytes_read);
            }
        }

        archive_entry_free(entry);
    }

    archive_read_close(a_read);
    archive_read_free(a_read);

    archive_write_close(a_write);
    archive_write_free(a_write);

    return 0;
}

int extract_archive(const void *archive_mem, size_t mem_len, const char *destpath, FILE *extract_log) {
    int output = 0;

    char outpath[PATH_MAX];
    snprintf(outpath, sizeof(outpath), "%s", destpath);
    size_t len = strlen(outpath);
    if (len > 0 && outpath[len - 1] == '/') {
        outpath[len - 1] = '\0';
    }

    struct archive *a_read;
    struct archive *a_write;
    struct archive_entry *entry;

    a_read = archive_read_new();
    if (!a_read) {
        return 1;
    }

    archive_read_support_filter_zstd(a_read);
    archive_read_support_format_tar(a_read);

    a_write = archive_write_disk_new();
    if (!a_write) {
        archive_read_free(a_read);
        return 1;
    }

    archive_write_disk_set_options(a_write,

        ARCHIVE_EXTRACT_SECURE_NODOTDOT
        | ARCHIVE_EXTRACT_SECURE_SYMLINKS
        | ARCHIVE_EXTRACT_TIME
        | ARCHIVE_EXTRACT_PERM
    );

    if (archive_read_open_memory(a_read, archive_mem, mem_len) != ARCHIVE_OK) {
        archive_read_free(a_read);
        archive_write_free(a_write);
        return 1;
    }

    int res;

    while ((res = archive_read_next_header(a_read, &entry)) == ARCHIVE_OK) {

        char new_path[PATH_MAX * 2];
        snprintf(new_path, sizeof(new_path), "%s/%s", outpath, archive_entry_pathname(entry));

        struct stat st;
        if (extract_log && lstat(new_path, &st) == -1) {
            if (errno == ENOENT) {
                char type = 0;
                
                if (archive_entry_filetype(entry) == AE_IFDIR) type = 'd';
                else if (archive_entry_filetype(entry) == AE_IFLNK) type = 'l';
                else type = 'f';

                fprintf(extract_log, "%c:%s\n", type, new_path);
                fflush(extract_log);
            } else {
                output = 1;
                goto out;
            }
        }

        archive_entry_set_pathname(entry, new_path);

        if (archive_write_header(a_write, entry) != ARCHIVE_OK) {
            output = 1;
            goto out;
        }

        if (archive_entry_size(entry) > 0) {
            const void *buffer;
            size_t bytes_read;
            int64_t offset;

            int data_res;

            while ((data_res = archive_read_data_block(a_read, &buffer, &bytes_read, &offset)) == ARCHIVE_OK) {
                if (archive_write_data_block(a_write, buffer, bytes_read, offset) != ARCHIVE_OK) {
                    output = 1;
                    goto out;
                }
            }

            if (data_res != ARCHIVE_EOF) {
                output = 1;
                goto out;
            }
        }

        if (archive_write_finish_entry(a_write) != ARCHIVE_OK) {
            output = 1;
            goto out;
        }
    }

    if (res != ARCHIVE_EOF) {
        output = 1;
        goto out;
    }

out:
    archive_read_close(a_read);
    archive_read_free(a_read);
    archive_write_close(a_write);
    archive_write_free(a_write);

    return output;
}