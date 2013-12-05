/******************************
* Sharang Phadke
* 11/10/2013
* ECE 357: OS
* 
* Project 6 - sockets
* 
* tcp_recv
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
void *getIpAddr(struct sockaddr *sa, int *isIPV4)
{
    if(sa->sa_family == AF_INET){
        return &(((struct sockaddr_in*)sa)->sin_addr);
        *isIPV4 = 1;
    }
    *isIPV4 = 0;
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


int main (int argc, char **argv)
{
	int buffsize = 1024;
    int sockfd = 0, connfd = 0, totalbytes = 0;
    int portNum, status, rbytes, wrbytes, isIPV4;
	char *hostname, *port, *buffer, client_addr[INET6_ADDRSTRLEN];
    clock_t t;
    struct sockaddr_in s;
    struct sockaddr_storage client;
    struct hostent *host;
    struct addrinfo h, *sinfo, *p;

    if(argc != 3)
        handle("Usage: tcp_recv hostname port...\n");

    hostname = argv[1];
    port = argv[2];
    portNum = atoi(port);

    //allocate a buffer
    buffer = malloc(buffsize*sizeof(char));
    if(buffer == NULL)
        handle("Could not malloc buffer\n");

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        handle("Error making the socket\n");
    
    s.sin_family = AF_INET;
    s.sin_port = htons(portNum);
    s.sin_addr.s_addr = INADDR_ANY;

    if(bind(sockfd, (struct sockaddr*)&s, sizeof(s))){
        close(sockfd);
        handle("Error binding socket\n");
    }

    if(listen(sockfd, 128) < 0)
        handle("Error with listening\n");

    int len = sizeof(s);
    if((connfd = accept(sockfd, (struct sockaddr*)&client, &len)) < 0)
        handle("Error with accept\n");

    void * ip = getIpAddr((struct sockaddr *)&client, &isIPV4);
    inet_ntop(client.ss_family, ip, client_addr, sizeof(client_addr));
    
    if(isIPV4)
        host = gethostbyaddr((struct in_addr *)ip, sizeof(ip), AF_INET);
    else
        host = gethostbyaddr((struct in6_addr *)ip, sizeof(ip), AF_INET6);
        
    if(host == NULL){
        perror("Couldn't reverse DNS lookup host\n");
        fprintf(stderr, "Established connection with host on %s:%s\n",client_addr, port);
    }
    else
        fprintf(stderr, "Established connection with %s on %s:%s\n", host->h_name, client_addr, port);


    t = clock();
    errno = 0;
    while((rbytes = read(connfd, buffer, sizeof(buffer))) > 0){
        if((wrbytes = write(1, buffer, rbytes)) == -1)
            handle("Error sending to server\n");

        //write errors
        errsv = errno;
        if(wrbytes < 0)
        {
            ioErrorReport(errsv, "Server: error occured while writing to stdout", NULL);
            exit(-1);
        }
        totalbytes += wrbytes;
        if(wrbytes != rbytes)
        {
            ioErrorReport(errsv, "Server: partial write occured while writing to stdout", NULL);
            status = 1;
        }
    }
    t = clock() - t;

    //read errors
    if(rbytes < 0)
        handle("Server: error occured while reading from socket\n");

    if(close(connfd) < 0)
        handle("Server: error closing socket\n");

    fprintf(stderr,"Received %d bytes at a rate of %f MB/s\n",
            totalbytes,
            totalbytes/(1000000*((double)t)/CLOCKS_PER_SEC));

	return connfd;
}
