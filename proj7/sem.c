/******************************
Sharang Phadke
11/10/2013
ECE 357: OS

sem.c
******************************/
#ifndef _SEM_C_
#define _SEM_C_

#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include "sem.h"


//void sigusr1handler(int sig)
//  Empty signal handler for SIGUSR1
void sigusr1handler(int sig){}

//void sem_init(struct sem *s, int count)
//  Initialize the semaphore *s with the initial count and
//  initialize any underlying datastructures
void sem_init(struct sem *s, int count){
    s->lock = 0;
    s->semval = count;
}

//int sem_try(struct sem *s)
//  Attempt to perform the "P" operation (atomically decrement the
//  semaphore). If this operation would block, return 0, else
//  return 1
int sem_try(struct sem *s){
    while(tas(&(s->lock)) != 0);    //acquire lock on semaphore
    if(s->semval > 0){
        s->lock = 0;
        return 1;
    }
    else{
        s->lock = 0;
        return 0;
    }
}

//void sem_wait(struct sem *s)
//  Perform the "P" operation, blocking until successful. Blocking
//  is accomplished by noting within the *s that the current VP
//  should be woken up, and then sleeping using the sigsuspend
//  system call until SIGUSR1 is received.
void sem_wait(struct sem *s){
    while(1){
        while(tas(&(s->lock)) != 0);    //acquire lock on semaphore
        if(s->semval > 0){
            s->semval--;
            s->lock = 0;
            return;
        }
        else{
            sigset_t mask, oldmask;
            s->waiting[my_procnum] = 1;
            s->sempids[my_procnum] = getpid(); 
            sigfillset(&mask);
            sigdelset(&mask, SIGUSR1);
            sigdelset(&mask, SIGINT);  //debug
            sigprocmask(SIG_BLOCK, &mask, NULL);
            s->lock = 0;
            signal(SIGUSR1, sigusr1handler);
            sigsuspend(&mask);
            sigprocmask(SIG_UNBLOCK, &mask, NULL);
        }
    }
}

//void sem_inc(struct sem *s)
//  Perform the V operation. If any other processors were sleeping
//  on this semaphore, wake them by sending a SIGUSR1 to their
//  UNIX process id
void sem_inc(struct sem *s){
    int i;

    while(tas(&(s->lock)) != 0);    //acquire lock on semaphore
    s->semval++;
    for(i = 0; i < N_PROC; i++){
        if(s->waiting[i]){
            s->waiting[i] = 0;
            kill(s->sempids[i], SIGUSR1);
        }
    }
    s->lock = 0;
}

#endif //_SEM_C_
