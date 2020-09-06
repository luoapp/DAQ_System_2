# comment the following line to disable debugging
#DEBUG = y

ifeq ($(DEBUG),y)
 EXTRA_CFLAGS += -DDAQDRV_DEBUG
endif

obj-m := daqdrv.o

daqdrv-objs := daqdrvmain.o gsc16aiss8ao4.o gsc16aiss16ao2.o

KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD       := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD)


clean:
	rm -rf $(daqdrv-objs) *~ *.o *.ko daqdrv.mod.c Module.symvers .tmp_versions .??*.cmd
