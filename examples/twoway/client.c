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
#include <labcomm.h>
#include <labcomm_fd_reader.h>
#include <labcomm_fd_writer.h>
#include <labcomm_pthread_mutex_lock.h>
#include "decimating.h"
#include "gen/types.h"

static void handle_A(int32_t *value, void *context)
{
}

static void handle_B(int32_t *value, void *context)
{
}

static void handle_C(int32_t *value, void *context)
{
}

static void *run_decoder(void *context)
{
  struct labcomm_decoder *decoder = context;
  int result;

  do {
    result = labcomm_decoder_decode_one(decoder);
    printf("Got index %d", result);
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
  
  labcomm_decoder_register_types_A(decoder, handle_A, NULL);
  labcomm_decoder_register_types_B(decoder, handle_B, NULL);
  labcomm_decoder_register_types_C(decoder, handle_C, NULL);
  pthread_create(&rdt, NULL, run_decoder, decoder);  
  labcomm_encoder_register_types_A(encoder);
  
out:
  return 0;
  
}

#if 0

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <linux/tcp.h>
#include "labcomm.h"
#include "orca_client.h"
#include "orca_messages.h"

orca_client_t *orca_client_new_tcp(
  char *hostname,
  int port)
{
  orca_client_t *result;
  struct hostent *host;
  int OK;

  result = malloc(sizeof(orca_client_t));
  OK = result != 0;

  if (OK) {
    result->encoder = 0;
    result->decoder = 0;
    result->directory.input.n_0 = 0;
    result->directory.input.a = 0;
    result->directory.output.n_0 = 0;
    result->directory.output.a = 0;
    result->directory.parameter.n_0 = 0;
    result->directory.parameter.a = 0;
    result->directory.log.n_0 = 0;
    result->directory.log.a = 0;
    result->fd = socket(PF_INET, SOCK_STREAM, 0);
    if (result->fd < 0) {
      fprintf(stderr, "failed to create socket\n");
      OK = 0;
    }
  }
      
  if (OK) {
    struct sockaddr_in adr;
    int err;
    
    adr.sin_family = AF_INET;
    adr.sin_port = 0;
    adr.sin_addr.s_addr = INADDR_ANY;
    err = bind(result->fd, (struct sockaddr*)&adr, sizeof(adr));
    if (err != 0) {
      fprintf(stderr, "failed to bind socket\n");
      OK = 0;
    }
  }
      
  if (OK) {
    host = gethostbyname(hostname);
    if (!host) {
      fprintf(stderr, "failed to lookup %s\n", hostname);
      OK = 0;
    }
  }
      
  if (OK) {
    struct sockaddr_in to;
    int err;
    
    to.sin_family = AF_INET;
    to.sin_port = htons(port);
    bcopy((char*)host->h_addr, (char*)&to.sin_addr, host->h_length);
    err = connect(result->fd, (struct sockaddr*)&to, sizeof(to));
    if (err != 0) {
      fprintf(stderr, "failed to connect %d@%s\n", port, hostname);
      OK = 0;
    }
  }
  
  if (OK) {
    int nodelay = 1;
    setsockopt(result->fd, IPPROTO_TCP, TCP_NODELAY, 
	       &nodelay, sizeof(nodelay));
  }

  if (OK) {
    result->encoder = labcomm_encoder_new(fd_writer, &result->fd);
    if (!result->encoder) {
      fprintf(stderr, "failed to allocate encoder\n");
      OK = 0;
    } else {
      labcomm_encoder_register_orca_messages_select_input(result->encoder);
      labcomm_encoder_register_orca_messages_select_output(result->encoder);
      labcomm_encoder_register_orca_messages_select_parameter(result->encoder);
      labcomm_encoder_register_orca_messages_select_log(result->encoder);
    }
  }

  if (OK) {
    result->decoder = labcomm_decoder_new(fd_reader, &result->fd);
    if (!result->decoder) {
      fprintf(stderr, "failed to allocate encoder\n");
      OK = 0;
    } else {
      labcomm_decoder_register_orca_messages_directory(result->decoder, 
						       directory_handler, 
						       result);
      labcomm_decoder_decode_one(result->decoder);
    }
  }

  if (!OK && result) {
    orca_client_free_tcp(result);
    result = 0;
  }
  
  return result;
}

void orca_client_free_tcp(
  struct orca_client *client)
{
  close(client->fd);
  fprintf(stderr, "IMPLEMENT %s\n", __FUNCTION__);
}

orca_internal_channel_t *select_tcp(
  struct orca_client *client,
  void (*encode)(struct labcomm_encoder *e, orca_messages_select_t *v),
  orca_messages_connection_list_t *list,
  orca_client_selection_t *selection,
  direction_t direction,
  int decimation)
{
  orca_internal_channel_t *result;
  int i, j, n;

  n = 0;
  for (i = 0 ; i < selection->n_0 ; i++) {
    for (j = 0 ; j < list->n_0 ; j++) {
      if (strcmp(selection->a[i], list->a[j].name) == 0) {
	n++;
	break;
      }
    }
  }
  fprintf(stderr, "%d matches\n", n);
  if (n) {
    int OK, server, port, fd;

    OK = 1;
    server = socket(PF_INET, SOCK_STREAM, 0);
    if (server < 0) {
      fprintf(stderr, "failed to create socket\n");
      OK = 0;
    }
    if (OK) {
      struct sockaddr_in adr;
      int err;

      adr.sin_family = AF_INET;
      adr.sin_port = htons(0);
      adr.sin_addr.s_addr = INADDR_ANY;
      err = bind(server, (struct sockaddr*)&adr, sizeof(adr));
      if (err != 0) {
	fprintf(stderr, "failed to bind socket\n");
	OK = 0;
      }
    }
     
    if (OK) {
      int err;

      err = listen(server, 1); 
      if (err != 0) {
	fprintf(stderr, "failed to listen on socket\n");
	OK = 0;
      }
    }

    if (OK) {
      struct sockaddr_in adr;
      unsigned int adrlen;
      int err;

      adrlen = sizeof(adr);
      err = getsockname(server, (struct sockaddr*)&adr, &adrlen);
      if (err != 0) {
	OK = 0;
      } else {
	port = ntohs(adr.sin_port);
      }
    }

    if (OK) {
      orca_messages_select_t select;

      select.port =  port;
      select.decimation = decimation;
      select.list.n_0 = n;
      select.list.a = malloc(select.list.n_0 * sizeof(*select.list.a));
      if (select.list.a) {
	n = 0;
	for (i = 0 ; i < selection->n_0 ; i++) {
	  for (j = 0 ; j < list->n_0 ; j++) {
	    if (strcmp(selection->a[i], list->a[j].name) == 0) {
	      fprintf(stderr, "selection->a[%d] = %s %d\n",
		      n, 
		      selection->a[i], 
		      list->a[j].index);
	      select.list.a[n] = list->a[j].index;
	      n++;
	      break;
	    }
	  }
	}
	encode(client->encoder, &select);
      }
      free(select.list.a);
    }
    
    if (OK) {
      struct sockaddr_in adr;
      unsigned int adrlen;
      
      adr.sin_family = AF_INET;
      adr.sin_port = 0;
      adr.sin_addr.s_addr = INADDR_ANY;
      fprintf(stderr, "Restrict accept: %s %d\n", __FILE__, __LINE__);
      //adr.sin_addr = client->remote.sin_addr;
      adrlen = sizeof(adr);
      fd = accept(server,  (struct sockaddr*)&adr, &adrlen); 
      if (fd < 0) {
	OK = 0;
      }
    }   

    if (OK) {
      result = malloc(sizeof(orca_internal_channel_t));
      if (! result) {
	OK = 0;
      }
    }

    if (OK) {
      result->fd = fd;
      result->channel.context = result;
      if (direction == direction_read || direction == direction_read_write) {
	result->channel.decoder = labcomm_decoder_new(fd_reader, &result->fd);
      } else {
	result->channel.decoder = 0;
      }
      if (direction == direction_write || direction == direction_read_write) {
 	result->channel.encoder = labcomm_encoder_new(fd_writer, &result->fd);
      } else {
	result->channel.encoder = 0;
      }
    }
    fprintf(stderr, "CONNECTED %p %p\n", 
	    result->channel.encoder, result->channel.decoder);
    close(server);
  }
  return result;
}
  
 
orca_client_channel_t *orca_client_select_input_tcp(
  struct orca_client *client,
  orca_client_selection_t *selection)
{
  orca_internal_channel_t *channel;

  channel = select_tcp(client, labcomm_encode_orca_messages_select_input,
		       &client->directory.input, selection,
		       direction_write, 1);
  return &channel->channel;
}

orca_client_channel_t *orca_client_select_output_tcp(
  struct orca_client *client,
  orca_client_selection_t *selection)
{
  orca_internal_channel_t *channel;
  
  channel = select_tcp(client, labcomm_encode_orca_messages_select_output,
		       &client->directory.output, selection,
		       direction_read, 1);
  return &channel->channel;
}

orca_client_channel_t *orca_client_select_parameter_tcp(
  struct orca_client *client,
  orca_client_selection_t *selection)
{
  orca_internal_channel_t *channel;

  channel = select_tcp(client, labcomm_encode_orca_messages_select_parameter,
		       &client->directory.parameter, selection,
		       direction_read_write, 1);
  return &channel->channel;
}

orca_client_channel_t *orca_client_select_log_tcp(
  struct orca_client *client,
  orca_client_selection_t *selection,
  int decimation)
{
  orca_internal_channel_t *channel;
  
  channel = select_tcp(client, labcomm_encode_orca_messages_select_log,
		       &client->directory.log, selection,
		       direction_read, decimation);
  return &channel->channel;
}

#endif
