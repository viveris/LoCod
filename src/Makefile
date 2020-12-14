CC ?= gcc
CFLAGS += -I.
OBJ = map_memory.o main.o

%.o: %.c %.h
	$(CC) -c -o $@ $< $(CFLAGS)

locod-cpu: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean: 
	rm -f *.o

mrproper: clean
	rm -f locod-cpu
