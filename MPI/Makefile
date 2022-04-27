CC=gcc
MPICC=mpicc
CFLAG=-g
targets = mpi sequential
objects = mpi.o sequential.o

.PHONY: default
default: all

.PHONY: all
all: $(targets)

sequential: sequential_dinamic.c
	$(CC) $(CFLAG) -o $@ $^
mpi: mpi_dinamic.c
	$(MPICC) $(CFLAG) -o $@ $^
clean:
	rm -f *~ $(targets) $(objects)


# echo 0 | sudo tee /proc/sys/kernel/yama/ptrace_scope