# Define the compiler being used with the compiler flags
CC = gcc
CFLAGS  = -g -Wall

default: laser-brains

# For the laser-brains executable, list here all object files to be included
laser-brains:  main.o client.o dwm1000.o lsm9ds1.o rifle_variables.o
	$(CC) $(CFLAGS) -l wiringpi -o laser-brains main.o client.o dwm1000.o lsm9ds1.o rifle_variables.o

imu_test: main.o lsm303.o 
	$(CC) $(CFLAGS) -l math -o imu_test

# Generate the unique device idenitfiier based on the mac address
gen_mac: gen_mac.o
	$(CC) $(CFLAGS) -o gen_mac gen_mac.o

# For each subsequent object file, define source files
client.o: client.c client.h
	$(CC) $(CFLAGS) -c client.c

dwm1000.o: dwm1000.c dwm1000.h
		$(CC) $(CFLAGS) -c dwm1000.c

gen_mac.o: gen_mac.c
	$(CC) $(CFLAGS) -c gen_mac.c

lsm303.o: lsm303.c lsm303.h
	$(CC) $(CFLAGS) -c lsm303.c

# lsm9ds1.o: lsm9ds1.c lsm9ds1.h
# 		$(CC) $(CFLAGS) -c lsm9ds1.c

main.o: main.c main.h
		$(CC) $(CFLAGS) -c main.c

rifle_variables.o: rifle_variables.c rifle_variables.h
		$(CC) $(CFLAGS) -c rifle_variables.c

clean:
	$(RM) laser-brains *.o *~
