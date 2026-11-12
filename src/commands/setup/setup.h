#ifndef SETUP_H
#define SETUP_H

#ifdef __cplusplus
extern "C" {
#endif

int perform_setup(const char *root, const char *repo, bool assume_yes);

#ifdef __cplusplus
}
#endif

#endif