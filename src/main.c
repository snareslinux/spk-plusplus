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
#endif



#include <string.h>
#include <unistd.h>

#include "ui/ui.h"
#include "commands/commands.h"
#include "package/package.h"

void print_help(void) {
    putchar('\n');
    printf("Usage: spk COMMAND [options]\n");
    putchar('\n');
    printf("List of Main Commands:\n");
    printf("    install <package> - Install package(s)\n");
    printf("    remove <package>  - Remove installed package(s)\n");
    printf("    update            - Update all repositories\n");
    printf("    upgrade           - Upgrade installed packages\n");
    printf("    search <query>    - Search for available package\n");
    printf("    info <package>    - Show package information\n");
    printf("    list              - List installed packages\n");
    printf("    inspect <package> - List files installed by package\n");
    printf("    setup             - Setup spk for this system\n");
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
    CMD_UPDATE,
    CMD_UPGRADE,
    CMD_SEARCH,
    CMD_INFO,
    CMD_LIST,
    CMD_INSPECT,
    CMD_SETUP,
    CMD_CREATE,
    CMD_EXTRACT,
};

int main(int argc, char **argv) {
    if (!argv[1]) {
        print_version();
        return 0;
    }

    ui_init();
    
    enum commands command = CMD_INVALID;
    if (!strcmp(argv[1], "install"))
        command = CMD_INSTALL;
    if (!strcmp(argv[1], "remove"))
        command = CMD_REMOVE;
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
    if (!strcmp(argv[1], "create"))
        command = CMD_CREATE;
    if (!strcmp(argv[1], "extract"))
        command = CMD_EXTRACT;

    switch (command) {
        case CMD_INVALID:
            print_help();
            break;
        
        case CMD_INSTALL: {
            install_spk(argc - 1, argv + 1);
            break;
        }

        case CMD_REMOVE: {
            remove_spk(argc - 1, argv + 1);
            break;
        }

        case CMD_UPDATE: {
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

        case CMD_CREATE: {
            if (argv[2])
                create_spk(argv[2]);
            break;
        }

        case CMD_EXTRACT: {
            if (argv[2] && argv[3]) {
                int res = extract_spk(argv[2], argv[3], NULL);
            }
            
            break;
        }

        default:
            print_help();
            break;
    }

    return 0;
}