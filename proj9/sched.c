/******************************
Sharang Phadke
12/08/2013
ECE 357: OS

sched.c
******************************/

#ifndef _SCHED_C_
#define _SCHED_C_

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/time.h>
#include "sched.h"

#define TICK_TIME 100000

// Current Process
struct sched_proc * current_proc;

// List of Processes
struct sched_proc proc_queue[SCHED_NPROC];


void sched_init(void (*init_fn)()){
    void *newsp;
    struct itimerval timer;
    if((current_proc = malloc(sizeof(struct sched_proc))) < 0)
        perror("malloc failed in sched_init");

    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = TICK_TIME;
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = TICK_TIME;

    current_proc->pid = 1;
    current_proc->ppid = 0;
    current_proc->task_state = SCHED_RUNNING;
    current_proc->cpu_ticks = 0;

    if ((newsp = mmap(0, STACK_SIZE, PROT_READ|PROT_WRITE,
            MAP_PRIVATE|MAP_ANONYMOUS, 0, 0)) == MAP_FAILED){
        perror("mmap failed");
    }

    current_proc->stack = newsp;
    current_proc->ctx.regs[JB_BP] = newsp;
    current_proc->ctx.regs[JB_SP] = newsp + STACK_SIZE;
    current_proc->ctx.regs[JB_PC] = init_fn;
    proc_queue[1] = *current_proc;

    if(setitimer(ITIMER_VIRTUAL, &timer, NULL) == -1)
        perror("setitimer failed");

    signal(SIGVTALRM, sched_tick);
    restorectx(&(proc_queue[1].ctx), 0);
}

int sched_fork(){
    //sigfillset(&mask);
    //sigdelset(&mask, SIGVTALRM);
    //sigdelset(&mask, SIGINT); //debug
    //sigprocmask(SIG_BLOCK, &mask, NULL);
}

void sched_exit(int code){
}

int sched_wait(int *exit_code){
}

void sched_nice(int niceval){
    if(niceval >= -20 && niceval <= 19)
        current_proc->priority = niceval;
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
    fprintf(stderr, "in sched_tick\n");
}

#endif //_SCHED_C_
