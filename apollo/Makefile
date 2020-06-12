PROGRAM=rm_plot

all: $(PROGRAM)

$(PROGRAM): rm.c 
	$(CC) $(CFLAGS) $? -o $@ -lm

clean:
	rm -rf $(PROGRAM)
	rm -rf rm.o
