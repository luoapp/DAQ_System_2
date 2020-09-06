# comment the following line to disable debugging
#DEBUG = y

ifeq ($(DEBUG),y)
 DEBUGFLAGS = -O -g -DDAQDRV_DEBUG
else
 DEBUGFLAGS = -O2
endif
CFLAGS += $(DEBUGFLAGS)

obj-m := daqdrv.o

daqdrv-objs := daqdrvmain.o gsc16aiss8ao4.o gsc16aiss16ao2.o

KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD       := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD)


clean:
	rm $(daqdrv-objs)
