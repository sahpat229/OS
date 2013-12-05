/******************************
* Sharang Phadke
* 11/10/2013
* ECE 357: OS
* 
* Project 6 - sockets
* 
* tcp_send
******************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>

#define handle(msg) \
    do{ perror(msg); exit(EXIT_FAILURE); } while(0)

extern int h_errno;
int errsv;

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

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if(sa->sa_family == AF_INET)
        return &(((struct sockaddr_in*)sa)->sin_addr);
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


int main (int argc, char **argv)
{
	int buffsize = 1024;
    int sockfd = 0, status = 0, totalbytes = 0;
    int portNum, rv, rbytes, wrbytes;
	char *hostname, *port, *buffer;
    clock_t t;
    struct sockaddr_in s;
    struct linger so_linger;
    struct addrinfo h, *sinfo, *p;

    if(argc != 3)
        handle("Usage: tcp_send hostname port\n");

    hostname = argv[1];
    port = argv[2];
    portNum = atoi(port);

    so_linger.l_onoff = 1;
    so_linger.l_linger = 5;

    //allocate a buffer
    buffer = malloc(buffsize*sizeof(char));
    if(buffer == NULL)
        handle("Could not malloc buffer\n");

    memset(&h, 0, sizeof(h));
    h.ai_family = AF_UNSPEC;
    h.ai_socktype = SOCK_STREAM;

    if((rv = getaddrinfo(hostname, port, &h, &sinfo)) != 0)
        handle("Error getting addrinfo\n");

    // loop through all the results and connect to the first we can
    for(p = sinfo; p != NULL; p = p->ai_next){
        if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
            perror("client: socket error");
            continue;
        }
        if(setsockopt(sockfd, SOL_SOCKET, SO_LINGER, &so_linger, sizeof(so_linger)) == -1)
            handle("Client: setsocopt error\n");
        if(connect(sockfd, p->ai_addr, p->ai_addrlen) == -1){
            close(sockfd);
            perror("client: connect error");
            continue;
        }
        break;
    }

    if(p == NULL)
        handle("Could not connect client\n");

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), hostname, sizeof(hostname));
    fprintf(stderr,"Client: successfully connected to %s\n", hostname);

    freeaddrinfo(sinfo); // all done with this structure

    errno = 0;
    t = clock();  
    while((rbytes = read(0, buffer, buffsize)) > 0)
    {
        if((wrbytes = send(sockfd, buffer, rbytes, 0)) == -1)
            handle("Error sending to server\n");

        //write errors
        errsv = errno;
        if(wrbytes < 0)
        {
            ioErrorReport(errsv, "Client: error occured while sending", NULL);
            exit(-1);
        }
        totalbytes += wrbytes;
        if(wrbytes != rbytes)
        {
            ioErrorReport(errsv, "Client: partial write occured while sending", NULL);
            status = 1;
        }
    }
    t = clock() - t;

    //read errors
    if(rbytes < 0)
        handle("Error occured while reading input\n");

	if(close(sockfd) < 0){
        errsv = errno;
        ioErrorReport(errsv, "Error closing socket", NULL);
        status = 2;
    }

    fprintf(stderr,"Transfered %d bytes at a rate of %f MB/s\n",
            totalbytes,
            totalbytes/(1000000*((double)t)/CLOCKS_PER_SEC));

    return status;
}
