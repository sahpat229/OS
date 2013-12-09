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
    current_proc->pid = 1;
    current_proc->ppid = 1;
    current_proc->cpu_ticks = 0;
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
    return current_proc->pid;
}

int sched_getppid(){
    return current_proc->ppid;
}

unsigned long sched_gettick(){
    return current_proc->cpu_ticks;
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
