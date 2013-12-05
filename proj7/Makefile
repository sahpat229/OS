$(CC) = gcc

testfifo.exe: testfifo.c fifo.c fifo.h sem.c sem.h tas64.S
	$(CC) -o testfifo.exe testfifo.c fifo.c sem.c tas64.S

testsem.exe: testsem.c sem.c sem.h tas64.S
	$(CC) -o testsem.exe testsem.c sem.c tas64.S

debug: CC += -g
debug: testfifo.exe

clean:
	rm *.exe *.out *.o
