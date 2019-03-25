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
int format_log_entry(char *logstring, struct sockaddr_in *sockaddr, char *uri, int size);

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
    int flag = 1;
    char * type = malloc(20*sizeof(char));
    char * addr = malloc(500*sizeof(char));
    char * xtra = malloc(200*sizeof(char));
    char * response = malloc(10000*sizeof(char));
    struct sockaddr_in client_addr;
    while(1){
	memset(&client_addr,0, sizeof(client_addr));
	socklen_t addr_size = sizeof(client_addr);
        int connfd = Accept(listenfd, (struct sockaddr*)&client_addr, &addr_size);
    	setsockopt(connfd, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int));
        char buffer[1000];
        int rbytes;
        bzero(type,20*sizeof(char));
        bzero(addr,500*sizeof(char));
        bzero(xtra,200*sizeof(char));
        if((rbytes = recv(connfd, buffer, sizeof(buffer), 0)) <= 0){

        }
	    buffer[rbytes] = '\0';
        sscanf(buffer,"%s %s %s", type, addr, xtra);
        char logstring[1000];
        char * tempAddr = malloc(sizeof(char) * strlen(addr)); 
	char * freeAddr = tempAddr;
	int modified = 0;
        strcpy(tempAddr,addr); 
	if(strstr(tempAddr,"http://") != NULL || strstr(tempAddr,"https://") != NULL){
            tempAddr = strstr(tempAddr,"://") + 3;
	    modified = 1;
    	}
	tempAddr = strtok(tempAddr,"/");
        char * uri = strtok(tempAddr,":");
        char * temp = strtok(NULL,":");
        char * connport = "80";
        if(temp != NULL){
            if(atoi(temp) != 0){
                connport = temp;
            }
        }
	int urilen = strlen(uri) - 1;
	if(uri[urilen] == '/'){
	    uri[urilen] = 0;
	}
        bzero(response,10000*sizeof(char));
        int received = request(uri,connport,buffer,rbytes,response,connfd);
	if(received > 0){
	    int trimlen = format_log_entry(logstring,(struct sockaddr_in*)&client_addr ,addr,received);
	    char trimmedlog[trimlen];
	    strncpy((const char *)&trimmedlog,(const char *)&logstring,trimlen);
            FILE * log = fopen("proxy.log","a+");
            fwrite(trimmedlog , 1 , sizeof(trimmedlog) , log );
            fclose(log);
	}
	close(connfd);
	if(!modified){
            free(tempAddr);
	}
	else{
	    free(freeAddr);
	}
    }
    exit(0);
}

int request(char * addr, char * port, char * req, int size, char * resp, int client){
    struct addrinfo hints, *servinfo, *p;
    int sockfd;
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
        rbytes = read(sockfd, resp, 9999);
        while(rbytes != 0){
	        Write(client,resp,rbytes);
	        bzero(resp, 9999);
	        tbytes += rbytes;
		if(rbytes != 9999){
		    break;
		}
	        rbytes = read(sockfd, resp, 9999); 
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
int format_log_entry(char *logstring, struct sockaddr_in *sockaddr, 
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
   return  sprintf(logstring, "%s: %d.%d.%d.%d %s %d\n", time_str, a, b, c, d, uri,size);
}


