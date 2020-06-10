PROGRAM=shedem

all: $(PROGRAM)
CFLAGS += -lpthread -lrt

$(PROGRAM): ex0.c
	$(CC) $(CFLAGS) $? -o $@

clean:
	rm -rf $(PROGRAM)
