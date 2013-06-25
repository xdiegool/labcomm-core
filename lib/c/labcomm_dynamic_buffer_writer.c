/*
  labcomm_dynamic_buffer_writer.c -- LabComm dynamic memory writer.

  Copyright 2006-2013 Anders Blomdell <anders.blomdell@control.lth.se>

  This file is part of LabComm.

  LabComm is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  LabComm is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <errno.h>
#include <stdlib.h>
#include <stdarg.h>
#include "labcomm.h"
#include "labcomm_private.h"
#include "labcomm_ioctl.h"
#include "labcomm_dynamic_buffer_writer.h"

static int dyn_alloc(struct labcomm_writer *w, 
		     struct labcomm_writer_action_context *action_context,
		     struct labcomm_encoder *encoder,
		     char *labcomm_version,
		     labcomm_encoder_enqueue enqueue)
{
  w->data_size = 1000;
  w->count = w->data_size;
  w->data = labcomm_memory_alloc(w->memory, 1, w->data_size);
  if (w->data == NULL) {
    w->error = -ENOMEM;
  }
  w->pos = 0;

  return w->error;
}

static int dyn_free(struct labcomm_writer *w, 
		    struct labcomm_writer_action_context *action_context)
{
  labcomm_memory_free(w->memory, 1, w->data);
  w->data = 0;
  w->data_size = 0;
  w->count = 0;
  w->pos = 0;
  labcomm_memory_free(w->memory, 0, action_context->context);
  return 0;
}

static int dyn_start(struct labcomm_writer *w, 
		     struct labcomm_writer_action_context *action_context,
		     int index,
		     struct labcomm_signature *signature,
		     void *value)
{
  void *tmp;

  w->data_size = 1000;
  w->count = w->data_size;
  tmp = labcomm_memory_realloc(w->memory, 1, w->data, w->data_size);
  if (tmp != NULL) {
    w->data = tmp;
    w->error = 0;
  } else {
    w->error = -ENOMEM;
  }
  w->pos = 0;

  return w->error;
}

static int dyn_end(struct labcomm_writer *w, 
		   struct labcomm_writer_action_context *action_context)
{
  return 0;
}

static int dyn_flush(struct labcomm_writer *w, 
		     struct labcomm_writer_action_context *action_context)
{
  void *tmp;

  w->data_size += 1000;
  w->count = w->data_size;
  tmp = labcomm_memory_realloc(w->memory, 1, w->data, w->data_size);
  if (tmp != NULL) {
    w->data = tmp;
    w->error = 0;
  } else {
    /* Old pointer in w->data still valid */
    w->error = -ENOMEM;
  }

  return w->error; 
}

static int dyn_ioctl(struct labcomm_writer *w, 
		     struct labcomm_writer_action_context *action_context, 
		     int signature_index,
		     struct labcomm_signature *signature,
		     uint32_t action, va_list arg)
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

struct labcomm_writer *labcomm_dynamic_buffer_writer_new(
  struct labcomm_memory *memory)
{
  struct result {
    struct labcomm_writer writer;
    struct labcomm_writer_action_context action_context;
  } *result;

  result = labcomm_memory_alloc(memory, 0, sizeof(*result));
  if (result != NULL) {
    result->action_context.next = NULL;
    result->action_context.context = result;
    result->action_context.action = &action;
    result->writer.action_context = &result->action_context;
    result->writer.memory = memory;
    return &result->writer;
  }
  return NULL;
}

