#include "cat.h"

int main(int argc, char *argv[]) {
  Flags flags = {0, 0, 0, 0, 0, 0};
  // int nflag = 0, bflag = 0, eflag = 0, vflag = 0, sflag = 0, tflag = 0;
  set_flags_by_options(argc, argv, &flags);
  print_files(argc, argv, flags);
}
void set_flags_by_options(int argc, char *argv[], Flags *flags) {
  static struct option long_options[] = {
      {"number-nonblank", no_argument, 0, 'b'},
      {"number", no_argument, 0, 'n'},
      {"squeeze-blank", no_argument, 0, 's'},
      {0, 0, 0, 0}};

  int opt, long_index = 0;
  while ((opt = getopt_long(argc, argv, "benstvTE", long_options,
                            &long_index)) != -1) {
    switch (opt) {
      case 'n':
        flags->nflag = 1;
        break;
      case 'e':
        flags->eflag = flags->vflag = 1;  // e implies v, same as vE
        break;
      case 'E':
        flags->eflag = 1;
        break;
      case 'v':
        flags->vflag = 1;
        break;
      case 'b':
        flags->bflag = flags->nflag = 1;  // b implies n
        break;
      case 's':
        flags->sflag = 1;
        break;
      case 't':
        flags->tflag = flags->vflag = 1;  // t implies v, same as vT
        break;
      case 'T':
        flags->tflag = 1;
        break;
      default:
        fprintf(stderr, "Usage: %s [-nevbst] [file ...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
  }
}

void print_files(int argc, char *argv[], Flags flags) {
  FILE *fp;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  int line_num = 1;
  int prev_blank_line = 0;
  int blank_line = 0;
  int no_new_line_at_the_EOF = 0;

  for (int i = optind; i < argc; i++) {
    fp = fopen(argv[i], "r");
    if (fp == NULL) {
      fprintf(stderr, "s21_cat: %s: No such file or directory\n", argv[i]);
      if (line != NULL) {
        free(line);
      }
      exit(EXIT_FAILURE);
    }

    while ((read = getline(&line, &len, fp)) != -1) {
      if (flags.sflag || flags.bflag) {
        blank_line = strcmp(line, "\n") == 0;
      }
      if (flags.sflag) {
        if (blank_line && prev_blank_line && !no_new_line_at_the_EOF) {
          continue;
        }
        prev_blank_line = blank_line && !no_new_line_at_the_EOF;
      }

      if (!no_new_line_at_the_EOF) {
        if (flags.bflag && !blank_line) {
          printf("%6d\t", line_num);
          line_num++;
        }
        if (flags.nflag && !flags.bflag) {
          printf("%6d\t", line_num);
          line_num++;
        }
      }
      no_new_line_at_the_EOF = 0;

      for (int i = 0; i < read; i++) {
        if (flags.tflag && line[i] == '\t') {
          printf("^I");
        } else if (flags.eflag && line[i] == '\n') {
          printf("$\n");
        } else if (flags.vflag) {
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