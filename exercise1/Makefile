PROGRAM=shedem

all: $(PROGRAM)
CFLAGS += -lpthread -lrt

$(PROGRAM): ex0.o memlog.o
	$(CC) $(CFLAGS) $^ -o $@

ex.o: ex0.c
	$(CC) $(CFLAGS) $? -c -o $@

memlog.o: memlog.c
	$(CC) $(CFLAGS) $? -c -o $@

clean:
	rm -rf $(PROGRAM)
	rm -rf ex0.o memlog.o
