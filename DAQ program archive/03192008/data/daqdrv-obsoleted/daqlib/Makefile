#
# makefile for daqcmd, daqlib
#DEBUG = y

ifeq ($(DEBUG),y)
 DEBUGFLAGS = -O -g -DDAQDEBUG
else
 DEBUGFLAGS = -O2
endif
CFLAGS += $(DEBUGFLAGS)


headers=daqdrv.h gsc16aiss8ao4regs.h gsc16aiss16ao2regs.h \
	daqfile.h daqio.h daqiodblbuf.h daqiom.h daqlib.h daqparse.h daqprint.h \
	daqregs.h daqthreshold.h daqthreshm.h daqutil.h daqwaveforms.h \
	fftlib.h fftrt.h gsclib.h quadraticlsq.h stringlib.h

libobjs=daqfile.o daqio.o daqiodblbuf.o daqiom.o daqlib.o daqparse.o daqprint.o \
	daqregs.o daqthreshold.o daqthreshm.o daqutil.o daqwaveforms.o \
	fft4g.o fftlib.o fftrt.o gsclib.o quadraticlsq.o stringlib.o

dobjs=daqfile.o daqio.o daqlib.o daqparse.o daqprint.o \
	daqregs.o daqthreshold.o daqutil.o \
	fft4g.o fftlib.o fftrt.o gsclib.o stringlib.o

all:	libdaq.a libdaqlib.a daqcmd daqcmdm readtonetriggerM binarytoascii binarytoraw fftutil

daqdrv.h: ../daqdriver/daqdrv.h
	cp -v ../daqdriver/daqdrv.h .

gsc16aiss8ao4regs.h: ../daqdriver/gsc16aiss8ao4regs.h
	cp -v ../daqdriver/gsc16aiss8ao4regs.h .

gsc16aiss16ao2regs.h: ../daqdriver/gsc16aiss16ao2regs.h
	cp -v ../daqdriver/gsc16aiss16ao2regs.h .

libdaq.a:  $(libobjs)
	$(AR) $(ARFLAGS) $@ $^

libdaqlib.a:  $(dobjs)
	$(AR) $(ARFLAGS) $@ $^

daqcmd: daqcmd.o ./libdaq.a
	gcc  -o $@ $^ -lpthread -lm

binarytoascii: binarytoascii.o ./libdaq.a
	gcc  -o $@ $^ -lpthread -lm

fftutil: fftutil.o ./libdaq.a
	gcc  -o $@ $^ -lpthread -lm

binarytoraw: binarytoraw.o ./libdaq.a
	gcc  -o $@ $^ -lpthread -lm

daqcmdm: daqcmdm.o ./libdaq.a
	gcc  -o $@ $^ -lpthread -lm

readtonetriggerM: readtonetriggerM.o ./libdaq.a
	gcc  -o $@ $^ -lpthread -lm

#.c.o:

%.o: %.c $(headers)
	gcc -c -Wall $(CFLAGS) $*.c 
#	gcc -c -Wall -O0 -g $*.c 
#	gcc -c -Wall -O2 -g $*.c 


clean:
	rm -f $(libobjs) daqcmd.o libdaq.a



