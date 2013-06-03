#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "decimating.h"
 
struct client {
  int fd;
  pthread_t main_thread;
  pthread_t decoder_thread;
  struct sockaddr_in adr;
  unsigned int adrlen;
};

static void *run_client(void *arg) 
{
  struct client *client = arg;

  shutdown(client->fd, SHUT_RDWR);
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

#if 0
#include <errno.h>
#include <netinet/in.h>
#include <unistd.h>
#include <linux/tcp.h>
#include "labcomm.h"
#include "labcomm_private.h"
#include "orca_messages.h"
#include "orca_server.h"

#if 0
static long long rdtscll() 
{
  long long result;                             
  __asm__ __volatile__("rdtsc" : "=A" (result));
  return result;
}
static long long epoch;

static void *my_malloc(int size, int line) {
  void *p = malloc(size);
  if (!epoch) {
    epoch = rdtscll();
  }
  fprintf(stderr, "malloc: %16.16Ld %p %d %d\n", 
	  rdtscll() - epoch, p, size, line);
  fflush(stderr);
  return p;
}

static void my_free(void *p, int line) {
  fprintf(stderr, "free:   %16.16Ld %p %d\n", 
	  rdtscll() - epoch, p, line);
  fflush(stderr);
  free(p);
}


#define malloc(size) my_malloc(size, __LINE__)
#define free(p) my_free(p, __LINE__)
#endif

typedef enum {
  input_kind = 0,
  output_kind = 1,
  parameter_kind = 2,
  log_kind = 3
} kind_t;

typedef struct sample {
  struct orca_server *server;
  labcomm_signature_t *signature;
  labcomm_encode_typecast_t encode;
  labcomm_decoder_typecast_t decode;
  labcomm_handler_typecast_t handle;
  void *handle_context;
  struct {
    int n_0;
    struct connection **a;
  } connection;
} sample_t;

typedef enum { 
  input_t = 0, 
  output_t = 1, 
  parameter_t = 2, 
  log_t = 3 
} channel_kind_t; 

typedef struct {
  struct orca_server *context;
  labcomm_decoder_t decoder;
  labcomm_encoder_t encoder;
  orca_channel_t orca_channel;
  struct {
    int n_0;
    struct sample **a;
  } sample;
} channel_t;

typedef struct orca_server {
  orca_lock_t *lock;
  orca_server_context_t *context;
  int input_buffer_size;
  int output_buffer_size;
  int parameter_buffer_size;
  int log_buffer_size;
  struct {
    int n_0;
    struct connection **a;
  } connection;
  channel_t channel[4];
} orca_server_t;

typedef struct connection {
  void *context;
  orca_server_t *server;
  struct {
    int current;
    int max;
  } decimation;
  struct labcomm_encoder *encoder;
  struct labcomm_decoder *decoder;
  struct {
    int n_0;
    struct sample **a;
  } sample; 
} connection_t;

static sample_t *sample_by_signature(
  orca_lock_t *lock,
  channel_t *channel,
  labcomm_signature_t *signature,
  int create)
{ 
  sample_t *result = 0;
  int i;

  if (lock) { lock->lock(lock); }
  for (i = 0 ; !result && i < channel->sample.n_0 ; i++) { 
    if (channel->sample.a[i]->signature == signature) {
      result = channel->sample.a[i];
    }
  }
  if (lock) { lock->unlock(lock); }
  
  if (!result && create) {
    int n;
    sample_t **new_a, **old_a;
    sample_t *new_sample;

    new_a = 0;
    old_a = 0;
    new_sample = 0;

    if (lock) { lock->lock(lock); }
    while (1) {
      n = channel->sample.n_0 + 1;
      
      // Allocation without holding lock
      if (lock) { lock->unlock(lock); }

      // Free list from previous iteration
      if (new_a) { free(new_a); }
      
      // Try new allocation
      if (!new_sample) { new_sample = malloc(sizeof(sample_t)); }
      new_a = malloc(n * sizeof(*new_a));
      if (lock) { lock->lock(lock); }

      // Break if list length is unchanged
      if (n == channel->sample.n_0 + 1) { 
	break; 
      }
    }
    
    if (new_sample && new_a) {
      result = new_sample;
      old_a = channel->sample.a;
      channel->sample.n_0 = n;
      for (i = 0 ; i < n - 1 ; i++) {
	new_a[i] = old_a[i];
      }
      new_a[n - 1] = result;
      channel->sample.a = new_a;

      result->server = channel->context;
      result->signature = signature;
      result->connection.n_0 = 0;
      result->connection.a = 0;
      result->encode = 0;
      result->decode = 0;
      result->handle = 0;
      result->handle_context = 0;
      new_a = 0;
      new_sample = 0;
    }
    if (lock) { lock->unlock(lock); }
    if (old_a) { free(old_a); }
    if (new_a) { free(new_a); }
    if (new_sample) { free(new_sample); }
  }
  return result;
}

static void do_decoder_register(
  struct labcomm_decoder *d, 
  labcomm_signature_t *signature, 
  labcomm_decoder_typecast_t decode,
  labcomm_handler_typecast_t handle,
  void *handle_context)
{
  channel_t *channel = d->context;
  orca_server_t *server = channel->context;
  sample_t *sample;

  sample = sample_by_signature(server->lock, channel, signature, 1);
  if (sample) {
    server->lock->lock(server->lock);
    sample->decode = decode;
    sample->handle = handle;
    sample->handle_context = handle_context;
    server->lock->unlock(server->lock); 
  }
}

static void do_encoder_register(
  struct labcomm_encoder *e, 
  labcomm_signature_t *signature, 
  labcomm_encode_typecast_t encode)
{
  channel_t *channel = e->context;
  orca_server_t *server = channel->context;
  sample_t *sample;

  sample = sample_by_signature(server->lock, channel, signature, 1);
  if (sample) {
    server->lock->lock(server->lock);
    sample->encode = encode;
    server->lock->unlock(server->lock); 
  }
}

static void do_encode(
  struct labcomm_encoder *encoder, 
  labcomm_signature_t *signature, 
  void *value)
{
  channel_t *channel = encoder->context;
  sample_t *sample;

  // Lock should be held when this is called !!
  sample = sample_by_signature(0, channel, signature, 0);
  if (sample && sample->connection.n_0) {
    int i, size, available;
    
    size = signature->encoded_size(value);
    for (i = 0 ; i < sample->connection.n_0 ; i++) {
      connection_t *c = sample->connection.a[i];

      if (c->decimation.current == 0) {
	labcomm_writer_t *writer = &c->encoder->writer;
      
	available = writer->write(writer, labcomm_writer_available);
	if (available >= size) {
	  labcomm_internal_encode(sample->connection.a[i]->encoder, 
				  signature, 
				  value);
	}
      }
    }
  }
}

static void connect_connection_and_server(
  connection_t *connection,
  orca_server_t *server)
{
  int n, i;
  connection_t **new_a, **old_a;

  new_a = 0;
  old_a = 0;
  server->lock->lock(server->lock);
  while (1) {
    n = server->connection.n_0 + 1;
      
    // Allocation without holding lock
    server->lock->unlock(server->lock);
    
    // Free list from previous iteration
    if (new_a) { free(new_a); }
    
    // Try new allocation
    new_a = malloc(n * sizeof(*new_a));
    
    // Reclaim lock
    server->lock->lock(server->lock);
    
    // Break if list length is the desired one
    if (n == server->connection.n_0 + 1) { 
      break; 
    }
  }
  
  if (new_a) {
    old_a = server->connection.a;
    server->connection.n_0 = n;
    for (i = 0 ; i < n - 1 ; i++) {
      new_a[i] = old_a[i];
    }
    new_a[n - 1] = connection;
    server->connection.a = new_a;
    new_a = 0;
  }
  server->lock->unlock(server->lock);
  
  if (old_a) { free(old_a); }
  if (new_a) { free(new_a); }
}

static void disconnect_connection_and_server(
  connection_t *connection)
{
  orca_server_t *server = connection->server;
  int i, j;

  server->lock->lock(server->lock);
  for (i = 0, j = 0 ; i < server->connection.n_0 ; i++) {
    if (server->connection.a[i] != connection) {
      server->connection.a[j] = server->connection.a[i];
      j++;
    }
  }
  server->connection.n_0 = j;
  server->lock->unlock(server->lock);
}

static void connect_channel_and_sample(
  connection_t *connection,
  sample_t *sample)
{
  orca_lock_t *lock;
  int connection_n, sample_n;
  sample_t **new_sample_a, **old_sample_a;
  connection_t **new_connection_a, **old_connection_a;

  lock = connection->server->lock;

  old_sample_a = 0;
  old_connection_a = 0;
  new_sample_a = 0;
  new_connection_a = 0;

  lock->lock(lock);
  while (1) {
    connection_n = sample->connection.n_0 + 1;
    sample_n = connection->sample.n_0 + 1;

    lock->unlock(lock);

    // Free result from previous iteration
    if (new_connection_a) { free(new_connection_a); }
    if (new_sample_a) { free(new_sample_a); }

    // Try new allocation
    new_sample_a = malloc(sample_n * sizeof(*new_sample_a));
    new_connection_a = malloc(connection_n * sizeof(*new_connection_a));

    lock->lock(lock);
    
    // Break if list length are unchanged
    if (connection_n == sample->connection.n_0 + 1 &&
	sample_n == connection->sample.n_0 + 1) {
      break;
    }
  }
  if (new_sample_a && new_connection_a) {
    int i;

    old_connection_a = sample->connection.a;
    sample->connection.a = new_connection_a;
    sample->connection.n_0 = connection_n;

    old_sample_a = connection->sample.a;
    connection->sample.a = new_sample_a;
    connection->sample.n_0 = sample_n;

    for (i = 0 ; i < connection_n - 1 ; i++) {
      new_connection_a[i] = old_connection_a[i];
    }
    new_connection_a[connection_n - 1] = connection;

    for (i = 0 ; i < sample_n - 1 ; i++) {
      new_sample_a[i] = old_sample_a[i];
    }
    new_sample_a[sample_n - 1] = sample;

    new_sample_a = 0;
    new_connection_a = 0;
  }
  lock->unlock(lock);

  if (new_connection_a) { free(new_connection_a); }
  if (old_connection_a) { free(old_connection_a); }
  if (new_sample_a) { free(new_sample_a); }
  if (old_sample_a) { free(old_sample_a); }
}

static void disconnect_channel_and_sample(
  connection_t *connection)
{
  orca_lock_t *lock;
  int i;
  void *a;

  lock = connection->server->lock;
  lock->lock(lock);
  for (i = 0 ; i < connection->sample.n_0 ; i++) {
    int j, k; 
    sample_t *sample = connection->sample.a[i];

    for (j = 0, k = 0 ; j < sample->connection.n_0 ; j++) {
      if (sample->connection.a[j] != connection) {
	sample->connection.a[k] = sample->connection.a[j];
	k++;
      }
    }
    sample->connection.n_0 = k;
  }
  a = connection->sample.a;
  connection->sample.a = 0;
  connection->sample.n_0 = 0;
  lock->unlock(lock);
  free(a);
}

orca_server_t *orca_server_new(
  orca_lock_t *lock,
  orca_server_context_t *context,
  int input_buffer_size,
  int output_buffer_size,
  int parameter_buffer_size,
  int log_buffer_size)
{
  orca_server_t *result;

  result = malloc(sizeof(orca_server_t));
  if (result) {
    channel_kind_t sc;

    result->lock = lock;
    result->context = context;
    result->input_buffer_size = input_buffer_size;
    result->output_buffer_size = output_buffer_size;
    result->parameter_buffer_size = parameter_buffer_size;
    result->log_buffer_size = log_buffer_size;
    result->connection.n_0 = 0;
    result->connection.a = 0;

    for (sc = input_t ; sc <= log_t ; sc++) {
      result->channel[sc].context = result;

      result->channel[sc].decoder.context = &result->channel[sc];
      result->channel[sc].decoder.reader.read = 0;
      result->channel[sc].decoder.do_register = do_decoder_register;
      result->channel[sc].decoder.do_decode_one = 0;

      result->channel[sc].encoder.context = &result->channel[sc];
      result->channel[sc].encoder.writer.write = 0;
      result->channel[sc].encoder.do_register = do_encoder_register;
      result->channel[sc].encoder.do_encode = do_encode;

      result->channel[sc].orca_channel.decoder = &result->channel[sc].decoder ;
      result->channel[sc].orca_channel.encoder = &result->channel[sc].encoder ;

      result->channel[sc].sample.n_0 = 0;
      result->channel[sc].sample.a = 0;
    }
  }
  return result;
}

void orca_server_free(
  struct orca_server *server)
{
  channel_kind_t sc;
  
  // What should we do about open connections?
  for (sc = input_t ; sc <= log_t ; sc++) {
    int i;

    for (i = 0 ; i < server->channel[sc].sample.n_0 ; i++) {
      free(server->channel[sc].sample.a[i]);
    }
    free(server->channel[sc].sample.a);
  }
  free(server);
  printf("CHECK implementation %s\n", __FUNCTION__);
}

void orca_server_next_sample(
  struct orca_server *server)
{
  int i;
  
  for (i = 0 ; i < server->connection.n_0 ; i++) {
    connection_t *c = server->connection.a[i];
    int max = c->decimation.max;
    
    if (max) {
      c->decimation.current++;
      if (c->decimation.current >= max) {
	c->decimation.current = 0;
      }
    }
  }
}

orca_channel_t *orca_server_get_input_channel(
  struct orca_server *server)
{
  return &server->channel[input_t].orca_channel;
}

orca_channel_t *orca_server_get_output_channel(
  struct orca_server *server)
{
  return &server->channel[output_t].orca_channel;
}

orca_channel_t *orca_server_get_parameter_channel(
  struct orca_server *server)
{
  return &server->channel[parameter_t].orca_channel;
}

orca_channel_t *orca_server_get_log_channel(
  struct orca_server *server)
{
  return &server->channel[log_t].orca_channel;
}

//
// TCP specific stuff
//

typedef struct {
  orca_server_t *server;
  int fd;
  struct sockaddr_in remote;
} tcp_setup_connection_t;

typedef struct {
  int fd;
  int write_pos;
  int established;
  int buffer_size;
} tcp_nonblocking_t;

typedef struct {
  int closed;
  int use_count;
  connection_t connection;
  int fd;
  tcp_nonblocking_t write_context;
} tcp_connection_t;

static int nonblocking_fd_writer(
  labcomm_writer_t *w, 
  labcomm_writer_action_t action)
{
  int result = 0;
  tcp_nonblocking_t *context = w->context;

  switch (action) {
    case labcomm_writer_alloc: {
      int size = context->buffer_size;
      w->data = malloc(size);
      if (! w->data) {
        result = -ENOMEM;
        w->data_size = 0;
        w->count = 0;
        w->pos = 0;
      } else {
        w->data_size = size;
        w->count = size;
        w->pos = 0;
      }
    } break;
    case labcomm_writer_free: {
      free(w->data);
      w->data = 0;
      w->data_size = 0;
      w->count = 0;
      w->pos = 0;
    } break;
    case labcomm_writer_start: {
      if (!context->established) {
	w->pos = 0;
      }
    } break;
    case labcomm_writer_continue: {
      if (!context->established) {
	result = write(context->fd, w->data, w->pos);
	w->pos = 0;
      } else {
	printf("Buffer overrun %s\n", __FUNCTION__);
	exit(1);
      }
    } break;
    case labcomm_writer_end: {
      if (!context->established) {
	result = write(context->fd, w->data, w->pos);
	w->pos = 0;
      }
    } break;
    case labcomm_writer_available: {
      result = w->count - w->pos; 
    } break;
  }
  return result;
}

static int fd_writer(
  labcomm_writer_t *w, 
  labcomm_writer_action_t action)
{
  int result = 0;
  int *fd = w->context;

  switch (action) {
    case labcomm_writer_alloc: {
      w->data = malloc(1000);
      if (! w->data) {
        result = -ENOMEM;
        w->data_size = 0;
        w->count = 0;
        w->pos = 0;
      } else {
        w->data_size = 1000;
        w->count = 1000;
        w->pos = 0;
      }
    } break;
    case labcomm_writer_free: {
      free(w->data);
      w->data = 0;
      w->data_size = 0;
      w->count = 0;
      w->pos = 0;
    } break;
    case labcomm_writer_start: {
      w->pos = 0;
    } break;
    case labcomm_writer_continue: {
      result = write(*fd, w->data, w->pos);
      w->pos = 0;
    } break;
    case labcomm_writer_end: {
      result = write(*fd, w->data, w->pos);
      w->pos = 0;
    } break;
    case labcomm_writer_available: {
      result = w->count - w->pos; 
    } break;
  }
  return result;
}

static int fd_reader(
  labcomm_reader_t *r, 
  labcomm_reader_action_t action)
{
  int result = -EINVAL;
  int *fd = r->context;
  
  switch (action) {
    case labcomm_reader_alloc: {
      r->data = malloc(1000);
      if (r->data) {
	r->data_size = 1000;
	result = r->data_size;
      } else {
	r->data_size = 0;
	result = -ENOMEM;
      }
      r->count = 0;
      r->pos = 0;
    } break;
    case labcomm_reader_start: 
    case labcomm_reader_continue: {
      if (r->pos < r->count) {
	result = r->count - r->pos;
      } else {
	int err;

        r->pos = 0;
        err = read(*fd, r->data, r->data_size);
	if (err <= 0) {
	  r->count = 0;
	  result = -1;
	} else {
	  r->count = err;
	  result = r->count - r->pos;
	}
      }
    } break;
    case labcomm_reader_end: {
      result = 0;
    } break;
    case labcomm_reader_free: {
      free(r->data);
      r->data = 0;
      r->data_size = 0;
      r->count = 0;
      r->pos = 0;
      result = 0;
    } break;
  }
  return result;
}

static tcp_connection_t *do_tcp_connect(
  tcp_setup_connection_t *connection, 
  int port,
  int buffer_size,
  int decimation)
{
  tcp_connection_t *result;
  int fd, OK;

  OK = 1;
  if (OK) {
    fd = socket(PF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
      printf("failed to create socket\n");
      OK = 0;
    }
  }

  if (OK) {
    int reuse;
    
    reuse = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
  }
      
  if (OK) {
    struct sockaddr_in adr;
    int err;
    
    adr.sin_family = AF_INET;
    adr.sin_port = 0;
    adr.sin_addr.s_addr = INADDR_ANY;
    err = bind(fd, (struct sockaddr*)&adr, sizeof(adr));
    if (err != 0) {
      printf("failed to bind socket\n");
      OK = 0;
    }
  }
      
  if (OK) {
    struct sockaddr_in to;
    int err;
    
    to.sin_family = AF_INET;
    to.sin_port = htons(port);
    to.sin_addr = connection->remote.sin_addr;
    err = connect(fd, (struct sockaddr*)&to, sizeof(to));
    if (err != 0) {
      printf("failed to connect %d@%s\n", port, inet_ntoa(to.sin_addr));
      printf("%d %d %x\n", port, htons(port), port);
      OK = 0;
    }
  }

  if (OK) {
    int nodelay = 1;
    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay));
  }

  if (!OK && fd >= 0) {
    close(fd);
    fd = -1;
  }

  if (OK) {
    result = malloc(sizeof(*result));
    if (!result) {
      OK = 0;
    }
  }
   
  if (!OK) {
    result = 0;
  } else {
    result->closed = 0;
    result->use_count = 0;
    result->connection.context = result;
    result->connection.server = connection->server;
    result->connection.decimation.current = decimation;
    result->connection.decimation.max = decimation;
    result->connection.encoder = 0;
    result->connection.decoder = 0;
    result->connection.sample.n_0 = 0;
    result->connection.sample.a = 0;
    result->fd = fd;
    result->write_context.fd = fd;
    result->write_context.write_pos = 0;
    result->write_context.established = 0;
    result->write_context.buffer_size = buffer_size;
  }

  return result;
}

