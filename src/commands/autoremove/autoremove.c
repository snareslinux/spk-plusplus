#include "err.h"
#include "autoremove.h"

int autoremove(int argc, char **argv) {
    perform_autoremove();
    return ERR_OK;
}