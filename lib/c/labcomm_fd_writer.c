#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "labcomm_private.h"
#include "labcomm_fd_writer.h"

#define BUFFER_SIZE 2048

struct labcomm_fd_writer {
  struct labcomm_writer writer;
  int fd;
  int close_fd_on_free;
};

static int fd_flush(struct labcomm_writer *w, void *context);

static int fd_alloc(struct labcomm_writer *w, void *context, 
		    struct labcomm_encoder *encoder,
		    char *version)
{
  w->data = malloc(BUFFER_SIZE);
  if (! w->data) {
    w->error = -ENOMEM;
    w->data_size = 0;
    w->count = 0;
    w->pos = 0;
  } else {
    w->data_size = BUFFER_SIZE;
    w->count = BUFFER_SIZE;
    w->pos = 0;
    if (version && version[0]) {
      labcomm_write_string(w, version);
      fd_flush(w, context);
    }
  }

  return w->error;
}

static int fd_free(struct labcomm_writer *w, void *context)
{
  struct labcomm_fd_writer *fd_context = context;

  free(w->data);
  w->data = 0;
  w->data_size = 0;
  w->count = 0;
  w->pos = 0;

  if (fd_context->close_fd_on_free) {
    close(fd_context->fd);
  }
  return 0;
}

static int fd_start(struct labcomm_writer *w, void *context,
		    struct labcomm_encoder *encoder,
		    int index,
		    struct labcomm_signature *signature,
		    void *value)
{
  w->pos = 0;
  
  return w->error;
}

static int fd_flush(struct labcomm_writer *w, void *context)
{
  struct labcomm_fd_writer *fd_context = context;
  int start, err;
  
  start = 0;
  err = 0;
  while (start < w->pos) {
    err = write(fd_context->fd, &w->data[start], w->pos - start);
    if (err <= 0) {
      break;
    }
    start = start + err;
  }
  if (err < 0) {
    w->error = -errno;
  } else if (err == 0) {
    w->error = -EINVAL;
  }
  w->pos = 0;
   
  return w->error;
}

static const struct labcomm_writer_action action = {
  .alloc = fd_alloc,
  .free = fd_free,
  .start = fd_start,
  .end = fd_flush,
  .flush = fd_flush,
  .ioctl = NULL
};

struct labcomm_writer *labcomm_fd_writer_new(int fd, int close_fd_on_free)
{
  struct labcomm_fd_writer *result;

  result = malloc(sizeof(*result));
  if (result == NULL) {
    return NULL;
  } else {
    result->fd = fd;
    result->close_fd_on_free = close_fd_on_free;
    result->writer.context = result;
    result->writer.action = &action;
    return &result->writer;
  }
}
