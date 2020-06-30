
all: posix_mq heap_mq 

heap_mq: heap_mq.c
	$(CC) $(LDFLAGS) $(CFLAGS) $^ -g -o $@ -lrt -lpthread

posix_mq: posix_mq.c
	$(CC) $(LDFLAGS) $(CFLAGS) $^ -g -o $@ -lrt -lpthread

clean:
	-rm -f *.o *.d posix_mq heap_mq
