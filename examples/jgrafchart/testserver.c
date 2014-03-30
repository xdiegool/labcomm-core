#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#include "jg.h"
#include "dec.h"
#include "enc.h"

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void do_labcomm(int sockfd, jg_foo *v)
{
    void *enc = enc_init(sockfd);
    int i;
    for(i=0; i<10;i++) {
        enc_run(enc, v);
    }
    enc_cleanup(enc);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    int newsockfd, clilen;
    struct sockaddr_in serv_addr, cli_addr;
    struct hostent *server;

    char buffer[256];
    if (argc < 2) {
       fprintf(stderr,"usage %s port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[1]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    int so_reuseaddr = 1; //TRUE;
    if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR, &so_reuseaddr, sizeof so_reuseaddr)) {
        error("ERROR setting socket options");
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }

    listen(sockfd,2);
    clilen = sizeof(cli_addr);

   /* Accept actual connection from the client */
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (newsockfd < 0) {
        perror("ERROR on accept");
       exit(1);
    }
    
    jg_foo v;

    v.b = 17.17;
    v.c = 1742;
    v.d = 4217;
    v.e = "hello";
    v.f = 17;
    v.g = 42;
    v.h = 2;
    v.i = 42.42;

    do_labcomm(newsockfd, &v);

    close(newsockfd);
    close(sockfd);
    return 0;
}
