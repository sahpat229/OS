/******************************
Sharang Phadke
11/10/2013
EHE 357: OS

sem.h
******************************/

#ifndef _SEM_H_
#define _SEM_H_
#define N_PROC 64


int my_procnum;

//semaphore structure definition
struct sem {
  char  lock;               /* lock state */
  int   semval;             /* current value */
  int   waiting[N_PROC];    /* num procs awating increase in semval */
  int   sempids[N_PROC];    /* array of pids to be woken */
};

//int tas(volatile char *lock)
//  atomic test and set function
int tas(volatile char *lock);

//void sem_init(struct sem *s, int count)
//  Initialize the semaphore *s with the initial count and
//  initialize any underlying datastructures
void sem_init(struct sem *s, int count);

//int sem_try(struct sem *s)
//  Attempt to perform the "P" operation (atomically decrement the
//  semaphore). If this operation would block, return 0, else
//  return 1
int sem_try(struct sem *s);

//void sem_wait(struct sem *s)
//  Perform the "P" operation, blocking until successful. Blocking
//  is accomplished by noting within the *s that the current VP
//  should be woken up, and then sleeping using the sigsuspend
//  system call until SIGUSR1 is received.
void sem_wait(struct sem *s);

//void sem_inc(struct sem *s)
//  Perform the V operation. If any other processors were sleeping
//  on this semaphore, wake them by sending a SIGUSR1 to their
//  UNIX process id
void sem_inc(struct sem *s);

#endif //_SEM_H_
