#include "err.h"
#include "commands/autoremove/autoremove.h"

int autoremove(int argc, char **argv) {
    perform_autoremove();
    return ERR_OK;
}