PROGRAM=shedem

all: $(PROGRAM) fibtest
CFLAGS += -lpthread -lrt -g

$(PROGRAM): ex0.o memlog.o
	$(CC) $(CFLAGS) $^ -o $@

fibtest: fibtest.o memlog.o
	$(CC) $(CFLAGS) $^ -o $@


ex.o: ex0.c
	$(CC) $(CFLAGS) $? -c -o $@

memlog.o: memlog.c
	$(CC) $(CFLAGS) $? -c -o $@

fibtest.o: fibtest.c
	$(CC) $(CFLAGS) $? -c -o $@


clean:
	rm -rf $(PROGRAM)
	rm -rf ex0.o memlog.o
	rm -rf fibtest fibtest.o
