PROGRAM=sums_priorities

all: sums_priorities erast

sums_priorities: sums_priorities.o 
	$(CC) $(CFLAGS) $^ -o $@ -lpthread -lrt

sums_priorities.o: sums_priorities.c 
	$(CC) $(CFLAGS) -c $? -o $@

erast: eratosthenes.o 
	$(CC) $(CFLAGS) $^ -o $@ -lpthread -lrt -lm

eratosthenes.o: eratosthenes.c
	$(CC) $(CFLAGS) -c $? -o $@

clean:
	rm -rf sums_priorities erast
	rm -rf *.o
