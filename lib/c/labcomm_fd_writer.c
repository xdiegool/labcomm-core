#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "labcomm_fd_writer.h"

#define BUFFER_SIZE 2048

static int fd_alloc(struct labcomm_writer *w, char *version)
{
#ifndef LABCOMM_FD_OMIT_VERSION
  int *fd = w->context;
  write(*fd, version, strlen(version));
#endif
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
  }

  return w->error;
}

static int fd_free(struct labcomm_writer *w)
{
  free(w->data);
  w->data = 0;
  w->data_size = 0;
  w->count = 0;
  w->pos = 0;

  return 0;
}

static int fd_start(struct labcomm_writer *w,
		    struct labcomm_encoder *encoder,
		    int index,
		    labcomm_signature_t *signature,
		    void *value)
{
  w->pos = 0;
  
  return w->error;
}

static int fd_flush(struct labcomm_writer *w)
{
  int *fd = w->context;
  int err;

  err = write(*fd, w->data, w->pos);
  if (err < 0) {
    w->error = -errno;
  } else if (err == 0) {
    w->error = -EINVAL;
  }
  w->pos = 0;
   
  return w->error;
}

const struct labcomm_writer_action labcomm_fd_writer = {
  .alloc = fd_alloc,
  .free = fd_free,
  .start = fd_start,
  .end = fd_flush,
  .flush = fd_flush,
  .ioctl = NULL
};
