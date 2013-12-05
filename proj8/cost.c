/******************************
Sharang Phadke
11/26/2013
ECE 357: OS

hello.c
******************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define BILLION 1000000000L
#define NLOOPS 10000000


void empty(){}

void quick(){
    getpid();
}

int main(int argc, char **argv){
    int i, s;
    double t;
    struct timespec start, stop;

    if(argc !=2){
        fprintf(stderr, "Usage: cost [1 | 2 | 3]\n");
        exit(1);
    }
    s = atoi(argv[1]);

    switch(s){
        case 1:
            clock_gettime(CLOCK_REALTIME, &start);
            for(i = 0; i < NLOOPS; i++){}
            clock_gettime(CLOCK_REALTIME, &stop);
            break;
        case 2:
            clock_gettime(CLOCK_REALTIME, &start);
            for(i = 0; i < NLOOPS; i++)
                empty();
            clock_gettime(CLOCK_REALTIME, &stop);
            break;
        case 3:
            clock_gettime(CLOCK_REALTIME, &start);
            for(i = 0; i < NLOOPS; i++)
                quick();
            clock_gettime(CLOCK_REALTIME, &stop);
            break;
        default:
            fprintf(stderr, "Usage: cost [1 | 2 | 3]\n");
            exit(1);
    }

    t = (stop.tv_sec - start.tv_sec) + (double)(stop.tv_nsec - start.tv_nsec)/(double)BILLION;
    printf("time per loop: %lf ns\n", t*(BILLION/NLOOPS));
    return 0;
}
