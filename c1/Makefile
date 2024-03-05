CC=gcc

DEPS = primes.h calc.h file.h shm.h
OBJ = main.c primes.c calc.c file.c shm.c

build: $(OBJ)
	$(CC) -o app $^

clean:
	rm app *.out

