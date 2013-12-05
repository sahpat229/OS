/******************************
Sharang Phadke
11/10/2013
ECE 357: OS

udp_client
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


int main (int argc, char **argv)
{
	const int buffsize = 1024;
    int sockfd = 0;
    int portNum, status, rbytes;
	char *hostname, *port, *request_string, buffer[buffsize];
    struct sockaddr_in s;
    struct addrinfo h, *sinfo, *p;

    if(argc != 4)
        handle("Usage: udp_client hostname port request_string\n");

    hostname = argv[1];
    port = argv[2];
    request_string = argv[3];
    portNum = atoi(port);

    memset(buffer,0,sizeof(buffer));

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        handle("Error making the socket\n");
    
    s.sin_family = AF_INET;
    s.sin_port = htons(portNum);
    s.sin_addr.s_addr = INADDR_ANY;

    memset(&h, 0, sizeof(h));
    h.ai_family = AF_UNSPEC;
    h.ai_socktype = SOCK_DGRAM;

    if(getaddrinfo(hostname, port, &h, &sinfo) != 0)
        handle("Error getting addrinfo\n");

    // loop through all the results and connect to the first we can
    for(p = sinfo; p != NULL; p = p->ai_next){
        if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
            perror("UDP Client: socket error");
            continue;
        }
        break;
    }

    if(p == NULL)
        handle("UDP Client: could not connect to server\n");

    sendto(sockfd, request_string, strlen(request_string), 0,
            (struct sockaddr *)&s, sizeof(s));
    
    int slen = sizeof(s);
    if((rbytes = recvfrom(sockfd, buffer, sizeof(buffer)-1, 0,
                (struct sockaddr *)&s, &slen)) < 0)
        perror("UDP Client: error in recvfrom\n");

    fprintf(stderr,"%s\n",buffer);

    if(shutdown(sockfd, 2) < 0) //close both sides of socket
        handle("UDP Client: error closing UDP connection\n");

	return sockfd;
}
