/******************************
* Sharang Phadke
* 11/15/2013
* ECE 357: OS
* 
* Project 7 - semaphores and locking
* 
* testsem
******************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "sem.h"

void handle_error(const char *e){
    perror(e);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv){
    int pid, pcnt, *t;
    struct sem *s;

    t = mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if(t == NULL)
        handle_error("mmap error");
    
    s = mmap(0, sizeof(struct sem), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if(s == NULL)
        handle_error("mmap error");
    sem_init(s, 1);

    my_procnum = 0;
    for(pcnt = 1; pcnt < 10; pcnt++){
        pid = fork();
        switch(pid){
            case -1:
                handle_error("Error with fork");
            case 0:
                printf("parent my_procnum: %d pid: %d\n", my_procnum, pid);
                break;
            default:
                my_procnum = pcnt;
                printf("child my_procnum: %d pid: %d\n", my_procnum, pid);
                sem_wait(s);
                printf("t: %d\n", ++(*t));
                sem_inc(s);
        }
    }

    return 0;
}
