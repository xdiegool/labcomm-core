#include <errno.h>
#include <stdlib.h>
#include <stdarg.h>
#include "labcomm_ioctl.h"
#include "labcomm_dynamic_buffer_writer.h"

static int labcomm_dynamic_buffer_writer_ioctl(
  struct labcomm_writer *w, 
  int action, 
  labcomm_signature_t *signature,
  va_list arg)
{
  int result = -ENOTSUP;
  switch (action) {
    case LABCOMM_IOCTL_WRITER_GET_BYTES_WRITTEN: {
      int *value = va_arg(arg, int*);
      *value = w->pos;
      result = 0;
    } break;
    case LABCOMM_IOCTL_WRITER_GET_BYTE_POINTER: {
      void **value = va_arg(arg, void**);
      *value = w->data;
      result = 0;
    } break;
  }
  return result;
}

int labcomm_dynamic_buffer_writer(
  labcomm_writer_t *w,
  labcomm_writer_action_t action,
  ...)
{
  switch (action) {
    case labcomm_writer_alloc: {
      w->data_size = 1000;
      w->count = w->data_size;
      w->data = malloc(w->data_size);
      if (w->data == NULL) {
	w->error = -ENOMEM;
      }
      w->pos = 0;
      w->ioctl = labcomm_dynamic_buffer_writer_ioctl;
    } break;
    case labcomm_writer_start: 
    case labcomm_writer_start_signature: {
      void *tmp;
      w->data_size = 1000;
      w->count = w->data_size;
      tmp = realloc(w->data, w->data_size);
      if (tmp != NULL) {
	w->data = tmp;
	w->error = 0;
      } else {
	w->error = -ENOMEM;
      }
      w->pos = 0;
    } break;
    case labcomm_writer_continue: 
    case labcomm_writer_continue_signature: {
      void *tmp;
      w->data_size += 1000;
      w->count = w->data_size;
      tmp = realloc(w->data, w->data_size);
      if (tmp != NULL) {
	w->data = tmp;
      } else {
	w->error = -ENOMEM;
      }
    } break;
    case labcomm_writer_end: 
    case labcomm_writer_end_signature: {
    } break;
    case labcomm_writer_free: {
      free(w->data);
      w->data = 0;
      w->data_size = 0;
      w->count = 0;
      w->pos = 0;
    } break;
  }
  return w->error;
}

