CC ?= gcc
CFLAGS += -I../../src
OBJ = map_memory.o time_measure.o main.o
BAMBU_OPT = --memory-allocation-policy=NO_BRAM --channels-type=MEM_ACC_11 --memory-ctrl-type=D21 -DLOCOD_FPGA

all: locod-cpu fpga

main.o: main.c ../../src/map_memory.h ../../src/locod.h
	$(CC) -c -o $@ $< $(CFLAGS) $(USER_DEF)

map_memory.o: ../../src/map_memory.c ../../src/map_memory.h
	$(CC) -c -o $@ $< $(CFLAGS) $(USER_DEF)

time_measure.o: ../../src/time_measure.c ../../src/time_measure.h
	$(CC) -c -o $@ $< $(CFLAGS) $(USER_DEF)

locod-cpu: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(USER_DEF)

fpga:
	$(eval FPGA_FUNC = $(shell ../../scripts/get_fpga_fun.sh main.c))
	mkdir -p bambu
	$(foreach acc,$(FPGA_FUNC),cd bambu;bambu $(BAMBU_OPT) --top-fname=$(acc) ../main.c; cp $(acc).v ../;)
	cd ..

clean:
	rm -f *.o
	rm -rf bambu

mrproper: clean
	rm -f locod-cpu
	rm -f *.v

