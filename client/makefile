# Define the compiler being used with the compiler flags
CC = gcc
CFLAGS  = -g -Wall

default: laser-brains

# For the laser-brains executable, list here all object files to be included
laser-brains:  client.o
	$(CC) $(CFLAGS) -o laser-brains client.o

# Generate the unique device idenitfiier based on the mac address
gen_mac: gen_mac.o
	$(CC) $(CFLAGS) -o gen_mac gen_mac.o

# For each subsequent object file, define source files
client.o: client.c client.h
	$(CC) $(CFLAGS) -c client.c

gen_mac.o: gen_mac.c
	$(CC) $(CFLAGS) -c gen_mac.c

clean:
	$(RM) laser-brains *.o *~
