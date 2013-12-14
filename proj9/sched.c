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

#define TICK_TIME 0
#define TICK_UTIME 100000


int total_ticks = 0;
struct sched_proc * current_proc;
struct sched_proc *proc_queue[SCHED_NPROC];

void sched_init(void (*init_fn)()){
    void *newsp;
    struct itimerval timer;
    if ((current_proc = malloc(sizeof(struct sched_proc))) < 0)
        perror("malloc failed in sched_init");

    timer.it_interval.tv_sec = TICK_TIME;;
    timer.it_interval.tv_usec = TICK_UTIME;
    timer.it_value.tv_sec = TICK_TIME;
    timer.it_value.tv_usec = TICK_UTIME;

    if ((newsp = mmap(0, STACK_SIZE, PROT_READ|PROT_WRITE,
            MAP_PRIVATE|MAP_ANONYMOUS, 0, 0)) == MAP_FAILED){
        perror("mmap failed");
    }

    current_proc->pid = 1;
    current_proc->task_state = SCHED_RUNNING;
    current_proc->stack = newsp;
    current_proc->ctx.regs[JB_BP] = newsp;
    current_proc->ctx.regs[JB_SP] = newsp + STACK_SIZE;
    current_proc->ctx.regs[JB_PC] = init_fn;
    proc_queue[1] = current_proc;

    if (setitimer(ITIMER_VIRTUAL, &timer, NULL) == -1)
        perror("setitimer failed");

    signal(SIGABRT, sched_ps);
    signal(SIGVTALRM, _sched_tick);
    restorectx(&(proc_queue[1]->ctx), 0);
}

int sched_fork(){
    int i;
    void *newsp;
    int newpid = 0;
    struct sched_proc *new_proc;
    sigset_t mask;
    
    // mask signals
    sigemptyset(&mask);
    sigaddset(&mask, SIGVTALRM);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGUSR2);
    sigprocmask(SIG_BLOCK, &mask, NULL);

    // allocate new struct sched_proc
    for (i = 1; i < SCHED_NPROC; i++){
        if (proc_queue[i] == NULL){
            newpid = i;
            break;
        }
    }

    if (newpid == 0){
        fprintf(stderr, "cannot find a free pid");
        sigprocmask(SIG_UNBLOCK, &mask, NULL);
        return -1;
    }

    if ((new_proc = malloc(sizeof(struct sched_proc))) < 0){
        sigprocmask(SIG_UNBLOCK, &mask, NULL);
        return -1;
    }

    proc_queue[newpid] = new_proc;

    // allocate new stack
    if ((newsp = mmap(0, STACK_SIZE, PROT_READ|PROT_WRITE,
            MAP_PRIVATE|MAP_ANONYMOUS, 0, 0)) == MAP_FAILED){
        sigprocmask(SIG_UNBLOCK, &mask, NULL);
        return -1;
    }

    // make a copy of the parent stack and adjust child stack
    memcpy(newsp, current_proc->stack, STACK_SIZE);
    adjstack(newsp, newsp + STACK_SIZE, newsp - current_proc->stack);

    new_proc->pid = newpid;
    new_proc->ppid = current_proc->pid;
    new_proc->task_state = SCHED_READY;
    new_proc->stack = newsp;

    // switch context and return to child and parent
    if (savectx(&(new_proc->ctx)) == 0){ //parent
        new_proc->ctx.regs[JB_BP] += newsp - current_proc->stack;
        new_proc->ctx.regs[JB_SP] += newsp - current_proc->stack;
        sigprocmask(SIG_UNBLOCK, &mask, NULL);
        return newpid;
    }

    // child
    sigprocmask(SIG_UNBLOCK, &mask, NULL);
    return 0;
}

void sched_exit(int code){
    int i;
    int *ecode;
    int pid = current_proc->pid;
    int ppid = current_proc->ppid;

    if (proc_queue[ppid] != NULL && proc_queue[ppid]->task_state == SCHED_SLEEPING){
        if ((ecode = malloc(sizeof(int))) == NULL)
            perror("malloc failed in sched_exit");

        *ecode = code;
        proc_queue[ppid]->waitq[pid] = ecode;
        proc_queue[ppid]->task_state = SCHED_READY;
    }

    // kill yourself
    current_proc->exit_code = code;
    current_proc->task_state = SCHED_ZOMBIE;
    _sched_switch();
}

