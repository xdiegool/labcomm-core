/*
  labcomm_decoder.c -- runtime for handling decoding of labcomm samples.

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
#define CURRENT_VERSION "LabComm2014"

#include <errno.h>
#include "labcomm.h"
#include "labcomm_private.h"
#include "labcomm_ioctl.h"
#include "labcomm_dynamic_buffer_writer.h"

struct sample_entry {
  int remote_index;
  struct labcomm_signature *signature;
  labcomm_decoder_function decode;
  labcomm_handler_function handler;
  void *context;
};

struct labcomm_decoder {
  struct labcomm_reader *reader;
  int reader_allocated;
  int version_ok;
  struct labcomm_error_handler *error;
  struct labcomm_memory *memory;
  struct labcomm_scheduler *scheduler;
  labcomm_error_handler_callback on_error;
  labcomm_handle_new_datatype_callback on_new_datatype;
  LABCOMM_SIGNATURE_ARRAY_DEF(local, struct sample_entry);
  LABCOMM_SIGNATURE_ARRAY_DEF(remote_to_local, int);
};

struct labcomm_decoder *labcomm_decoder_new(
  struct labcomm_reader *reader,
  struct labcomm_error_handler *error,
  struct labcomm_memory *memory,
  struct labcomm_scheduler *scheduler)
{
  struct labcomm_decoder *result;

  result = labcomm_memory_alloc(memory, 0, sizeof(*result));
  if (result) {
    result->reader = reader;
    result->reader->decoder = result;
    result->reader->data = 0;
    result->reader->data_size = 0;
    result->reader->count = 0;
    result->reader->pos = 0;
    result->reader->error = 0;
    result->reader_allocated = 0;
    result->version_ok = 0;
    result->error = error;
    result->memory = memory;
    result->scheduler = scheduler;
    result->on_error = on_error_fprintf;
    LABCOMM_SIGNATURE_ARRAY_INIT(result->local, struct sample_entry);
    LABCOMM_SIGNATURE_ARRAY_INIT(result->remote_to_local, int);
  }
  return result;
}

void labcomm_decoder_free(struct labcomm_decoder* d)
{
  struct labcomm_memory *memory = d->memory;

  labcomm_reader_free(d->reader, d->reader->action_context);
  LABCOMM_SIGNATURE_ARRAY_FREE(memory, d->local, struct sample_entry);
  LABCOMM_SIGNATURE_ARRAY_FREE(memory, d->remote_to_local, int);
  labcomm_memory_free(memory, 0, d);
}

static int decode_sample(struct labcomm_decoder *d, int kind)
{
  int result;
  struct labcomm_signature signature, *local_signature;
  int remote_index, local_index, i;
  
  local_signature = NULL;
  local_index = 0;
  remote_index = labcomm_read_packed32(d->reader);
  if (d->reader->error < 0) {
    result = d->reader->error;
    goto out;
  }
  signature.name = labcomm_read_string(d->reader);
  if (d->reader->error < 0) {
    result = d->reader->error;
    goto out;
  }
  signature.size = labcomm_read_packed32(d->reader);
  if (d->reader->error < 0) {
    result = d->reader->error;
    goto free_signature_name;
  }
  signature.signature = labcomm_memory_alloc(d->memory, 1,  signature.size);
  if (d->reader->error < 0) {
    result = d->reader->error;
    goto free_signature_name;
  }
  for (i = 0 ; i < signature.size ; i++) {
    signature.signature[i] = labcomm_read_byte(d->reader);
    if (d->reader->error < 0) {
      result = d->reader->error;
      goto free_signature_signature;
    }
  }
  labcomm_scheduler_data_lock(d->scheduler);
  LABCOMM_SIGNATURE_ARRAY_FOREACH(d->local, struct sample_entry, i) {
    struct sample_entry *s;
    int *remote_to_local;
      
    result = -ENOENT;
    s = LABCOMM_SIGNATURE_ARRAY_REF(d->memory, 
				      d->local,  struct sample_entry, i);
    if (s->signature &&
        s->signature->size == signature.size &&
        strcmp(s->signature->name, signature.name) == 0 &&
        memcmp((void*)s->signature->signature, (void*)signature.signature,
	       signature.size) == 0) {
      s->remote_index = remote_index;
      local_signature = s->signature;
      local_index = i;
      remote_to_local = LABCOMM_SIGNATURE_ARRAY_REF(d->memory,
                                                    d->remote_to_local, int,
                                                    remote_index);
      *remote_to_local = i;
      result = remote_index;
      break;
    }
  }
  labcomm_scheduler_data_unlock(d->scheduler);
  if (local_signature) {
    labcomm_reader_start(d->reader, d->reader->action_context,
                         local_index, remote_index, local_signature,
                         NULL);
    labcomm_reader_end(d->reader, d->reader->action_context);
  }
free_signature_signature:
  labcomm_memory_free(d->memory, 1,  signature.signature);
free_signature_name:
  labcomm_memory_free(d->memory, 0, signature.name);
out:
  return result;
}

struct call_handler_context {
  struct labcomm_reader *reader;
  int local_index;
  int remote_index;
  struct labcomm_signature *signature;
  labcomm_handler_function handler;
  void *context;
};

static void call_handler(void *value, void *context)
{
  struct call_handler_context *wrap = context;

  if (wrap->reader->error >= 0) {
    labcomm_reader_start(wrap->reader, wrap->reader->action_context,
			 wrap->local_index, wrap->remote_index, wrap->signature,
			 value);
    wrap->handler(value, wrap->context);
    labcomm_reader_end(wrap->reader, wrap->reader->action_context);
  }
}

static void reader_alloc(struct labcomm_decoder *d)
{
  if (!d->reader_allocated) {
    d->reader_allocated = 1;
    labcomm_reader_alloc(d->reader, d->reader->action_context);
  }
}

int labcomm_decoder_decode_one(struct labcomm_decoder *d)
{
  int result, remote_index, length;

  reader_alloc(d);
  remote_index = labcomm_read_packed32(d->reader);
  if (d->reader->error < 0) {
    result = d->reader->error;
    goto out;
  }
  length = labcomm_read_packed32(d->reader);
  if (d->reader->error < 0) {
    result = d->reader->error;
    goto out;
  }
  if (remote_index == LABCOMM_VERSION) {
    char *version = labcomm_read_string(d->reader);
    if (d->reader->error < 0) {
      result = d->reader->error;
      goto out;
    }
    if (strcmp(version, CURRENT_VERSION) == 0) {
      result = LABCOMM_VERSION;
      d->version_ok = 1;
    } else {
      result = -ECONNRESET;
    }  
    labcomm_memory_free(d->memory, 1,  version);
  } else if (! d->version_ok) {
    fprintf(stderr, "No VERSION %d %d\n", remote_index, length);
    result = -ECONNRESET;
  } else if (remote_index == LABCOMM_SAMPLE) {
    result = decode_sample(d, remote_index); 
  } else if (remote_index == LABCOMM_PRAGMA && 0 /* d->pragma_handler*/) {
    /* d->prama_handler(...); */
  } else if (remote_index < LABCOMM_USER) {
    fprintf(stderr, "SKIP %d %d\n", remote_index, length);
    result = remote_index;
  } else {
    int *local_index;
    struct call_handler_context wrap = {
      .reader = d->reader,
      .remote_index = remote_index,
      .signature = NULL,
      .handler = NULL,
      .context = NULL,
    };
    labcomm_decoder_function do_decode = NULL;

    labcomm_scheduler_data_lock(d->scheduler);
    local_index = LABCOMM_SIGNATURE_ARRAY_REF(d->memory,
					      d->remote_to_local, int,
					      remote_index);
    if (*local_index != 0) {
      struct sample_entry *entry;

      entry = LABCOMM_SIGNATURE_ARRAY_REF(d->memory,
					  d->local, struct sample_entry,
					  *local_index);
      wrap.local_index = *local_index;
      wrap.signature = entry->signature;
      wrap.handler = entry->handler;
      wrap.context = entry->context;
      do_decode = entry->decode;
      result = *local_index;
    }
    labcomm_scheduler_data_unlock(d->scheduler);
    if (do_decode) {
      do_decode(d->reader, call_handler, &wrap);
      if (d->reader->error < 0) {
	result = d->reader->error;
      }
    } else {
      result = -ENOENT;
    }
  }
