#ifndef UPGRADE_H
#define UPGRADE_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

int upgrade_packages(bool assume_yes);

#ifdef __cplusplus
}
#endif

#endif