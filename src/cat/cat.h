#include <ctype.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
  int nflag;
  int bflag;
  int eflag;
  int vflag;
  int sflag;
  int tflag;
} Flags;

void set_flags_by_options(int argc, char *argv[], Flags *flags);
void print_files(int argc, char *argv[], Flags flags);
void output_vflag(char ch);
char last_character(FILE *fp);