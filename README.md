# microshell

## Learning Objectives
1. Demonstrate how to implement the fork-exec-wait pattern.
2. Demonstrate how to dynamically allocate memory and manage it.
3. Create system utilities using system calls.
4. Construct a complex C program with high quality (good style, robust, efficient).
5. Inspect code to find defects and suggest solutions.
6. Evaluate memory management using Valgrind.

## Overview/Program Structure
Create a microshell, myshell, that adheres to the following requirements. The program will be written in C. myshell will parse the user input. myshell has its own set of built-in commands.

### Program Structure
- **Program myshell** made from `myshell.o`, `argparse.o`, and `builtin.o`
- `myshell.c` includes `argparse.h`, `builtin.h`
- `argparse.c` includes `argparse.h`
- `builtin.c` includes `builtin.h`

## Instructions/Function Requirements

1. **Download the zipfile LINK (github classroom?)**
2. Your implementation files MUST be named `myshell.c`, `argparse.c`, `argparse.h`, `builtin.c`, `builtin.h`

### Create a Makefile to your project with the following targets:
- **default:** builds myshell
- **clean:** delete object files

### myshell.c
- The driver of the program. It contains `main()` and calls functions from other files as appropriate. It should launch the program, keep running until the user passes the command “exit”. It should determine whether to call a builtin function or a process.
- myshell will read the user input from stdin, one line at a time. Each line contains only one command and its arguments if it takes arguments, e.g., `mv file1 file2`. `mv` is the command, and `file1` and `file2` are the arguments.
- myshell will parse the user input into a list of arguments. Assume that only whitespaces are used to separate the arguments from each other. Multiple whitespaces should be handled. For example, the input "cat filename" should be parsed and accepted as a valid command. Assume no quoting or backslash escaping will be used. No other special characters are expected to be handled.
- myshell will execute the command. If the input is one of myshell’s built-in commands, myshell will call its corresponding user function. Otherwise, it will execute the default Unix command.
- myshell will be terminated if the user enters the command “exit”.
- myshell will parse the line (user input) to separate it into an array of strings.

### argparse.c
- Contains the functions needed to parse the input (a set of strings) assuming that whitespace is the only delimiter. Assume that only whitespaces are used to separate the arguments from each other. Multiple whitespaces should be handled. For example, the input "cat filename" should be parsed and accepted as a valid command. Assume no quoting or backslash escaping will be used.

**Sample output argCount:**
```
argCount(“ls - /home”) #returns 3
argCount("   ls    -l   /home  ") #returns 3
```

**Sample output argParse:**
```
argparse("ls -l /home", &argc)
# returns ["ls", "-l", "/home"] and set argc = 3

argparse("   ls    -l   /home  ", &argc)
# returns ["ls", "-l", "/home"] and set argc = 3
```

### builtin.c
- Contains the implementation of the commands `exit`, `pwd`, `cd`, and your_group_commands. It also checks whether a command matches one of the built-in commands. You are only required to implement the functionality mentioned in the points below.
- The built-in commands are done directly by myshell and are not forked to another process to do. If errors occur, myshell should report the error and then continue.

#### Built-in Commands

- **exit [value]:** Exit the shell with the value. If value is not given, exit with value 0. This built-in command should call the `exit(3)` library call.
- **pwd:** Prints the current working directory.

**Sample output:**
```
username@linux-01: ~/Project2$ ./myshell
%pwd
/home/username/Project2

username@linux-01: ~/Project2$ ./myshell
%pwd
/some/path/to/directory
```

- **cd [directory]:** It will change the current working directory. When no parameters are used, `cd` will change to the home directory.
  - One dot refers to the current directory, and dot dot refers to the parent directory. Consider reading the man page for `chdir(2)`.

**Sample output:**
```
username@linux-01: ~/Project2$ ./myshell
%pwd
/home/username/Project2
%cd ..
%pwd
/home/username

username@linux-01: ~/Project2$ ./myshell
%pwd
/some/path/to/directory
%cd ..
%pwd
/some/path/to
```

#### Group A Built-in Commands
- **ls [-l]:** Lists all files in the current directory. When the flag lowercase "l" is used, files should be listed in the long format.

**Sample output:**
```
username@linux-01: ~/Project2/$ ./myshell
%ls
argparse.c argparse.h builtin.h builtin.c myshell myshell.c Makefile

username@linux-01: ~/Project2/$ ./myshell
%ls -l
-rw-r–-r– 1 username grp.csci.students 2028 Feb 22 21:44 argparse.c
-rw-r–-r– 1 username grp.csci.students 2028 Feb 22 22:45 argparse.h
-rw-r–-r– 1 username grp.csci.students 2028 Feb 22 07:20 builtin.c
-rw-r–-r– 1 username grp.csci.students 2028 Feb 22 06:14 builtin.c
-rw-r–-r– 1 username grp.csci.students 2028 Feb 22 04:28 myshell.c
-rw-r–-r– 1 username grp.csci.students 2028 Feb 22 07:44 myshell
-rw-r–-r– 1 username grp.csci.students 2028 Feb 22 03:56 Makefile
```

- **cp <src_file_name target_file_name>:** Copies `src_file_name` to `target_file_name`. All file types, except directories, are allowed.

**Sample output:**
```
username@linux-01: ~/Project2/folder1$ ./myshell
%pwd
/home/username/Project2/folder1
%ls
test1.txt
%cp test1.txt /home/username/Project2/folder2
```

