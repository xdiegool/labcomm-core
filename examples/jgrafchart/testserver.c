#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#include "dec.h"
#include "enc.h"

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void do_labcomm(int sockfd)
{
    void *enc = enc_init(sockfd);
    enc_run(enc);
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

    do_labcomm(newsockfd);

    close(newsockfd);
    close(sockfd);
    return 0;
}