static void do_tcp_disconnect(tcp_connection_t *tcp_connection)
{
  orca_lock_t *lock = tcp_connection->connection.server->lock;
  int use_count;

  shutdown(tcp_connection->fd, SHUT_RDWR);
  close(tcp_connection->fd);

  lock->lock(lock);
  tcp_connection->closed = 1;
  tcp_connection->use_count--;
  use_count = tcp_connection->use_count;
  lock->unlock(lock);

  printf("%s use %d\n", __FUNCTION__, use_count);
  if (use_count == 0) {
    disconnect_channel_and_sample(&tcp_connection->connection);
    disconnect_connection_and_server(&tcp_connection->connection);
    if (tcp_connection->connection.decoder) {
      labcomm_decoder_free(tcp_connection->connection.decoder);
    }
    if (tcp_connection->connection.encoder) {
      labcomm_encoder_free(tcp_connection->connection.encoder);
    }
    free(tcp_connection);
  }
}

static void lock_and_handle(
  void *value, 
  void *context)
{
  sample_t *sample = context;
  orca_lock_t *lock = sample->server->lock;
  
  lock->lock(lock);
  sample->handle(value, sample->handle_context);
  lock->unlock(lock);
}

static int register_select_handler_tcp(
  channel_t *channel,
  orca_messages_select_t *v,
  tcp_connection_t *tcp_connection)
{
  int i;
  int n = 0;

  for (i = 0 ; i < v->list.n_0 ; i++) {
    if (v->list.a[i] < channel->sample.n_0) {
      sample_t *sample = channel->sample.a[v->list.a[i]];

      n++;
      if (tcp_connection->connection.decoder) {
	labcomm_internal_decoder_register(tcp_connection->connection.decoder, 
					  sample->signature,
					  sample->decode, 
					  lock_and_handle,
					  sample);
      }
      if (tcp_connection->connection.encoder) {
	labcomm_internal_encoder_register(tcp_connection->connection.encoder, 
					  sample->signature,
					  sample->encode);	
      }
    }
  }
  // Switch to non-blocking writes
  tcp_connection->write_context.established = 1;
  for (i = 0 ; i < v->list.n_0 ; i++) {
    if (v->list.a[i] < channel->sample.n_0) {
      sample_t *sample = channel->sample.a[v->list.a[i]];
      connect_channel_and_sample(&tcp_connection->connection, sample);
    }
  }
  return n;
}

