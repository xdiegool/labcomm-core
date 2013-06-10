#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "labcomm_private.h"
#include "labcomm_fd_reader.h"

#define BUFFER_SIZE 2048

struct labcomm_fd_reader {
  struct labcomm_reader reader;
  int fd;
  int close_fd_on_free;
};

static int fd_alloc(struct labcomm_reader *r, void *context, 
		    struct labcomm_decoder *decoder,
		    char *version)
{
  int result = 0;
  
  r->count = 0;
  r->pos = 0;
  r->data = malloc(BUFFER_SIZE);
  if (! r->data) {
    r->data_size = 0;
    result = -ENOMEM;
  } else {

    r->data_size = BUFFER_SIZE;
    result = r->data_size;
    if (version && version[0]) {
      char *tmp;
      
      tmp = labcomm_read_string(r);
      if (strcmp(tmp, version) != 0) {
	result = -EINVAL;
      } else {
	result = r->data_size;
      }
      free(tmp);
    }
  }
  return result;
}

static int fd_free(struct labcomm_reader *r, void *context)
{
  struct labcomm_fd_reader *fd_context = context;

  free(r->data);
  r->data = 0;
  r->data_size = 0;
  r->count = 0;
  r->pos = 0;

  if (fd_context->close_fd_on_free) {
    close(fd_context->fd);
  }
  free(fd_context);

  return 0;
}

static int fd_fill(struct labcomm_reader *r, void *context)
{
  int result = 0;
  struct labcomm_fd_reader *fd_context = context;

  if (r->pos < r->count) {
    result = r->count - r->pos;
  } else {
    int err;
    
    r->pos = 0;
    err = read(fd_context->fd, r->data, r->data_size);
    if (err <= 0) {
      r->count = 0;
      r->error = -EPIPE;
      result = -EPIPE;
    } else {
      r->count = err;
      result = r->count - r->pos;
    }
  }
  return result;
}

static int fd_start(struct labcomm_reader *r, void *context)
{
  int available;

  available = r->count - r->pos;
  if (available == 0) {
    available = fd_fill(r, context);
  }
  return available;
}

static int fd_end(struct labcomm_reader *r, void *context)
{
  return 0;
}

static int fd_ioctl(struct labcomm_reader *r, void *context,
		    int signature_index, 
		    struct labcomm_signature *signature, 
		    uint32_t action, va_list args)
{
  return -ENOTSUP;
}


static const struct labcomm_reader_action action = {
  .alloc = fd_alloc,
  .free = fd_free,
  .start = fd_start,
  .fill = fd_fill,
  .end = fd_end,
  .ioctl = fd_ioctl
};

struct labcomm_reader *labcomm_fd_reader_new(int fd, int close_fd_on_free)
{
  struct labcomm_fd_reader *result;

  result = malloc(sizeof(*result));
  if (result == NULL) {
    return NULL;
  } else {
    result->fd = fd;
    result->close_fd_on_free = close_fd_on_free;
    result->reader.context = result;
    result->reader.action = &action;
    return &result->reader;
  }
}
