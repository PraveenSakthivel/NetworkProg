/*
 * proxy.c - A Simple Sequential Web proxy
 *
 * Course Name: 14:332:456-Network Centric Programming
 * Assignment 2
 * Student Name:______________________
 * 
 * IMPORTANT: Give a high level description of your code here. You
 * must also provide a header comment at the beginning of each
 * function that describes what that function does.
 */ 

#include "csapp.h"
#include <netinet/tcp.h>
/*
 * Function prototypes
 */
void format_log_entry(char *logstring, struct sockaddr_in *sockaddr, char *uri, int size);

int request(char * addr, char * port, char * req,int size, char * resp, int client);
/* 
 * main - Main routine for the proxy program 
 */
int main(int argc, char **argv)
{
    /* Check arguments */
    if (argc != 2) {
	    fprintf(stderr, "Usage: %s <port number>\n", argv[0]);
	    exit(0);
    }
    int port = atoi(argv[1]);
    struct sockaddr_in serv_addr;
    int listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);
    Bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    Listen(listenfd, 10);
    while(1){
        int connfd = Accept(listenfd, (struct sockaddr*)NULL, NULL);
        char buffer[1000];
        int rbytes;
        char * type = calloc(1,20);
        char * addr = calloc(1,500);
        char * xtra = calloc(1,200);
        if((rbytes = recv(connfd, buffer, sizeof(buffer), 0)) <= 0){

        }
	    buffer[rbytes] = '\0';
        sscanf(buffer,"%s %s %s", type, addr, xtra);
        char logstring[1000];
        format_log_entry(logstring,(struct sockaddr_in*)&serv_addr ,addr,rbytes);
	    if(strstr(addr,"http://") != NULL || strstr(addr,"https://") != NULL){
            addr = strstr(addr,"://") + 3;
    	}
        char * tempAddr = malloc(sizeof(char) * strlen(addr));
        strcpy(tempAddr,addr);
        char * uri = strtok(tempAddr,":");
        char * temp = strtok(NULL,":");
        char * connport = "80";
        if(temp != NULL){
            if(atoi(temp) != 0){
                connport = temp;
            }
        }
        char * response = calloc(1,10000);
        request(uri,connport,buffer,rbytes,response,connfd);
        FILE * log = fopen("proxy.log","a+");
        fwrite(logstring , 1 , sizeof(log) , log );
        fclose(log);
        free(type);
        free(addr);
        free(xtra);
        free(response);
        free(tempAddr);
    }
    exit(0);
}

int request(char * addr, char * port, char * req, int size, char * resp, int client){
    struct addrinfo hints, *servinfo, *p;
    int sockfd, bytes, sent, received, total;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    getaddrinfo(addr,port,&hints,&servinfo);
    int flag = 1;
    setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int));
    int tbytes = 0;
    for(p = servinfo; p != NULL; p = p->ai_next) {
    	if ((sockfd = socket(p->ai_family, p->ai_socktype,
            p->ai_protocol)) == -1) {
            continue;
    	}
    	if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            continue;
    	}
        Write(sockfd,req,size);
        int rbytes = 0;
        rbytes = recv(sockfd, resp, 9999, 0);
        while(rbytes != 0){
	        printf("%s",resp);
	        bzero(resp, 9999);
	        tbytes += rbytes;
	        rbytes = recv(sockfd, resp, 9999, 0); 
        }
	    break;
    }
    if(p == NULL){
	    printf("Unable to Connect");
    }
    close(sockfd);
    freeaddrinfo(servinfo);
    return tbytes;
}

/*
 * format_log_entry - Create a formatted log entry in logstring. 
 * 
 * The inputs are the socket address of the requesting client
 * (sockaddr), the URI from the request (uri), and the size in bytes
 * of the response from the server (size).
 */
void format_log_entry(char *logstring, struct sockaddr_in *sockaddr, 
		      char *uri, int size)
{
    time_t now;
    char time_str[MAXLINE];
    unsigned long host;
    unsigned char a, b, c, d;

    /* Get a formatted time string */
    now = time(NULL);
    strftime(time_str, MAXLINE, "%a %d %b %Y %H:%M:%S %Z", localtime(&now));

    /* 
     * Convert the IP address in network byte order to dotted decimal
     * form. Note that we could have used inet_ntoa, but chose not to
     * because inet_ntoa is a Class 3 thread unsafe function that
     * returns a pointer to a static variable (Ch 13, CS:APP).
     */
    host = ntohl(sockaddr->sin_addr.s_addr);
    a = host >> 24;
    b = (host >> 16) & 0xff;
    c = (host >> 8) & 0xff;
    d = host & 0xff;


    /* Return the formatted log entry string */
    sprintf(logstring, "%s: %d.%d.%d.%d %s", time_str, a, b, c, d, uri);
}


