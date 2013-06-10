#include <arpa/inet.h>
#include <linux/tcp.h>
#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <labcomm.h>
#include <labcomm_fd_reader.h>
#include <labcomm_fd_writer.h>
#include <labcomm_pthread_mutex_lock.h>
#include "decimating.h"
#include "gen/types.h"

static void handle_Sum(int32_t *value, void *context)
{
  printf("A+B=%d\n", *value);
}

static void handle_Diff(int32_t *value, void *context)
{
  printf("A-B=%d\n", *value);
}

static void *run_decoder(void *context)
{
  struct labcomm_decoder *decoder = context;
  int result;

  do {
    result = labcomm_decoder_decode_one(decoder);
  } while (result >= 0);
  return NULL;
}


int main(int argc, char *argv[])
{
  int fd;
  struct sockaddr_in adr;
  int err;
  struct hostent *host;
  struct sockaddr_in to;
  int nodelay;
  struct decimating *decimating;
  char *hostname;
  int port;
  struct labcomm_lock *lock;
  struct labcomm_decoder *decoder;
  struct labcomm_encoder *encoder;
  int32_t i, j;

  hostname = argv[1];
  port = atoi(argv[2]);
  
  fd = socket(PF_INET, SOCK_STREAM, 0);
  if (fd < 0) {
    fprintf(stderr, "failed to create socket\n");
    goto out;
  }
  
  adr.sin_family = AF_INET;
  adr.sin_port = 0;
  adr.sin_addr.s_addr = INADDR_ANY;
  err = bind(fd, (struct sockaddr*)&adr, sizeof(adr));
  if (err != 0) {
    fprintf(stderr, "failed to bind socket\n");
    goto out;
  }
      
  host = gethostbyname(hostname);
  if (!host) {
    fprintf(stderr, "failed to lookup %s\n", hostname);
    goto out;
  }
      
  to.sin_family = AF_INET;
  to.sin_port = htons(port);
  bcopy((char*)host->h_addr, (char*)&to.sin_addr, host->h_length);
  err = connect(fd, (struct sockaddr*)&to, sizeof(to));
  if (err != 0) {
    fprintf(stderr, "failed to connect %d@%s\n", port, hostname);
    goto out;
  }
  
  nodelay = 1;
  setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay));
  lock = labcomm_pthread_mutex_lock_new();
  decimating = decimating_new(labcomm_fd_reader_new(fd, 1),
			      labcomm_fd_writer_new(fd, 0),
			      lock);
  if (decimating == NULL) {
    goto out;
  }
  decoder = labcomm_decoder_new(decimating->reader, lock);
  encoder = labcomm_encoder_new(decimating->writer, lock);
  pthread_t rdt;
  
  labcomm_decoder_register_types_Sum(decoder, handle_Sum, NULL);
  labcomm_decoder_register_types_Diff(decoder, handle_Diff, NULL);
  pthread_create(&rdt, NULL, run_decoder, decoder);  
  labcomm_encoder_register_types_A(encoder);
  labcomm_encoder_register_types_B(encoder);
  labcomm_encoder_register_types_Terminate(encoder);

  usleep(100000);

  err = labcomm_decoder_ioctl_types_Sum(decoder, SET_DECIMATION, 2);
  err = labcomm_decoder_ioctl_types_Diff(decoder, SET_DECIMATION, 4);

  for (i = 0 ; i < 4 ; i++) {
    for (j = 0 ; j < 4 ; j++) {
      printf("A=%d B=%d\n", i, j);
      labcomm_encode_types_A(encoder, &i);
      labcomm_encode_types_B(encoder, &j);
      sleep(1);
    }
  }
  labcomm_encode_types_Terminate(encoder, NULL);
out:
  return 0;
  
}

