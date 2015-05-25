/*
  labcomm2014_decoder.c -- runtime for handling decoding of labcomm2014 samples.

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
#include "labcomm2014.h"
#include "labcomm2014_private.h"
#include "labcomm2014_ioctl.h"
#include "labcomm2014_dynamic_buffer_writer.h"

#ifdef DEBUG
#define DEBUG_FPRINTF(str, ...) fprintf(str, ##__VA_ARGS__)
#else
#define DEBUG_FPRINTF(str, ...)
#endif

#ifdef DECODER_DEBUG
#define DECODER_DEBUG_FPRINTF(str, ...) fprintf(str, ##__VA_ARGS__) 
#else
#define DECODER_DEBUG_FPRINTF(str, ...)
#endif

struct sample_entry {
  int remote_index;
  const struct labcomm2014_signature *signature;
  labcomm2014_decoder_function decode;
  labcomm2014_handler_function handler;
  void *context;
};

struct decoder {
  struct labcomm2014_decoder decoder;
  LABCOMM_SIGNATURE_ARRAY_DEF(local, struct sample_entry);
  LABCOMM_SIGNATURE_ARRAY_DEF(remote_to_local, int);
  LABCOMM_SIGNATURE_ARRAY_DEF(local_ref, const struct labcomm2014_signature *);
  LABCOMM_SIGNATURE_ARRAY_DEF(remote_to_local_ref, int);
};

static int handle_sample_def(struct labcomm2014_decoder *d, int remote_index,
                             const struct labcomm2014_signature *signature)
{
  struct decoder *id = d->context;
  int result;
  int i;
  const struct labcomm2014_signature *local_signature = NULL;
  int local_index = 0;

  labcomm2014_scheduler_data_lock(d->scheduler);
  LABCOMM_SIGNATURE_ARRAY_FOREACH(id->local, struct sample_entry, i) {
    struct sample_entry *s;
    int *remote_to_local;

    result = -ENOENT;
    s = LABCOMM_SIGNATURE_ARRAY_REF(d->memory, 
                                    id->local, struct sample_entry, i);
    if (s->signature &&
        s->signature->size == signature->size &&
        strcmp(s->signature->name, signature->name) == 0 &&
        memcmp((void*)s->signature->signature, (void*)signature->signature,
	       signature->size) == 0) {
      s->remote_index = remote_index;
      local_signature = s->signature;
      local_index = i;
      remote_to_local = LABCOMM_SIGNATURE_ARRAY_REF(d->memory,
                                                    id->remote_to_local, int,
                                                    remote_index);
      *remote_to_local = i;
      result = remote_index;
      break;
    }
  }
  labcomm2014_scheduler_data_unlock(d->scheduler);
  if (local_signature) {
    labcomm2014_reader_start(d->reader, d->reader->action_context,
                         local_index, remote_index, local_signature,
                         NULL);
    labcomm2014_reader_end(d->reader, d->reader->action_context);
  }
  return result;
}

static int handle_sample_ref(struct labcomm2014_decoder *d, int remote_index,
                             const struct labcomm2014_signature *signature)
{
  struct decoder *id = d->context;
  int result;
  int i;

  labcomm2014_scheduler_data_lock(d->scheduler);
  LABCOMM_SIGNATURE_ARRAY_FOREACH(id->local_ref,
                                  const struct labcomm2014_signature *, i) {
    const struct labcomm2014_signature *s;
    int *remote_to_local_ref;

    result = -ENOENT;
    s = LABCOMM_SIGNATURE_ARRAY_GET(id->local_ref, const struct labcomm2014_signature *, i, 0);
    if (s &&
        s->signature &&
        s->size == signature->size &&
        strcmp(s->name, signature->name) == 0 &&
        memcmp((void*)s->signature, (void*)signature->signature, signature->size) == 0) {
      remote_to_local_ref = LABCOMM_SIGNATURE_ARRAY_REF(d->memory,
                                                        id->remote_to_local_ref,
                                                        int,
                                                        remote_index);
      *remote_to_local_ref = i;
      result = remote_index;
      break;
    }
  }
  labcomm2014_scheduler_data_unlock(d->scheduler);
  return result;
}

static int reader_skip(struct labcomm2014_reader *r, int len, int tag)
{
  int i;
  DECODER_DEBUG_FPRINTF(stdout, "got tag 0x%x, skipping %d bytes\n", tag, len);
  for(i = 0; i <len; i++){
    DECODER_DEBUG_FPRINTF(stderr, ".");
    labcomm2014_read_byte(r);
    if (r->error < 0) {
      DECODER_DEBUG_FPRINTF(stderr, "\nerror while skipping: %d\n",  r->error);
      return r->error;
    }
  }
  DECODER_DEBUG_FPRINTF(stderr, "\n");
  return tag;
}

static char* TODO_read_intentions(struct labcomm2014_reader *r)
{

    int numInts = labcomm2014_read_byte(r);
    int i;
    char *name=NULL;

    printf("TODO_read_intentions: numInts=%d\n", numInts);
    for(i=0; i <numInts; i++){
        int klen = labcomm2014_read_packed32(r);
        printf("TODO_read_intentions: klen=%d\n", klen);
        if(klen == 0) {
            name = labcomm2014_read_string(r);
            printf("TODO_read_intentions: name=%s\n", name);
        }else{
            int vlen;
            reader_skip(r, klen, 1);
            vlen = labcomm2014_read_packed32(r);
            reader_skip(r, vlen, 1);
            printf("TODO_read_intentions: skipping value, %d bytes\n", vlen);
        }
    }
    return name;
}

static int decode_sample_def_or_ref(struct labcomm2014_decoder *d, int kind)
{
  int result;
  struct labcomm2014_signature signature;
  int i, remote_index;

  remote_index = labcomm2014_read_packed32(d->reader);
  if (d->reader->error < 0) {
    result = d->reader->error;
    goto out;
  }

  signature.name = TODO_read_intentions(d->reader);
  if (d->reader->error < 0) {
    result = d->reader->error;
    goto out;
  }
  signature.size = labcomm2014_read_packed32(d->reader);
  if (d->reader->error < 0) {
    result = d->reader->error;
    goto free_signature_name;
  }
  signature.signature = labcomm2014_memory_alloc(d->memory, 1,  signature.size);
  if (d->reader->error < 0) {
    result = d->reader->error;
    goto free_signature_name;
  }
  for (i = 0 ; i < signature.size ; i++) {
    signature.signature[i] = labcomm2014_read_byte(d->reader);
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
  labcomm2014_memory_free(d->memory, 1,  signature.signature);
free_signature_name:
  labcomm2014_memory_free(d->memory, 0, signature.name);
out:
  return result;
}

struct call_handler_context {
  struct labcomm2014_reader *reader;
  int local_index;
  int remote_index;
  const struct labcomm2014_signature *signature;
  labcomm2014_handler_function handler;
  void *context;
};

static void call_handler(void *value, void *context)
{
  struct call_handler_context *wrap = context;

  if (wrap->reader->error >= 0) {
    labcomm2014_reader_start(wrap->reader, wrap->reader->action_context,
			 wrap->local_index, wrap->remote_index, wrap->signature,
			 value);
    wrap->handler(value, wrap->context);
    labcomm2014_reader_end(wrap->reader, wrap->reader->action_context);
  }
}

static void reader_alloc(struct labcomm2014_decoder *d)
{
  if (!d->reader_allocated) {
    d->reader_allocated = 1;
    labcomm2014_reader_alloc(d->reader, d->reader->action_context);
  }
}

/* d        - decoder to read from
   registry - decoder to lookup signatures (registry != d only if
                nesting decoders, e.g., when decoding pragma)
   len      - length of the labcomm2014 packet )
*/
static int decode_pragma(struct labcomm2014_decoder *d,
		         struct labcomm2014_decoder *registry,
		         int len)
{
  char *pragma_type;
  int result;
  pragma_type = labcomm2014_read_string(d->reader);
  if (d->reader->error < 0) {
    result = d->reader->error;
    goto out;
  }
  int bytes = labcomm2014_size_string(pragma_type);
  int psize = len-bytes;
  result = reader_skip(d->reader, psize, LABCOMM_PRAGMA);
out:
  return result;
}

