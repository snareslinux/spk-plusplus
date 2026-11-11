#ifndef COMMANDS_H
#define COMMANDS_H

#ifdef __cplusplus
extern "C" {
#endif

int setup(int argc, char **argv);
int install_spk(int argc, char **argv);
int remove_spk(int argc, char **argv);
int autoremove(int argc, char **argv);
int info(int argc, char **argv);
int list(int argc, char **argv);
int search(int argc, char **argv);
int inspect(const char *package);
int upgrade_spk(int argc, char **argv);

int write_index(int argc, char **argv);
int write_indexfile(const char *repository, const char *out);
int create_spk(int argc, char **argv);
int extract_spk(int argc, char **argv);

#ifdef __cplusplus
}
#endif

#endif