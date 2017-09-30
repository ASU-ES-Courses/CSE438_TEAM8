ifeq (${KERNELRELEASE},)
    KERNEL_SOURCE := /usr/src/linux
    PWD := $(shell pwd)
default:
	${MAKE} -C ${KERNEL_SOURCE} M=${PWD} modules

clean:
	${MAKE} -C ${KERNEL_SOURCE} M=${PWD} clean
else
    obj-m := squeue.o

endif
