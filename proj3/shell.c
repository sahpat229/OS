/********************************
*
* Sharang Phadke
* 10/14/2013
* ECE 357: OS
* Project 3 - shell
*
* ********************************/

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
#include <time.h>

#define LINESIZE 80

int errsv;


//errorReport
//  function to report errors to stderr
void errorReport(int errnum, char description[], char source[])
{
    fprintf(stderr, "%s", strerror(errnum));
    fprintf(stderr, "\n");
    fprintf(stderr, "%s", description);
    fprintf(stderr, "%s", source);
    fprintf(stderr, "\n");
}


//ioRedir
//  function to parse and execute I/O redirection
void ioRedir(char *redirStr)
{
    int f1, fd, d;
    char *redir = NULL;
    char *f2;

    //parse the redirection string
    char *ind1 = index(redirStr,'>');
    char *rind1 = rindex(redirStr,'>');
    char *rind2 = rindex(redirStr,'<');
    f1 = (strcspn(redirStr,"<>") > 0)
        ? redirStr[0]
        : (redirStr[0] == '>')
        ? 1
        : 0;
    f2 = (rind2 != NULL)
        ? rind2+1
        : rind1+1;
    redir = (rind2 != NULL)
        ? strndup(rind2,1)
        : strndup(ind1,rind1-ind1+1);
    
    //open the file to redirect to/from
    if(strcmp(redir,"<") == 0)
        fd = open(f2,O_RDONLY);
    else if(strcmp(redir,">") == 0)
        fd = open(f2,O_TRUNC|O_CREAT|O_WRONLY,S_IRUSR|S_IWUSR);
    else
        fd = open(f2,O_APPEND|O_CREAT|O_WRONLY,S_IRUSR|S_IWUSR);
    if(fd < 0)
    {
        errsv = errno;
        errorReport(errsv,"Error opening file: ",f2);
        exit(-1);
    }

    //perform file struct alterations
    if(dup2(fd,f1) < 0)
    {
        errsv = errno;
        errorReport(errsv,"Error dup2ing file: ",f2);
        exit(-1);
    }

    //close the dangling fd
    if(close(fd) < 0)
    {
        errsv = errno;
        errorReport(errsv,"Error closing file: ",f2);
        exit(-1);
    }
}


//runCmd
//  function to fork and exec a command while the current
//  (parent) process waits for a return value
//  performs I/O redirection if specified
void runCmd(int ncmd, char *cmd[], int nio, char *io[])
{
    int status, i;
    struct rusage ru;

    pid_t pid = fork();
    switch(pid)
    {
        case -1:
            errsv = errno;
            errorReport(errsv,"Failed to fork on command: ",cmd[0]);
            exit(-1);
            break;
        case 0:
            for(i = 0; i < nio; i++)
                ioRedir(io[i]);

            fprintf(stderr,"Executing command %s with arguments \"",cmd[0]);
            for(i = 1; i < ncmd; i++)
                fprintf(stderr," %s",cmd[i]);
            fprintf(stderr," \" \n");
            execvp(cmd[0],cmd);
            break;
        default:
            if(wait3(&status,0,&ru) == -1)
            {
                errsv = errno;
                errorReport(errsv,"Error with wait3",NULL);
                exit(-1);
            }
            else
            {
                //print exit status and real, user, and system time
                fprintf(stderr,
                        "Child process %d exited with status %d and consumed:\n\t%ld.%.6ld seconds of real time\n\t%ld.%.6ld seconds of user time\n\t%ld.%.6ld seconds of system time\n",
                        pid,
                        status,
                        ru.ru_utime.tv_sec + ru.ru_stime.tv_sec,
                        ru.ru_utime.tv_usec + ru.ru_stime.tv_usec,
                        ru.ru_utime.tv_sec, ru.ru_utime.tv_usec,
                        ru.ru_stime.tv_sec, ru.ru_stime.tv_usec);
            }
            break;
    }
}


int main (int argc, char **argv)
{
    char in[LINESIZE];

    //set input file
    FILE *fp = (argv[1] == NULL)
        ? stdin
        : fopen(argv[1],"r");
    if(fp == NULL)
    {
        errsv = errno;
        errorReport(errsv,"Error opening input file: ", argv[1]);
        exit(-1);
    }

    //parse and execute each line of input
    while(fgets(in,sizeof(in),fp) != NULL)
    {
        int pr;
        char *pch;
        int ncmd = 0;
        int nio = 0;
        char **cmd = malloc(sizeof(char*)*LINESIZE);
        char **io = malloc(sizeof(char*)*LINESIZE);
        if(cmd == NULL || io == NULL)
        {
            errsv = errno;
            errorReport(errsv,"Malloc failed",NULL);
            exit(-1);
        }

        //parse input line and attempt to run command
        if((pch = strtok(in," \t\n")) == NULL)
            continue; //skip empty commands
        if(*pch == '#')
            continue; //skip comments
        while(pch != NULL)
        {
            if(strcspn(pch,"<>") < (int)strlen(pch))
                io[nio++] = strdup(pch);
            else
                cmd[ncmd++] = strdup(pch);
            pch = strtok(NULL, " \t\n");
        }
        cmd[ncmd] = NULL;
        if(realloc(cmd,sizeof(char*)*(ncmd+1)) == NULL)
        {
            errsv = errno;
            errorReport(errsv,"Realloc failed",NULL);
            exit(-1);
        }
        runCmd(ncmd,cmd,nio,io);
        free(cmd);
        free(io);
    }

    if(fp != stdin)
        if(fclose(fp) != 0)
        {
            errsv = errno;
            errorReport(errsv,"Error closing input file: ",argv[1]);
            exit(-1);
        }
	return 0;
}
