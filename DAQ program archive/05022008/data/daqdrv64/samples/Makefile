#
# makefile for daqlib sample
#

#exes = exercise exerciseslowly readburst writewave echoburst #readburstsum readcircle \
#	readleveltrigger readedgetrigger readedgetrigger1MHz \
#	readtonetrigger readtonetrigger1MHz readusertrigger readcont #udel_printbin udel_readcont udel_daq.o

exes = udel_printbin udel_readonly udel_readwrite
   

all:	$(exes)

udel_readcont: udel_readcont.o udel_daq.o udel_daq.h
	gcc  -o $@ udel_readcont.o udel_daq.o -lpthread -lm -L../daqlib -ldaqlib 

udel_printbin: udel_printbin.o udel_daq.o udel_daq.h
	gcc  -o $@ udel_printbin.o udel_daq.o -lpthread -lm -L../daqlib -ldaqlib -L/usr/X11/lib -lX11

readcont: readcont.o udel_daq.o udel_daq.h
	gcc  -o $@ readcont.o udel_daq.o -lpthread -lm -L../daqlib -ldaqlib 

exercise: exercise.o 
	gcc  -o $@ $^ -lpthread -lm -L../daqlib -ldaqlib

exerciseslowly: exerciseslowly.o
	gcc  -o $@ $^ -lpthread -lm -L../daqlib -ldaqlib

exercisesmall: exercisesmall.o
	gcc  -o $@ $^ -lpthread -lm -L../daqlib -ldaqlib

readburstsum: readburstsum.o
	gcc  -o $@ $^ -lpthread -lm -L../daqlib -ldaqlib

readburst: readburst.o
	gcc  -o $@ $^ -lpthread -lm -L../daqlib -ldaqlib

writewave: writewave.o
	gcc  -o $@ $^ -lpthread -lm -L../daqlib -ldaqlib

echoburst: echoburst.o udel_daq.o
	gcc  -o $@ $^ -lpthread -lm -L../daqlib -ldaqlib  -L/usr/X11/lib -lX11

echoburstinternal: echoburstinternal.o
	gcc  -o $@ $^ -lpthread -lm -L../daqlib -ldaqlib

readcircle: readcircle.o
	gcc  -o $@ $^ -lpthread -lm -L../daqlib -ldaqlib

readleveltrigger: readleveltrigger.o
	gcc  -o $@ $^ -lpthread -lm -L../daqlib -ldaqlib

readedgetrigger: readedgetrigger.o
	gcc  -o $@ $^ -lpthread -lm -L../daqlib -ldaqlib

readedgetrigger1MHz: readedgetrigger1MHz.o
	gcc  -o $@ $^ -lpthread -lm -L../daqlib -ldaqlib

readtonetrigger: readtonetrigger.o
	gcc  -o $@ $^ -lpthread -lm -L../daqlib -ldaqlib

readtonetrigger1MHz: readtonetrigger1MHz.o
	gcc  -o $@ $^ -lpthread -lm -L../daqlib -ldaqlib

readusertrigger: readusertrigger.o
	gcc  -o $@ $^ -lpthread -lm -L../daqlib -ldaqlib

%.o: %.c $(headers) udel_daq.h
	gcc -c -Wall -g $(CFLAGS) $*.c -I../daqlib


udel_daq.o: udel_daq.c udel_daq.h
	gcc -c -Wall -g udel_daq.c -I../daqlib

udel_readonly:udel_readonly.o udel_daq.o
	gcc -o $@ $^ -Wall -lpthread -lm -L../daqlib -ldaqlib -L/usr/X11/lib -lX11

udel_readonly.o: udel_readonly.c udel_daq.h
	gcc -c -Wall -g udel_readonly.c -I../daqlib

udel_readwrite: udel_readwrite.o udel_daq.o
	gcc  -o $@ $^ -lpthread -lm -L../daqlib -ldaqlib  -L/usr/X11/lib -lX11



clean:
	\rm $(exes) *.o

