/******************************
Sharang Phadke
12/08/2013
ECE 357: OS

main.c
******************************/

#include <stdlib.h>
#include <stdio.h>
#include "sched.h"


void testfn(){
    fprintf(stderr, "test\n");
    switch (sched_fork()){
        case 0:
            fprintf(stderr, "in child\n");
            sched_ps();
        default:
            fprintf(stderr, "in parent\n");
            sched_ps();
    }
    exit(0);
}


int main(int argc, char **argv){
    sched_init(testfn);
    //printf("pid: %d, ppid: %d, cpu_ticks: %l", sched_getpid(), sched_getppid(), sched_gettick());
}
