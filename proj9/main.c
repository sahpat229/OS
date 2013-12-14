/******************************
Sharang Phadke
12/08/2013
ECE 357: OS

main.c
******************************/

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "sched.h"


void testfn(){
    int pid1, pid2;
    int retval = 0;
    double c1 = 0, c2 = 0;

    fprintf(stderr, "PID %d START\n", sched_getpid());

    switch (pid1 = sched_fork()){
        case 0:
            fprintf(stderr, "PID %d IS START\n", sched_getpid());
            switch (pid2 = sched_fork()){
                case 0:
                    pid2 = sched_getpid();
                    fprintf(stderr, "PID %d IS START\n", sched_getpid());
                    sched_nice(10);
                    for(c2 = 0; c2 < 1E10; c2 += 1) {}
                    sched_exit(2);
                    
                default:
                    pid1 = sched_getpid();
                    sched_nice(-10);
                    for(c1 = 0; c1 < 5E9; c1 += 1) {}
                    sched_exit(1);
            }
   
        default:
            sched_wait(&retval);
            fprintf(stderr, "pid %d returned with %d\n", pid1, retval);
            while(1) {}
    }
}


int main(int argc, char **argv){
    sched_init(testfn);
    return 0;
}
