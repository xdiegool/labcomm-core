/*
  labcomm.c -- runtime for handling encoding and decoding of
               labcomm samples.

  Copyright 2006-2013 Anders Blomdell <anders.blomdell@control.lth.se>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
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

#include "labcomm.h"
#include "labcomm_private.h"
#include "labcomm_ioctl.h"
#include "labcomm_dynamic_buffer_writer.h"

#define LABCOMM_VERSION "LabComm2013"

struct labcomm_decoder {
  void *context;
  struct labcomm_reader *reader;
  struct {
    void *context;
    const struct labcomm_lock_action *action;
  } lock;
  labcomm_error_handler_callback on_error;
  labcomm_handle_new_datatype_callback on_new_datatype;
};

struct labcomm_encoder {
  void *context;
  struct labcomm_writer *writer;
  struct {
    void *context;
    const struct labcomm_lock_action *action;
  } lock;
  labcomm_error_handler_callback on_error;
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

#ifndef LABCOMM_ENCODER_LINEAR_SEARCH
extern  struct labcomm_signature labcomm_first_signature;
extern  struct labcomm_signature labcomm_last_signature;
#endif

struct labcomm_encoder_context {
#ifdef LABCOMM_ENCODER_LINEAR_SEARCH
  struct labcomm_sample_entry *sample;
  int index;
#else
  struct labcomm_sample_entry *by_section;
#endif
};

struct labcomm_decoder_context {
  struct labcomm_sample_entry *sample;
};

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

#ifdef LABCOMM_ENCODER_LINEAR_SEARCH

static int get_encoder_index_by_search(
  struct labcomm_encoder *e,
  struct labcomm_signature *s)
{
  int result = 0;
  struct labcomm_encoder_context *context = e->context;
  struct labcomm_sample_entry *sample = context->sample;
  while (sample) {
    if (sample->signature == s) { break; }
    sample = sample->next;
  }
  if (sample) {
    result = sample->index;
  }
  return result;
}

#else

static int get_encoder_index_by_section(
  struct labcomm_encoder *e,
  struct labcomm_signature *s)
{
  int result = -ENOENT;
  if (&labcomm_first_signature <= s && s <= &labcomm_last_signature) {
    //fprintf(stderr, "%d\n", (int)(s - &labcomm_start));
    result = s - &labcomm_first_signature + LABCOMM_USER;
  }
  return result;
}

#endif
static int get_encoder_index(
  struct labcomm_encoder *e,
  struct labcomm_signature *s)
{
#ifdef LABCOMM_ENCODER_LINEAR_SEARCH
  return get_encoder_index_by_search(e, s);
#else
  return get_encoder_index_by_section(e, s);
#endif
}

static void labcomm_encode_signature(struct labcomm_encoder *e,
				     struct labcomm_signature *signature) 
{
  int i, index;

  index = get_encoder_index(e, signature);
  e->writer->action->start(e->writer, e, index, signature, NULL);
  labcomm_write_packed32(e->writer, signature->type);
  labcomm_write_packed32(e->writer, index);

  labcomm_write_string(e->writer, signature->name);
  for (i = 0 ; i < signature->size ; i++) {
    if (e->writer->pos >= e->writer->count) {
      e->writer->action->flush(e->writer);
    }
    e->writer->data[e->writer->pos] = signature->signature[i];
    e->writer->pos++;
  }
  e->writer->action->end(e->writer);
}

#ifdef LABCOMM_ENCODER_LINEAR_SEARCH
static int encoder_add_signature_by_search(struct labcomm_encoder *e,
					   struct labcomm_signature *signature,
					   labcomm_encoder_function encode)
{
  int result;
  struct labcomm_encoder_context *context = e->context;
  struct labcomm_sample_entry *sample;

  sample = (struct labcomm_sample_entry *)malloc(sizeof(*sample));
  if (sample == NULL) {
    result = -ENOMEM;
  } else {
    sample->next = context->sample;
    sample->index = context->index;
    sample->signature = signature;
    sample->encode = encode;
    context->index++;
    context->sample = sample;
    result = sample->index;
  }
  return result;
}
#endif

#ifndef LABCOMM_ENCODER_LINEAR_SEARCH
static int encoder_add_signature_by_section(struct labcomm_encoder *e,
					    struct labcomm_signature *s,
					    labcomm_encoder_function encode)
{
  int result = -ENOENT;
  
  if (&labcomm_first_signature <= s && s <= &labcomm_last_signature) {
    /* Signature is in right linker section */
    struct labcomm_encoder_context *context = e->context;
    int index = s - &labcomm_first_signature;

    if (context->by_section == NULL) {
      int n = &labcomm_last_signature - &labcomm_first_signature;
      context->by_section = malloc(n * sizeof(context->by_section[0]));
    }
    if (context->by_section == NULL) {
      result = -ENOMEM;
      goto out;
    }
    context->by_section[index].next = NULL;
    context->by_section[index].index = index + LABCOMM_USER;
    context->by_section[index].signature = s;
    context->by_section[index].encode = encode;
    result = context->by_section[index].index;
  }
