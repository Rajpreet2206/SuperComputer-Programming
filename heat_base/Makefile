# Intel compiler
CC =  icc
CFLAGS = -Ofast -xCORE-AVX512 -qopt-report=5 -qopt-report-annotate -qopt-report-phase=vec,loop
MPICC = mpicc

all: heat

heat : heat.o input.o misc.o timing.o relax_gauss.o relax_jacobi.o
	$(CC) $(CFLAGS) -o $@ $+ -lm 

%.o : %.c heat.h timing.h input.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o heat *~ *.ppm

remake : clean all
