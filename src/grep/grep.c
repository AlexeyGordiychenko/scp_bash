#include "grep.h"

int main(int argc, char *argv[]) {
  Arguments arguments = {0, NULL, 0, 0, 0, 0, 0, 0, 0, 0, NULL, 0};

  // Parse command-line arguments
  parse_arguments(argc, argv, &arguments);
  return process_files(argc, argv, arguments);
}

regex_t compile_regex(char *pattern, bool ignore_case) {
  regex_t regex;
  int reti = regcomp(&regex, pattern, ignore_case ? REG_ICASE : 0);
  if (reti != 0) {
    char error_message[MAX_LINE_LENGTH];
    regerror(reti, &regex, error_message, MAX_LINE_LENGTH);
    fprintf(stderr, "Failed to compile regex: %s\n", error_message);
    exit(EXIT_FAILURE);
  }
  return regex;
}

void parse_arguments(int argc, char *argv[], Arguments *arguments) {
  int opt;
  while ((opt = getopt(argc, argv, "e:ivclnhsf:o")) != -1) {
    switch (opt) {
      case 'e':
        arguments->eflag = 1;
        arguments->pattern = optarg;
        break;
      case 'i':
        arguments->iflag = 1;
        break;
      case 'v':
        arguments->vflag = 1;
        break;
      case 'c':
        arguments->cflag = 1;
        break;
      case 'l':
        arguments->lflag = 1;
        break;
      case 'n':
        arguments->nflag = 1;
        break;
      case 'h':
        arguments->hflag = 1;
        break;
      case 's':
        arguments->sflag = 1;
        break;
      case 'f':
        arguments->fflag = 1;
        arguments->regex_file = optarg;
        break;
      case 'o':
        arguments->oflag = 1;
        break;
      default:
        fprintf(stderr,
                "Usage: %s [-e pattern] [-i] [-v] [-c] [-l] [-n] [-h] [-s] "
                "[-w] [-f file] [-o]\n",
                argv[0]);
        exit(EXIT_FAILURE);
    }
  }

  // Check for the presence of the pattern and filename
  if (arguments->pattern == NULL && optind < argc) {
    arguments->pattern = argv[optind];
    optind++;
  } else {
    fprintf(stderr, "Pattern not specified.\n");
    exit(EXIT_FAILURE);
  }
}

FILE *open_file(char *cmd, char *filename, bool quiet) {
  FILE *fp = NULL;
  fp = fopen(filename, "r");
  if (fp == NULL && !quiet) {
    fprintf(stderr, "%s: %s: No such file or directory\n", cmd, filename);
    // if (line != NULL) {
    //   free(line);
    // }
    // exit(EXIT_FAILURE);
  }
  return fp;
}

int process_files(int argc, char *argv[], Arguments arguments) {
  regex_t regex = compile_regex(arguments.pattern, arguments.iflag);

  char *line = NULL;
  size_t len = 0, match_found = 0, num_matches = 0;
  int res = 0;
  ssize_t read;

  for (int i = optind; i < argc; i++) {
    FILE *fp = open_file(argv[0], argv[i], arguments.sflag);
    if (fp == NULL) {
      res = 2;
      continue;
    }
    size_t line_num = 0;
    while ((read = getline(&line, &len, fp)) != -1) {
      line_num++;
      int reti = regexec(&regex, line, 0, NULL, 0);
      // if (reti == 0) {
      //   match_found = 1;
      // } else if (reti == REG_NOMATCH) {
      //   match_found = 0;
      // } else {
      //   char error_message[MAX_LINE_LENGTH];
      //   regerror(reti, &regex, error_message, MAX_LINE_LENGTH);
      //   fprintf(stderr, "Failed to match regex: %s\n", error_message);
      //   exit(EXIT_FAILURE);
      // }
      match_found = (reti == 0) ? 1 : 0;
      // if (arguments.vflag) {
      //   match_found = !match_found;
      // }

      // if (match_found) {
      if ((!arguments.vflag && match_found) ||
          (arguments.vflag && !match_found)) {
        num_matches++;

        if (!arguments.cflag) {
          if (arguments.lflag) {
            printf("%s\n", argv[i]);
            break;
          } else {
            if (!arguments.hflag && optind != argc - 1) {
              printf("%s:", argv[i]);
            }
            if (arguments.nflag) {
              printf("%ld:", line_num);
            }
            printf("%s", line);
            if (line[read - 1] != '\n') {
              printf("\n");
            }
          }
        }
      }
    }
    fclose(fp);
  }
  free(line);
  regfree(&regex);

  if (arguments.cflag) {
    printf("%ld\n", num_matches);
  }

  if (res != 2) {
    res = num_matches == 0;
  }
  return res;
}

char last_character(FILE *fp) {
  fseek(fp, -1, SEEK_END);
  char ch;
  fread(&ch, 1, 1, fp);
  return ch;
}
