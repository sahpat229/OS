/******************************
* Sharang Phadke
* 11/10/2013
* ECE 357: OS
* 
* Project 7 - semaphores and locking
* 
* fifo.c
******************************/

#ifndef _FIFO_C_
#define _FIFO_C_

#include <stdlib.h>
#include <sys/mman.h>
#include "sem.h"
#include "fifo.h"


//void fifo_init(struct fifo *f)
//  Initialize the shared memory fifo *f including any
//  required underlying initializations. The fifo will
//  have MYFIFO_BUFSIZ elements, as defined in fifo.h
void fifo_init(struct fifo *f){
    sem_init(&(f->access), 1);
    sem_init(&(f->write), MYFIFO_BUFSIZ);
    sem_init(&(f->read), 0);
    f->head = 0;
    f-> tail = 0;
}

//void fifo_wr(struct fifo *f, unsigned long d)
//  Enque the data word d into the fifo, blocking until
//  the fifo has room to accept it
void fifo_wr(struct fifo *f, unsigned long d){
    //acquire write and access locks on fifo
    sem_wait(&(f->write));
    sem_wait(&(f->access));

    //add data word to fifo
    f->buf[f->tail] = d;
    ++(f->tail);
    f->tail %= MYFIFO_BUFSIZ;

    //appropriate resources and release locks
    sem_inc(&(f->read));
    sem_inc(&(f->access));
}

//unsigned long fifo_rd(struct fifo *f)
//  Deque the next data word from the fifo and return it,
//  blocking until there are words available
unsigned long fifo_rd(struct fifo *f){
    //acquire read and access locks on fifo
    sem_wait(&(f->read));
    sem_wait(&(f->access));

    //read data word from fifo
    unsigned long d = f->buf[f->head];
    ++(f->head);
    f->head %= MYFIFO_BUFSIZ;

    //appropriate resources and release locks
    sem_inc(&(f->write));
    sem_inc(&(f->access));

    return d;
}

#endif //_FIFO_C_
