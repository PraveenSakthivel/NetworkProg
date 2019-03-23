#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/tcp.h>

int main()
{
    char * addr = "www.google.com";
    int port = 80;
    char * req = "GET  HTTP/1.1\nHost: www.google.com\ncache-control: no-cache\n";
    char * resp = malloc(10000);
    int size = strlen(req);
    struct addrinfo hints, *servinfo, *p;
    int sockfd, bytes, sent, received, total;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    getaddrinfo(addr,"80",&hints,&servinfo);
    int flag = 1;
    setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int));
    for(p = servinfo; p != NULL; p = p->ai_next) {
    	if ((sockfd = socket(p->ai_family, p->ai_socktype,
            p->ai_protocol)) == -1) {
            continue;
    	}
    	if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            continue;
    	}
    	printf("%d",write(sockfd,req,size));
    	while(read(sockfd, resp, 9999) != 0){
	    fprintf(stderr, "%s", resp);
	    bzero(resp, 9999);
    	}
	break;
    }
    if(p == NULL){
	printf("Unable to Connect");
    }
    close(sockfd);
    freeaddrinfo(servinfo);
    return 0;
}
