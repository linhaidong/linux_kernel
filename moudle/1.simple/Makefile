ifneq ($(KERNELERLEASE),)
obj-m:=hello.o
else
KERDIR=/lib/modules/$(shell uname -r)/build
CURDIR=$(shell pwd)

all:
	#make -C $(KERDIR) M=$(CURDIR) modules
	make -C $(KERDIR) SUBDIRS=$(CURDIR) modules
clean:
	make -C $(KERDIR) M=$(CURDIR) clean
endif
