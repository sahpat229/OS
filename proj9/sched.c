/******************************
Sharang Phadke
12/08/2013
ECE 357: OS

sched.c
******************************/

#ifndef _SCHED_C_
#define _SCHED_C_

#include "sched.h"


void sched_init(void (*init_fn)()){
}

int sched_fork(){
}

void sched_exit(int code){
}

int sched_wait(int *exit_code){
}

void sched_nice(int niceval){
}

int sched_getpid(){
}

int sched_getppid(){
}

long sched_gettick(){
}

void sched_ps(){
}

void sched_switch(){
}

void sched_tick(){
    /* remember SIGVTALARM will, by default, be masked on entry to your
     * signal handler*/
}

#endif //_SCHED_C_