static void *decoder_run_main(void *context)
{
  tcp_connection_t *tcp_connection = context;

  printf("Start %s %p\n", __FUNCTION__, context);
  labcomm_decoder_run(tcp_connection->connection.decoder);
  do_tcp_disconnect(tcp_connection);

  printf("Finish %s %p\n", __FUNCTION__, context);
  return 0;
}

static void *tcp_deferred_write_main(void *context)
{
  tcp_connection_t *tcp_connection = context;
  orca_server_t *server = tcp_connection->connection.server;
  orca_lock_t *lock = tcp_connection->connection.server->lock;

  printf("Start %s %p\n", __FUNCTION__, context);
  while (1) {
    int start, end;

    server->context->await_deferred_write(server->context);
    start = tcp_connection->write_context.write_pos;
    end = tcp_connection->connection.encoder->writer.pos;
    if (tcp_connection->closed) {
      break;
    } else if (end - start > 0) {
      int err;
      err = write(tcp_connection->write_context.fd, 
		   &tcp_connection->connection.encoder->writer.data[start],
		   end - start);
      if (err < 0) {
	break;
      } else {
	tcp_connection->write_context.write_pos += err;
	lock->lock(lock);
	if (tcp_connection->connection.encoder->writer.pos ==
	    tcp_connection->write_context.write_pos) {
	  tcp_connection->connection.encoder->writer.pos = 0;
	  tcp_connection->write_context.write_pos = 0;
	}
	lock->unlock(lock);
      }
    }
  }
  do_tcp_disconnect(tcp_connection);

  printf("Finish %s %p\n", __FUNCTION__, context);
  return 0;
}


