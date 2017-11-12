CC = gcc
#OPT = -O3 -m32 --std=c99
OPT = -g
WARN = -Wall
CFLAGS = $(OPT) $(WARN) $(INC) $(LIB)

# List all your .cc files here (source files, excluding header files)
SIM_SRC = main.c ds.c fifo.c cache.c

# List corresponding compiled object files here (.o files)
SIM_OBJ = main.o ds.o fifo.o cache.o
 
#################################

# default rule
all: sim
	@echo "my work is done here..."


# rule for making sim
sim: $(SIM_OBJ)
	$(CC) -o sim $(CFLAGS) $(SIM_OBJ) -lm
	@echo "-----------DONE WITH SIM -----------"


%.o:
	$(CC) $(CFLAGS) -c $*.c


clean:
	rm -f *.o sim


clobber:
	rm -f *.o

