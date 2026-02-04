obj-m += main.o
EXTRAVERSION = -android12-9-00019-g4ea09a298bb4-ab12292661
KDIR ?= /lib/modules/$(shell uname -r)/build
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