static labcomm2014_decoder_function lookup_h(struct labcomm2014_decoder *d,
		                         struct call_handler_context *wrap,
		                         int remote_index,
					 int **local_index)
{
  struct decoder *id = d->context;
  labcomm2014_decoder_function do_decode = NULL;
  labcomm2014_scheduler_data_lock(d->scheduler);
  *local_index = LABCOMM_SIGNATURE_ARRAY_REF(d->memory,
                                             id->remote_to_local, int,
                                             remote_index);
  if (**local_index != 0) {
    struct sample_entry *entry;

    entry = LABCOMM_SIGNATURE_ARRAY_REF(d->memory,
                                        id->local, struct sample_entry,
                                        **local_index);
    wrap->local_index = **local_index;
    wrap->signature = entry->signature;
    wrap->handler = entry->handler;
    wrap->context = entry->context;
    do_decode = entry->decode;
  }
  labcomm2014_scheduler_data_unlock(d->scheduler);
  return do_decode;
}
/* d            - decoder to read from
   registry     - decoder to lookup signatures (registry != d only if
                    nesting decoders, e.g., when decoding pragma)
   remote_index -  received type index )
*/
static int decode_and_handle(struct labcomm2014_decoder *d,
		             struct labcomm2014_decoder *registry,
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
  labcomm2014_decoder_function do_decode = lookup_h(registry, &wrap, remote_index, &local_index);
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

int labcomm2014_decoder_decode_one(struct labcomm2014_decoder *d)
{
  return d->decode_one(d);
}

static int do_decode_one(struct labcomm2014_decoder *d)
{
  int result, remote_index, length;

  reader_alloc(d);
  remote_index = labcomm2014_read_packed32(d->reader);
  if (d->reader->error < 0) {
    result = d->reader->error;
    goto out;
  }
  length = labcomm2014_read_packed32(d->reader);
  if (d->reader->error < 0) {
    result = d->reader->error;
    goto out;
  }
  if (remote_index == LABCOMM_VERSION) {
    char *version = labcomm2014_read_string(d->reader);
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
    labcomm2014_memory_free(d->memory, 1,  version);
  } else if (! d->version_ok) {
    DEBUG_FPRINTF(stderr, "No VERSION %d %d\n", remote_index, length);
    result = -ECONNRESET;
  } else if (remote_index == LABCOMM_SAMPLE_DEF) {
    result = decode_sample_def_or_ref(d, LABCOMM_SAMPLE_DEF); 
  } else if (remote_index == LABCOMM_SAMPLE_REF) {
    result = decode_sample_def_or_ref(d, LABCOMM_SAMPLE_REF); 
  } else if (remote_index == LABCOMM_TYPE_DEF) {
    result = decode_and_handle(d, d, remote_index);
    if(result == -ENOENT) { 
        //No handler for type_defs, skip
        result = reader_skip(d->reader, length, remote_index);
    }
  } else if (remote_index == LABCOMM_TYPE_BINDING) {
    result = decode_and_handle(d, d, remote_index);
    if(result == -ENOENT) { 
        //No handler for type_bindings, skip
        result = reader_skip(d->reader, length, remote_index);
    }
  } else if (remote_index == LABCOMM_PRAGMA) {
    result = decode_pragma(d, d, length);
  } else if (remote_index < LABCOMM_USER) {
    DECODER_DEBUG_FPRINTF(stderr, "SKIP %d %d\n", remote_index, length);
    result = reader_skip(d->reader, length, remote_index);
  } else {
    result = decode_and_handle(d, d, remote_index);
  }
out:   
  return result;
}

void labcomm2014_decoder_run(struct labcomm2014_decoder *d)
{
  while (labcomm2014_decoder_decode_one(d) > 0) {
  }
}

int labcomm2014_decoder_ioctl(struct labcomm2014_decoder *d, 
                              uint32_t action,
                              ...)
{
  int result;  
  va_list va;
    
  va_start(va, action);
  result = d->ioctl(d, NULL, action, va);
  va_end(va);
  return result;
}

int labcomm2014_decoder_sample_ref_register(
  struct labcomm2014_decoder *d,
  const struct labcomm2014_signature *signature)
{
  return d->ref_register(d, signature);
}

static int do_ref_register(
  struct labcomm2014_decoder *d,
  const struct labcomm2014_signature *signature)
{
  struct decoder *id = d->context;
  int local_index, *remote_to_local_ref;
  const struct labcomm2014_signature **s;

  local_index = labcomm2014_get_local_index(signature);
  if (local_index <= 0) { goto out; }
  labcomm2014_scheduler_data_lock(d->scheduler);
  s = LABCOMM_SIGNATURE_ARRAY_REF(d->memory, 
                                  id->local_ref, 
                                  const struct labcomm2014_signature*, local_index);
  if (s == NULL) { local_index = -ENOMEM; goto unlock; };
  if (*s) { goto unlock; }
  *s = signature;	
  remote_to_local_ref = LABCOMM_SIGNATURE_ARRAY_REF(d->memory, 
                                                    id->remote_to_local_ref, 
                                                    int, local_index);
  *remote_to_local_ref = 0;
unlock:
  labcomm2014_scheduler_data_unlock(d->scheduler);
out:
  return local_index;
}

static int do_ioctl(struct labcomm2014_decoder *d, 
                    const struct labcomm2014_signature *signature,
                    uint32_t action, va_list va)
{
  struct decoder *id = d->context;
  int result;
  int local_index, remote_index;

  local_index = labcomm2014_get_local_index(signature);
  if (local_index == 0) {
    remote_index = 0;
  } else {
    labcomm2014_scheduler_data_lock(d->scheduler);
    remote_index = LABCOMM_SIGNATURE_ARRAY_REF(d->memory,
                                               id->local,
                                               struct sample_entry,
                                               local_index)->remote_index;
    labcomm2014_scheduler_data_unlock(d->scheduler);
  }
  result = labcomm2014_reader_ioctl(d->reader, d->reader->action_context,
                                    local_index, remote_index, 
                                    signature, action, va);
  return result;
}

#ifndef LABCOMM_NO_TYPEDECL
//// Code for allowing user code to handle type_defs 
//// (should perhaps be moved to another file)

static void decode_raw_type_def(
  struct labcomm2014_reader *r,
  void (*handle)(
    struct labcomm2014_raw_type_def *v,
    void *context
  ),
  void *context
)
{
  struct labcomm2014_raw_type_def v;
  v.index = labcomm2014_read_packed32(r);
  if (r->error < 0) { goto out; }
  v.name  = TODO_read_intentions(r);
  if (r->error < 0) { goto free_name; }
  v.length = labcomm2014_read_packed32(r);
  if (r->error < 0) { goto free_name; }
  int i;
  v.signature_data = labcomm2014_memory_alloc(r->memory, 1, v.length);
  if(v.signature_data) {
    for(i=0; i<v.length; i++) {
      v.signature_data[i] = labcomm2014_read_byte(r);
      if (r->error < 0) { goto free_sig; }
    }  
    handle(&v, context);
    }
free_sig:
  labcomm2014_memory_free(r->memory, 1, v.signature_data);
free_name:
  labcomm2014_memory_free(r->memory, 1, v.name);
out:
  return;
}
int labcomm2014_decoder_register_labcomm2014_type_def(
  struct labcomm2014_decoder *d,
  void (*handler)(
    struct labcomm2014_raw_type_def *v,
    void *context
  ),
  void *context
)
{
  struct decoder *id = d->context;
  int tag = LABCOMM_TYPE_DEF;
  struct sample_entry *entry;
  int *remote_to_local;
 
  labcomm2014_scheduler_data_lock(d->scheduler);
  entry = LABCOMM_SIGNATURE_ARRAY_REF(d->memory,
				      id->local, struct sample_entry,
				      tag);
  if (entry == NULL) { tag = -ENOMEM; goto unlock; }
  entry->remote_index = tag;
  entry->signature = NULL;
  entry->decode = (labcomm2014_decoder_function) decode_raw_type_def;
  entry->handler =(labcomm2014_handler_function) handler;
  entry->context = context;

  remote_to_local = LABCOMM_SIGNATURE_ARRAY_REF(d->memory,
                                                id->remote_to_local, int,
                                                tag);
  *remote_to_local = tag;
unlock:
  labcomm2014_scheduler_data_unlock(d->scheduler);

  return tag;
}


static void decode_type_binding(
  struct labcomm2014_reader *r,
  void (*handle)(
    struct labcomm2014_type_binding *v,
    void *context
  ),
  void *context
)
{
  struct labcomm2014_type_binding v;
  v.sample_index = labcomm2014_read_packed32(r);
  if (r->error < 0) { goto out; }
  v.type_index = labcomm2014_read_packed32(r);
  if (r->error < 0) { goto out; }
  handle(&v, context);
out:
  return;
}

int labcomm2014_decoder_register_labcomm2014_type_binding(
  struct labcomm2014_decoder *d,
  void (*handler)(
    struct labcomm2014_type_binding *v,
    void *context
  ),
  void *context
)
{
  struct decoder *id = d->context;
  int tag = LABCOMM_TYPE_BINDING;
  struct sample_entry *entry;
  int *remote_to_local;
 
  labcomm2014_scheduler_data_lock(d->scheduler);
  entry = LABCOMM_SIGNATURE_ARRAY_REF(d->memory,
				      id->local, struct sample_entry,
				      tag);
  if (entry == NULL) { tag = -ENOMEM; goto unlock; }
  entry->remote_index = tag;
  entry->signature = NULL;
  entry->decode = (labcomm2014_decoder_function) decode_type_binding;
  entry->handler =(labcomm2014_handler_function) handler;
  entry->context = context;

  remote_to_local = LABCOMM_SIGNATURE_ARRAY_REF(d->memory,
                                                id->remote_to_local, int,
                                                tag);
  *remote_to_local = tag;
unlock:
  labcomm2014_scheduler_data_unlock(d->scheduler);

  return tag;
}

//// End type_def handling
#endif

static int do_register_sample(
  struct labcomm2014_decoder *d,
  const struct labcomm2014_signature *signature,
  labcomm2014_decoder_function decode, 
  labcomm2014_handler_function handler,
  void *context)
{
  struct decoder *id = d->context;
  int local_index;
  struct sample_entry *entry;
 
  reader_alloc(d);
  local_index = labcomm2014_get_local_index(signature);
  if (local_index <= 0) { goto out; }
  labcomm2014_reader_start(d->reader, d->reader->action_context,
		       local_index, 0, signature,
		       NULL);
  labcomm2014_reader_end(d->reader, d->reader->action_context);

  labcomm2014_scheduler_data_lock(d->scheduler);
  entry = LABCOMM_SIGNATURE_ARRAY_REF(d->memory,
				      id->local, struct sample_entry,
				      local_index);
  if (entry == NULL) { local_index = -ENOMEM; goto unlock; }
  entry->remote_index = 0;
  entry->signature = signature;
  entry->decode = decode;
  entry->handler = handler;
  entry->context = context;
unlock:
  labcomm2014_scheduler_data_unlock(d->scheduler);
out:
  return local_index;
}

static const struct labcomm2014_sample_ref *do_index_to_sample_ref(
  struct labcomm2014_decoder *d, int index)
{
  const struct labcomm2014_signature *result = 0;
  struct decoder *id = d->context;
  int local_index;

  labcomm2014_scheduler_data_lock(d->scheduler);
  local_index = LABCOMM_SIGNATURE_ARRAY_GET(id->remote_to_local_ref, 
                                            int, index, 0);
  if (local_index) {
    result = LABCOMM_SIGNATURE_ARRAY_GET(id->local_ref, 
                                         const struct labcomm2014_signature*, 
                                         local_index, 0);
  }
  labcomm2014_scheduler_data_unlock(d->scheduler);
  return labcomm2014_signature_to_sample_ref(result);
}

static const struct labcomm2014_sample_ref *do_ref_get(
  struct labcomm2014_decoder *d,
  const struct labcomm2014_signature *signature)
{
  return (const struct labcomm2014_sample_ref *) signature;
}

static void do_free(struct labcomm2014_decoder* d)
{
  struct decoder *id = d->context;
  struct labcomm2014_memory *memory = d->memory;

  labcomm2014_reader_free(d->reader, d->reader->action_context);
  LABCOMM_SIGNATURE_ARRAY_FREE(memory, id->local, struct sample_entry);
  LABCOMM_SIGNATURE_ARRAY_FREE(memory, id->remote_to_local, int);
  LABCOMM_SIGNATURE_ARRAY_FREE(memory, id->local_ref, 
                               const struct labcomm2014_signature*);
  LABCOMM_SIGNATURE_ARRAY_FREE(memory, id->remote_to_local_ref, int);
  labcomm2014_memory_free(memory, 0, id);
}

void labcomm2014_decoder_free(struct labcomm2014_decoder* d)
{
  d->free(d);
}

struct labcomm2014_decoder *labcomm2014_decoder_new(
  struct labcomm2014_reader *reader,
  struct labcomm2014_error_handler *error,
  struct labcomm2014_memory *memory,
  struct labcomm2014_scheduler *scheduler)
{
  struct decoder *result;

  result = labcomm2014_memory_alloc(memory, 0, sizeof(*result));
  if (result) {
    result->decoder.context = result;
    result->decoder.reader = reader;
    result->decoder.reader->decoder = &result->decoder;
    result->decoder.reader->data = 0;
    result->decoder.reader->data_size = 0;
    result->decoder.reader->count = 0;
    result->decoder.reader->pos = 0;
    result->decoder.reader->error = 0;
    result->decoder.reader_allocated = 0;
    result->decoder.version_ok = 0;
    result->decoder.error = error;
    result->decoder.memory = memory;
    result->decoder.scheduler = scheduler;
    result->decoder.on_error = labcomm20142014_on_error_fprintf;
    result->decoder.free = do_free;
    result->decoder.decode_one = do_decode_one;
    result->decoder.ref_register = do_ref_register;
    result->decoder.sample_register = do_register_sample;
    result->decoder.ioctl = do_ioctl;
    result->decoder.index_to_sample_ref = do_index_to_sample_ref;
    result->decoder.ref_get = do_ref_get;
    LABCOMM_SIGNATURE_ARRAY_INIT(result->local, struct sample_entry);
    LABCOMM_SIGNATURE_ARRAY_INIT(result->remote_to_local, int);
    LABCOMM_SIGNATURE_ARRAY_INIT(result->local_ref, 
                                 const struct labcomm2014_signature*);
    LABCOMM_SIGNATURE_ARRAY_INIT(result->remote_to_local_ref, int);
  }
  return &(result->decoder);
}


const struct labcomm2014_sample_ref *labcomm2014_decoder_get_sample_ref(
  struct labcomm2014_decoder *decoder,
  const struct labcomm2014_signature *signature)
{
  return decoder->ref_get(decoder, signature);
}

