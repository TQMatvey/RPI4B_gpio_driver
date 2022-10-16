obj-m += gpio_driver.o

KDIR = /lib/modules/$(shell uname -r)/build

all:
	make modules && make uninstall && make install && make clean

modules:
	make -C $(KDIR) M=$(shell pwd) modules

clean:
	make -C $(KDIR) M=$(shell pwd) clean

install:
	insmod gpio_driver.ko

uninstall:
	rmmod gpio_driver.ko