out:
  return result;
}
#endif

static int encoder_add_signature(struct labcomm_encoder *e,
				  struct labcomm_signature *signature,
				  labcomm_encoder_function encode)
{
  int index = -ENOENT;

#ifdef LABCOMM_ENCODER_LINEAR_SEARCH
  index = encoder_add_signature_by_search(e, signature, encode);
#else
  index = encoder_add_signature_by_section(e, signature, encode);
#endif
  return index;
}

/*
static struct labcomm_sample_entry *encoder_get_sample_by_signature_address(
  struct labcomm_encoder *encoder,
  struct labcomm_signature *s)
{
  struct labcomm_sample_entry *result = NULL;
  struct labcomm_encoder_context *context = encoder->context;
  
#ifndef LABCOMM_ENCODER_LINEAR_SEARCH
  if (&labcomm_first_signature <= s && s <= &labcomm_last_signature) {
    result = &context->by_section[s - &labcomm_first_signature];
  }
#else
  result = get_sample_by_signature_address(context->sample, s);
#endif
  return result;
}
*/
						    
struct labcomm_encoder *labcomm_encoder_new(
  struct labcomm_writer *writer,
  const struct labcomm_lock_action *lock,
  void *lock_context)
{
  struct labcomm_encoder *result = malloc(sizeof(*result));
  if (result) {
    struct labcomm_encoder_context *context;

    context = malloc(sizeof(*context));
#ifdef LABCOMM_ENCODER_LINEAR_SEARCH
    context->sample = NULL;
    context->index = LABCOMM_USER;
#else
    context->by_section = NULL;
#endif
    result->context = context;
    result->writer = writer;
    result->writer->data = 0;
    result->writer->data_size = 0;
    result->writer->count = 0;
    result->writer->pos = 0;
    result->writer->error = 0;
    result->lock.action = lock;
    result->lock.context = lock_context;
    result->on_error = on_error_fprintf;
    result->writer->action->alloc(result->writer, LABCOMM_VERSION);
  }
  return result;
}

void labcomm_internal_encoder_register(
  struct labcomm_encoder *e,
  struct labcomm_signature *signature,
  labcomm_encoder_function encode)
{
  if (signature->type == LABCOMM_SAMPLE) {
    if (get_encoder_index(e, signature) == 0) {
      int index = encoder_add_signature(e, signature, encode);
      
      if (index > 0) {
	struct labcomm_ioctl_register_signature ioctl_data;
	int err;
	
	ioctl_data.index = index;
	ioctl_data.signature = signature;	
	err = labcomm_encoder_ioctl(e, LABCOMM_IOCTL_REGISTER_SIGNATURE,
				    &ioctl_data);
	if (err != 0) {
	  labcomm_encode_signature(e, signature);
	}
      }
    }
  }
}

