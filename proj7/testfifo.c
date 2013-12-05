/******************************
* Sharang Phadke
* 11/15/2013
* ECE 357: OS
* 
* Project 7 - semaphores and locking
* 
* testfifo
******************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "sem.h"
#include "fifo.h"


void handle_error(const char *e){
    perror(e);
    exit(-1);
}

int main(int argc, char **argv){
    int i, j, val, pid, nwords, nwriters;
    int total = 0;
    struct fifo *f;

    if(argc != 3)
        handle_error("Usage: testfifo nwords nwriters\n");

    nwords = atoi(argv[1]);
    nwriters = atoi(argv[2]);

    f = mmap(0, sizeof(struct fifo), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if(f == NULL){
        perror("mmap error");
        exit(-1);
    }

    fifo_init(f);
    my_procnum = 0;

    //fork writers and write nwords unique, sequential data words each
    for(i = 1; i < nwriters+1; i++){
        pid = fork();
        switch(pid){
            case -1:
                handle_error("Error with fork");
            case 0: //writers
                my_procnum = i;
                int start = i*nwords;
                for(j = start; j < start+nwords; j++){
                    fprintf(stderr, "process %d writing %d\n", i, j);
                    fifo_wr(f, j);
                }
                exit(0);
        }
    }

    //read all the data
    for(i = 0; i < nwriters; i++){
        for(j = 0; j < nwords; j++){
           val = fifo_rd(f);
           fprintf(stderr, "read %d\n", val);
           total++;
        }
    }

    printf("total received: %d\n", total);

    return 0;
}
