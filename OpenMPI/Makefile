CC=gcc
MPICC=mpicc
CFLAG=-g -O3
targets = parallel sequential
objects = parallel.o sequential.o

.PHONY: default
default: all

.PHONY: all
all: $(targets)

sequential: sequential_dinamic.c
	$(CC) $(CFLAG) -o $@ $^
parallel: mpi_dinamic.c
	$(MPICC) $(CFLAG) -o $@ $^
clean:
	rm -f *~ $(targets) $(objects)