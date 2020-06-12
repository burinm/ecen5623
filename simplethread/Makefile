INCLUDE_DIRS = 
LIB_DIRS = 
CC=gcc

CDEFS=
CFLAGS= -O3 $(INCLUDE_DIRS) $(CDEFS)
LIBS= 

HFILES= 
CFILES= simplethread.c incdecthread.c

SRCS= ${HFILES} ${CFILES}
OBJS= ${CFILES:.c=.o}

all:	simplethread incdecthread

clean:
	-rm -f *.o *.d
	-rm -f simplethread
	-rm -f incdecthread

simplethread: simplethread.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $@.o -lpthread

incdecthread: incdecthread.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $@.o -lpthread

depend:

.c.o:
	$(CC) $(CFLAGS) -c $<
