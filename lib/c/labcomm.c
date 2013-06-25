/*
  labcomm.c -- runtime for handling encoding and decoding of
               labcomm samples.

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

#ifdef LABCOMM_COMPAT
  #include LABCOMM_COMPAT
#else
  #include <stdio.h>
  #include <strings.h>
#endif

#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>

#include "labcomm.h"
#include "labcomm_private.h"
#include "labcomm_ioctl.h"
#include "labcomm_dynamic_buffer_writer.h"

#define LABCOMM_VERSION "LabComm2013"

struct labcomm_decoder {
  void *context;
  struct labcomm_reader *reader;
  struct labcomm_lock *lock;
  struct labcomm_memory *memory;
  labcomm_error_handler_callback on_error;
  labcomm_handle_new_datatype_callback on_new_datatype;
  LABCOMM_SIGNATURE_ARRAY_DEF(local_to_remote, int);
  LABCOMM_SIGNATURE_ARRAY_DEF(remote_to_local, int);
};

struct labcomm_encoder {
  void *context;
  struct labcomm_writer *writer;
  struct labcomm_lock *lock;
  struct labcomm_memory *memory;
  struct labcomm_encoder *is_deferred;
  int busy;
  int waiting;
  struct encoder_alloc_action {
    struct encoder_alloc_action *next;
    void (*action)(struct labcomm_encoder *encoder, void *context);
    void *context;
  } *alloc_action;
  labcomm_error_handler_callback on_error;
  LABCOMM_SIGNATURE_ARRAY_DEF(registered, int);
};

struct labcomm_sample_entry {
  struct labcomm_sample_entry *next;
  int index;
  struct labcomm_signature *signature;
  labcomm_decoder_function decoder;
  labcomm_handler_function handler;
  labcomm_encoder_function encode;
  void *context;
};

extern  struct labcomm_signature labcomm_first_signature;
extern  struct labcomm_signature labcomm_last_signature;

struct labcomm_encoder_context {
  struct labcomm_sample_entry *by_section;
};

struct labcomm_decoder_context {
  struct labcomm_sample_entry *sample;
};

/* Lock wrappers */
#define CONDCALL_lock(lock, ...) lock
#define CONDCALL(func, ...)					\
  if (CONDCALL_lock(__VA_ARGS__) &&				\
      CONDCALL_lock(__VA_ARGS__)->action->func) {		\
  return CONDCALL_lock(__VA_ARGS__)->action->func(__VA_ARGS__);	\
  }								\
  return -ENOSYS;

int labcomm_lock_free(struct labcomm_lock *lock) {
  CONDCALL(free, lock);
}

int labcomm_lock_acquire(struct labcomm_lock *lock)
{
  CONDCALL(acquire, lock);
}

int labcomm_lock_release(struct labcomm_lock *lock)
{
  CONDCALL(release, lock);
}

int labcomm_lock_wait(struct labcomm_lock *lock, useconds_t usec){
  CONDCALL(wait, lock, usec);
}

int labcomm_lock_notify(struct labcomm_lock *lock)
{
  CONDCALL(notify, lock);
}

#undef CONDCALL
#undef CONDCALL_lock

/* Unwrapping reader/writer functions */
#define UNWRAP_ac(rw, ac, ...) ac
#define UNWRAP(func, ...)	     \
  while (1) {								\
    if (UNWRAP_ac(__VA_ARGS__)->action->func) {				\
      return UNWRAP_ac(__VA_ARGS__)->action->func(__VA_ARGS__); }	\
    if (UNWRAP_ac(__VA_ARGS__)->next == NULL) { return -ENOSYS; }	\
    UNWRAP_ac( __VA_ARGS__) = UNWRAP_ac(__VA_ARGS__)->next;		\
  }

int labcomm_reader_alloc(struct labcomm_reader *r, 
                         struct labcomm_reader_action_context *action_context, 
                         struct labcomm_decoder *decoder, 
                         char *labcomm_version)
{
  UNWRAP(alloc, r, action_context, decoder, labcomm_version);
}

int labcomm_reader_free(struct labcomm_reader *r, 
                        struct labcomm_reader_action_context *action_context)
{
  UNWRAP(free, r, action_context);
}

int labcomm_reader_start(struct labcomm_reader *r, 
                         struct labcomm_reader_action_context *action_context,
			 int index, struct labcomm_signature *signature,
			 void *value)
{
  UNWRAP(start, r, action_context, index, signature, value);
}

