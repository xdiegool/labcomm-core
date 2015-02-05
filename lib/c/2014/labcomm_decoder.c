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
  const struct labcomm_signature *signature;
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
  LABCOMM_SIGNATURE_ARRAY_DEF(local_ref, const struct labcomm_signature *);
  LABCOMM_SIGNATURE_ARRAY_DEF(remote_to_local_ref, int);
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
    result->on_error = labcomm2014_on_error_fprintf;
    LABCOMM_SIGNATURE_ARRAY_INIT(result->local, struct sample_entry);
    LABCOMM_SIGNATURE_ARRAY_INIT(result->remote_to_local, int);
    LABCOMM_SIGNATURE_ARRAY_INIT(result->local_ref, 
                                 const struct labcomm_signature*);
    LABCOMM_SIGNATURE_ARRAY_INIT(result->remote_to_local_ref, int);
  }
  return result;
}

void labcomm_decoder_free(struct labcomm_decoder* d)
{
  struct labcomm_memory *memory = d->memory;

  labcomm_reader_free(d->reader, d->reader->action_context);
  LABCOMM_SIGNATURE_ARRAY_FREE(memory, d->local, struct sample_entry);
  LABCOMM_SIGNATURE_ARRAY_FREE(memory, d->remote_to_local, int);
  LABCOMM_SIGNATURE_ARRAY_FREE(memory, d->local_ref, 
                               const struct labcomm_signature*);
  LABCOMM_SIGNATURE_ARRAY_FREE(memory, d->remote_to_local_ref, int);
  labcomm_memory_free(memory, 0, d);
}