out:   
  return result;
}

void labcomm_decoder_run(struct labcomm_decoder *d)
{
  while (labcomm_decoder_decode_one(d) > 0) {
  }
}

int labcomm_decoder_ioctl(struct labcomm_decoder *d, 
			  uint32_t action,
			  ...)
{
  int result;  
  va_list va;
    
  va_start(va, action);
  result = labcomm_reader_ioctl(d->reader, 
				d->reader->action_context,
				0, 0, NULL, action, va);
  va_end(va);
  return result;
}

int labcomm_internal_decoder_ioctl(struct labcomm_decoder *d, 
				   struct labcomm_signature *signature,
				   uint32_t action, va_list va)
{
  int result;
  int local_index, remote_index;

  local_index = labcomm_get_local_index(signature);
  labcomm_scheduler_data_lock(d->scheduler);
  remote_index = LABCOMM_SIGNATURE_ARRAY_REF(d->memory,
					     d->local,
					     struct sample_entry,
					     local_index)->remote_index;
  labcomm_scheduler_data_unlock(d->scheduler);
  result = labcomm_reader_ioctl(d->reader, d->reader->action_context,
				local_index, remote_index, 
				signature, action, va);
  return result;
}

int labcomm_internal_decoder_register(
  struct labcomm_decoder *d,
  struct labcomm_signature *signature,
  labcomm_decoder_function decode, 
  labcomm_handler_function handler,
  void *context)
{
  int local_index;
  struct sample_entry *entry;
 
  reader_alloc(d);
  local_index = labcomm_get_local_index(signature);
  if (local_index <= 0) { goto out; }
  labcomm_reader_start(d->reader, d->reader->action_context,
		       local_index, 0, signature,
		       NULL);
  labcomm_reader_end(d->reader, d->reader->action_context);

  labcomm_scheduler_data_lock(d->scheduler);
  entry = LABCOMM_SIGNATURE_ARRAY_REF(d->memory,
				      d->local, struct sample_entry,
				      local_index);
  if (entry == NULL) { local_index = -ENOMEM; goto unlock; }
  entry->remote_index = 0;
  entry->signature = signature;
  entry->decode = decode;
  entry->handler = handler;
  entry->context = context;
unlock:
  labcomm_scheduler_data_unlock(d->scheduler);
out:
  return local_index;
}

