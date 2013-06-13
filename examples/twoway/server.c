/*
  server.c -- LabComm example of using stacked readers/writers.

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

#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <labcomm_pthread_mutex_lock.h>
#include <labcomm_fd_reader.h>
#include <labcomm_fd_writer.h>
#include "decimating.h"
#include "introspecting.h"
#include "gen/types.h"

struct client {
  int fd;
  pthread_t main_thread;
  pthread_t decoder_thread;
  struct sockaddr_in adr;
  unsigned int adrlen;
  int32_t A, B, Sum, Diff;
  struct labcomm_decoder *decoder;
  struct labcomm_encoder *encoder;
};

static void handle_A(int32_t *value, void *context)
{
  struct client *client = context;

  client->A = *value;
}

static void handle_B(int32_t *value, void *context)
{
  struct client *client = context;

  client->B = *value;
  client->Sum = client->A + client->B;
  client->Diff = client->A - client->B;
  labcomm_encode_types_Sum(client->encoder, &client->Sum);
  labcomm_encode_types_Diff(client->encoder, &client->Diff);
}

static void handle_Terminate(types_Terminate *value, void *context)
{
  exit(0);
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

static void *run_client(void *arg) 
{
  struct client *client = arg;
  struct decimating *decimating;
  struct introspecting *introspecting;
  struct labcomm_lock *lock;

  printf("Client start\n");
  client->A = 0;
  client->B = 0;
  lock = labcomm_pthread_mutex_lock_new();
  decimating = decimating_new(labcomm_fd_reader_new(client->fd, 1),
			      labcomm_fd_writer_new(client->fd, 0),
			      lock);
  if (decimating == NULL) {
    /* Warning: might leak reader and writer at this point */
    goto out;
  }
  introspecting = introspecting_new(decimating->reader,
				    decimating->writer,
				    lock);
  if (introspecting == NULL) {
    /* Warning: might leak reader and writer at this point */
    goto out;
  }
  client->decoder = labcomm_decoder_new(introspecting->reader, lock);
  client->encoder = labcomm_encoder_new(introspecting->writer, lock);
  pthread_t rdt;
  
  labcomm_decoder_register_types_A(client->decoder, handle_A, client);
  labcomm_decoder_register_types_B(client->decoder, handle_B, client);
  labcomm_decoder_register_types_Terminate(client->decoder, handle_Terminate, 
					   NULL);
  pthread_create(&rdt, NULL, run_decoder, client->decoder);  
  labcomm_encoder_register_types_Sum(client->encoder);
  labcomm_encoder_register_types_Diff(client->encoder);
  pthread_join(rdt, NULL);
out:
  printf("Client end\n");
  shutdown(client->fd, SHUT_RDWR);
  close(client->fd);
  free(client);
  return NULL;
}

int main(int argc, char *argv[])
{
  int fd;
  int result = 0;
  int reuse;
  int port;
  struct sockaddr_in adr;
  
  port = atoi(argv[1]);
  fd = socket(PF_INET, SOCK_STREAM, 0);
  if (fd < 0) {
    result = errno;
    goto no_server_socket;
  }
  reuse = 1;
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

  adr.sin_family = AF_INET;
  adr.sin_port = htons(port);
  adr.sin_addr.s_addr = INADDR_ANY;
  if (bind(fd, (struct sockaddr*)&adr, sizeof(adr)) != 0) {
    result = errno;
    goto failed_to_bind;
  }
  if (listen(fd, 5) != 0) {
    result = errno;
    goto failed_to_listen;
  }
  while (1) {
    struct client *client;

    client = malloc(sizeof(*client));
    if (client == NULL) {
      result = errno;
      goto failed_to_alloc_client;
    }
    client->adr.sin_family = AF_INET;
    client->adr.sin_port = 0;
    client->adr.sin_addr.s_addr = INADDR_ANY;
    client->adrlen = sizeof(client->adr);
    client->fd = accept(fd,  (struct sockaddr*)&client->adr, &client->adrlen); 
    if (client->fd < 0) {
      result = errno;
      goto failed_to_accept;
    }
    pthread_create(&client->main_thread, NULL, run_client, client);
  } 

failed_to_alloc_client:
failed_to_accept:
failed_to_listen: 
failed_to_bind:
  close(fd);
no_server_socket:
  return result;
  
}

