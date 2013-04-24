#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "labcomm_fd_writer.h"

#define BUFFER_SIZE 2048

int labcomm_fd_writer(
  labcomm_writer_t *w, 
  labcomm_writer_action_t action,
  ...)
{
  int result = 0;
  int *fd = w->context;

  switch (action) {
    case labcomm_writer_alloc: {
      va_list ap;
      va_start(ap, action);
      char *version = va_arg(ap, char *);

      write(*fd, version, strlen(version));
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
      va_end(ap);
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
