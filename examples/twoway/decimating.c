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
  struct labcomm_lock *lock;
  struct labcomm_encoder *encoder;
  int encoder_initialized;
  struct labcomm_reader_action_context reader_action_context;
  struct labcomm_writer_action_context writer_action_context;
  LABCOMM_SIGNATURE_ARRAY_DEF(decimation, 
			      struct decimation {
				int n;
				int current;
			      });
};

static void set_decimation(
  decimating_messages_set_decimation *value,
  void * context)
{
  struct decimating_private *decimating = context;
  struct decimation *decimation;

  decimation = LABCOMM_SIGNATURE_ARRAY_REF(decimating->decimation, 
					   struct decimation, 
					   value->signature_index);
  decimation->n = value->decimation;
  decimation->current = 0;
}

static int wrap_reader_alloc(
  struct labcomm_reader *r, 
  struct labcomm_reader_action_context *action_context, 
  struct labcomm_decoder *decoder,
  char *labcomm_version)
{
  int result;

  struct decimating_private *decimating = action_context->context;
  
  fprintf(stderr, "%s %s\n", __FILE__, __FUNCTION__);
  /* Stash away decoder for later use */
  result = labcomm_reader_alloc(r, action_context->next, 
				decoder, labcomm_version);
  labcomm_decoder_register_decimating_messages_set_decimation(
    decoder, set_decimation, decimating);
  return result;
}

static int wrap_reader_ioctl(
  struct labcomm_reader *r,   
  struct labcomm_reader_action_context *action_context,
  int signature_index,
  struct labcomm_signature *signature, 
  uint32_t action, va_list args)
{
  struct decimating_private *decimating = action_context->context;

  if (action == SET_DECIMATION) {
    decimating_messages_set_decimation decimation;
    va_list va;

    va_copy(va, args);
    decimation.decimation = va_arg(va, int);
    decimation.signature_index = signature_index;
    va_end(va);
    return labcomm_encode_decimating_messages_set_decimation(
      decimating->encoder, &decimation);
  } else {
    return labcomm_reader_ioctl(r, action_context->next,
				signature_index, signature, action, args);
  }
  
}

struct labcomm_reader_action decimating_reader_action = {
  .alloc = wrap_reader_alloc,
  .free = NULL,
  .start = NULL,
  .end = NULL,
  .fill = NULL,
  .ioctl = wrap_reader_ioctl
};

static void register_signatures(struct labcomm_encoder *encoder,
				void *context)
{
  labcomm_encoder_register_decimating_messages_set_decimation(
    encoder);
}

static int wrap_writer_alloc(
  struct labcomm_writer *w, 
  struct labcomm_writer_action_context *action_context, 
  struct labcomm_encoder *encoder, char *labcomm_version,
  labcomm_encoder_enqueue enqueue)
{
  int result;
  struct decimating_private *decimating = action_context->context;

  fprintf(stderr, "%s %s\n", __FILE__, __FUNCTION__);
  /* Stash away encoder for later use */
  decimating->encoder = encoder;
  result = labcomm_writer_alloc(w, action_context->next,
				encoder, labcomm_version, enqueue);
  enqueue(encoder, register_signatures, NULL);

  return result;
}

static int wrap_writer_start(
  struct labcomm_writer *w, 
  struct labcomm_writer_action_context *action_context, 
  int index, struct labcomm_signature *signature,
  void *value)
{
  struct decimating_private *decimating = action_context->context;
  struct decimation *decimation;

  decimation = LABCOMM_SIGNATURE_ARRAY_REF(decimating->decimation, 
					   struct decimation, index);
  decimation->current++;
  if (decimation->current < decimation->n) {
    return -EALREADY;
  } else {
    decimation->current = 0;
    return labcomm_writer_start(w, action_context->next,
				index, signature, value);
  }
}

struct labcomm_writer_action decimating_writer_action = {
  .alloc = wrap_writer_alloc,
  .free = NULL, 
  .start = wrap_writer_start,
  .end = NULL,
  .flush = NULL,
  .ioctl = NULL
};

extern struct decimating *decimating_new(
  struct labcomm_reader *reader,
  struct labcomm_writer *writer,
  struct labcomm_lock *lock)
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
  result->lock = lock;
  result->encoder = NULL;
  result->encoder_initialized = 0;
  LABCOMM_SIGNATURE_ARRAY_INIT(result->decimation, struct decimation);

  goto out_ok;

out_fail:
  return NULL;

out_ok:
  return &result->decimating;
}