int labcomm_internal_encode(
  struct labcomm_encoder *e,
  struct labcomm_signature *signature,
  labcomm_encoder_function encode,
  void *value)
{
  int result;
  int index;

  index = get_encoder_index(e, signature);
  result = e->writer->action->start(e->writer, e, index, signature, value);
  if (result == -EALREADY) { result = 0; goto no_end; }
  if (result != 0) { goto out; }
  result = labcomm_write_packed32(e->writer, index);
  if (result != 0) { goto out; }
  result = encode(e->writer, value);
out:
  e->writer->action->end(e->writer);
no_end:
  return result;
}

void labcomm_encoder_free(struct labcomm_encoder* e)
{
  e->writer->action->free(e->writer);
  struct labcomm_encoder_context *context = (struct labcomm_encoder_context *) e->context;

#ifdef LABCOMM_ENCODER_LINEAR_SEARCH
  struct labcomm_sample_entry *entry = context->sample;
  struct labcomm_sample_entry *entry_next;
  while (entry != NULL) {
    entry_next = entry->next;
    free(entry);
    entry = entry_next;
  }
#else
  free(context->by_section);
#endif
  free(e->context);
  free(e);
}

int labcomm_encoder_ioctl(struct labcomm_encoder *encoder, 
                           int action,
                           ...)
{
  int result = -ENOTSUP;
  
  if (encoder->writer->action->ioctl != NULL) {
    va_list va;
    
    va_start(va, action);
    result = encoder->writer->action->ioctl(encoder->writer, action, NULL, va);
    va_end(va);
  }
  return result;
}

static int labcomm_writer_ioctl(struct labcomm_writer *writer,
				int action,
				...)
{
  int result = -ENOTSUP;
  
  if (writer->action->ioctl != NULL) {
    va_list va;
    
    va_start(va, action);
    result = writer->action->ioctl(writer, action, NULL, va);
    va_end(va);
  }
  return result;
}

int labcomm_internal_encoder_ioctl(struct labcomm_encoder *encoder, 
				   int action,
				   struct labcomm_signature *signature,
                                   va_list va)
{
  int result = -ENOTSUP;
  
  if (encoder->writer->action->ioctl != NULL) {
    result = encoder->writer->action->ioctl(encoder->writer, action, 
					    signature, va);
  }
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
	  free(name);
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
  const struct labcomm_lock_action *lock,
  void *lock_context)
{
  struct labcomm_decoder *result = malloc(sizeof(*result));
  if (result) {
    struct labcomm_decoder_context *context =
      (struct labcomm_decoder_context *)malloc(sizeof(*context));
    context->sample = 0;
    result->context = context;
    result->reader = reader;
    result->reader->data = 0;
    result->reader->data_size = 0;
    result->reader->count = 0;
    result->reader->pos = 0;
    result->lock.action = lock;
    result->lock.context = lock_context;
    result->on_error = on_error_fprintf;
    result->on_new_datatype = on_new_datatype;
    result->reader->action->alloc(result->reader, LABCOMM_VERSION);
  }
  return result;
}

void labcomm_internal_decoder_register(
  struct labcomm_decoder *d,
  struct labcomm_signature *signature,
  labcomm_decoder_function type_decoder,
  labcomm_handler_function handler,
  void *handler_context)
{
  struct labcomm_decoder_context *context = d->context;
  struct labcomm_sample_entry *sample;
  sample = get_sample_by_signature_address(context->sample,
					   signature);
  if (!sample) {
    sample = (struct labcomm_sample_entry *)malloc(sizeof(*sample));
    sample->next = context->sample;
    context->sample = sample;
    sample->index = 0;
    sample->signature = signature;
  }
  sample->decoder = type_decoder;
  sample->handler = handler;
  sample->context = handler_context;
}

