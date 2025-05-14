/*
 * Author: Andrew Cox
 * CSCI 347 - Project 2
 */

#include "argparse.h"
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define FALSE (0)
#define TRUE (1)

/*
 * argCount
 * argCount is a helper function that takes in a String and returns the number
 * of "words" in the string assuming that whitespace is the only possible
 * delimeter.
 */
static int argCount(char *line) {

  int count = 0;

  // pointer to the line
  char *c = line;

  // skip whitespace while end of line not reached
  while (*c != '\0') {
    while (isspace(*c)) {
      c++;
    }
    // increment count if end of line not reached
    if (*c != '\0') {
      count++;
    }
    // skip over the word
    while (!isspace(*c) && *c != '\0') {
      c++;
    }
  }

  // if count is 0, return 0
  if (count == 0) {
    return 0;
  }

  return count;
}

/*
 * Argparse takes in a String and returns an array of strings from the input.
 * The arguments in the String are broken up by whitespaces in the string.
 * The count of how many arguments there are is saved in the argcp pointer
 */
char **argparse(char *line, int *argcp) {

  // count arguments and assign to argcp
  int count = argCount(line);
  *argcp = count;

  // allocate memory for the arguments
  char **args = malloc(sizeof(char *) * (count + 1));

  // malloc error check
  if (args == NULL) {
    fprintf(stderr, "argparse: malloc failed\n");
    return NULL;
  }

  // pointer to the line
  char *c = line;
  // index tracker
  int i = 0;
  // while end of line not reached
  while (*c != '\0') {
    // skip leading whitespace
    while (isspace(*c)) {
      c++;
    }
    // if end of line not reached
    if (*c != '\0') {
      // current index = current pointer position
      args[i] = c;
      i++;
    }
    // skip rest of word
    while (!isspace(*c) && *c != '\0') {
      c++;
    }
    // if end of line not reached separate the word
    if (*c != '\0') {
      *c = '\0';
      c++;
    }
  }
  // prevent memory leak
  args[i] = NULL;

  return args;
}
