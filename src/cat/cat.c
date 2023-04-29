#include "cat.h"

int main(int argc, char *argv[]) {
  Arguments arguments = {0, 0, 0, 0, 0, 0, 0};
  parse_arguments(argc, argv, &arguments);
  if (arguments.hflag) {
    print_help(argv[0]);
  } else {
    return process_files(argc, argv, arguments);
  }
}

void parse_arguments(int argc, char *argv[], Arguments *arguments) {
  static struct option long_options[] = {
      {"number-nonblank", no_argument, 0, 'b'},
      {"number", no_argument, 0, 'n'},
      {"squeeze-blank", no_argument, 0, 's'},
      {"help", no_argument, 0, 'h'},
      {0, 0, 0, 0}};

  int opt, long_index = 0;
  while ((opt = getopt_long(argc, argv, "benstvTEh", long_options,
                            &long_index)) != -1) {
    switch (opt) {
      case 'n':
        arguments->nflag = 1;
        break;
      case 'e':
        arguments->eflag = arguments->vflag = 1;  // e implies v, same as vE
        break;
      case 'E':
        arguments->eflag = 1;
        break;
      case 'v':
        arguments->vflag = 1;
        break;
      case 'b':
        arguments->bflag = arguments->nflag = 1;  // b implies n
        break;
      case 's':
        arguments->sflag = 1;
        break;
      case 't':
        arguments->tflag = arguments->vflag = 1;  // t implies v, same as vT
        break;
      case 'T':
        arguments->tflag = 1;
        break;
      case 'h':
        arguments->hflag = 1;
        break;
      default:
        fprintf(stderr, "Try '%s --help' for more information.\n", argv[0]);
        exit(EXIT_FAILURE);
    }
  }
}

FILE *open_file(char *cmd, char *filename) {
  FILE *fp = NULL;
  fp = fopen(filename, "r");
  if (fp == NULL) {
    fprintf(stderr, "%s: %s: No such file or directory\n", cmd, filename);
  }
  return fp;
}

bool process_files(int argc, char *argv[], Arguments arguments) {
  char *line = NULL;
  size_t len = 0, line_num = 1;
  ssize_t read;
  bool prev_blank_line = 0, blank_line = 0, no_new_line_at_the_EOF = 0, res = 0;

  for (int i = optind; i < argc; i++) {
    FILE *fp = open_file(argv[0], argv[i]);
    if (fp == NULL) {
      res = 1;
      continue;
    }

    while ((read = getline(&line, &len, fp)) != -1) {
      if (arguments.sflag || arguments.bflag) {
        blank_line = line[0] == '\n';
      }
      if (arguments.sflag) {
        if (blank_line && prev_blank_line && !no_new_line_at_the_EOF) {
          continue;
        }
        prev_blank_line = blank_line && !no_new_line_at_the_EOF;
      }

      if (!no_new_line_at_the_EOF) {
        if (arguments.bflag && !blank_line) {
          printf("%6ld\t", line_num);
          line_num++;
        }
        if (arguments.nflag && !arguments.bflag) {
          printf("%6ld\t", line_num);
          line_num++;
        }
      }
      no_new_line_at_the_EOF = 0;

      for (int i = 0; i < read; i++) {
        if (arguments.tflag && line[i] == '\t') {
          printf("^I");
        } else if (arguments.eflag && line[i] == '\n') {
          printf("$\n");
        } else if (arguments.vflag) {
          output_vflag(line[i]);
        } else {
          printf("%c", line[i]);
        }
      }
    }

    /* reading last char of the file coz valgrind doesn't
    like if I try to get it from the 'line' */
    no_new_line_at_the_EOF = last_character(fp) != '\n';
    fclose(fp);
  }
  free(line);
  return res;
}

void output_vflag(char ch) {
  if (ch < -128 + 32) {
    printf("M-^%c", ch + 128 + 64);
  } else if (ch < 0) {
    printf("M-%c", ch + 128);
  } else if (ch < 32 && ch != 9 && ch != 10) {
    printf("^%c", ch + 64);
  } else if (ch < 127) {
    printf("%c", ch);
  } else {
    printf("^?");
  }
}

char last_character(FILE *fp) {
  fseek(fp, -1, SEEK_END);
  char ch;
  fread(&ch, 1, 1, fp);
  return ch;
}

void print_help(char *name) {
  printf(
      "Usage: %s [OPTION]... [FILE]...\n"
      "Concatenate FILE(s) to standard output.\n\n"
      "  -b, --number-nonblank    number nonempty output lines, overrides -n\n"
      "  -e                       equivalent to -vE\n"
      "  -E, --show-ends          display $ at end of each line\n"
      "  -n, --number             number all output lines\n"
      "  -s, --squeeze-blank      suppress repeated empty output lines\n"
      "  -t                       equivalent to -vT\n"
      "  -T, --show-tabs          display TAB characters as ^I\n"
      "  -v, --show-nonprinting   use ^ and M- notation, except for LFD and "
      "TAB\n\n"
      "  -h  --help               display this help and exit\n",
      name);
}