static void select_input_handler_tcp(
  orca_messages_select_input *v,
  void *context)
{
  tcp_setup_connection_t *tcp_setup_connection = context;
  orca_server_t *server = tcp_setup_connection->server;
  tcp_connection_t *tcp_connection;
  
  tcp_connection = do_tcp_connect(tcp_setup_connection, v->port, 
				  server->input_buffer_size, 0);
  if (tcp_connection) {
    tcp_connection->use_count += 1;
    tcp_connection->connection.decoder = 
      labcomm_decoder_new(fd_reader, &tcp_connection->fd);
    connect_connection_and_server(&tcp_connection->connection, server);
    if (register_select_handler_tcp(&server->channel[input_t], 
				    v, tcp_connection)) {
      server->context->spawn_task(decoder_run_main, tcp_connection);     
    } else {
      do_tcp_disconnect(tcp_connection);
    }
  }
}

static void select_output_handler_tcp(
  orca_messages_select_output *v,
  void *context)
{
  tcp_setup_connection_t *tcp_setup_connection = context;
  orca_server_t *server = tcp_setup_connection->server;
  tcp_connection_t *tcp_connection;
  
  tcp_connection = do_tcp_connect(tcp_setup_connection, v->port, 
				  server->output_buffer_size, v->decimation);
  if (tcp_connection) {
    tcp_connection->use_count += 1;
    tcp_connection->connection.encoder = 
      labcomm_encoder_new(nonblocking_fd_writer, 
			  &tcp_connection->write_context);
    connect_connection_and_server(&tcp_connection->connection, server);
    if (register_select_handler_tcp(&server->channel[output_t], 
				    v, tcp_connection)) {
      server->context->spawn_task(tcp_deferred_write_main, tcp_connection);
    } else {
      do_tcp_disconnect(tcp_connection);
    }
  }
}

