CC=gcc
CFLAGS=-I
DEPS = hellomake.h
OBJ = SerialHDR.o

# %.o: %.c $(DEPS)
#	$(CC) -c -o $@ $< $(CFLAGS)

hellomake: $(OBJ)
	$(CC) `pkg-config --libs --cflags opencv` -o $@ $^ $(CFLAGS)
