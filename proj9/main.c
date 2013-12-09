/******************************
Sharang Phadke
12/08/2013
ECE 357: OS

main.c
******************************/

#include <stdio.h>
#include "sched.h"

int main(int argc, char **argv){
    void * tits = NULL;
    sched_init(tits);
    printf("pid: %d, ppid: %d, cpu_ticks: %l", sched_getpid(), sched_getppid(), sched_gettick());
}
