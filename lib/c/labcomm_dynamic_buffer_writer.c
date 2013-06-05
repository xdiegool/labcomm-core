#include <errno.h>
#include <stdlib.h>
#include <stdarg.h>
#include "labcomm.h"
#include "labcomm_private.h"
#include "labcomm_ioctl.h"
#include "labcomm_dynamic_buffer_writer.h"

static int dyn_alloc(struct labcomm_writer *w, void *context,
		     struct labcomm_encoder *encoder,
		     char *labcomm_version)
{
  w->data_size = 1000;
  w->count = w->data_size;
  w->data = malloc(w->data_size);
  if (w->data == NULL) {
    w->error = -ENOMEM;
  }
  w->pos = 0;

  return w->error;
}

static int dyn_free(struct labcomm_writer *w, void *context)
{
  free(w->data);
  w->data = 0;
  w->data_size = 0;
  w->count = 0;
  w->pos = 0;

  return 0;
}

static int dyn_start(struct labcomm_writer *w, void *context,
		     struct labcomm_encoder *encoder,
		     int index,
		     struct labcomm_signature *signature,
		     void *value)
{
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

  return w->error;
}

static int dyn_end(struct labcomm_writer *w, void *context)
{
  return 0;
}

static int dyn_flush(struct labcomm_writer *w, void *context)
{
  void *tmp;

  w->data_size += 1000;
  w->count = w->data_size;
  tmp = realloc(w->data, w->data_size);
  if (tmp != NULL) {
    w->data = tmp;
    w->error = 0;
  } else {
    w->error = -ENOMEM;
  }

  return w->error; 
}

static int dyn_ioctl(struct labcomm_writer *w, void *context, 
		     int signature_index,
		     struct labcomm_signature *signature,
		     int action, va_list arg)
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

static const struct labcomm_writer_action action = {
  .alloc = dyn_alloc,
  .free = dyn_free,
  .start = dyn_start,
  .end = dyn_end,
  .flush = dyn_flush,
  .ioctl = dyn_ioctl
};
const struct labcomm_writer_action *labcomm_dynamic_buffer_writer_action = 
  &action;

struct labcomm_writer *labcomm_dynamic_buffer_writer_new()
{
  struct labcomm_writer *result;

  result = malloc(sizeof(*result));
  if (result != NULL) {
    result->context = NULL;
    result->action = &action;
  }
  return result;
}

