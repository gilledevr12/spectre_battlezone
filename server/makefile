# Define the compiler being used with the compiler flags
CC = gcc
CFLAGS  = -g -Wall -l pthread -lm -l mosquitto

default: hub

# For the hub executable, list here all object files to be included
hub:  main.o players.o rifle_variables.o server.o setup.o play.o publish.o inventory.h bool.h
	$(CC) $(CFLAGS) -o hub main.o players.o rifle_variables.o server.o setup.o play.o publish.o

# For each subsequent object file, define source files
main.o: main.c
	$(CC) $(CFLAGS) -c main.c

players.o: players.c players.h
	$(CC) $(CFLAGS) -c players.c

rifle_variables.o: rifle_variables.c rifle_variables.h
	$(CC) $(CFLAGS) -c rifle_variables.c

server.o: server.c server.h
	$(CC) $(CFLAGS) -c server.c

setup.o: setup.c setup.h
	$(CC) $(CFLAGS) -c setup.c

play.o: play.c play.h
	$(CC) $(CFLAGS) -c play.c

publish.o: publish.c publish.h
	$(CC) $(CFLAGS) -c publish.c

clean:
	$(RM) hub *.o *~
	
