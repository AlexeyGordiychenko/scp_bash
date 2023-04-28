#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int nflag;
  int bflag;
  int eflag;
  int vflag;
  int sflag;
  int tflag;
  int hflag;
} Flags;

void set_flags_by_options(int argc, char *argv[], Flags *flags);
void print_files(int argc, char *argv[], Flags flags);
void output_vflag(char ch);
char last_character(FILE *fp);
void print_help(char *name);
FILE *open_file(char *cmd, char *filename, char *line);