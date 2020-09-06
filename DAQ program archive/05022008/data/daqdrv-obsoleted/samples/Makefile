#
# makefile for daqlib sample
#

exes = exercise readburst writewave echoburst readburstsum readcircle \
	readleveltrigger readedgetrigger readedgetrigger1MHz \
	readtonetrigger readtonetrigger1MHz readusertrigger

all:	$(exes) ../daqlib/libdaqlib.a

exercise: exercise.o
	gcc  -o $@ $^ -lpthread -lm -L../daqlib -ldaqlib

readburstsum: readburstsum.o
	gcc  -o $@ $^ -lpthread -lm -L../daqlib -ldaqlib

readburst: readburst.o
	gcc  -o $@ $^ -lpthread -lm -L../daqlib -ldaqlib

writewave: writewave.o
	gcc  -o $@ $^ -lpthread -lm -L../daqlib -ldaqlib

echoburst: echoburst.o
	gcc  -g -o $@ $^ -lpthread -lm -L../daqlib -ldaqlib

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

%.o: %.c $(headers)
	gcc -c -Wall -g $(CFLAGS) $*.c -I../daqlib

clean:
	\rm $(exes) *.o

