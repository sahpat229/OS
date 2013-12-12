CC = gcc

all:
	$(CC) -o main.exe main.c sched.c savectx64.S

debug: CC += -g
debug: all

clean:
	rm *.o *.exe