int sched_wait(int *exit_code){
    int i, foundChild = 0, waitpid = 0;

    // clean up the first zombie child found
    for (i = 1; i < SCHED_NPROC; i++){
        if (proc_queue[i] != NULL && proc_queue[i]->ppid == current_proc->pid){
            if (proc_queue[i]->task_state == SCHED_ZOMBIE){
                *exit_code = proc_queue[i]->exit_code;
                if (munmap(proc_queue[i]->stack, STACK_SIZE) < 0)
                    perror("munmap failed in sched_wait");
                free(current_proc->waitq[i]);
                current_proc->waitq[i] = NULL;
                free(proc_queue[i]);
                proc_queue[i] = NULL;
                return 0;
            }
            else
                waitpid = i;
        }
    }

    // wait for non-zombie children to finish
    if (waitpid != 0){
        current_proc->task_state = SCHED_SLEEPING;
        _sched_switch();
        *exit_code = *(current_proc->waitq[waitpid]);

        // clean up zombie
        if (munmap(proc_queue[waitpid]->stack, STACK_SIZE) < 0)
            perror("munmap failed in sched_wait");
        free(current_proc->waitq[waitpid]);
        current_proc->waitq[waitpid] = NULL;
        free(proc_queue[waitpid]);
        proc_queue[waitpid] = NULL;
        return 0;
    }

    return -1;
}

void sched_nice(int niceval){
    if (niceval <= -20)
        current_proc->s_priority = -20;
    else if (niceval >= 19)
        current_proc->s_priority = 19;
    else
        current_proc->s_priority = niceval;
}

int sched_getpid(){
    return current_proc->pid;
}

int sched_getppid(){
    return current_proc->ppid;
}

int sched_gettick(){
    return current_proc->total_ticks;
}

void sched_ps(){
    int i;
    fprintf(stderr, "\n|   pid  |  ppid  |  static  |  dynamic |  ticks |     stack    |  state |  wait_queue  |\n");
    for (i = 1; i < SCHED_NPROC; i++){
        if (proc_queue[i] != NULL){
            fprintf(stderr, "%9d%9d%11d%11d%9d %p",
                    proc_queue[i]->pid,
                    proc_queue[i]->ppid,
                    proc_queue[i]->s_priority,
                    proc_queue[i]->d_priority,
                    proc_queue[i]->total_ticks,
                    proc_queue[i]->stack);
            if (proc_queue[i]->task_state == SCHED_SLEEPING)
                fprintf(stderr, " SLEEPING %p\n", &proc_queue);
            else if (proc_queue[i]->task_state == SCHED_READY)
                fprintf(stderr, "    READY\n");
            else if (proc_queue[i]->task_state == SCHED_RUNNING)
                fprintf(stderr, "  RUNNING\n");
            else if (proc_queue[i]->task_state == SCHED_ZOMBIE)
                fprintf(stderr, "   ZOMBIE\n");
        }
    }
}

void _sched_switch(){
    int i, newpid = 0, nicest = 21;
    sigset_t mask;
    
    // mask signals
    sigemptyset(&mask);
    sigaddset(&mask, SIGVTALRM);
    sigprocmask(SIG_BLOCK, &mask, NULL);

    if (current_proc->task_state == SCHED_RUNNING)
        current_proc->task_state = SCHED_READY;

    // choose a new proces
    for (i = 1; i < SCHED_NPROC; i++){
        if (proc_queue[i] != NULL && proc_queue[i]->task_state == SCHED_READY){
            int p = proc_queue[i]->s_priority + proc_queue[i]->d_priority;
            if (p < nicest){
                nicest = p;
                newpid = i;
            }
            else
                proc_queue[i]->d_priority -= ((19 - proc_queue[i]->s_priority)/10 + 1);
        }
    }

    // save current context and make a context switch
    if (savectx(&(current_proc->ctx)) == 0){ // parent
        current_proc = proc_queue[newpid];
        current_proc->d_priority = 0;
        current_proc->remaining_ticks = (21 - current_proc->s_priority)/4 + 5;
        current_proc->task_state = SCHED_RUNNING;
        sched_ps();
        sigprocmask(SIG_UNBLOCK, &mask, NULL);
        restorectx(&(current_proc->ctx), 1);
        return;
    }

    // child
    sigprocmask(SIG_UNBLOCK, &mask, NULL);
}

void _sched_tick(){
    total_ticks++;
    current_proc->total_ticks++;

    if (--(current_proc->remaining_ticks) == 0){
        _sched_switch();
    }
}

#endif //_SCHED_C_
