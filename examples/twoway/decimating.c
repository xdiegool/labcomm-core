/*
  decimating.c -- LabComm example of a twoway stacked decimation 
                  reader/writer.

  Copyright 2013 Anders Blomdell <anders.blomdell@control.lth.se>

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
#include <stdio.h>
#include <stdlib.h>
#include "labcomm_private.h"
#include "decimating.h"
#include "gen/decimating_messages.h"

struct decimating_private {
  struct decimating decimating;
  struct labcomm_error_handler *error;
  struct labcomm_memory *memory;
  struct labcomm_scheduler *scheduler;
  int encoder_initialized;
  struct labcomm_reader_action_context reader_action_context;
  struct labcomm_writer_action_context writer_action_context;
  LABCOMM_SIGNATURE_ARRAY_DEF(writer_decimation, 
			      struct decimation {
				int n;
				int current;
			      });
  LABCOMM_SIGNATURE_ARRAY_DEF(reader_decimation, int);
};

static void set_decimation(
  decimating_messages_set_decimation *value,
  void * context)
{
  struct decimating_private *decimating = context;
  struct decimation *decimation;

  labcomm_scheduler_data_lock(decimating->scheduler);
  decimation = LABCOMM_SIGNATURE_ARRAY_REF(decimating->memory,
					   decimating->writer_decimation, 
					   struct decimation, 
					   value->signature_index);
  decimation->n = value->decimation;
  decimation->current = 0;
  labcomm_scheduler_data_unlock(decimating->scheduler);
}

static int wrap_reader_alloc(
  struct labcomm_reader *r, 
  struct labcomm_reader_action_context *action_context)
{
  struct decimating_private *decimating = action_context->context;
  
  labcomm_decoder_register_decimating_messages_set_decimation(
    r->decoder, set_decimation, decimating);
  return labcomm_reader_alloc(r, action_context->next);
}

struct send_set_decimation {
  struct decimating_private *decimating;
  decimating_messages_set_decimation set_decimation;

};

static void send_set_decimation(void *arg)
{
  struct send_set_decimation *msg = arg;
  struct labcomm_memory *memory = msg->decimating->memory;

  labcomm_encode_decimating_messages_set_decimation(
    msg->decimating->decimating.writer->encoder, &msg->set_decimation);
  labcomm_memory_free(memory, 1, msg);
}

static void enqueue_decimation(struct decimating_private *decimating,
			       int remote_index, 
			       int amount) 
{
  struct send_set_decimation *msg;
  msg = labcomm_memory_alloc(decimating->memory, 1, sizeof(*msg));
  if (msg) {
    msg->decimating = decimating;
    msg->set_decimation.decimation = amount;
    msg->set_decimation.signature_index = remote_index;
    
    labcomm_scheduler_enqueue(decimating->scheduler, 0, 
			      send_set_decimation, msg);
  }
}

static int wrap_reader_start(
  struct labcomm_reader *r, 
  struct labcomm_reader_action_context *action_context,
  int local_index, int remote_index, struct labcomm_signature *signature,
  void *value)
{
  struct decimating_private *decimating = action_context->context;

  if (value == NULL) {
    int *decimation, amount;
    
    labcomm_scheduler_data_lock(decimating->scheduler);
    decimation = LABCOMM_SIGNATURE_ARRAY_REF(decimating->memory,
					     decimating->reader_decimation, 
					     int,
					     local_index);
    amount = *decimation;
    labcomm_scheduler_data_unlock(decimating->scheduler);
    if (remote_index != 0 && amount != 0) {
      enqueue_decimation(decimating, remote_index, amount);
    }
  }
  return labcomm_reader_start(r, action_context->next, 
                              local_index, remote_index, signature, value);
}

static int wrap_reader_ioctl(
  struct labcomm_reader *r,   
  struct labcomm_reader_action_context *action_context,
  int local_index, int remote_index,
  struct labcomm_signature *signature, 
  uint32_t action, va_list args)
{
  struct decimating_private *decimating = action_context->context;

  if (action == SET_DECIMATION) {
    va_list va;
    int amount;
    int *decimation;

    va_copy(va, args);
    amount = va_arg(va, int);
    va_end(va);
   
    labcomm_scheduler_data_lock(decimating->scheduler);
    decimation = LABCOMM_SIGNATURE_ARRAY_REF(decimating->memory,
					     decimating->reader_decimation, 
					     int,
					     local_index);
    *decimation = amount;
    labcomm_scheduler_data_unlock(decimating->scheduler);
    if (remote_index) {
      enqueue_decimation(decimating, remote_index, amount);
    }
    
  } else {
    return labcomm_reader_ioctl(r, action_context->next,
				local_index, remote_index, signature, 
				action, args);
  }
  return 0;
}

struct labcomm_reader_action decimating_reader_action = {
  .alloc = wrap_reader_alloc,
  .free = NULL,
  .start = wrap_reader_start,
  .end = NULL,
  .fill = NULL,
  .ioctl = wrap_reader_ioctl
};

static void register_signatures(void *context)
{
  struct decimating_private *decimating = context;

  labcomm_encoder_register_decimating_messages_set_decimation(
    decimating->decimating.writer->encoder);
}

static int wrap_writer_alloc(
  struct labcomm_writer *w, 
  struct labcomm_writer_action_context *action_context)
{
  struct decimating_private *decimating = action_context->context;

  labcomm_scheduler_enqueue(decimating->scheduler, 
			    0, register_signatures, decimating);
  return labcomm_writer_alloc(w, action_context->next);
}

static int wrap_writer_start(
  struct labcomm_writer *w, 
  struct labcomm_writer_action_context *action_context, 
  int index, struct labcomm_signature *signature,
  void *value)
{
  struct decimating_private *decimating = action_context->context;
  struct decimation *decimation;
  int result;

  if (index < LABCOMM_USER) {
    result = 0;
  } else {
    labcomm_scheduler_data_lock(decimating->scheduler);
    decimation = LABCOMM_SIGNATURE_ARRAY_REF(decimating->memory, 
                                             decimating->writer_decimation, 
                                             struct decimation, index);
    decimation->current++;
    if (decimation->current < decimation->n) {
      result = -EALREADY;
    } else {
      decimation->current = 0;
      result = 0;
    }
    labcomm_scheduler_data_unlock(decimating->scheduler);
  }
  if (result == 0) {
    result = labcomm_writer_start(w, action_context->next,
				 index, signature, value);
  }
  return result;
}

struct labcomm_writer_action decimating_writer_action = {
  .alloc = wrap_writer_alloc,
  .free = NULL, 
  .start = wrap_writer_start,
  .end = NULL,
  .flush = NULL,
  .ioctl = NULL
};

struct decimating *decimating_new(
  struct labcomm_reader *reader,
  struct labcomm_writer *writer,
  struct labcomm_error_handler *error,
  struct labcomm_memory *memory,
  struct labcomm_scheduler *scheduler)
{
  struct decimating_private *result;

  result = malloc(sizeof(*result));
  if (result == NULL) {
    goto out_fail;
  }

  /* Wrap reader and writer */
  result->reader_action_context.next = reader->action_context;
  result->reader_action_context.action = &decimating_reader_action;
  result->reader_action_context.context = result;
  reader->action_context = &result->reader_action_context;

  result->writer_action_context.next = writer->action_context;
  result->writer_action_context.action = &decimating_writer_action;
  result->writer_action_context.context = result;
  writer->action_context = &result->writer_action_context;

  /* Init visible result struct */
  result->decimating.reader = reader;
  result->decimating.writer = writer;

  /* Init other fields */
  result->error = error;
  result->memory = memory;
  result->scheduler = scheduler;
  LABCOMM_SIGNATURE_ARRAY_INIT(result->writer_decimation, struct decimation);
  LABCOMM_SIGNATURE_ARRAY_INIT(result->reader_decimation, int);

  goto out_ok;

out_fail:
  return NULL;

out_ok:
  return &result->decimating;
}


