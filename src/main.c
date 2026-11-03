#define VERSION_MAJOR 1
#define VERSION_MINOR 0
#define VERSION_PATCH 0

#ifdef __i386__
    #define ARCH "i386"
#elif defined(__x86_64__)
    #define ARCH "x86_64"
#elif defined(__arm__)
    #define ARCH "arm"
#elif defined(__aarch64__)
    #define ARCH "aarch64"
#elif defined(__riscv)
    #define ARCH "RISC-V"
#elif defined(__powerpc__)
    #define ARCH "PowerPC"
#else
    #define ARCH "Unknown"
#endif



#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <stdbool.h>

#include "err.h"
#include "ui/ui.h"
#include "commands/commands.h"
#include "package/package.h"
#include "repository/repository.h"

void print_help(void) {
    putchar('\n');
    printf("Usage: spk COMMAND [options]\n");
    putchar('\n');
    printf("List of Main Commands:\n");
    #ifdef CONFIG_DEVELOPMENT_TOOLS
    printf("    install <package>             - Install package(s)\n");
    printf("    remove <package>              - Remove installed package(s)\n");
    printf("    autoremove                    - Remove all unneeded packages originally installed as dependencies.\n");
    printf("    update                        - Update all repositories\n");
    printf("    upgrade                       - Upgrade installed packages\n");
    printf("    search <query>                - Search for available package\n");
    printf("    info <package>                - Show package information\n");
    printf("    list                          - List installed packages\n");
    printf("    inspect <package>             - List files installed by package\n");
    printf("    setup                         - Setup spk for this system\n");
    printf("    extract <package> <root path> - Extracts a spk file into root\n");
    printf("    create <directory>            - Creates a spk file from the directories content\n");
    printf("    write-index <directory>       - Creates a index file for the repository in the directory\n");
    #else
    printf("    install <package> - Install package(s)\n");
    printf("    remove <package>  - Remove installed package(s)\n");
    printf("    autoremove        - Remove all unneeded packages originally installed as dependencies.\n");
    printf("    update            - Update all repositories\n");
    printf("    upgrade           - Upgrade installed packages\n");
    printf("    search <query>    - Search for available package\n");
    printf("    info <package>    - Show package information\n");
    printf("    list              - List installed packages\n");
    printf("    inspect <package> - List files installed by package\n");
    printf("    setup             - Setup spk for this system\n");
    #endif
    putchar('\n');
    printf("General spk options:\n");
    printf("    -v, --version   Print spk version\n");
    printf("    -h, --help      Print this help message\n");
    printf("    -y, --yes       Automatically answer yes for all questions\n");
}

void print_version(void) {
    printf("spk version %d.%d.%d (%s)\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, ARCH);
}

bool check_root(void) {
    if (getuid()) {
        return false;
    } else {
        return true;
    }
}

enum commands {
    CMD_INVALID,
    CMD_INSTALL,
    CMD_REMOVE,
    CMD_AUTOREMOVE,
    CMD_UPDATE,
    CMD_UPGRADE,
    CMD_SEARCH,
    CMD_INFO,
    CMD_LIST,
    CMD_INSPECT,
    CMD_SETUP,
#ifdef CONFIG_DEVELOPMENT_TOOLS
    CMD_CREATE,
    CMD_EXTRACT,
    CMD_WRITE_INDEX,
#endif
};

static struct option long_options[] = {
    {"help", no_argument, NULL, 'h'},
    {"version", no_argument, NULL, 'v'},
    {NULL, 0, NULL, 0}
};

int main(int argc, char **argv) {
    if (!argv[1]) {
        print_help();
        return 0;
    }

    int opt;
    while ((opt = getopt_long(argc, argv, "+hv", long_options, NULL)) != -1) {
        switch (opt) {
            case 'h':
                print_help();
                return ERR_OK;

            case 'v':
                print_version();
                return ERR_OK;
        }
    }

    ui_init();
    
    enum commands command = CMD_INVALID;
    if (!strcmp(argv[1], "install"))
        command = CMD_INSTALL;
    if (!strcmp(argv[1], "remove"))
        command = CMD_REMOVE;
    if (!strcmp(argv[1], "autoremove"))
        command = CMD_AUTOREMOVE;
    if (!strcmp(argv[1], "update"))
        command = CMD_UPDATE;
    if (!strcmp(argv[1], "upgrade"))
        command = CMD_UPGRADE;
    if (!strcmp(argv[1], "search"))
        command = CMD_SEARCH;
    if (!strcmp(argv[1], "info"))
        command = CMD_INFO;
    if (!strcmp(argv[1], "list"))
        command = CMD_LIST;
    if (!strcmp(argv[1], "inspect"))
        command = CMD_INSPECT;
    if (!strcmp(argv[1], "setup"))
        command = CMD_SETUP;

#ifdef CONFIG_DEVELOPMENT_TOOLS
    if (!strcmp(argv[1], "create"))
        command = CMD_CREATE;
    if (!strcmp(argv[1], "extract"))
        command = CMD_EXTRACT;
    if (!strcmp(argv[1], "write-index"))
        command = CMD_WRITE_INDEX;
#endif

    switch (command) {
        case CMD_INVALID:
            print_help();
            return ERR_EXIT;
        
        case CMD_INSTALL: {
            install_spk(argc - 1, argv + 1);
            break;
        }

        case CMD_REMOVE: {
            remove_spk(argc - 1, argv + 1);
            break;
        }

        case CMD_AUTOREMOVE: {
            autoremove(argc - 1, argv + 1);
            break;
        }

        case CMD_UPDATE: {
            update_repositories();
            break;
        }

        case CMD_UPGRADE: {
            break;
        }
        
        case CMD_SEARCH: {
            break;
        }

        case CMD_LIST: {
            break;
        }

        case CMD_INSPECT: {
            break;
        }

        case CMD_SETUP: {
            setup();
            break;
        }

#ifdef CONFIG_DEVELOPMENT_TOOLS
        case CMD_CREATE: {
            if (argv[2]) {
                return create_spk(argv[2]);
            }

            return ERR_EXIT;
        }

        case CMD_EXTRACT: {
            if (argv[2] && argv[3]) {
                extract_spk(argv[2], argv[3], NULL);
            }

            return ERR_EXIT;
        }

        case CMD_WRITE_INDEX: {
            if (argv[2]) {
                write_index(argv[2]);
            }

            return ERR_EXIT;
        }
#endif

        default:
            print_help();
            return ERR_EXIT;
    }

    return 0;
}