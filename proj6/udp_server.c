/******************************
Sharang Phadke
11/10/2013
ECE 357: OS

udp_server
******************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/sysinfo.h>
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
    int sockfd = 0, connfd = 0, totalbytes = 0;
    int portNum, status, rbytes, wrbytes, isIPV4;
	char *port, buffer[buffsize], resp[buffsize];
    struct sockaddr_in s, c;
    struct sockaddr_storage client;
    struct hostent *host;
    struct addrinfo h, *sinfo, *p;
    struct sysinfo info;

    if(argc != 2)
        handle("Usage: udp_server port...\n");

    port = argv[1];
    portNum = atoi(port);

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        handle("Error making the socket\n");
    
    s.sin_family = AF_INET;
    s.sin_port = htons(portNum);
    s.sin_addr.s_addr = INADDR_ANY;

    if(bind(sockfd, (struct sockaddr*)&s, sizeof(s))){
        close(sockfd);
        handle("Error binding socket\n");
    }
   
    fprintf(stderr, "UDP Server listening on port %d\n", portNum);

    socklen_t addrlen = sizeof(s);
    while(1){
        memset(buffer,0,sizeof(buffer));
        if((rbytes = recvfrom(sockfd, buffer, sizeof(buffer), 0,
                    (struct sockaddr *)&s, &addrlen)) < 0)
            perror("UDP Server: error in recvfrom\n");

        fprintf(stderr,"%s\n",buffer);

        memset(resp,0,sizeof(resp));
        if(!strcmp(buffer,"UPTIME")){
            if(sysinfo(&info) < 0)
                handle("UDP Server: could not get sysinfo!\n");
            sprintf(resp,"%ld\n",info.uptime);
        }
        else if(!strcmp(buffer,"DATE")){
            time_t t = time(NULL);
            ctime_r(&t,resp);
        }

        sendto(sockfd, resp, strlen(resp), 0,
                (struct sockaddr *)&s, sizeof(s));
    }
    
    if(shutdown(sockfd, 2) < 0) //close both sides of socket
        handle("UDP Server: error closing UDP connection\n");

	return sockfd;
}
