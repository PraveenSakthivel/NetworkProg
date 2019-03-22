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

/*
 * Function prototypes
 */
void format_log_entry(char *logstring, struct sockaddr_in *sockaddr, char *uri, int size);

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
        int size = 100;
        int loc = 0;
        int rbytes;
        char type[20];
        char addr[500];
        char xtra[200];
        if((rbytes = recv(connfd, buffer, sizeof(buffer), 0)) <= 0){

        }
        sscanf(buffer,"%s %s %s", type, addr, xtra);
        char logstring[1000];
        format_log_entry(logstring,serv_addr.sin_addr.s_addr,addr,rbytes);
        FILE * log = fopen("proxy.log","a+");
        fwrite(logstring , 1 , sizeof(log) , log );
        fclose(log);
        buffer[999] = '\n';
        printf("\n%s",buffer);
    }
    exit(0);
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


