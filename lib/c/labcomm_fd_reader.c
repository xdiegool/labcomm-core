#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "labcomm_fd_reader.h"

#define BUFFER_SIZE 2048

int labcomm_fd_reader(
  labcomm_reader_t *r, 
  labcomm_reader_action_t action,
  ...)
{
  int result = -EINVAL;
  int *fd = r->context;
  
  switch (action) {
    case labcomm_reader_alloc: {
#ifndef LABCOMM_FD_OMIT_VERSION
      va_list ap;
      va_start(ap, action);
      char *version = va_arg(ap, char *);
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
#ifndef LABCOMM_FD_OMIT_VERSION
	va_end(ap);
#endif
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
    case labcomm_reader_ioctl: {
      result = -ENOTSUP;
    }
  }
  return result;
}