int labcomm_reader_end(struct labcomm_reader *r, 
                       struct labcomm_reader_action_context *action_context)
{
  UNWRAP(end, r, action_context);
}

int labcomm_reader_fill(struct labcomm_reader *r, 
                        struct labcomm_reader_action_context *action_context)
{
  UNWRAP(fill, r, action_context);
}

int labcomm_reader_ioctl(struct labcomm_reader *r, 
                         struct labcomm_reader_action_context *action_context,
                         int index, 
                         struct labcomm_signature *signature, 
                         uint32_t ioctl_action, va_list args)
{
  UNWRAP(ioctl, r, action_context, index, signature, ioctl_action, args);
}

int labcomm_writer_alloc(struct labcomm_writer *w, 
                         struct labcomm_writer_action_context *action_context, 
                         struct labcomm_encoder *encoder, 
                         char *labcomm_version,
			 labcomm_encoder_enqueue enqueue)
{
  UNWRAP(alloc, w, action_context, encoder, labcomm_version, enqueue);
}

int labcomm_writer_free(struct labcomm_writer *w, 
                        struct labcomm_writer_action_context *action_context)
{
  UNWRAP(free, w, action_context);
}

int labcomm_writer_start(struct labcomm_writer *w, 
                         struct labcomm_writer_action_context *action_context,
                         int index, struct labcomm_signature *signature,
                         void *value)
{
  UNWRAP(start, w, action_context, index, signature, value);
}

int labcomm_writer_end(struct labcomm_writer *w, 
                       struct labcomm_writer_action_context *action_context)
{
  UNWRAP(end, w, action_context);
} 

int labcomm_writer_flush(struct labcomm_writer *w, 
                         struct labcomm_writer_action_context *action_context)
{
  UNWRAP(flush, w, action_context);
} 

int labcomm_writer_ioctl(struct labcomm_writer *w, 
                         struct labcomm_writer_action_context *action_context, 
                         int index, 
                         struct labcomm_signature *signature, 
                         uint32_t ioctl_action, va_list args)
{
  UNWRAP(ioctl, w, action_context, index, signature, ioctl_action, args);
} 

#undef UNWRAP
#undef UNWRAP_ac



void labcomm_register_error_handler_encoder(struct labcomm_encoder *encoder, labcomm_error_handler_callback callback)
{
 encoder->on_error = callback; 
}

void labcomm_register_error_handler_decoder(struct labcomm_decoder *decoder, labcomm_error_handler_callback callback)
{
 decoder->on_error = callback; 
}

static const char *labcomm_error_string[] = { 
#define LABCOMM_ERROR(name, description) description ,
#include "labcomm_error.h"
#undef LABCOMM_ERROR
};
static const int labcomm_error_string_count = (sizeof(labcomm_error_string) / 
					       sizeof(labcomm_error_string[0]));


const char *labcomm_error_get_str(enum labcomm_error error_id)
{
  const char *error_str = NULL;
  // Check if this is a known error ID.
  if (0 <= error_id && error_id < labcomm_error_string_count) {
    error_str = labcomm_error_string[error_id];
  }
  return error_str;
}

void labcomm_decoder_register_new_datatype_handler(struct labcomm_decoder *d, labcomm_handle_new_datatype_callback on_new_datatype)
{
	d->on_new_datatype = on_new_datatype;
}

int on_new_datatype(struct labcomm_decoder *d, struct labcomm_signature *sig)
{
  d->on_error(LABCOMM_ERROR_DEC_UNKNOWN_DATATYPE, 4, "%s(): unknown datatype '%s'\n", __FUNCTION__, sig->name);
	  return 0;
}

void on_error_fprintf(enum labcomm_error error_id, size_t nbr_va_args, ...)
{
#ifndef LABCOMM_NO_STDIO
  const char *err_msg = labcomm_error_get_str(error_id); // The final string to print.
  if (err_msg == NULL) {
    err_msg = "Error with an unknown error ID occured.";
  }
  fprintf(stderr, "%s\n", err_msg);

 if (nbr_va_args > 0) {
   va_list arg_pointer;
   va_start(arg_pointer, nbr_va_args);

   fprintf(stderr, "%s\n", "Extra info {");
   char *print_format = va_arg(arg_pointer, char *);
   vfprintf(stderr, print_format, arg_pointer);
   fprintf(stderr, "}\n");

   va_end(arg_pointer);
 } 
#else
 ; // If labcomm can't be compiled with stdio the user will have to make an own error callback functionif he/she needs error reporting.
#endif
}


