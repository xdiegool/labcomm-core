#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "labcomm_fd_reader.h"

#define BUFFER_SIZE 2048

static int fd_alloc(struct labcomm_reader *r, char *version)
{
  int result;
#ifndef LABCOMM_FD_OMIT_VERSION
  int *fd = r->context;
  char *tmp = strdup(version);
  
  read(*fd, tmp, strlen(version));
  free(tmp);
#endif
  r->data = malloc(BUFFER_SIZE);
  if (r->data) {
    r->data_size = BUFFER_SIZE;
    result = r->data_size;
  } else {
    r->data_size = 0;
    result = -ENOMEM;
  }
  r->count = 0;
  r->pos = 0;

  return result;
}

static int fd_free(struct labcomm_reader *r)
{
  free(r->data);
  r->data = 0;
  r->data_size = 0;
  r->count = 0;
  r->pos = 0;

  return 0;
}

static int fd_fill(struct labcomm_reader *r)
{
  int result;
  int *fd = r->context;

  if (r->pos < r->count) {
    result = r->count - r->pos;
  } else {
    int err;
    
    r->pos = 0;
    err = read(*fd, r->data, r->data_size);
    if (err <= 0) {
      r->count = 0;
      result = -EPIPE;
    } else {
      r->count = err;
      result = r->count - r->pos;
    }
  }
  return result;
}

static int fd_start(struct labcomm_reader *r)
{
  return fd_fill(r);
}

static int fd_end(struct labcomm_reader *r)
{
  return 0;
}

const struct labcomm_reader_action labcomm_fd_reader = {
  .alloc = fd_alloc,
  .free = fd_free,
  .start = fd_start,
  .fill = fd_fill,
  .end = fd_end,
  .ioctl = NULL
};
