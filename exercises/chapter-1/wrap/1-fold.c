#include <stdio.h>
#include <stdlib.h>

/* Write to the specified `out` file all characters from the specified `in`
 * file with the addition of line feeds such that a line feed appears no later
 * than at the specified one-based `break_column` column for each output line.
 * Return zero on success, or a nonzero value if an error occurs.
 * The behavior is undefined unless `break_column >= 2`. 
 */
static int fold(FILE *out, FILE *in, int break_column);

/* Interpret the specified command line `args`, which contain the specified
 * `num_args` number of elements.  `args` should not include the program name.
 * Print usage information to the specified `out` file, if requested.  Print
 * errors to the specified `error` file if an error occurs.  Assign through the
 * specified `break_column` if the corresponding option occurs in `args`.
 * Return a negative value if the program should exit immediately with a
 * success status.
 * Return zero if the program should continue with the fold operation.
 * Return a positive value if an error occurred and the program should exit
 * immediately with a failure status.
 * Note that this function will not modify the command line arguments.
 */
static int parse_command_line(char **args, int num_args, FILE *out, FILE *error, int *break_column);

/* Print program usage information to the specified `out` file.
 */
static void print_usage(FILE *out);

/* Return `1` if the specified null-terminated `left` and `right` contain the
 * same characters.  Return `0` if they do not.
 */
static int strequal(const char *left, const char *right);

int main(int argc, char **argv) {
  int break_column = 80;
  int rc = parse_command_line(argv + 1, argc - 1, stdout, stderr, &break_column);
  if (rc) {
    return rc > 0;
  }
  return fold(stdout, stdin, break_column);
}

int strequal(const char *left, const char *right) {
  while (*left != '\0' && *right != '\0' && *left == *right) {
    ++left;
    ++right;
  }
  return *left == *right;
}

int parse_command_line(char **args, int num_args, FILE *out, FILE *error, int *break_column) {
  char **end = args + num_args;
  for (char **arg = args; arg != end; ++arg) {
    if (strequal(*arg, "-h") || strequal(*arg, "--help")) {
      print_usage(out);
      return -1;
    } else if (strequal(*arg, "-w") || strequal(*arg, "--width")) {
      ++arg;
      if (arg == end) {
        print_usage(error);
        return 1;
      }
      const int width = atoi(*arg);
      if (width < 2) {
        print_usage(error);
        return 2;
      }
      *break_column = width;
    } else {
      print_usage(error);
      return 3;
    }
  }

  return 0;
}

void print_usage(FILE *out) {
  static const char *const usage =
    "usage:\n"
    "    fold\n"
    "        Wrap standard input lines at the 80th column, and print the result to\n"
    "        standard output.\n"
    "    fold --width WIDTH\n"
    "    fold -w WIDTH\n"
    "        Wrap standard input lines at the WIDTH column, and print the result to\n"
    "        standard output.\n"
    "    fold --help\n"
    "    fold -h\n"
    "        Print this message to standard output.\n";
   fputs(usage, out); 
}

int fold(FILE *out, FILE *in, int break_column) {
  // macro that wraps `putc` and its error handling
#define PUTC(CHAR, OUT_FILE) \
  do { \
    if (EOF == putc(CHAR, OUT_FILE) && ferror(OUT_FILE)) { \
      return 1; \
    } \
  } while (0)
  
  int column = 0;
  int ch;  /* the most recently read character */
  for (;;) {
    ch = getc(in);
    if (ch == EOF) {
      return !feof(in);
    }
    ++column;

    if (column == break_column && ch != '\n') {
      PUTC('\n', out);
      PUTC(ch, out);
      column = 1;
    } else if (ch == '\n') {
      PUTC(ch, out);
      column = 0;
    } else {
      PUTC(ch, out);
    }
  }

#undef PUTC
}
