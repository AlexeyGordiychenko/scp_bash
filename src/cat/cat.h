#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
  int nflag;
  int bflag;
  int eflag;
  int vflag;
  int sflag;
  int tflag;
  int hflag;
} Arguments;

void parse_arguments(int argc, char *argv[], Arguments *flags);
bool process_files(int argc, char *argv[], Arguments flags);
void output_vflag(char ch);
char last_character(FILE *fp);
void print_help(char *name);
FILE *open_file(char *cmd, char *filename);