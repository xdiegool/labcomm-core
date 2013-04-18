#include "labcomm_dynamic_buffer_writer.h"

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
      w->pos = 0;
    } break;
    case labcomm_writer_start: 
    case labcomm_writer_start_signature: {
      w->data_size = 1000;
      w->count = w->data_size;
      w->data = realloc(w->data, w->data_size);
      w->pos = 0;
    } break;
    case labcomm_writer_continue: 
    case labcomm_writer_continue_signature: {
      w->data_size += 1000;
      w->count = w->data_size;
      w->data = realloc(w->data, w->data_size);
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
  return 0;
}