static void select_parameter_handler_tcp(
  orca_messages_select_parameter *v,
  void *context)
{
  tcp_setup_connection_t *tcp_setup_connection = context;
  orca_server_t *server = tcp_setup_connection->server;
  tcp_connection_t *tcp_connection;
  
  tcp_connection = do_tcp_connect(tcp_setup_connection, v->port, 
				  server->parameter_buffer_size, 0);
  if (tcp_connection) {
    tcp_connection->use_count += 2;

    tcp_connection->connection.decoder = 
      labcomm_decoder_new(fd_reader, &tcp_connection->fd);
    tcp_connection->connection.encoder = 
      labcomm_encoder_new(nonblocking_fd_writer, 
			  &tcp_connection->write_context);
    connect_connection_and_server(&tcp_connection->connection, server);
    if (register_select_handler_tcp(&server->channel[parameter_t], 
				    v, tcp_connection)) {
      server->context->spawn_task(decoder_run_main, tcp_connection);
      server->context->spawn_task(tcp_deferred_write_main, tcp_connection);
      // Force current values to be sent to all clients
      {
	int i;
	channel_t *channel = &server->channel[parameter_t];
	
	for (i = 0 ; i < v->list.n_0 ; i++) {
	  if (v->list.a[i] < channel->sample.n_0) {
	    sample_t *sample = channel->sample.a[v->list.a[i]];
	    lock_and_handle(0, sample);
	  }
	}
      }
    } else {
      do_tcp_disconnect(tcp_connection);
    }
  }
}

