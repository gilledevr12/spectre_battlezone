# Define the compiler being used with the compiler flags
CC = gcc
CFLAGS  = -g -Wall

default: central-hub

# For the central- executable, list here all object files to be included
central-hub:  server.o
	$(CC) $(CFLAGS) -l pthread -o central-hub server.o

# For each subsequent object file, define source files
server.o: server.c server.h
	$(CC) $(CFLAGS) -l pthread -c server.c

setup.o: setup.c setup.h
	$(CC) $(CFLAGS) setup.c

clean:
	$(RM) central-hub *.o *~

