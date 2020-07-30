PROGRAM=three

all: $(PROGRAM)

$(PROGRAM): sequencer.o realtime.o timetools.o queue.o resources.o memlog.o
	$(CC) $(CFLAGS) $^ -o $@ -lpthread -lrt

sequencer.o: sequencer.c
	$(CC) $(CFLAGS) $? -c -o $@

realtime.o: realtime.c
	$(CC) $(CFLAGS) $? -c -o $@

timetools.o: timetools.c
	$(CC) $(CFLAGS) $? -c -o $@

queue.o: queue.c
	$(CC) $(CFLAGS) $? -c -o $@

resources.o: resources.c
	$(CC) $(CFLAGS) $? -c -o $@

memlog.o: memlog.c
	$(CC) $(CFLAGS) $? -c -o $@

clean:
	rm -rf $(PROGRAM)
	rm -rf *.o 
	sudo rm -f /dev/mqueue/w1q  /dev/mqueue/w2q