static struct labcomm_sample_entry *get_sample_by_signature_address(
  struct labcomm_sample_entry *head,
  struct labcomm_signature *signature)
{
  struct labcomm_sample_entry *p;
  for (p = head ; p && p->signature != signature ; p = p->next) {

  }
  return p;
}

static struct labcomm_sample_entry *get_sample_by_signature_value(
  struct labcomm_sample_entry *head,
  struct labcomm_signature *signature)
{
  struct labcomm_sample_entry *p;
  for (p = head ; p ; p = p->next) {
    if (p->signature->type == signature->type &&
	p->signature->size == signature->size &&
	strcmp(p->signature->name, signature->name) == 0 &&
	bcmp((void*)p->signature->signature, (void*)signature->signature,
	     signature->size) == 0) {
      break;
    }
  }
  return p;
}

static struct labcomm_sample_entry *get_sample_by_index(
  struct labcomm_sample_entry *head,
  int index)
{
  struct labcomm_sample_entry *p;
  for (p = head ; p && p->index != index ; p = p->next) {
  }
  return p;
}

static int get_local_index(
  struct labcomm_signature *s)
{
  int result = -ENOENT;

  if (&labcomm_first_signature <= s && s < &labcomm_last_signature) {
    result = (int)(s - &labcomm_first_signature) + LABCOMM_USER;
  }
  return result;
}

static int get_encoder_index(
  struct labcomm_encoder *e,
  struct labcomm_signature *s)
{
  return get_local_index(s);
}

struct labcomm_encoder *labcomm_encoder_new(
  struct labcomm_writer *writer,
  struct labcomm_lock *lock,
  struct labcomm_memory *memory)
{
  struct labcomm_encoder *result;

  result = labcomm_memory_alloc(memory, 0, sizeof(*result));
  if (result) {
    struct labcomm_encoder_context *context;

    context = labcomm_memory_alloc(memory, 0, sizeof(*context));
#ifdef LABCOMM_ENCODER_LINEAR_SEARCH
    context->sample = NULL;
    context->index = LABCOMM_USER;
#else
    context->by_section = NULL;
#endif
    result->context = context;
    result->writer = writer;
    result->writer->encoder = result;
    result->writer->data = NULL;
    result->writer->data_size = 0;
    result->writer->count = 0;
    result->writer->pos = 0;
    result->writer->error = 0;
    result->lock = lock;
    result->memory = memory;
    result->is_deferred = NULL;
    result->busy = 0;
    result->waiting = 0;
    result->alloc_action = NULL;
    result->on_error = on_error_fprintf;
    LABCOMM_SIGNATURE_ARRAY_INIT(result->registered, int);
  }
  return result;
}

static int encoder_enqueue_action(
  struct labcomm_encoder *encoder, 
  void (*action)(struct labcomm_encoder *encoder, void *context),
  void *context)
{
  int result;
  struct encoder_alloc_action *element, **next;

  element = labcomm_memory_alloc(encoder->memory, 0, sizeof(*action));
  if (element == NULL) {
    result = -ENOMEM;
    goto out;
  }
  element->next = NULL;
  element->action = action;
  element->context = context;
  for (next = &encoder->alloc_action ; *next ; next = &(*next)->next) {
  }
  *next = element;
  result = 0;
out:
  return result;
}

static struct labcomm_encoder *enter_encoder(struct labcomm_encoder *e)
{
  if (e->is_deferred) {
    return e->is_deferred;
  } else {
    labcomm_lock_acquire(e->lock); 
    e->waiting++;
    while (e->busy) { labcomm_lock_wait(e->lock, 10000000); }
    e->busy = 1;
    labcomm_lock_release(e->lock);
    
    if (e->writer->data == NULL) {
      labcomm_writer_alloc(e->writer,e->writer->action_context,
			   e, LABCOMM_VERSION, encoder_enqueue_action);
      if (e->alloc_action) {
	struct labcomm_encoder deferred;
	struct encoder_alloc_action *p;

	deferred.is_deferred = e;
	p = e->alloc_action;
	e->alloc_action = NULL;
	while (p) {
	  struct encoder_alloc_action *tmp;

	  p->action(&deferred, p->context);
	  tmp = p;
	  p = p->next;
	  labcomm_memory_free(e->memory, 1, tmp);
	}
      }
    }
  }
  return e;
}
static void leave_encoder(struct labcomm_encoder *e)
{
  if (!e->is_deferred) {
    labcomm_lock_acquire(e->lock); {
      e->busy = 0;
      e->waiting--;
      if (e->waiting) {
	labcomm_lock_notify(e->lock);
      }
    } labcomm_lock_release(e->lock);
  }
}

