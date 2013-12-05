$(CC) = gcc

all: hello hello2 hello3 hello4 cost

hello: hello.c
	$(CC) -S hello.c

hello2: hello2.S
	gcc -c hello2.S -o hello2.o
	ld -s hello2.o -o hello2.exe

hello3: hello3.S
	gcc -c hello3.S -o hello3.o
	ld -s hello3.o -o hello3.exe

hello4: hello4.S
	gcc -c hello4.S -o hello4.o
	ld -s hello4.o -o hello4.exe

cost: cost.c
	$(CC) cost.c -o cost.exe

debug: CC += -g

clean:
	rm *.exe *.out *.o
