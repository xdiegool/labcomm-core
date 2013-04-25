#include <errno.h>
#include <unistd.h>
#include "labcomm_fd_reader_writer.h"

#define BUFFER_SIZE 2048

int labcomm_fd_reader(
  labcomm_reader_t *r, 
  labcomm_reader_action_t action)
{
  int result = -EINVAL;
  int *fd = r->context;
  
  switch (action) {
    case labcomm_reader_alloc: {
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

int labcomm_fd_writer(
  labcomm_writer_t *w, 
  labcomm_writer_action_t action, ...)
{
  int result = 0;
  int *fd = w->context;

  switch (action) {
    case labcomm_writer_alloc: {
      w->data = malloc(BUFFER_SIZE);
      if (! w->data) {
        result = -ENOMEM;
        w->data_size = 0;
        w->count = 0;
        w->pos = 0;
      } else {
        w->data_size = BUFFER_SIZE;
        w->count = BUFFER_SIZE;
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
    case labcomm_writer_start: 
    case labcomm_writer_start_signature: {
      w->pos = 0;
    } break;
    case labcomm_writer_continue: 
    case labcomm_writer_continue_signature: {
      result = write(*fd, w->data, w->pos);
      w->pos = 0;
    } break;
    case labcomm_writer_end: 
    case labcomm_writer_end_signature: {
      result = write(*fd, w->data, w->pos);
      w->pos = 0;
    } break;
  }
  return result;
}
