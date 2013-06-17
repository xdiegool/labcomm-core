/*
  introspecting.c -- LabComm example of a twoway stacked introspection 
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
#include "introspecting.h"
#include "gen/introspecting_messages.h"

struct introspecting_private {
  struct introspecting introspecting;
  struct labcomm_encoder *encoder;
  int encoder_initialized;
  struct labcomm_decoder *decoder;
  int decoder_initialized;
  struct labcomm_lock *lock;
  struct labcomm_reader_action_context reader_action_context;
  struct labcomm_writer_action_context writer_action_context;
  LABCOMM_SIGNATURE_ARRAY_DEF(introspection, 
			      struct introspection {
				int has_got_response;
				int has_handler;
			      });
};

static void handles_signature(
  introspecting_messages_handles_signature *value,
  void * context)
{
  fprintf(stderr, "### %s %x %s\n", __FUNCTION__, value->index, value->name);
/*
  struct introspecting_private *introspecting = context;
  struct introspection *introspection;

  introspection = LABCOMM_SIGNATURE_ARRAY_REF(introspecting->introspection, 
					      struct introspection, 
					      value->index);

  introspection->has_handler = value->has_handler;
*/
}

static int wrap_reader_alloc(
  struct labcomm_reader *r, 
  struct labcomm_reader_action_context *action_context, 
  struct labcomm_decoder *decoder,
  char *labcomm_version)
{
  int result;
  struct introspecting_private *introspecting = action_context->context;

  fprintf(stderr, "%s %s\n", __FILE__, __FUNCTION__);
  /* Stash away decoder for later use */
  introspecting->decoder = decoder;
  result = labcomm_reader_alloc(r, action_context->next, 
				decoder, labcomm_version);
  labcomm_decoder_register_introspecting_messages_handles_signature(
    introspecting->decoder, handles_signature, introspecting);
  introspecting->decoder_initialized = 1;
  return result;
}

static int wrap_reader_start(
  struct labcomm_reader *r, 
  struct labcomm_reader_action_context *action_context,
  int index, struct labcomm_signature *signature,
  void *value)
{
  struct introspecting_private *introspecting = action_context->context;

  if (value == NULL && introspecting->decoder_initialized) {
    introspecting_messages_handles_signature handles_signature;

    handles_signature.index = index;
    handles_signature.name = signature->name;
    handles_signature.signature.n_0 = signature->size;
    handles_signature.signature.a = signature->signature;
    labcomm_encode_introspecting_messages_handles_signature(
      introspecting->encoder, &handles_signature);
  }
  return labcomm_reader_start(r, action_context->next, index, signature, value);
}

 void encode_handles_signature(
  struct labcomm_encoder *encoder,
  void *context)
{
  struct labcomm_signature *signature = context;
  introspecting_messages_handles_signature handles_signature;
  int index = 0;

  fprintf(stderr, "## Handles %x %s\n", index, signature->name);
  handles_signature.index = index;
  handles_signature.name = signature->name;
  handles_signature.signature.n_0 = signature->size;
  handles_signature.signature.a = signature->signature;

  labcomm_encode_introspecting_messages_handles_signature(
    NULL, &handles_signature);
}

struct labcomm_reader_action introspecting_reader_action = {
  .alloc = wrap_reader_alloc,
  .free = NULL,
  .start = wrap_reader_start,
  .end = NULL,
  .fill = NULL,
  .ioctl = NULL
};

static void register_signatures(struct labcomm_encoder *encoder,
				void *context)
{
  labcomm_encoder_register_introspecting_messages_handles_signature(
    encoder);
}

static int wrap_writer_alloc(
  struct labcomm_writer *w, 
  struct labcomm_writer_action_context *action_context, 
  struct labcomm_encoder *encoder, char *labcomm_version,
  labcomm_encoder_enqueue enqueue)
{
  int result;
  struct introspecting_private *introspecting = action_context->context;

  fprintf(stderr, "%s %s\n", __FILE__, __FUNCTION__);
  /* Stash away encoder for later use */
  introspecting->encoder = encoder;
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
//  struct introspecting_private *introspecting = action_context->context;

  fprintf(stderr, "%s %p\n", __FUNCTION__, value);
  return labcomm_writer_start(w, action_context->next, index, signature, value);
}

struct labcomm_writer_action introspecting_writer_action = {
  .alloc = wrap_writer_alloc,
  .free = NULL, 
  .start = wrap_writer_start,
  .end = NULL,
  .flush = NULL,
  .ioctl = NULL
};

extern struct introspecting *introspecting_new(
  struct labcomm_reader *reader,
  struct labcomm_writer *writer,
  struct labcomm_lock *lock)
{
  struct introspecting_private *result;

  result = malloc(sizeof(*result));
  if (result == NULL) {
    goto out_fail;
  }

  /* Wrap reader and writer */
  result->reader_action_context.next = reader->action_context;
  result->reader_action_context.action = &introspecting_reader_action;
  result->reader_action_context.context = result;
  reader->action_context = &result->reader_action_context;

  result->writer_action_context.next = writer->action_context;
  result->writer_action_context.action = &introspecting_writer_action;
  result->writer_action_context.context = result;
  writer->action_context = &result->writer_action_context;

  /* Init visible result struct */
  result->introspecting.reader = reader;
  result->introspecting.writer = writer;

  /* Init other fields */
  result->encoder = NULL;
  result->encoder_initialized = 0;
  result->decoder = NULL;
  result->decoder_initialized = 0;
  result->lock = lock;
  LABCOMM_SIGNATURE_ARRAY_INIT(result->introspection, struct introspection);

  goto out_ok;

out_fail:
  return NULL;

out_ok:
  return &result->introspecting;
}


