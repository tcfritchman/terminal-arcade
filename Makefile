CC=gcc
CFLAGS=-lncurses -Wall
DEPS = display.h
OBJ = driver.o display.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

driver.out: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o *.out