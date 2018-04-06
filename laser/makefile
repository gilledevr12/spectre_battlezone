# Define the compiler being used with the compiler flags
CC = gcc
CFLAGS  = -g  -l wiringPi
DECAFILES = decadriver/dwm_api/deca_device.c decadriver/dwm_api/deca_params_init.c decadriver/dwm_api/my_deca_spi.c

#default: laser-brains
default: laser-brains

# For the laser-brains executable, list here all object files to be included
laser-brains:   client.o dwm1000.o lsm9ds1.o mqtt.o main.o
	$(CC) $(CFLAGS) -lm -lmosquitto main.o client.o dwm1000.o lsm9ds1.o mqtt.o $(DECAFILES) -o laser-brains

test-imu: test_imu.o lsm9ds1.o 
	$(CC) $(CFLAGS) test_imu.o lsm9ds1.o -o test-imu 

# Generate the unique device idenitfiier based on the mac address
gen_mac: gen_mac.o
	$(CC) $(CFLAGS) -o gen_mac gen_mac.o

# For each subsequent object file, define source files
client.o: client.c client.h
	$(CC) $(CFLAGS) -c client.c

dwm1000.o: dwm1000.c dwm1000.h
		$(CC) $(CFLAGS) -lmosquitto -c dwm1000.c $(DECAFILES)

gen_mac.o: gen_mac.c
	$(CC) $(CFLAGS) -c gen_mac.c

lsm303.o: lsm303.c lsm303.h
	$(CC) $(CFLAGS) -c lsm303.c

lsm9ds1.o: lsm9ds1.c lsm9ds1.h
		$(CC) $(CFLAGS) -c lsm9ds1.c

mqtt.o: mqtt.c mqtt.h
		$(CC) $(CFLAGS) -lmosquitto -c mqtt.c

main.o: main.c main.h
		$(CC) $(CFLAGS) -c main.c

test_imu.o: test_imu.c main.h
		$(CC) $(CFLAGS) -c test_imu.c

rifle_variables.o: rifle_variables.c rifle_variables.h
		$(CC) $(CFLAGS) -c rifle_variables.c

clean:
	$(RM) -rf laser-brains test-imu *.o *~
