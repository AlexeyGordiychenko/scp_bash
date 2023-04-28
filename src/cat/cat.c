#include "cat.h"

int main(int argc, char *argv[]) {
  int nflag = 0, bflag = 0, eflag = 0, vflag = 0, sflag = 0, tflag = 0;
  set_flags_by_options(argc, argv, &nflag, &bflag, &eflag, &vflag, &sflag,
                       &tflag);
  print_files(argc, argv, nflag, bflag, eflag, vflag, sflag, tflag);
}
void set_flags_by_options(int argc, char *argv[], int *nflag, int *bflag,
                          int *eflag, int *vflag, int *sflag, int *tflag) {
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
        *nflag = 1;
        break;
      case 'e':
        *eflag = *vflag = 1;  // e implies v, same as vE
        break;
      case 'E':
        *eflag = 1;
        break;
      case 'v':
        *vflag = 1;
        break;
      case 'b':
        *bflag = *nflag = 1;  // b implies n
        break;
      case 's':
        *sflag = 1;
        break;
      case 't':
        *tflag = *vflag = 1;  // t implies v, same as vT
        break;
      case 'T':
        *tflag = 1;
        break;
      default:
        fprintf(stderr, "Usage: %s [-nevbst] [file ...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
  }
}

void print_files(int argc, char *argv[], int nflag, int bflag, int eflag,
                 int vflag, int sflag, int tflag) {
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
      if (sflag || bflag) {
        blank_line = strcmp(line, "\n") == 0;
      }
      if (sflag) {
        if (blank_line && prev_blank_line && !no_new_line_at_the_EOF) {
          continue;
        }
        prev_blank_line = blank_line && !no_new_line_at_the_EOF;
      }

      if (!no_new_line_at_the_EOF) {
        if (bflag && !blank_line) {
          printf("%6d\t", line_num);
          line_num++;
        }
        if (nflag && !bflag) {
          printf("%6d\t", line_num);
          line_num++;
        }
      }
      no_new_line_at_the_EOF = 0;

      for (int i = 0; i < read; i++) {
        if (tflag && line[i] == '\t') {
          printf("^I");
        } else if (eflag && line[i] == '\n') {
          printf("$\n");
        } else if (vflag) {
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