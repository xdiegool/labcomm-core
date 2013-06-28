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
#define LABCOMM_VERSION "LabComm2013"

#include <errno.h>
#include "labcomm.h"
#include "labcomm_private.h"
#include "labcomm_signature.h"
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
    result->error = error;
    result->memory = memory;
    result->scheduler = scheduler;
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

static void collect_flat_signature(
  struct labcomm_decoder *decoder,
  struct labcomm_writer *writer)
{
  int type = labcomm_read_packed32(decoder->reader); 
  if (type >= LABCOMM_USER) {
    decoder->on_error(LABCOMM_ERROR_UNIMPLEMENTED_FUNC, 3,
			"Implement %s ... (1) for type 0x%x\n", __FUNCTION__, type);
  } else {
    labcomm_write_packed32(writer, type); 
    switch (type) {
      case LABCOMM_ARRAY: {
	int dimensions, i;

	dimensions = labcomm_read_packed32(decoder->reader);
	labcomm_write_packed32(writer, dimensions);
	for (i = 0 ; i < dimensions ; i++) {
	  int n = labcomm_read_packed32(decoder->reader);
	  labcomm_write_packed32(writer, n);
	}
	collect_flat_signature(decoder, writer);
      } break;
      case LABCOMM_STRUCT: {
	int fields, i;

	fields = labcomm_read_packed32(decoder->reader); 
	labcomm_write_packed32(writer, fields); 
	for (i = 0 ; i < fields ; i++) {
	  char *name = labcomm_read_string(decoder->reader);
	  labcomm_write_string(writer, name);
	  labcomm_memory_free(decoder->memory, 1, name);
	  collect_flat_signature(decoder, writer);
	}
      } break;
      case LABCOMM_BOOLEAN:
      case LABCOMM_BYTE:
      case LABCOMM_SHORT:
      case LABCOMM_INT:
      case LABCOMM_LONG:
      case LABCOMM_FLOAT:
      case LABCOMM_DOUBLE:
      case LABCOMM_STRING: {
      } break;
      default: {
        decoder->on_error(LABCOMM_ERROR_UNIMPLEMENTED_FUNC, 3,
				"Implement %s (2) for type 0x%x...\n", __FUNCTION__, type);
      } break;
    }
  }
}

static int writer_ioctl(struct labcomm_writer *writer,
			uint32_t action,
			...)
{
  int result;
  va_list va;

  if (LABCOMM_IOC_SIG(action) != LABCOMM_IOC_NOSIG) {
    result = -EINVAL;
    goto out;
  }
  
  va_start(va, action);
  result = labcomm_writer_ioctl(writer, writer->action_context, 
				0, NULL, action, va);
  va_end(va);
out:
  return result;
}

static int decode_typedef_or_sample(struct labcomm_decoder *d, int kind)
{
  int result;

  /* TODO: should the labcomm_dynamic_buffer_writer be 
     a permanent part of labcomm_decoder? */
  struct labcomm_writer_action_context action_context = {
    .next = NULL,
    .action = labcomm_dynamic_buffer_writer_action,
    .context = NULL
  };
  struct labcomm_writer writer = {
    .action_context = &action_context,
    .memory = d->memory,
    .data = NULL,
    .data_size = 0,
    .count = 0,
    .pos = 0,
    .error = 0,
  };
  struct labcomm_signature signature, *local_signature;
  int remote_index, local_index, err;
  
  local_signature = NULL;
  local_index = 0;
  labcomm_writer_alloc(&writer, writer.action_context, "");
  labcomm_writer_start(&writer, writer.action_context, 0, NULL, NULL);
  remote_index = labcomm_read_packed32(d->reader); //int
  signature.name = labcomm_read_string(d->reader);
  signature.type = kind;
  collect_flat_signature(d, &writer);
  labcomm_writer_end(&writer, writer.action_context);
  err = writer_ioctl(&writer, 
		     LABCOMM_IOCTL_WRITER_GET_BYTES_WRITTEN,
		     &signature.size);
  if (err < 0) {
    fprintf(stderr, "Failed to get size: %s\n", strerror(-err));
    result = -ENOENT;
    goto free_signature_name;
  }
  err = writer_ioctl(&writer, 
		     LABCOMM_IOCTL_WRITER_GET_BYTE_POINTER,
		     &signature.signature);
  if (err < 0) {
    fprintf(stderr, "Failed to get pointer: %s\n", strerror(-err));
    result = -ENOENT;
    goto free_signature_name;
  }
  {
    int i;

    labcomm_scheduler_data_lock(d->scheduler);
    LABCOMM_SIGNATURE_ARRAY_FOREACH(d->local, struct sample_entry, i) {
      struct sample_entry *s;
      int *remote_to_local;
      
      result = -ENOENT;
      s = LABCOMM_SIGNATURE_ARRAY_REF(d->memory, 
				      d->local,  struct sample_entry, i);
      if (s->signature &&
	  s->signature->type == signature.type &&
	  s->signature->size == signature.size &&
	  strcmp(s->signature->name, signature.name) == 0 &&
	  bcmp((void*)s->signature->signature, (void*)signature.signature,
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
  }
#if 0
  if (! entry) {
    /* Unknown datatype, bail out */
    d->on_new_datatype(d, &signature);
    result = -ENOENT;
  } else if (entry->index && entry->index != remote_index) {
    d->on_error(LABCOMM_ERROR_DEC_INDEX_MISMATCH, 5, 
		"%s(): index mismatch '%s' (id=0x%x != 0x%x)\n", 
		__FUNCTION__, signature.name, entry->index, remote_index);
    result = -ENOENT;
#endif
free_signature_name:
  labcomm_memory_free(d->memory, 0, signature.name);
  labcomm_writer_free(&writer, writer.action_context);
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
    labcomm_reader_alloc(d->reader, d->reader->action_context,
			 LABCOMM_VERSION);
  }
}

int labcomm_decoder_decode_one(struct labcomm_decoder *d)
{
  int result, remote_index;

  reader_alloc(d);
  remote_index = labcomm_read_packed32(d->reader);
  if (d->reader->error < 0) {
    result = d->reader->error;
    goto out;
  }
  if (remote_index == LABCOMM_TYPEDEF || remote_index == LABCOMM_SAMPLE) {
    result = decode_typedef_or_sample(d, remote_index); 
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

  local_index = labcomm_signature_local_index(signature);
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
  local_index = labcomm_signature_local_index(signature);
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

