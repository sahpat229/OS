/******************************
 * 
 * Sharang Phadke
 * 10/30/2013
 * ECE 357: OS
 * Project 4 - catgrepmore
 * 
 *****************************/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <string.h>
#include <errno.h>

#define BUFFSIZE 1024

int errsv;
int numFiles = 0;
long numBytes = 0;

//sigintHandler
//  function to print number of files and bytes processed before exiting
void sigintHandler(int sig){
    fprintf(stderr,"Program interrupted (SIGINT)\n Processed %d files and %ld bytes\n",numFiles,numBytes);
    exit(-1);
}

//sigpipeHandler
//  function to indicate broken pipes
void sigpipeHandler(int sig){
    fprintf(stderr,"Program interrupted (SIGPIPE)\n");
}

//errorReport
//  function to translate errno and report errors to stderr
void errorReport(int errnum, char description[], char pathname[]){
    fprintf(stderr, "%s", strerror(errnum));
    fprintf(stderr, "\n");
    fprintf(stderr, "%s", description);
    fprintf(stderr, "%s", pathname);
    fprintf(stderr, "\n");
}

//processFile
//  Function to process each input file by opening the file
//  and performing necessary redirections to grep and more
//  and reporting errors as needed
void processFile(char *pattern, char *pathname){
    pid_t pid;
    int infd, rbytes, wbytes, pgrep[2], pmore[2];
    char *buffer = malloc(BUFFSIZE*sizeof(char));
    if(buffer == NULL){
        perror("Malloc error");
        exit(-1);
    }

    if((pipe(pgrep) != 0) || (pipe(pmore) != 0)){
        perror("Error making pipes");
        exit(-1);
    }
    
    pid = fork();
    switch(pid){
        case -1:
            perror("failed to fork for grep");
            exit(-1);
        case 0: //grep
            //close the dangling fds
            if((close(pgrep[1]) < 0) || (close(pmore[0]) < 0)){
                errsv = errno;
                errorReport(errsv, "Error occured while closing file descriptors",NULL);
                exit(-1);
            }

            //redirect stdin to the first pipe
            //and stdout to the second pipe
            if((dup2(pgrep[0],0) < 0) || (dup2(pmore[1],1) < 0)){
                perror("dup2 error in grep process");
                exit(-1);
            }

            //close the redirected fds
            if((close(pgrep[0] < 0) || (close(pmore[1]) < 0))){
                errsv = errno;
                errorReport(errsv, "Error occured while closing file descriptors",NULL);
                exit(-1);
            }

            execlp("grep","grep",pattern,NULL);
            break;
        default:
            pid = fork();
            switch(pid){
                case -1:
                    perror("failed to fork for more");
                    exit(-1);
                case 0: //more
                    //close the dangling fds
                    if((close(pmore[1] < 0)) || (close(pgrep[0]) < 0) || (close(pgrep[1]) < 0)){
                        errsv = errno;
                        errorReport(errsv, "Error occured while closing file descriptors",NULL);
                        exit(-1);
                    }

                    //redirect stdin to the second pipe
                    if(dup2(pmore[0],0) < 0){
                        perror("dup2 error in more process");
                        exit(-1);
                    }

                    //close the pipe fd
                    if(close(pmore[0]) < 0){
                        errsv = errno;
                        errorReport(errsv, "Error occured while closing file descriptors",NULL);
                        exit(-1);
                    }

                    execlp("more","more",NULL);
                    break;
                default: //parent
                    //close the dangling fds
                    if((close(pmore[0]) < 0) || (close(pmore[1] < 0)) || (close(pgrep[0]) < 0)){
                        errsv = errno;
                        errorReport(errsv, "Error occured while closing file descriptors",NULL);
                        exit(-1);
                    }

                    //open infile
                    if((infd = open(pathname, O_RDONLY)) < 0){
                        errsv = errno;
                        errorReport(errsv, "Error occured while opening file: ", pathname);
                        exit(-1);
                    }

                    //read from infd, write to buffer, and report errors
                    while((rbytes = read(infd, buffer, BUFFSIZE)) > 0){
                        wbytes = write(pgrep[1], buffer, rbytes);
                        numBytes += wbytes;

                        //write errors
                        errsv = errno;
                        if(wbytes < 0){
                            errorReport(errsv, "Error occured while writing to output", NULL);
                            exit(-1);
                        }
                        else if(wbytes != rbytes){
                            errorReport(errsv, "Partial write occured while writing to output", NULL);
                            exit(-1);
                        }
                    }

                    //read errors
                    if(rbytes < 0){
                        errorReport(rbytes, "Error occured while reading file: ", pathname);
                        exit(-1);
                    }

                    //close infile and pipe
                    if((close(pgrep[1]) < 0) || (close(infd) < 0)){
                        errsv = errno;
                        errorReport(errsv, "Error occured while closing file: ", pathname);
                        exit(-1);
                    }

                    if(wait(NULL) == -1){
                        errsv = errno;
                        errorReport(errsv,"Error waiting in parent 1",NULL);
                        exit(-1);
                    }
                    break;
            }
            if(wait(NULL) == -1){
                errsv = errno;
                errorReport(errsv,"Error waiting in parent 2",NULL);
                exit(-1);
            }
            break;
    }
}


int main (int argc, char **argv){
    int i;
	char *pattern = argv[1];
    char **infile = NULL;

    signal(SIGINT,sigintHandler);
    signal(SIGPIPE,sigpipeHandler);

    //loop through input files
    for(i = 2; i < argc; i++){
        processFile(pattern, argv[i]);
        numFiles++;
    }

	return 0;
}
