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

enum status {unknown, unhandled, unregistered, registered};
struct introspecting_private {
  struct introspecting introspecting;
  struct labcomm_error_handler *error;
  struct labcomm_memory *memory;
  struct labcomm_scheduler *scheduler;

  struct labcomm_reader_action_context reader_action_context;
  struct labcomm_writer_action_context writer_action_context;
  LABCOMM_SIGNATURE_ARRAY_DEF(remote, 
			      struct remote {
				char *name;
				int size;
				uint8_t *signature;
			      });
  LABCOMM_SIGNATURE_ARRAY_DEF(local, 
			      struct local {
				enum status status;
				struct labcomm_signature *signature;
			      });
};

static void handles_signature(
  introspecting_messages_handles_signature *value,
  void * context)
{
  fprintf(stderr, "### %s %x %s\n", __FUNCTION__, value->index, value->name);
  struct introspecting_private *introspecting = context;
  struct remote *remote;

  remote = LABCOMM_SIGNATURE_ARRAY_REF(introspecting->memory,
				       introspecting->remote, 
				       struct remote, 
				       value->index);
  remote->name = strdup(value->name);
  remote->signature = malloc(value->signature.n_0);
  if (remote->signature) {
    int i;

    memcpy(remote->signature, value->signature.a, value->signature.n_0);
    remote->size = value->signature.n_0;
    LABCOMM_SIGNATURE_ARRAY_FOREACH(introspecting->local, struct local, i) {
      struct local *l;
      
      l = LABCOMM_SIGNATURE_ARRAY_REF(introspecting->memory,
				      introspecting->local, struct local, i);
      if (l->signature && 
	  l->status == unhandled &&
	  l->signature->name && 
	  strcmp(remote->name, l->signature->name) == 0 &&
	  remote->size == l->signature->size &&
	  memcmp(l->signature->signature, remote->signature, 
		 l->signature->size) == 0) {
	fprintf(stderr, "OK %s %x %x\n", __FUNCTION__, value->index, i);
	l->status = unregistered;
      }
    }
  }
}

static int wrap_reader_alloc(
  struct labcomm_reader *r, 
  struct labcomm_reader_action_context *action_context, 
  char *labcomm_version)
{
  int result;
  struct introspecting_private *introspecting = action_context->context;

  fprintf(stderr, "%s %s\n", __FILE__, __FUNCTION__);
  result =  labcomm_reader_alloc(r, action_context->next, labcomm_version);
  labcomm_decoder_register_introspecting_messages_handles_signature(
    introspecting->introspecting.reader->decoder, 
    handles_signature, introspecting);
  return result;
}

struct handles_signature {
  struct introspecting_private *introspecting;
  int index;
  struct labcomm_signature *signature;
};

static void send_handles_signature(void *arg)
{
  struct handles_signature *h = arg;

  introspecting_messages_handles_signature handles_signature;
  handles_signature.index = h->index;
  handles_signature.name = h->signature->name;
  handles_signature.signature.n_0 = h->signature->size;
  handles_signature.signature.a = h->signature->signature;
  labcomm_encode_introspecting_messages_handles_signature(
    h->introspecting->introspecting.writer->encoder, &handles_signature);
}

static int wrap_reader_start(
  struct labcomm_reader *r, 
  struct labcomm_reader_action_context *action_context,
  int local_index, int remote_index, struct labcomm_signature *signature,
  void *value)
{
  struct introspecting_private *introspecting = action_context->context;
  
  if (value == NULL) {
    struct handles_signature *handles_signature;

    handles_signature = labcomm_memory_alloc(introspecting->memory, 1,
					     sizeof(*handles_signature));
    handles_signature->introspecting = introspecting;
    handles_signature->index = local_index;
    handles_signature->signature = signature;
    labcomm_scheduler_enqueue(introspecting->scheduler, 
			      0, send_handles_signature, handles_signature);

  }
  return labcomm_reader_start(r, action_context->next, 
			      local_index, remote_index, signature, value);
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

static void register_encoder_signatures(void *context)
{
  struct introspecting_private *introspecting = context;

  labcomm_encoder_register_introspecting_messages_handles_signature(
    introspecting->introspecting.writer->encoder);
}

static int wrap_writer_alloc(
  struct labcomm_writer *w, 
  struct labcomm_writer_action_context *action_context, 
  char *labcomm_version)
{
  struct introspecting_private *introspecting = action_context->context;

  fprintf(stderr, "%s %s\n", __FILE__, __FUNCTION__);
  labcomm_scheduler_enqueue(introspecting->scheduler, 
			    0, register_encoder_signatures, introspecting);
  return labcomm_writer_alloc(w, action_context->next, labcomm_version);
}

static int wrap_writer_start(
  struct labcomm_writer *w, 
  struct labcomm_writer_action_context *action_context, 
  int index, struct labcomm_signature *signature,
  void *value)
{
  struct introspecting_private *introspecting = action_context->context;
  struct local *local;

  fprintf(stderr, "%s %x %s\n", __FUNCTION__, index, signature->name);
  local = LABCOMM_SIGNATURE_ARRAY_REF(introspecting->memory,
				      introspecting->local, 
				      struct local, 
				      index);
  if (local->signature == NULL) {
    local->signature = signature;
  }
  if (local->status == unknown) {
    int i;
    int found = 0;

    LABCOMM_SIGNATURE_ARRAY_FOREACH(introspecting->remote, struct remote, i) {
      struct remote *r;
      
      r = LABCOMM_SIGNATURE_ARRAY_REF(introspecting->memory,
				      introspecting->remote, struct remote, i);
      if (r->name && 
	  strcmp(signature->name, r->name) == 0 &&
	  r->size == signature->size &&
	  memcmp(signature->signature, r->signature, signature->size) == 0) {
	fprintf(stderr, "OK %s %x %x\n", __FUNCTION__, index, i);
	found = i;
      }
    }
    if (found == 0) {
      local->status = unhandled;
    }
    fprintf(stderr, "Found: %d\n", found);
  }
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
  struct labcomm_error_handler *error,
  struct labcomm_memory *memory,
  struct labcomm_scheduler *scheduler)
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
  result->error = error;
  result->memory = memory;
  result->scheduler = scheduler;
  LABCOMM_SIGNATURE_ARRAY_INIT(result->remote, struct remote);
  LABCOMM_SIGNATURE_ARRAY_INIT(result->local, struct local);

  goto out_ok;

out_fail:
  return NULL;

out_ok:
  return &result->introspecting;
}


