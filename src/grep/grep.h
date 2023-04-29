#include <ctype.h>
#include <getopt.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_LINE_LENGTH 1024

typedef struct {
  bool eflag;
  char *pattern;
  bool iflag;
  bool vflag;
  bool cflag;
  bool lflag;
  bool nflag;
  bool hflag;
  bool sflag;
  bool fflag;
  char *regex_file;
  bool oflag;
} Arguments;

regex_t compile_regex(char *pattern, bool ignore_case);
FILE *open_file(char *cmd, char *filename, bool quiet);
void parse_arguments(int argc, char *argv[], Arguments *flags);
int process_files(int argc, char *argv[], Arguments arguments);
void process_lines(FILE *fp, regex_t *regex, Arguments *flags);
char last_character(FILE *fp);