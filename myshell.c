/* CS 347 -- Mini Shell!
 * Original author Phil Nelson 2000
 *
 * Current author: Andrew Cox
 */
#include "argparse.h"
#include "builtin.h"
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/* PROTOTYPES */

void processline(char *line);
ssize_t getinput(char **line, size_t *size);

/* main
 * This function is the main entry point to the program.  This is essentially
 * the primary read-eval-print loop of the command interpreter.
 */

int main() {
  char *line = NULL;
  // use getinput and processline as appropriate
  while (1) {
    // get input
    size_t size = 0;
    ssize_t len = getinput(&line, &size);
    // check for EOF
    if (len == -1) {
      break;
    }
    // process line
    processline(line);
    // free line
    free(line);
    line = NULL;
  }
  free(line);
  return EXIT_SUCCESS;
}

/* getinput
 * line     A pointer to a char* that points at a buffer of size *size or NULL.
 * size     The size of the buffer *line or 0 if *line is NULL.
 * returns  The length of the string stored in *line.
 *
 * This function prompts the user for input, e.g., %myshell%.  If the input fits
 * in the buffer pointed to by *line, the input is placed in *line.  However, if
 * there is not enough room in *line, *line is freed and a new buffer of
 * adequate space is allocated.  The number of bytes allocated is stored in
 * *size. Hint: There is a standard i/o function that can make getinput easier
 * than it sounds.
 */
ssize_t getinput(char **line, size_t *size) {
  // initialize len
  ssize_t len = 0;
  // allocate buffer if *line is NULL or *size is 0
  if (*line == NULL || *size == 0) {
    *size = 1024;
    *line = malloc(*size);
    // malloc error check
    if (*line == NULL) {
      printf("malloc failed\n");
      return -1;
    }
  }

  // get current working directory and print user prompt
  char cwd[1024];
  if (getcwd(cwd, sizeof(cwd)) != NULL) {
    char *curdir = strrchr(cwd, '/') + 1;
    printf("myshell %s> ", curdir);
  } else {
    perror("getcwd");
    return -1;
  }

  // get input
  if (fgets(*line, *size, stdin) == NULL) {
    return -1;
  }
  // get length of input
  if (*line != NULL) {
    len = strlen(*line);
    // replace newline with null terminator
    if ((*line)[len - 1] == '\n') {
      (*line)[len - 1] = '\0';
      len--;
    }
  }

  return len;
}

/* processline
 * The parameter line is interpreted as a command name.  This function creates a
 * new process that executes that command.
 * Note the three cases of the switch: fork failed, fork succeeded and this is
 * the child, fork succeeded and this is the parent (see fork(2)).
 * processline only forks when the line is not empty, and the line is not trying
 * to run a built in command
 */
void processline(char *line) {
  /*check whether line is empty*/
  if (line == NULL || *line == '\0') {
    return;
  } // whitespace == empty
  if (isspace(*line)) {
    return;
  }

  pid_t cpid;
  int status;
  int argCount;
  char **arguments = argparse(line, &argCount);

  /*check whether arguments are builtin commands
   *if not builtin, fork to execute the command.
   */
  if (builtIn(arguments, argCount)) {
    return;
  } else {
    cpid = fork();
    if (cpid == -1) {
      perror("fork");
      exit(EXIT_FAILURE);
    }
    if (cpid == 0) {
      /* Child */
      execvp(arguments[0], arguments);
      perror("execvp");
      exit(EXIT_FAILURE);
    } else {
      /* Parent */
      waitpid(cpid, &status, 0);
    }
  }
  // free arguments
  free(arguments);
}
