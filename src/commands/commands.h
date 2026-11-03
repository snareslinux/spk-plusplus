#ifndef COMMANDS_H
#define COMMANDS_H

#ifdef __cplusplus
extern "C" {
#endif

int setup(void);
int install_spk(int argc, char **argv);
int remove_spk(int argc, char **argv);
int autoremove(int argc, char **argv);
int write_index(const char *repository);

#ifdef __cplusplus
}
#endif

#endif