static void select_log_handler_tcp(
  orca_messages_select_log *v,
  void *context)
{
  tcp_setup_connection_t *tcp_setup_connection = context;
  orca_server_t *server = tcp_setup_connection->server;
  tcp_connection_t *tcp_connection;
  
  tcp_connection = do_tcp_connect(tcp_setup_connection, v->port,
				  server->log_buffer_size, v->decimation);
  if (tcp_connection) {
    tcp_connection->use_count += 1;
    tcp_connection->connection.encoder = 
      labcomm_encoder_new(nonblocking_fd_writer, 
			  &tcp_connection->write_context);
    connect_connection_and_server(&tcp_connection->connection, server);
    if (register_select_handler_tcp(&server->channel[log_t], 
				    v, tcp_connection)) {
      server->context->spawn_task(tcp_deferred_write_main, tcp_connection);
    } else {
      do_tcp_disconnect(tcp_connection);
    }
  }
}

static void make_connection_list(
  orca_lock_t *lock,
  orca_messages_connection_list_t *dest,
  channel_t *src)
{
  int i;

  dest->n_0 = src->sample.n_0;
  dest->a = malloc(dest->n_0 * sizeof(*dest->a));
  if (dest->a) {
    lock->lock(lock);
    for (i = 0 ; i < dest->n_0 ; i++) {
      dest->a[i].index = i;
      dest->a[i].name = src->sample.a[i]->signature->name;
      dest->a[i].signature.n_0 = src->sample.a[i]->signature->size;
      dest->a[i].signature.a = src->sample.a[i]->signature->signature;
    }
    lock->unlock(lock);
  }
}

