

APP = main

obj-m:= sensor1.o LedDisplay.o

ARCH=x86

CC=/opt/iot-devkit/1.7.2/sysroots/x86_64-pokysdk-linux/usr/bin/i586-poky-linux/i586-poky-linux-gcc

CROSS_COMPILE=/opt/iot-devkit/1.7.2/sysroots/x86_64-pokysdk-linux/usr/bin/i586-poky-linux/i586-poky-linux-

SROOT=/opt/iot-devkit/1.7.2/sysroots/i586-poky-linux

	MAKE = make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE)
	KDIR = $(SROOT)/usr/src/kernel/


all :
	$(MAKE) -C $(KDIR) M=$(PWD) modules
	$(CC) -pthread -Wall -o $(APP) user1.c -lm

clean:
	rm -f *.ko
	rm -f *.o
	rm -f Module.symvers
	rm -f modules.order
	rm -f *.mod.c
	rm -rf .tmp_versions
	rm -f *.mod.c
	rm -f *.mod.o
	rm -f \.*.cmd
	rm -f Module.markers
	rm -f $(APP) 
