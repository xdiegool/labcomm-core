/*
  client.c -- LabComm example of using stacked readers/writers.

  Copyright 2013 Anders Blomdell <anders.blomdell@control.lth.se>

  This file is part of LabComm.

  LabComm is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  LabComm is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <errno.h>
#include <arpa/inet.h>
#ifndef LABCOMM_OS_DARWIN
#include <linux/tcp.h>
#else
#include <netinet/in.h>
#include <netinet/tcp.h> 
#endif
#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <labcomm2014.h>
#include <labcomm2014_fd_reader.h>
#include <labcomm2014_fd_writer.h>
#include <labcomm2014_default_error_handler.h>
#include <labcomm2014_default_memory.h>
#include <labcomm2014_pthread_scheduler.h>
#include "decimating.h"
#include "introspecting.h"
#include "gen/types.h"

static void handle_Sum(int32_t *value, void *context)
{
  printf("A+B=%d ", *value);
}

static void handle_Diff(int32_t *value, void *context)
{
  printf("A-B=%d ", *value);
}

static void handle_Product(int32_t *value, void *context)
{
  printf("A*B=%d ", *value);
}

static void *run_decoder(void *context)
{
  struct labcomm2014_decoder *decoder = context;
  int result;

  labcomm2014_decoder_register_types_Sum(decoder, handle_Sum, NULL);
  labcomm2014_decoder_register_types_Diff(decoder, handle_Diff, NULL);
  do {
    result = labcomm2014_decoder_decode_one(decoder);
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
  struct introspecting *introspecting;
  char *hostname;
  int port;
  struct labcomm2014_scheduler *scheduler;
  struct labcomm2014_decoder *decoder;
  struct labcomm2014_encoder *encoder;
  struct labcomm2014_time *next;
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
  scheduler = labcomm2014_pthread_scheduler_new(labcomm2014_default_memory);
  decimating = decimating_new(labcomm2014_fd_reader_new(labcomm2014_default_memory, 
						    fd, 1),
			      labcomm2014_fd_writer_new(labcomm2014_default_memory, 
						    fd, 0),
			      labcomm2014_default_error_handler,
			      labcomm2014_default_memory,
			      scheduler);
  if (decimating == NULL) {
    /* Warning: might leak reader and writer at this point */
    goto out;
  }
  introspecting = introspecting_new(decimating->reader,
				    decimating->writer,
				    labcomm2014_default_error_handler,
				    labcomm2014_default_memory,
				    scheduler);
  if (introspecting == NULL) {
    /* Warning: might leak reader and writer at this point */
    goto out;
  }
  decoder = labcomm2014_decoder_new(introspecting->reader, 
				labcomm2014_default_error_handler,
				labcomm2014_default_memory,
				scheduler);
  encoder = labcomm2014_encoder_new(introspecting->writer, 
				labcomm2014_default_error_handler,
				labcomm2014_default_memory,
				scheduler);
  pthread_t rdt;
  pthread_create(&rdt, NULL, run_decoder, decoder);  
  labcomm2014_encoder_register_types_A(encoder);
  labcomm2014_encoder_register_types_B(encoder);
  labcomm2014_encoder_register_types_Terminate(encoder);

  err = labcomm2014_decoder_ioctl_types_Sum(decoder, SET_DECIMATION, 2);
  err = labcomm2014_decoder_ioctl_types_Diff(decoder, SET_DECIMATION, 4);

  next = labcomm2014_scheduler_now(scheduler);
  for (i = 0 ; i < 4 ; i++) {
    if (i == 2) {
      labcomm2014_decoder_register_types_Product(decoder, handle_Product, NULL);
    }
    for (j = 0 ; j < 4 ; j++) {
      printf("\nA=%d B=%d: ", i, j);
      labcomm2014_encode_types_A(encoder, &i);
      labcomm2014_encode_types_B(encoder, &j);
      labcomm2014_time_add_usec(next, 100000);
      labcomm2014_scheduler_sleep(scheduler, next);
    }
  }
  printf("\n");
  labcomm2014_encode_types_Terminate(encoder);
out:
  return 0;
  
}

