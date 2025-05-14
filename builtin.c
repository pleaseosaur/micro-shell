/*
 * Author: Andrew Cox
 * CSCI 347 - Project 2
 */

#include "builtin.h"
#include <assert.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

// Prototypes
static void exitProgram(char **args, int argcp);
static void cd(char **args, int argpcp);
static void pwd(char **args, int argcp);
static void ls(char **args, int argcp);
static void cp(char **args, int argcp);
static void env(char **args, int argcp);
static long long calcTotalBlocks(DIR *d);

/* builtIn
 * builtIn checks each built in command the given command, if the given
 * command matches one of the built in commands, that command is called and
 * builtin returns 1. If none of the built in commands match the wanted command,
 * builtin returns 0;
 */
int builtIn(char **args, int argcp) {
  // switch on first character
  switch (args[0][0]) {
  // check for exit or env
  case 'e':
    if (strcmp(args[0], "exit") == 0) {
      exitProgram(args, argcp);
      return 1;
    } else if (strcmp(args[0], "env") == 0) {
      env(args, argcp);
      return 1;
    }
    break;

    // check for cd or cp
  case 'c':
    if (strcmp(args[0], "cd") == 0) {
      cd(args, argcp);
      return 1;
    } else if (strcmp(args[0], "cp") == 0) {
      cp(args, argcp);
      return 1;
    }
    break;

    // check for pwd
  case 'p':
    if (strcmp(args[0], "pwd") == 0) {
      pwd(args, argcp);
      return 1;
    }
    break;

    // check for ls
  case 'l':
    if (strcmp(args[0], "ls") == 0) {
      ls(args, argcp);
      return 1;
    }
    break;
  default:
    return 0;
  }
  return 0;
}

/*
 * exitProgram
 * exitProgram takes in the arguments and the number of arguments and exits the
 * program with the given exit code. If no exit code is given, the program exits
 * with a code of 0.
 */
static void exitProgram(char **args, int argcp) {
  switch (argcp) {
  // exit without argument
  case 1:
    exit(0);
    break;
    // exit with argument
  case 2:
    if (atoi(args[1]) > 255) { // memory limit
      fprintf(stderr, "exit: %s: maximum allowable argument is 255\n", args[1]);
      break;
    }
    exit(atoi(args[1]));
    break;
    // error if too many arguments
  default:
    fprintf(stderr, "exit: too many arguments\n");
    break;
  }
}

/*
 * pwd
 * pwd prints the current working directory to the screen.
 */
static void pwd(char **args, int argpc) {

  char cwd[1024];
  if (getcwd(cwd, sizeof(cwd)) != NULL) {
    printf("%s\n", cwd);
  } else {
    perror("getcwd");
  }
}

/*
 * cd
 * cd changes the current working directory to the given directory. If no
 * directory is given, cd changes the current working directory to the home
 * directory.
 */
static void cd(char **args, int argcp) {

  char *dir;

  // if there are more than 2 arguments, print an error
  if (argcp > 2) {
    fprintf(stderr, "cd: too many arguments\n");
    return;
  }

  // if there is no second argument, set dir to the home directory
  if (argcp == 1) {
    dir = getenv("HOME");
  }

  // if there is a second argument, set dir to that
  if (argcp == 2) {
    dir = args[1];
  }

  // if the directory does not exist, print an error
  if (chdir(dir) != 0) {
    perror("cd");
  }
}

/*
 * calcTotalBlocks
 * calcTotalBlocks calculates the total number of blocks in the current
 * directory.
 *
 * NOTE: This function is not a required part of the assignment, but it is
 * how a normal terminal operates so I wanted to give it a shot.
 */
static long long calcTotalBlocks(DIR *d) {
  // calculate total number of blocks
  long long totalBlocks = 0;
  struct dirent *dir;
  struct stat fileStat;
  while ((dir = readdir(d)) != NULL) {
    // skip . and .. directories
    if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) {
      continue;
    }
    if (stat(dir->d_name, &fileStat) == 0) {
      totalBlocks += fileStat.st_blocks;
    }
  }

  // reset directory to the beginning
  rewinddir(d);

  return totalBlocks / 2;
}

/*
 * ls
 * ls lists the files in the current directory. If the -l flag is given, ls
 * prints the long format, which includes the file permissions, the number of
 * links, the owner, the group, the size, the date, and the name of the file.
 */