- **env [NAME=VALUE]:** Prints the environment variables list. When `Name=Value` is used, it sets the environment variable of the same name to the given value if the NAME already exists. If NAME does not exist, it will add a new NAME and set it to VALUE.

**Sample output:**
```
username@linux-01: ~/Project2$ ./myshell
% env
SHELL=/bin/bash
COLORTERM=truecolor
TERM_PROGRAM_VERSION=1.89.1
PWD=/home/user/347/Project2
.
.
.
LOGNAME=user
HOME=/home/user
LANG=en_US.UTF-8
USER=user

% env HOME=/home/user2
SHELL=/bin/bash
COLORTERM=truecolor
TERM_PROGRAM_VERSION=1.89.1
PWD=/home/user/347/Project2
.
.
.
LOGNAME=user
HOME=/home/user2
LANG=en_US.UTF-8
USER=user

% env TEST=test_value
SHELL=/bin/bash
COLORTERM=truecolor
TERM_PROGRAM_VERSION=1.89.1
PWD=/home/user/347/Project2
.
.
.
LOGNAME=user
HOME=/home/user2
LANG=en_US.UTF-8
USER=user
TEST=test_value
```

#### Group B Built-in Commands
- **stat <file1 or directory1...fileN or directoryN>:** Prints the information about the files.

**Sample output:**
```
username@linux-01: ~/Project2$ ./myshell
%stat argparse.c

File Name: argparse.c
Size: 2680 	Blocks: 18 	IO BLock: 1048576 regular file
Device 0,60	Inode: 479470	Links: 1
Permissions: (644/-rw-r--r--)	UID: (123, user)	GID: (321, students)
Access: Thu Feb 22 21:34:26 2024
Modify: Thu Feb 22 21:34:26 2024
Change: Thu Feb 22 21:34:26 2024
```

- **tail <file1...fileN>:** Writes the last 10 lines of the file to the standard output. If more than one file is used, each set of output is prefixed with a header showing the file name.

**Sample output:**
```
username@linux-01: ~/Project2$ ./myshell
%tail test1.txt


Three
Four
Five
Six
Seven
Eight
Nine
Ten
Eleven
Twelve

username@linux-01: ~/Project2/folder1$ ./myshell
%tail test1.txt test2.txt
Filename: test1.txt


Three
Four
Five
Six
Seven
Eight
Nine
Ten
Eleven
Twelve

Filename: test2.txt
A text file with some text in it
```

- **touch <file1 or directory1...fileN or directoryN>:** Sets the modification and access times of a file. If the file does not exist, a new file shall be created.

**Sample output:**
```
username@linux-01: ~/Project2/folder1$ ./myshell
%stat argparse.c

File Name: argparse.c
Size: 2680 	Blocks: 18 	IO BLock: 1048576 regular file
Device 0,60	Inode: 479470	Links: 1
Permissions: (644/-rw-r--r--)	UID: (123, user)	GID: (321, students)
Access: Thu Feb 22 21:34:26 2024
Modify: Thu Feb 22 21:34:26 2024
Change: Thu Feb 22 21:34:26 2024

%touch argparse.c

%stat argparse.c
File Name: argparse.c
Size: 2680 	Blocks: 18 	IO BLock: 1048576 regular file
Device 0,60	Inode: 479470	Links: 1
Permissions: (644/-rw-r--r--)	UID: (123, user)	GID: (321, students)
Access: Tue May 21 09:48:26 2024
Modify: Tue May 21 09:48:26 2024
Change: Tue May 21 09:48:26 2024
```

## Quality Requirements
- Check the results to make sure there are no failures.
- Proper memory management. Use Valgrind to evaluate your project and check for memory leaks and errors. Hint: review the Valgrind class activity.
- Defensive coding:
  - C function errors → check for returned values of C functions.
  - Check errors for system calls.
  - Only give necessary permissions.
  - Only allocate the necessary amount of memory.
- Reference the code review checklist for guidance.

## Assumptions/Scope of the Project
- Assume that only whitespaces are used to separate the arguments from each other. Multiple whitespaces should be handled. For example, the input "cat filename" should be parsed and accepted as a valid command. Assume no quoting or backslash escaping will be used.
- FOR MOVE FUNCTION WE ARE ASSUMING strings are just one word not things within “ ”.

## Hints/Tips
- All built-in functions should check the number of arguments given for the correct number. Don’t assume your arguments are correct.
- You are not required to implement the builtin commands options unless the options are specified in the requirements.
- If you are in doubt about the correct output for any of our built-in commands, run the command on the linux machines and compare its output to your program's output.
- Use the above sample outputs to guide you in your testing. Consider other test cases.
- Check how many parameters your argument can take, sample outputs are examples of a simple case, however multiple commands can also handle multiple files.
- The Makefile created will closely represent the structure of the program and how each file interacts with each other. You can reference class slides and the Program Structure section to assist you with this task.
- You can minimize big chunks of code in the IDE.

## Allowed Libraries
You are allowed to use (depending on your Built-In Group, you might not use all of these):
- `exit(3)` library call
- `time.h` library
- `grp.g` library
- `pwd.h` library
- `sys/sysmacros.h` library

## Forbidden Libraries
You are NOT allowed to use:
- `strtok(3)`
- `system(3)`
- `popen(3)`

## Acknowledgement
This assignment has been made clearer and more compliant with UDL with the efforts of the research students:
- Hamza Magsi
- Andrew Cox
- Hidemi Mitani Shen