int labcomm_internal_encoder_register(
  struct labcomm_encoder *encoder,
  struct labcomm_signature *signature,
  labcomm_encoder_function encode)
{
  int result = -EINVAL;
  struct labcomm_encoder *e;
  int index;

  e = enter_encoder(encoder);
  index = get_encoder_index(e, signature);
  if (signature->type == LABCOMM_SAMPLE) {
    if (index > 0) {
      int *registered = LABCOMM_SIGNATURE_ARRAY_REF(e->memory, e->registered, 
						    int, index);
      if (! *registered) {
	int err;
	
	*registered = 1;	
	err = labcomm_writer_start(e->writer, e->writer->action_context, 
				   index, signature, NULL);
	if (err == -EALREADY) {
	  result = 0;
	} else if (err != 0) {
	  result = err;
	} else if (err == 0) {
	  int i;

	  labcomm_write_packed32(e->writer, signature->type);
	  labcomm_write_packed32(e->writer, index);
	  labcomm_write_string(e->writer, signature->name);
	  for (i = 0 ; i < signature->size ; i++) {
	    if (e->writer->pos >= e->writer->count) {
	      labcomm_writer_flush(e->writer, e->writer->action_context);
	    }
	    e->writer->data[e->writer->pos] = signature->signature[i];
	    e->writer->pos++;
	  }
	  labcomm_writer_end(e->writer, e->writer->action_context);
	  result = e->writer->error;
	}
      }
    }
  }
  leave_encoder(encoder);
  return result;
}

int labcomm_internal_encode(
  struct labcomm_encoder *encoder,
  struct labcomm_signature *signature,
  labcomm_encoder_function encode,
  void *value)
{
  int result;
  struct labcomm_encoder *e;
  int index;

  e = enter_encoder(encoder);
  index = get_encoder_index(e, signature);
  result = labcomm_writer_start(e->writer, e->writer->action_context, 
				index, signature, value);
  if (result == -EALREADY) { result = 0; goto no_end; }
  if (result != 0) { goto out; }
  result = labcomm_write_packed32(e->writer, index);
  if (result != 0) { goto out; }
  result = encode(e->writer, value);
out:
  labcomm_writer_end(e->writer, e->writer->action_context);
no_end:
  leave_encoder(encoder);
  return result;
}

void labcomm_encoder_free(struct labcomm_encoder* e)
{
  struct labcomm_encoder_context *context;
  struct labcomm_memory * memory = e->memory;

  context = (struct labcomm_encoder_context *) e->context;
  labcomm_writer_free(e->writer, e->writer->action_context);
  LABCOMM_SIGNATURE_ARRAY_FREE(e->memory, e->registered, int);

#ifdef LABCOMM_ENCODER_LINEAR_SEARCH
  struct labcomm_sample_entry *entry = context->sample;
  struct labcomm_sample_entry *entry_next;
  while (entry != NULL) {
    entry_next = entry->next;
    labcomm_memory_free(memory, 0, entry);
    entry = entry_next;
  }
#else
  labcomm_memory_free(memory, 0, context->by_section);
#endif
  labcomm_memory_free(memory, 0, e->context);
  labcomm_memory_free(memory, 0, e);
}

