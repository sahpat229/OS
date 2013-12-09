/******************************
Sharang Phadke
12/08/2013
ECE 357: OS

sched.h
******************************/

#define SCHED_NPROC 256
#define SCHED_READY 1
#define SCHED_RUNNING 2
#define SCHED_SLEEPING 3
#define SCHED_ZOMBIE 4

struct sched_proc {
    //task_state
    //priority
    //accumulated
    //cpu time
    //stack address
    //etc
};

struct sched_waitq {
}
