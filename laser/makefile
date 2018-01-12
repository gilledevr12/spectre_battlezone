# Define the compiler being used with the compiler flags
CC = gcc
CFLAGS  = -g -Wall

default: laser-brains

# For the laser-brains executable, list here all object files to be included
laser-brains:  main.o client.o lsm303.o
	$(CC) $(CFLAGS) -o laser-brains main.o client.o lsm303.o

# Generate the unique device idenitfiier based on the mac address
gen_mac: gen_mac.o
	$(CC) $(CFLAGS) -o gen_mac gen_mac.o

# For each subsequent object file, define source files
client.o: client.c client.h
	$(CC) $(CFLAGS) -c client.c

gen_mac.o: gen_mac.c
	$(CC) $(CFLAGS) -c gen_mac.c

lsm303.o: lsm303.c lsm303.h
	$(CC) $(CFLAGS) -c lsm303.c

main.o: main.c
		$(CC) $(CFLAGS) -c main.c

clean:
	$(RM) laser-brains *.o *~
