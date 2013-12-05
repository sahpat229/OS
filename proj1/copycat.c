/******************************
Sharang Phadke
9/20/2013
ECE 357: OS

copycat

******************************/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>


//ioErrorReport
//  function to report I/O errors to stderr
void ioErrorReport(int errnum, char description[], char pathname[])
{
    fprintf(stderr, "%s", strerror(errnum));
    fprintf(stderr, "\n");
    fprintf(stderr, "%s", description);
    fprintf(stderr, "%s", pathname);
    fprintf(stderr, "\n");
}


//processFile
//  Function to process each input file, including
//  reading, writing to correct output, and
//  reporting errors
void processFile(char *pathname, int outfd, char *buffer, int buffsize)
{
    int infd, rbytes, wrbytes, errsv;

    if(*pathname == '-')
        infd = 0;
    else if((infd = open(pathname, O_RDONLY)) < 0)
    {
        //open errors
        errsv = errno;
        ioErrorReport(errsv, "Error occured while opening file: ", pathname);
        exit(-1);
    }

    while((rbytes = read(infd, buffer, buffsize)) > 0)
    {
        wrbytes = write(outfd, buffer, rbytes);

        //write errors
        errsv = errno;
        if(wrbytes < 0)
        {
            ioErrorReport(errsv, "Error occured while writing to output", NULL);
            exit(-1);
        }
        else if(wrbytes != rbytes)
        {
            ioErrorReport(errsv, "Partial write occured while writing to output", NULL);
            exit(-1);
        }
    }

    //read errors
    if(rbytes < 0)
    {
        ioErrorReport(rbytes, "Error occured while reading file: ", pathname);
        exit(-1);
    }

    if(infd != 0)
        close(infd);
}


int main (int argc, char **argv)
{
	int buffsize = 1024;
    int i, c, infd, outfd, errsv;
	char *outfile = NULL;
    char *buffer = NULL;
    char infiles[argc];

    //initialize array to 0s
    for(i = 0; i < argc; i++)
        infiles[i] = 0;

    //extract options and arguments
	opterr = 0;
	while ((c = getopt(argc, argv, "-b:o:")) != -1)
	{
		switch (c)
		{
			case 'b':
				buffsize = atoi(optarg);
				break;
			case 'o':
				outfile = optarg;
				break;
            case 1:
                //track infiles by index in argv
                infiles[optind] = 1;
                break;
			case '?':
				if (optopt == 'o' || optopt == 'b')
					fprintf (stderr, "Option -%c requires an argument.\n", optopt);
				else if (isprint (optopt))
					fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				else
					fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
				exit(-1);
		}
	}

    //allocate a buffer
    buffer = malloc(buffsize*sizeof(char));
    if(buffer == NULL)
    {
        fprintf(stderr,"Could not malloc a buffer of size %d\n", buffsize);
        exit(-1);
    }

    //get a file descriptor to the output stream
    if(outfile != NULL)
    {
        if((outfd = open(outfile, O_TRUNC | O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH)) < 0)
        {
            errsv = errno;
            ioErrorReport(errsv, "Error occured while opening file: ", outfile);
            exit(-1);
        }
    }
    else
        outfd = 1; //stdout

    //loop through input files
    char foundInfile = 0;
    for(i = 1; i <= argc; i++)
    {
        if(infiles[i] == 1)
        {
            foundInfile = 1;
            processFile(argv[i-1], outfd, buffer, buffsize);
        }
    }

    //if no infiles are found, use stdin
    if(!foundInfile)
    {
        char defaultInfile = '-';
        processFile(&defaultInfile, outfd, buffer, buffsize);
    }

    close(outfd);
	return 0;
}
