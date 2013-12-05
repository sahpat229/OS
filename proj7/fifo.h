/******************************
Sharang Phadke
11/10/2013
EHE 357: OS

fifo.h
******************************/

#ifndef _FIFO_H_
#define _FIFO_H_
#define MYFIFO_BUFSIZ 4096


//fifo structure definition
struct fifo{
    struct sem access, write, read;     /* semaphores to lock */
    int head, tail;                     /* pointer to current head and tail */
    unsigned long buf[MYFIFO_BUFSIZ];   /* data buffer */
};

//void fifo_init(struct fifo *f)
//  Initialize the shared memory fifo *f including any
//  required underlying initializations. The fifo will
//  have MYFIFO_BUFSIZ elements, as defined in fifo.h
void fifo_init(struct fifo *f);

//void fifo_wr(struct fifo *f, unsigned long d)
//  Enque the data word d into the fifo, blocking until
//  the fifo has room to accept it
void fifo_wr(struct fifo *f, unsigned long d);

//unsigned long fifo_rd(struct fifo *f)
//  Deque the next data word from the fifo and return it,
//  blocking until there are words available
unsigned long fifo_rd(struct fifo *f);

#endif //_FIFO_H_
