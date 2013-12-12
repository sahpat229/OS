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
#include <string.h>
#include <sys/mman.h>
#include <sys/time.h>
#include "adjstack.c"
#include "sched.h"

#define TICK_TIME 2 //debug
#define TICK_UTIME 100000


int total_ticks = 0;
struct sched_proc * current_proc;
struct sched_waitq wait_queue;

void sched_init(void (*init_fn)()){
    void *newsp;
    struct itimerval timer;
    if ((current_proc = malloc(sizeof(struct sched_proc))) < 0)
        perror("malloc failed in sched_init");

    timer.it_interval.tv_sec = TICK_TIME;;
    timer.it_interval.tv_usec = 0; //TICK_UTIME; debug
    timer.it_value.tv_sec = TICK_TIME;
    timer.it_value.tv_usec = 0; //TICK_UTIME; debug

    current_proc->pid = 1;
    current_proc->ppid = 0;
    current_proc->task_state = SCHED_RUNNING;
    current_proc->cpu_ticks = 0;
    current_proc->total_ticks = 0;

    if ((newsp = mmap(0, STACK_SIZE, PROT_READ|PROT_WRITE,
            MAP_PRIVATE|MAP_ANONYMOUS, 0, 0)) == MAP_FAILED){
        perror("mmap failed");
    }

    current_proc->stack = newsp;
    current_proc->ctx.regs[JB_BP] = newsp;
    current_proc->ctx.regs[JB_SP] = newsp + STACK_SIZE;
    current_proc->ctx.regs[JB_PC] = init_fn;
    wait_queue.proc_queue[0] = current_proc;

    if (setitimer(ITIMER_VIRTUAL, &timer, NULL) == -1)
        perror("setitimer failed");

    signal(SIGABRT, sched_ps);
    signal(SIGVTALRM, sched_tick);
    restorectx(&(wait_queue.proc_queue[0]->ctx), 0);
}

int sched_fork(){
    int i;
    void *newsp;
    int newpid = 0;
    struct sched_proc *new_proc;
    struct savectx newctx;
    sigset_t mask, oldmask;
    
    fprintf(stderr, "in sched_fork\n");

    // mask signals
    sigemptyset(&mask);
    sigaddset(&mask, SIGVTALRM);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGUSR2);
    sigprocmask(SIG_BLOCK, &mask, &oldmask);

    // allocate new struct sched_proc
    for (i = 1; i < SCHED_NPROC; i++)
        if (wait_queue.proc_queue[i] == NULL)
            newpid = i;

    if (newpid == 0){
        fprintf(stderr, "cannot find a free pid");
        sigprocmask(SIG_UNBLOCK, &mask, &oldmask);
        return -1;
    }

    if ((new_proc = malloc(sizeof(struct sched_proc))) < 0){
        perror("malloc failed in fork");
        -sigprocmask(SIG_UNBLOCK, &mask, &oldmask);
        return -1;
    }
    wait_queue.proc_queue[newpid] = new_proc;

    // allocate new stack
    if ((newsp = mmap(0, STACK_SIZE, PROT_READ|PROT_WRITE,
            MAP_PRIVATE|MAP_ANONYMOUS, 0, 0)) == MAP_FAILED){
        perror("mmap failed in fork");
        sigprocmask(SIG_UNBLOCK, &mask, &oldmask);
        return -1;
    }

    //copy parent stack into new memory
    memcpy(newsp, current_proc->stack, STACK_SIZE);
    
    // adjust new stack
    adjstack(newsp, newsp + STACK_SIZE, newsp - current_proc->stack);

    new_proc->pid = newpid;
    new_proc->ppid = current_proc->pid;
    new_proc->task_state = SCHED_READY;
    new_proc->stack = newsp;
    new_proc->ctx = newctx;

    // switch context and return to child and parent
    switch (savectx(&newctx)){
        case 0: //parent
            new_proc->ctx.regs[JB_BP] += newsp - current_proc->stack;
            new_proc->ctx.regs[JB_SP] += newsp - current_proc->stack;
            sigprocmask(SIG_UNBLOCK, &mask, &oldmask);
            return newpid;

        default: //child
            sigprocmask(SIG_UNBLOCK, &mask, &oldmask);
            return 0;
    }
}

void sched_exit(int code){
}

int sched_wait(int *exit_code){
}

void sched_nice(int niceval){
    if (niceval >= -20 && niceval <= 19)
        current_proc->priority = niceval;
}

int sched_getpid(){
    return current_proc->pid;
}

int sched_getppid(){
    return current_proc->ppid;
}

int sched_gettick(){
    return current_proc->cpu_ticks;
}

void sched_ps(){
    int i;
    fprintf(stderr, "|---pid|--ppid|priority|exit_code|cpu_ticks|total_ticks|---------stack|---state|----wait_queue|\n");
    for (i = 1; i < SCHED_NPROC; i++){
        if (wait_queue.proc_queue[i] != NULL){
            fprintf(stderr, "%7d%7d%9d%10d%10d%12d %p",
                    wait_queue.proc_queue[i]->pid,
                    wait_queue.proc_queue[i]->ppid,
                    wait_queue.proc_queue[i]->priority,
                    wait_queue.proc_queue[i]->exit_code,
                    wait_queue.proc_queue[i]->cpu_ticks,
                    wait_queue.proc_queue[i]->total_ticks,
                    wait_queue.proc_queue[i]->stack);
            if (wait_queue.proc_queue[i]->task_state == SCHED_SLEEPING)
                fprintf(stderr, " SLEEPING %p\n", &wait_queue);
            else if (wait_queue.proc_queue[i]->task_state == SCHED_READY)
                fprintf(stderr, "    READY\n");
            else if (wait_queue.proc_queue[i]->task_state == SCHED_RUNNING)
                fprintf(stderr, "  RUNNING\n");
            else if (wait_queue.proc_queue[i]->task_state == SCHED_ZOMBIE)
                fprintf(stderr, "   ZOMBIE\n");
        }
    }
    exit(0);
}

void sched_switch(){
    sigset_t mask, oldmask;
    
    fprintf(stderr, "in sched_tick\n");

    sigemptyset(&mask);
    sigaddset(&mask, SIGVTALRM);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGUSR2);
    sigprocmask(SIG_BLOCK, &mask, &oldmask);

    //sched_ps();

    sigprocmask(SIG_UNBLOCK, &mask, &oldmask);
}

void sched_tick(){
    /* remember SIGVTALARM will, by default, be masked on entry to your
     * signal handler*/
    fprintf(stderr, "in sched_tick\n");
    total_ticks++;
    current_proc->cpu_ticks++;
    //if (logic to see whether we should switch){
    //    current_proc->task_state = SCHED_READY;
        //sched_switch();
}

#endif //_SCHED_C_
