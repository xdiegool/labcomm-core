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
				enum introspecting_status status;
				const struct labcomm_signature *signature;
			      });
};

static struct local *get_local(struct introspecting_private *introspecting,
			       int index,
			       const struct labcomm_signature *signature)
{
  /* Called with data_lock held */
  struct local *local;
  
  local = LABCOMM_SIGNATURE_ARRAY_REF(introspecting->memory,
				      introspecting->local, 
				      struct local, 
				      index);
  if (local->signature == NULL) {
    local->signature = signature;
    local->status = introspecting_unknown;
  }  
  if (local->status == introspecting_unknown) {
    int i;

    local->status = introspecting_unhandled;
    LABCOMM_SIGNATURE_ARRAY_FOREACH(introspecting->remote, struct remote, i) {
      struct remote *r;
      
      r = LABCOMM_SIGNATURE_ARRAY_REF(introspecting->memory,
				      introspecting->remote, struct remote, i);
      if (r->name && 
	  strcmp(signature->name, r->name) == 0 &&
	  r->size == signature->size &&
	  memcmp(signature->signature, r->signature, signature->size) == 0) {
	local->status = introspecting_unregistered;
	break;
      }
    }
  }
  return local;
}

static void handles_signature(
  introspecting_messages_handles_signature *value,
  void * context)
{
  struct introspecting_private *introspecting = context;
  struct remote *remote;

  labcomm_scheduler_data_lock(introspecting->scheduler);
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
	  l->status == introspecting_unhandled &&
	  l->signature->name && 
	  strcmp(remote->name, l->signature->name) == 0 &&
	  remote->size == l->signature->size &&
	  memcmp(l->signature->signature, remote->signature, 
		 l->signature->size) == 0) {
	l->status = introspecting_unregistered;
      }
    }
  }
  labcomm_scheduler_data_unlock(introspecting->scheduler);
}

static int wrap_reader_alloc(
  struct labcomm_reader *r, 
  struct labcomm_reader_action_context *action_context)
{
  struct introspecting_private *introspecting = action_context->context;

  labcomm_decoder_register_introspecting_messages_handles_signature(
    introspecting->introspecting.reader->decoder, 
    handles_signature, introspecting);
  return labcomm_reader_alloc(r, action_context->next);
}

struct handles_signature {
  struct introspecting_private *introspecting;
  int index;
  const struct labcomm_signature *signature;
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
  int local_index, int remote_index, const struct labcomm_signature *signature,
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
  const struct labcomm_signature *signature = context;
  introspecting_messages_handles_signature handles_signature;
  int index = 0;

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
  struct labcomm_writer_action_context *action_context)
{
  struct introspecting_private *introspecting = action_context->context;

  labcomm_scheduler_enqueue(introspecting->scheduler, 
			    0, register_encoder_signatures, introspecting);
  return labcomm_writer_alloc(w, action_context->next);
}

static int wrap_writer_start(
  struct labcomm_writer *w, 
  struct labcomm_writer_action_context *action_context, 
  int index, const struct labcomm_signature *signature,
  void *value)
{
  struct introspecting_private *introspecting = action_context->context;

  if (index >= LABCOMM_USER && value == NULL) {
    struct local *local;

    labcomm_scheduler_data_lock(introspecting->scheduler);
    local = get_local(introspecting, index, signature);
    local->status = introspecting_registered;
    labcomm_scheduler_data_unlock(introspecting->scheduler);
  }
  return labcomm_writer_start(w, action_context->next, index, signature, value);
}

static int wrap_writer_ioctl(
  struct labcomm_writer *w, 
  struct labcomm_writer_action_context *action_context, 
  int index, const struct labcomm_signature *signature, 
  uint32_t ioctl_action, va_list args)
{
  struct introspecting_private *introspecting = action_context->context;

  switch (ioctl_action) {
    case HAS_SIGNATURE: {
      struct local *local;
      int result;

      labcomm_scheduler_data_lock(introspecting->scheduler);
      local = get_local(introspecting, index, signature);
      result = local->status;
      labcomm_scheduler_data_unlock(introspecting->scheduler);
      return result;
    }
    default: {
      return labcomm_writer_ioctl(w, action_context->next, index, signature, 
				  ioctl_action, args);  
    } break;
  }
}

struct labcomm_writer_action introspecting_writer_action = {
  .alloc = wrap_writer_alloc,
  .free = NULL, 
  .start = wrap_writer_start,
  .end = NULL,
  .flush = NULL,
  .ioctl = wrap_writer_ioctl
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