static void encode_orca_directory(
  struct labcomm_encoder *encoder, 
  orca_server_t *server)
{
  orca_messages_directory directory;

  make_connection_list(server->lock,
		       &directory.input, 
		       &server->channel[input_t]);

  make_connection_list(server->lock,
		       &directory.output, 
		       &server->channel[output_t]);

  make_connection_list(server->lock,
		       &directory.parameter, 
		       &server->channel[parameter_t]);

  make_connection_list(server->lock,
		       &directory.log, 
		       &server->channel[log_t]);

  labcomm_encode_orca_messages_directory(encoder, &directory);

  free(directory.input.a);
  free(directory.output.a);
  free(directory.parameter.a);
  free(directory.log.a);
}

static void *tcp_setup_connection_main(void *argument)
{
  tcp_setup_connection_t *tcp_setup_connection = argument;
  struct labcomm_encoder *encoder;
  struct labcomm_decoder *decoder;

  encoder = labcomm_encoder_new(fd_writer, &tcp_setup_connection->fd);
  decoder = labcomm_decoder_new(fd_reader, &tcp_setup_connection->fd);
  labcomm_encoder_register_orca_messages_directory(encoder);
  labcomm_decoder_register_orca_messages_select_input(
    decoder, select_input_handler_tcp, tcp_setup_connection);
  labcomm_decoder_register_orca_messages_select_output(
    decoder, select_output_handler_tcp, tcp_setup_connection);
  labcomm_decoder_register_orca_messages_select_parameter(
    decoder, select_parameter_handler_tcp, tcp_setup_connection);
  labcomm_decoder_register_orca_messages_select_log(
    decoder, select_log_handler_tcp, tcp_setup_connection);
  encode_orca_directory(encoder, tcp_setup_connection->server);
  labcomm_decoder_run(decoder);

  shutdown(tcp_setup_connection->fd, SHUT_RDWR);
  close(tcp_setup_connection->fd);
  labcomm_encoder_free(encoder);
  labcomm_decoder_free(decoder);
  free(tcp_setup_connection);
  return 0;
} 