static void ls(char **args, int argcp) {

  // handle -l flag
  if (argcp == 2 && strcmp(args[1], "-l") == 0) {
    // file information
    struct stat fileStat;

    // directory pointers
    struct dirent *dir;
    DIR *d;

    // open the current directory
    d = opendir(".");

    // print total number of blocks
    printf("total %lld\n", calcTotalBlocks(d));

    if (d) {

      // loop through the directory
      while ((dir = readdir(d)) != NULL) {
        // get file information
        if (stat(dir->d_name, &fileStat) == -1) {
          perror("ls");
          return;
        } // if directory name is . or .., skip
        if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) {
          continue;
        }

        /* print file information */

        // file type
        printf((S_ISDIR(fileStat.st_mode)) ? "d" : "-");

        // permissions
        printf((fileStat.st_mode & S_IRUSR) ? "r" : "-");
        printf((fileStat.st_mode & S_IXUSR) ? "x" : "-");
        printf((fileStat.st_mode & S_IRGRP) ? "r" : "-");
        printf((fileStat.st_mode & S_IWGRP) ? "w" : "-");
        printf((fileStat.st_mode & S_IXGRP) ? "x" : "-");
        printf((fileStat.st_mode & S_IROTH) ? "r" : "-");
        printf((fileStat.st_mode & S_IWOTH) ? "w" : "-");
        printf((fileStat.st_mode & S_IXOTH) ? "x" : "-");

        // number of links
        printf(" %lu ", fileStat.st_nlink); // linux - comment out to run on mac
        // printf(" %d ", fileStat.st_nlink); // mac - comment out to run on
        // linux

        // owner and group
        printf("%s ", getpwuid(fileStat.st_uid)->pw_name);
        printf("%s ", getgrgid(fileStat.st_gid)->gr_name);

        // size
        printf("%6ld ", fileStat.st_size); // linux - comment out to run on mac
        // printf("%6lld ", fileStat.st_size); // mac - comment out to run on
        // linux

        // datetime
        char time[80];
        struct tm *time_info = localtime(&fileStat.st_mtime);
        strftime(time, sizeof(time), "%b %d %H:%M", time_info);
        printf("%s ", time);

        // file name
        printf("%s ", dir->d_name);
        printf("\n");
      }
      // close the directory
      closedir(d);
    }
    return;
  }
  // if the -l flag is not given, print the file names
  else {
    // directory pointers
    struct dirent *dir;
    DIR *d;

    // open the current directory
    d = opendir(".");

    if (d) {
      int count = 0;

      // loop through the directory
      while ((dir = readdir(d)) != NULL) {
        if (count == 4) {
          printf("\n");
          count = 0;
        }
        // skip . and ..
        if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) {
          continue;
        } else {
          // print the file name
          printf("%s\t", dir->d_name);
          count++;
        }
      }

      printf("\n");
      // close the directory
      closedir(d);
    }
  }
}

/*
 * cp
 * cp takes in the arguments and the number of arguments and copies the source
 * file to the destination file. If the destination file does not exist, it is
 * created. If the source file does not exist, an error is printed.
 */
static void cp(char **args, int argcp) {

  // check for correct number of arguments
  if (argcp != 3) {
    fprintf(stderr, "cp: invalid number of arguments\n");
    return;
  }
  // open the source file
  int source = open(args[1], O_RDONLY);

  // print error if source file does not exist
  if (source == -1) {
    perror("cp");
    return;
  }

  // open the destination file
  int dest = open(args[2], O_WRONLY | O_CREAT, 0666);

  // if file can't be created, print error
  if (dest == -1) {
    perror("cp");
    return;
  }

  // buffer to store the contents of the source file
  char buffer[4096];
  // number of bytes read from the source file
  int bytesRead;
  // read the source file and write the contents to the destination file
  while ((bytesRead = read(source, buffer, sizeof(buffer))) > 0) {
    write(dest, buffer, bytesRead);
    // if there is a write error, print error
    if (write(dest, buffer, bytesRead) == -1) {
      perror("cp");
      return;
    }
  }
  // close files
  close(source);
  close(dest);
}

/*
 * env
 * env gets and prints the environment variables or sets the environment value
 * if the second argument is in the form of NAME=VALUE
 */
static void env(char **args, int argcp) {

  // get the environment variables
  extern char **environ;
  char **env = environ;

  // if there is only one argument, print all the environment variables
  if (argcp == 1) {
    while (*env) {
      printf("%s\n", *env);
      env++;
    }
  }
  // if name=value is the second argument, assign value to env variable
  else if (argcp == 2) {
    char *esign = strchr(args[1], '=');
    if (esign != NULL) {
      *esign = '\0';
      char *value = esign + 1;
      char *name = args[1];
      if (setenv(name, value, 1) != 0) {
        perror("Unable to set environment variable");
      }
    }
  }
  // error handling
  else {
    fprintf(stderr, "env: Expected format NAME=value\n");
  }
}
