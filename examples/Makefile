EX=ex1.bin ex2.bin

LDFLAGS=-L../
CFLAGS=-I../ -I../patchwerk -I../nodes
LIBS=-lsndkit
default: $(EX)

%.bin: %.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) $(LIBS)

clean:
	$(RM) $(EX)
