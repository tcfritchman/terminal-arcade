CC=gcc
CFLAGS=-lncurses -Wall
DEPS = display.h tetris.h
OBJ = driver.o display.o tetris.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

driver.out: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o *.out