#include <ctype.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void set_flags_by_options(int argc, char *argv[], int *nflag, int *bflag,
                          int *eflag, int *vflag, int *sflag, int *tflag);
void print_files(int argc, char *argv[], int nflag, int bflag, int eflag,
                 int vflag, int sflag, int tflag);
void output_vflag(char ch);
char last_character(FILE *fp);