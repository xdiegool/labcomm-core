#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFLEN 512
#define NPACK 10
#define PORT 9930

void diep(char *s)
{
  perror(s);
  exit(1);
}

typedef struct {
	struct sockaddr_in si_me, si_other;
	int s, i, slen;
	char buf[BUFLEN];
} server_context_t;

server_context_t* server_init(void)
{
  server_context_t* ctx;
  ctx = malloc(sizeof(server_context_t));
  if(!ctx)
    diep("malloc");
  ctx->slen=sizeof(struct sockaddr_in);
  if ((ctx->s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
    diep("socket");

  memset((char *) &ctx->si_me, 0, sizeof(ctx->si_me));
  ctx->si_me.sin_family = AF_INET;
  ctx->si_me.sin_port = htons(PORT);
  ctx->si_me.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind(ctx->s, &ctx->si_me, sizeof(ctx->si_me))==-1)
      diep("bind");
  return ctx;
}

int udp_recv(server_context_t *ctx, char *buf, int len){
    int l;
    if ((l=recvfrom(ctx->s, buf, len, 0, &ctx->si_other, &ctx->slen))==-1)
      diep("recvfrom()");
    return l;
//    printf("Received packet from %s:%d\nData: %s\n\n", 
//           inet_ntoa(ctx->si_other.sin_addr), ntohs(ctx->si_other.sin_port), ctx->buf);
}

int server_run(server_context_t *ctx){
  for (ctx->i=0; ctx->i<NPACK; ctx->i++) {
    int l = udp_recv(ctx, ctx->buf, BUFLEN);
    printf("Received %d bytes\n",l);
    printf("Received data: %s\n", 
           ctx->buf);
  }
}

int server_exit(server_context_t *ctx) {
  close(ctx->s);
  free(ctx);
  return 0;
}

#define SRV_IP "127.0.0.1"

server_context_t* client_init(void)
{

  server_context_t *ctx = malloc(sizeof(server_context_t));
  if(!ctx)
    diep("malloc");
  ctx->slen=sizeof(struct sockaddr_in);

  if ((ctx->s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
    diep("socket");

  memset((char *) &ctx->si_other, 0, sizeof(ctx->si_other));
  ctx->si_other.sin_family = AF_INET;
  ctx->si_other.sin_port = htons(PORT);
  if (inet_aton(SRV_IP, &ctx->si_other.sin_addr)==0) {
    fprintf(stderr, "inet_aton() failed\n");
    exit(1);
  }
  return ctx;
}

int udp_send(server_context_t* ctx, char *s, int len)
{
    int result;
    if ( (result=sendto(ctx->s, s, len, 0, &(ctx->si_other), ctx->slen))==-1)
      diep("sendto()");
    return result;
}

int client_run(server_context_t* ctx)
{
  for (ctx->i=0; ctx->i<NPACK; ctx->i++) {
    printf("Sending packet %d\n", ctx->i);
//    sprintf(ctx->buf, "This is packet %d\n", ctx->i);
//    udp_send(ctx);
      udp_send(ctx, "Hello, world", 12);
  }
  return 0;
}

int client_exit(server_context_t* ctx)
{

  close(ctx->s);
  free(ctx);
  return 0;
}