int orca_server_run_tcp_cb(
  struct orca_server *server,
  int port,
  int backlog,
  void (*cb)(struct orca_server *server, char *message, int err))
{
  int result;
  int server_fd;



  if (cb) { cb(server, "Listening for client connections", 0); }
  while (1) {
    struct sockaddr_in adr;
    unsigned int adrlen;
    int fd;
    tcp_setup_connection_t *tcp_setup_connection;

    adr.sin_family = AF_INET;
    adr.sin_port = 0;
    adr.sin_addr.s_addr = INADDR_ANY;
    adrlen = sizeof(adr);
    fd = accept(server_fd,  (struct sockaddr*)&adr, &adrlen); 
    if (fd < 0) {
      if (cb) { cb(server, "Failed to accept on socket", errno); }
      result = -errno;
      goto failed_to_accept;
    } 
    tcp_setup_connection = malloc(sizeof(tcp_setup_connection_t));
    if (!tcp_setup_connection) {
      cb(server, "Failed to allocate client memeory", -ENOMEM);
      shutdown(fd, SHUT_RDWR);
    } else {
      if (cb) {
	char buf[128];
	
	sprintf(buf, "Got server connection: %s %x", 
		inet_ntoa(adr.sin_addr), ntohs(adr.sin_port));
	cb(server, buf, errno);
      }
      tcp_setup_connection->fd = fd;
      tcp_setup_connection->remote = adr;
      tcp_setup_connection->server = server;
      server->context->spawn_task(tcp_setup_connection_main, 
				  tcp_setup_connection);
    }
  }
failed_to_accept:
failed_to_listen:
failed_to_bind:
  close(server_fd);
no_server_socket:
  return result;
}

int orca_server_run_tcp(
  struct orca_server *server,
  int port,
  int backlog)
{
  return orca_server_run_tcp_cb(server, port, backlog, NULL);
}
#endif