int labcomm_decoder_decode_one(struct labcomm_decoder *d)
{
  int result;

  result = d->reader->action->start(d->reader);
  if (result > 0) {
    struct labcomm_decoder_context *context = d->context;
    
    result = labcomm_read_packed32(d->reader);
    if (result == LABCOMM_TYPEDEF || result == LABCOMM_SAMPLE) {
      /* TODO: should the labcomm_dynamic_buffer_writer be 
	 a permanent part of labcomm_decoder? */
      struct labcomm_writer writer = {
	.context = NULL,
	.data = NULL,
	.data_size = 0,
	.count = 0,
	.pos = 0,
	.error = 0,
	.action = labcomm_dynamic_buffer_writer_action,
      };
      struct labcomm_signature signature;
      struct labcomm_sample_entry *entry = NULL;
      int index, err;
      
      writer.action->alloc(&writer, "");
      writer.action->start(&writer, NULL, 0, NULL, NULL);
      index = labcomm_read_packed32(d->reader); //int
      signature.name = labcomm_read_string(d->reader);
      signature.type = result;
      collect_flat_signature(d, &writer);
      writer.action->end(&writer);
      err = labcomm_writer_ioctl(&writer, 
				 LABCOMM_IOCTL_WRITER_GET_BYTES_WRITTEN,
				 &signature.size);
      if (err < 0) {
	printf("Failed to get size: %s\n", strerror(-err));
	goto free_signature_name;
      }
      err = labcomm_writer_ioctl(&writer, 
				 LABCOMM_IOCTL_WRITER_GET_BYTE_POINTER,
				 &signature.signature);
      if (err < 0) {
	printf("Failed to get pointer: %s\n", strerror(-err));
	goto free_signature_name;
      }
      entry = get_sample_by_signature_value(context->sample, &signature);
      if (! entry) {
	/* Unknown datatype, bail out */
	d->on_new_datatype(d, &signature);
      } else if (entry->index && entry->index != index) {
	d->on_error(LABCOMM_ERROR_DEC_INDEX_MISMATCH, 5, 
		    "%s(): index mismatch '%s' (id=0x%x != 0x%x)\n", 
		    __FUNCTION__, signature.name, entry->index, index);
      } else {
	// TODO unnessesary, since entry->index == index in above if statement
	entry->index = index;
      }
    free_signature_name:
      free(signature.name);
      writer.action->free(&writer);
      if (!entry) {
	// No handler for found type, bail out (after cleanup)
	result = -ENOENT;
      }
    } else {
      struct labcomm_sample_entry *entry;
      
      entry = get_sample_by_index(context->sample, result);
      if (!entry) {
	// printf("Error: %s: type not found (id=0x%x)\n",
	//__FUNCTION__, result);
	d->on_error(LABCOMM_ERROR_DEC_TYPE_NOT_FOUND, 3, "%s(): type not found (id=0x%x)\n", __FUNCTION__, result);
	result = -ENOENT;
      } else {
	entry->decoder(d->reader, entry->handler, entry->context);
      }
    }
  }
  d->reader->action->end(d->reader);
  return result;
}

void labcomm_decoder_run(struct labcomm_decoder *d)
{
  while (labcomm_decoder_decode_one(d) > 0) {
  }
}

void labcomm_decoder_free(struct labcomm_decoder* d)
{
  d->reader->action->free(d->reader);
  struct labcomm_decoder_context *context = (struct labcomm_decoder_context *) d->context;
  struct labcomm_sample_entry *entry = context->sample;
  struct labcomm_sample_entry *entry_next;

  while (entry != NULL) {
    entry_next = entry->next;
    free(entry);
    entry = entry_next;
  }

  free(d->context);
  free(d);
}

int labcomm_decoder_ioctl(struct labcomm_decoder *decoder, 
			  int action,
			  ...)
{
  int result = -ENOTSUP;
  
  if (decoder->reader->action->ioctl != NULL) {
    va_list va;
    
    va_start(va, action);
    result = decoder->reader->action->ioctl(decoder->reader, action, NULL, va);
    va_end(va);
  }
  return result;
}

int labcomm_internal_decoder_ioctl(struct labcomm_decoder *decoder, 
				   int action,
				   struct labcomm_signature *signature,
                                   va_list va)
{
  int result = -ENOTSUP;
  
  if (decoder->reader->action->ioctl != NULL) {
    result = decoder->reader->action->ioctl(decoder->reader, action, NULL, va);
  }
  return result;
}