static int handle_sample_def(struct labcomm_decoder *d, int remote_index,
                             const struct labcomm_signature *signature)
{
  int result;
  int i;
  const struct labcomm_signature *local_signature = NULL;
  int local_index = 0;

  labcomm_scheduler_data_lock(d->scheduler);
  LABCOMM_SIGNATURE_ARRAY_FOREACH(d->local, struct sample_entry, i) {
    struct sample_entry *s;
    int *remote_to_local;

    result = -ENOENT;
    s = LABCOMM_SIGNATURE_ARRAY_REF(d->memory, 
                                    d->local, struct sample_entry, i);
    if (s->signature &&
        s->signature->size == signature->size &&
        strcmp(s->signature->name, signature->name) == 0 &&
        memcmp((void*)s->signature->signature, (void*)signature->signature,
	       signature->size) == 0) {
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
  return result;
}

static int handle_sample_ref(struct labcomm_decoder *d, int remote_index,
                             const struct labcomm_signature *signature)
{
  int result;
  int i;

  labcomm_scheduler_data_lock(d->scheduler);
  LABCOMM_SIGNATURE_ARRAY_FOREACH(d->local_ref, const struct labcomm_signature *, i) {
    const struct labcomm_signature *s;
    int *remote_to_local_ref;

    result = -ENOENT;
    s = LABCOMM_SIGNATURE_ARRAY_GET(d->local_ref, const struct labcomm_signature *, i, 0);
    if (s &&
        s->signature &&
        s->size == signature->size &&
        strcmp(s->name, signature->name) == 0 &&
        memcmp((void*)s->signature, (void*)signature->signature, signature->size) == 0) {
      remote_to_local_ref = LABCOMM_SIGNATURE_ARRAY_REF(d->memory,
                                                        d->remote_to_local_ref, int,
                                                        remote_index);
      *remote_to_local_ref = i;
      result = remote_index;
      break;
    }
  }
  labcomm_scheduler_data_unlock(d->scheduler);
  return result;
}

static int decoder_skip(struct labcomm_decoder *d, int len, int tag)
{
  int i;
#ifdef LABCOMM_DECODER_DEBUG
  printf("got tag 0x%x, skipping %d bytes\n", tag, len);
#endif
  for(i = 0; i <len; i++){
    fprintf(stderr,".");
    labcomm_read_byte(d->reader);
    if (d->reader->error < 0) {
#ifdef LABCOMM_DECODER_DEBUG
      fprintf(stderr, "\nerror while skipping: %d\n",  d->reader->error);
#endif
      return d->reader->error;
    }
  }
#ifdef LABCOMM_DECODER_DEBUG
  fprintf(stderr, "\n");
#endif
  return tag;
}

#ifdef OLD_type_def_DECODING_TEST_CODE 
static int decode_type_binding(struct labcomm_decoder *d, int kind)
{
  int result;
  int sample_index = result = labcomm_read_packed32(d->reader);
  if(d->reader->error < 0 ) {
      result =  d->reader->error;
      goto out;
  }
  int type_def_index = labcomm_read_packed32(d->reader);
  if(d->reader->error < 0 ) {
      result =  d->reader->error;
      goto out;
  }
#ifdef LABCOMM_DECODER_DEBUG
  printf("type_binding: 0x%x -> 0x%x\n", sample_index, type_def_index);
#endif
out:
  return result;
} 

static int decode_type_def(struct labcomm_decoder *d, int kind){
  int i, remote_index, result;
  char *name;
  int size;
  remote_index = labcomm_read_packed32(d->reader);
  result = remote_index;
  if (d->reader->error < 0) {
    result = d->reader->error;
    goto out;
  }
  name = labcomm_read_string(d->reader);
  if (d->reader->error < 0) {
    result = d->reader->error;
    goto out;
  }
  size = labcomm_read_packed32(d->reader);
  if (d->reader->error < 0) {
    result = d->reader->error;
    goto free_signature_name;
  }
  //printf("got type_def 0x%x : %s, skipping %d signature bytes", remote_index, name, size); 
#if 0
  signature.signature = labcomm_memory_alloc(d->memory, 1,  signature.size);
  if (d->reader->error < 0) {
    result = d->reader->error;
    goto free_signature_name;
  }
#endif
  for (i = 0 ; i < size ; i++) {
    /*signature.signature[i] =*/ labcomm_read_byte(d->reader);
    if (d->reader->error < 0) {
      result = d->reader->error;
      goto free_signature_signature;
    }
  }
free_signature_signature:
//  labcomm_memory_free(d->memory, 1,  signature.signature);
free_signature_name:
  labcomm_memory_free(d->memory, 0, name);
out:
  return result;
}
#endif

static int decode_sample_def_or_ref(struct labcomm_decoder *d, int kind)
{
  int result;
  struct labcomm_signature signature;
  int i, remote_index;

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
  switch (kind) {
    case LABCOMM_SAMPLE_DEF: {
      result = handle_sample_def(d, remote_index, &signature);
      break;
    } 
    case LABCOMM_SAMPLE_REF: {
      result = handle_sample_ref(d, remote_index, &signature);
      if (result == -ENOENT) {
        /* Dummy value to silently continue */
        result = LABCOMM_SAMPLE_REF;
      }
      break;
    } 
    default:
      result = -EINVAL;
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
  const struct labcomm_signature *signature;
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

/* d        - decoder to read from
   registry - decoder to lookup signatures (registry != d only if
                nesting decoders, e.g., when decoding pragma)
   len      - length of the labcomm packet )
*/
static int decode_pragma(struct labcomm_decoder *d,
		         struct labcomm_decoder *registry,
		         int len)
{
  char *pragma_type;
  int result;
  pragma_type = labcomm_read_string(d->reader);
  if (d->reader->error < 0) {
    result = d->reader->error;
    goto out;
  }
  int bytes = labcomm_size_string(pragma_type);
  int psize = len-bytes;
  result = decoder_skip(d, psize, LABCOMM_PRAGMA);
out:
  return result;
}

static labcomm_decoder_function lookup_h(struct labcomm_decoder *d,
		                         struct call_handler_context *wrap,
		                         int remote_index,
					 int **local_index)
{
  labcomm_decoder_function do_decode = NULL;
  labcomm_scheduler_data_lock(d->scheduler);
  *local_index = LABCOMM_SIGNATURE_ARRAY_REF(d->memory,
      				      d->remote_to_local, int,
      				      remote_index);
  if (**local_index != 0) {
    struct sample_entry *entry;

    entry = LABCOMM_SIGNATURE_ARRAY_REF(d->memory,
      				  d->local, struct sample_entry,
      				  **local_index);
    wrap->local_index = **local_index;
    wrap->signature = entry->signature;
    wrap->handler = entry->handler;
    wrap->context = entry->context;
    do_decode = entry->decode;
  }
  labcomm_scheduler_data_unlock(d->scheduler);
  return do_decode;
}
/* d            - decoder to read from
   registry     - decoder to lookup signatures (registry != d only if
                    nesting decoders, e.g., when decoding pragma)
   remote_index -  received type index )
*/
static int decode_and_handle(struct labcomm_decoder *d,
		             struct labcomm_decoder *registry,
		             int remote_index)
{
  int result;
  int *local_index;
  struct call_handler_context wrap = {
    .reader = d->reader,
    .remote_index = remote_index,
    .signature = NULL,
    .handler = NULL,
    .context = NULL,
  };
  labcomm_decoder_function do_decode = lookup_h(registry, &wrap, remote_index, &local_index);
  result = *local_index;
  if (do_decode) {
    do_decode(d->reader, call_handler, &wrap);
    if (d->reader->error < 0) {
      result = d->reader->error;
    }
  } else {
    result = -ENOENT;
  }
  return result;
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
#ifdef LABCOMM_DECODER_DEBUG
    fprintf(stderr, "No VERSION %d %d\n", remote_index, length);
#endif
    result = -ECONNRESET;
  } else if (remote_index == LABCOMM_SAMPLE_DEF) {
    result = decode_sample_def_or_ref(d, LABCOMM_SAMPLE_DEF); 
  } else if (remote_index == LABCOMM_SAMPLE_REF) {
    result = decode_sample_def_or_ref(d, LABCOMM_SAMPLE_REF); 
  } else if (remote_index == LABCOMM_TYPE_DEF) {
    result = decode_and_handle(d, d, remote_index);
    if(result == -ENOENT) { 
        //No handler for type_defs, skip
#ifdef OLD_type_def_DECODING_TEST_CODE 
        result = decode_type_def(d, LABCOMM_TYPE_DEF); 
#else
        result = decoder_skip(d, length, remote_index);
#endif
    }
  } else if (remote_index == LABCOMM_TYPE_BINDING) {
    result = decode_and_handle(d, d, remote_index);
    if(result == -ENOENT) { 
        //No handler for type_bindings, skip
#ifdef OLD_type_def_DECODING_TEST_CODE 
      result = decode_type_binding(d, LABCOMM_TYPE_BINDING); 
#else
        result = decoder_skip(d, length, remote_index);
#endif
    }
  } else if (remote_index == LABCOMM_PRAGMA) {
    result = decode_pragma(d, d, length);
  } else if (remote_index < LABCOMM_USER) {
#ifdef LABCOMM_DECODER_DEBUG
    fprintf(stderr, "SKIP %d %d\n", remote_index, length);
#endif
    result = decoder_skip(d, length, remote_index);
  } else {
    result = decode_and_handle(d, d, remote_index);
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

int labcomm_decoder_sample_ref_register(
  struct labcomm_decoder *d,
  const struct labcomm_signature *signature)
{
  int local_index, *remote_to_local_ref;
  const struct labcomm_signature **s;

  local_index = labcomm_get_local_index(signature);
  if (local_index <= 0) { goto out; }
  labcomm_scheduler_data_lock(d->scheduler);
  s = LABCOMM_SIGNATURE_ARRAY_REF(d->memory, 
                                  d->local_ref, 
                                  const struct labcomm_signature*, local_index);
  if (s == NULL) { local_index = -ENOMEM; goto unlock; };
  if (*s) { goto unlock; }
  *s = signature;	
  remote_to_local_ref = LABCOMM_SIGNATURE_ARRAY_REF(d->memory, 
                                                    d->remote_to_local_ref, 
                                                    int, local_index);
  *remote_to_local_ref = 0;
unlock:
  labcomm_scheduler_data_unlock(d->scheduler);
out:
  return local_index;
}

int labcomm_internal_decoder_ioctl(struct labcomm_decoder *d, 
				   const struct labcomm_signature *signature,
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

#ifndef LABCOMM_NO_TYPEDECL
//// Code for allowing user code to handle type_defs 
//// (should perhaps be moved to another file)

static void decode_raw_type_def(
  struct labcomm_reader *r,
  void (*handle)(
    struct labcomm_raw_type_def *v,
    void *context
  ),
  void *context
)
{
  struct labcomm_raw_type_def v;
  v.index = labcomm_read_packed32(r);
  if (r->error < 0) { goto out; }
  v.name  = labcomm_read_string(r);
  if (r->error < 0) { goto free_name; }
  v.length = labcomm_read_packed32(r);
  if (r->error < 0) { goto free_name; }
  int i;
  v.signature_data = labcomm_memory_alloc(r->memory, 1, v.length);
  if(v.signature_data) {
    for(i=0; i<v.length; i++) {
      v.signature_data[i] = labcomm_read_byte(r);
      if (r->error < 0) { goto free_sig; }
    }  
    handle(&v, context);
    }
free_sig:
  labcomm_memory_free(r->memory, 1, v.signature_data);
free_name:
  labcomm_memory_free(r->memory, 1, v.name);
out:
  return;
}
int labcomm_decoder_register_labcomm_type_def(
  struct labcomm_decoder *d,
  void (*handler)(
    struct labcomm_raw_type_def *v,
    void *context
  ),
  void *context
)
{
  int tag = LABCOMM_TYPE_DEF;
  struct sample_entry *entry;
  int *remote_to_local;
 
  labcomm_scheduler_data_lock(d->scheduler);
  entry = LABCOMM_SIGNATURE_ARRAY_REF(d->memory,
				      d->local, struct sample_entry,
				      tag);
  if (entry == NULL) { tag = -ENOMEM; goto unlock; }
  entry->remote_index = tag;
  entry->signature = NULL;
  entry->decode = (labcomm_decoder_function) decode_raw_type_def;
  entry->handler =(labcomm_handler_function) handler;
  entry->context = context;

  remote_to_local = LABCOMM_SIGNATURE_ARRAY_REF(d->memory,
                                                    d->remote_to_local, int,
                                                    tag);
  *remote_to_local = tag;
unlock:
  labcomm_scheduler_data_unlock(d->scheduler);

  return tag;
}


static void decode_type_binding(
  struct labcomm_reader *r,
  void (*handle)(
    struct labcomm_type_binding *v,
    void *context
  ),
  void *context
)
{
  struct labcomm_type_binding v;
  v.sample_index = labcomm_read_packed32(r);
  if (r->error < 0) { goto out; }
  v.type_index = labcomm_read_packed32(r);
  if (r->error < 0) { goto out; }
  handle(&v, context);
out:
  return;
}

int labcomm_decoder_register_labcomm_type_binding(
  struct labcomm_decoder *d,
  void (*handler)(
    struct labcomm_type_binding *v,
    void *context
  ),
  void *context
)
{
  int tag = LABCOMM_TYPE_BINDING;
  struct sample_entry *entry;
  int *remote_to_local;
 
  labcomm_scheduler_data_lock(d->scheduler);
  entry = LABCOMM_SIGNATURE_ARRAY_REF(d->memory,
				      d->local, struct sample_entry,
				      tag);
  if (entry == NULL) { tag = -ENOMEM; goto unlock; }
  entry->remote_index = tag;
  entry->signature = NULL;
  entry->decode = (labcomm_decoder_function) decode_type_binding;
  entry->handler =(labcomm_handler_function) handler;
  entry->context = context;

  remote_to_local = LABCOMM_SIGNATURE_ARRAY_REF(d->memory,
                                                    d->remote_to_local, int,
                                                    tag);
  *remote_to_local = tag;
unlock:
  labcomm_scheduler_data_unlock(d->scheduler);

  return tag;
}

//// End type_def handling
#endif

int labcomm_internal_decoder_register(
  struct labcomm_decoder *d,
  const struct labcomm_signature *signature,
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

const struct labcomm_signature *labcomm_internal_decoder_index_to_signature(
  struct labcomm_decoder *d, int index)
{
  const struct labcomm_signature *result = 0;
  int local_index;

  labcomm_scheduler_data_lock(d->scheduler);
  local_index = LABCOMM_SIGNATURE_ARRAY_GET(d->remote_to_local_ref, 
                                            int, index, 0);
  if (local_index) {
    result = LABCOMM_SIGNATURE_ARRAY_GET(d->local_ref, 
                                         const struct labcomm_signature*, 
                                         local_index, 0);
  }
  labcomm_scheduler_data_unlock(d->scheduler);
  return result;
}