int labcomm_encoder_ioctl(struct labcomm_encoder *encoder, 
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
  result = labcomm_writer_ioctl(encoder->writer, 
			       encoder->writer->action_context,
			       0, NULL, action, va);
  va_end(va);

out:
  return result;
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

int labcomm_internal_encoder_ioctl(struct labcomm_encoder *encoder, 
				   struct labcomm_signature *signature,
				   uint32_t action, va_list va)
{
  int result = -ENOTSUP;
  
  result = labcomm_writer_ioctl(encoder->writer, 
				encoder->writer->action_context, 
				-1, signature, action, va);
  return result;
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

struct labcomm_decoder *labcomm_decoder_new(
  struct labcomm_reader *reader,
  struct labcomm_lock *lock,
  struct labcomm_memory *memory)
{
  struct labcomm_decoder *result;

  result = labcomm_memory_alloc(memory, 0, sizeof(*result));
  if (result) {
    struct labcomm_decoder_context *context;

    context = labcomm_memory_alloc(memory, 0, sizeof(*context));
    context->sample = 0;
    result->context = context;
    result->reader = reader;
    result->reader->decoder = result;
    result->reader->data = 0;
    result->reader->data_size = 0;
    result->reader->count = 0;
    result->reader->pos = 0;
    result->reader->error = 0;
    result->lock = lock;
    result->memory = memory;
    result->on_error = on_error_fprintf;
    result->on_new_datatype = on_new_datatype;
    LABCOMM_SIGNATURE_ARRAY_INIT(result->local_to_remote, int);
    LABCOMM_SIGNATURE_ARRAY_INIT(result->remote_to_local, int);
  }
  return result;
}

int labcomm_internal_decoder_register(
  struct labcomm_decoder *d,
  struct labcomm_signature *signature,
  labcomm_decoder_function type_decoder,
  labcomm_handler_function handler,
  void *handler_context)
{
  struct labcomm_decoder_context *context = d->context;
  int index;
  
  index = get_local_index(signature);
  if (index >= 0) {
    struct labcomm_sample_entry *sample;

    labcomm_reader_start(d->reader, d->reader->action_context,
			 index, signature,
			 NULL);
    labcomm_reader_end(d->reader, d->reader->action_context);
    sample = get_sample_by_signature_address(context->sample,
					     signature);
    if (!sample) {
      sample = labcomm_memory_alloc(d->memory, 0, sizeof(*sample));
      sample->next = context->sample;
      context->sample = sample;
      sample->index = 0;
      sample->signature = signature;
    }
    sample->decoder = type_decoder;
    sample->handler = handler;
    sample->context = handler_context;
  }
  return 0;
}

static int decode_typedef_or_sample(struct labcomm_decoder *d, int kind)
{
  int result;
  struct labcomm_decoder_context *context = d->context;

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
  struct labcomm_signature signature;
  struct labcomm_sample_entry *entry = NULL;
  int remote_index, err;
      
  labcomm_writer_alloc(&writer, writer.action_context, NULL, "", NULL);
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
    printf("Failed to get size: %s\n", strerror(-err));
    result = -ENOENT;
    goto free_signature_name;
  }
  err = writer_ioctl(&writer, 
		     LABCOMM_IOCTL_WRITER_GET_BYTE_POINTER,
		     &signature.signature);
  if (err < 0) {
    printf("Failed to get pointer: %s\n", strerror(-err));
    result = -ENOENT;
    goto free_signature_name;
  }
  entry = get_sample_by_signature_value(context->sample, &signature);
  if (! entry) {
    /* Unknown datatype, bail out */
    d->on_new_datatype(d, &signature);
    result = -ENOENT;
  } else if (entry->index && entry->index != remote_index) {
    d->on_error(LABCOMM_ERROR_DEC_INDEX_MISMATCH, 5, 
		"%s(): index mismatch '%s' (id=0x%x != 0x%x)\n", 
		__FUNCTION__, signature.name, entry->index, remote_index);
    result = -ENOENT;
  } else {
    int local_index;
    int *local_to_remote, *remote_to_local;
    // TODO unnessesary, since entry->index == index in above if statement
    entry->index = remote_index;
    local_index = get_local_index(entry->signature);
    local_to_remote = LABCOMM_SIGNATURE_ARRAY_REF(d->memory,
						  d->local_to_remote, int,
						  local_index);
    *local_to_remote = remote_index;
    remote_to_local = LABCOMM_SIGNATURE_ARRAY_REF(d->memory,
						  d->remote_to_local, int,
						  remote_index);
    *remote_to_local = local_index;
    result = remote_index;
  }
free_signature_name:
  labcomm_memory_free(d->memory, 0, signature.name);
  labcomm_writer_free(&writer, writer.action_context);
  if (!entry) {
    // No handler for found type, bail out (after cleanup)
    result = -ENOENT;
  }
  return result;
}

struct call_handler_context {
  struct labcomm_sample_entry *entry;
  struct labcomm_reader *reader;
};

static void call_handler(void *value, void *context)
{
  struct call_handler_context *wrap = context;

  labcomm_reader_start(wrap->reader, wrap->reader->action_context,
		       wrap->entry->index, wrap->entry->signature,
		       value);
  wrap->entry->handler(value, wrap->entry->context);
  labcomm_reader_end(wrap->reader, wrap->reader->action_context);
}

int labcomm_decoder_decode_one(struct labcomm_decoder *d)
{
  int result, index;
  struct labcomm_decoder_context *context = d->context;
  
  if (d->reader->data == NULL) {
    result = labcomm_reader_alloc(d->reader, d->reader->action_context,
				  d, LABCOMM_VERSION);
    if (result <= 0) {
      goto out;
    }
  }
  index = labcomm_read_packed32(d->reader);
  if (d->reader->error < 0) {
    result = d->reader->error;
    goto out;
  }
  if (index == LABCOMM_TYPEDEF || index == LABCOMM_SAMPLE) {
    result = decode_typedef_or_sample(d, index); 
  } else {
    struct labcomm_sample_entry *entry;
    
    entry = get_sample_by_index(context->sample, index);
    if (!entry) {
      // printf("Error: %s: type not found (id=0x%x)\n",
      //__FUNCTION__, result);
      d->on_error(LABCOMM_ERROR_DEC_TYPE_NOT_FOUND, 3, 
		  "%s(): type not found (id=0x%x)\n", 
		  __FUNCTION__, index);
      result = -ENOENT;
    } else {
      struct call_handler_context wrap = {
	.entry = entry,
	.reader = d->reader
      };
      entry->decoder(d->reader, call_handler, &wrap);
      result = index;
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

void labcomm_decoder_free(struct labcomm_decoder* d)
{
  struct labcomm_decoder_context *context = (struct labcomm_decoder_context *) d->context;
  struct labcomm_sample_entry *entry = context->sample;
  struct labcomm_sample_entry *entry_next;
  struct labcomm_memory *memory = d->memory;

  labcomm_reader_free(d->reader, d->reader->action_context);
  LABCOMM_SIGNATURE_ARRAY_FREE(memory, d->local_to_remote, int);
  LABCOMM_SIGNATURE_ARRAY_FREE(memory, d->remote_to_local, int);
  while (entry != NULL) {
    entry_next = entry->next;
    labcomm_memory_free(memory, 0, entry);
    entry = entry_next;
  }

  labcomm_memory_free(memory, 0, d->context);
  labcomm_memory_free(memory, 0, d);
}

int labcomm_decoder_ioctl(struct labcomm_decoder *decoder, 
			  uint32_t action,
			  ...)
{
  int result;  
  va_list va;
    
  va_start(va, action);
  result = labcomm_reader_ioctl(decoder->reader, 
				decoder->reader->action_context,
				0, NULL, action, va);
  va_end(va);
  return result;
}

int labcomm_internal_decoder_ioctl(struct labcomm_decoder *decoder, 
				   struct labcomm_signature *signature,
				   uint32_t action, va_list va)
{
  int result;
  int local_index, *remote_index;

  local_index = get_local_index(signature);
  remote_index = LABCOMM_SIGNATURE_ARRAY_REF(decoder->memory,
					     decoder->local_to_remote, int,
					     local_index);
  if (*remote_index == 0) {
    result = -EAGAIN;
  } else {
    result = labcomm_reader_ioctl(decoder->reader, 
				  decoder->reader->action_context,
				  *remote_index, signature, 
				  action, va);
  }
  return result;
}

#if 0
static void dump(void *p, int size, int first, int last)
{
  int i, j;

  printf("%d %d (%p): ", first, last, p);
  for (i = first ; i < last ; i++) {
    for (j = 0 ; j < size ; j++) {
      printf("%2.2d", ((char*)p)[(i-first)*size + j]);
    }
    printf(" ");
  }
  printf("\n");
}
#endif

void *labcomm_signature_array_ref(struct labcomm_memory *memory,
				  int *first, int *last, void **data,
				  int size, int index)
{
  if (*first == 0 && *last == 0) {
    *first = index;
    *last = index + 1;
    *data = labcomm_memory_alloc(memory, 0, size);
    if (*data) { 
      memset(*data, 0, size); 
    }
  } else if (index < *first || *last <= index) {
    void *old_data = *data;
    int old_first = *first;
    int old_last = *last;
    int n;
    *first = (index<old_first)?index:old_first;
    *last = (old_last<=index)?index+1:old_last;
    n = (*last - *first);
    *data = labcomm_memory_alloc(memory, 0, n * size);
    if (*data) {
      memset(*data, 0, n * size);
      memcpy(*data + (old_first - *first) * size, 
	     old_data, 
	     (old_last - old_first) * size);
    }
//    dump(old_data, size, old_first, old_last);
    labcomm_memory_free(memory, 0, old_data);
  }
  if (*data) {
//    dump(*data, size, *first, *last);
    return *data + (index - *first) * size;
  } else {
    return NULL;
  }
}
