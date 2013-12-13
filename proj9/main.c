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

int t = 0;

void testfn(){
    fprintf(stderr, "test\n");
    switch (sched_fork()){
        case 0:
            fprintf(stderr, "YO PID %d IS RUNNING\n", sched_getpid());
            t = 1;
            sched_exit(0);
            while(1) { fprintf(stderr, "THIS PROCESS SHOULD BE DEAD!\n");}
        default:
            fprintf(stderr, "YO PID %d IS RUNNING\n", sched_getpid());
            fprintf(stderr, "t = %d\n", t);
            while(1) {}
    }
}


int main(int argc, char **argv){
    sched_init(testfn);
    return 0;
}
