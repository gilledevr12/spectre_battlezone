# Define the compiler being used with the compiler flags
CC = gcc
CFLAGS  = -g -Wall -l pthread

default: central-hub

# For the central- executable, list here all object files to be included
central-hub:  players.o rifle_variables.o server.o setup.o inventory.h
	$(CC) $(CFLAGS) -o central-hub players.o rifle_variables.o server.o setup.o

# For each subsequent object file, define source files
players.o: players.c players.h
	$(CC) $(CFLAGS) -c players.c

rifle_variables.o: rifle_variables.c rifle_variables.h
	$(CC) $(CFLAGS) -c rifle_variables.c

server.o: server.c server.h
	$(CC) $(CFLAGS) -c server.c

setup.o: setup.c setup.h
	$(CC) $(CFLAGS) -c setup.c

clean:
	$(RM) central-hub *.o *~
	
