all: myshell

myshell: myshell.o argparse.o builtin.o
	gcc -Wall -g -o myshell myshell.o argparse.o builtin.o

myshell.o: myshell.c argparse.h builtin.h
	gcc -Wall -g -c myshell.c

argparse.o: argparse.c argparse.h
	gcc -Wall -g -c argparse.c

builtin.o: builtin.c builtin.h
	gcc -Wall -g -c builtin.c

clean:
	rm -f myshell.o argparse.o builtin.o myshell
