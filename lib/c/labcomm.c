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

typedef struct labcomm_sample_entry {
  struct labcomm_sample_entry *next;
  int index;
  labcomm_signature_t *signature;
  labcomm_decoder_typecast_t decoder;
  labcomm_handler_typecast_t handler;
  labcomm_encode_typecast_t encode;
  void *context;
} labcomm_sample_entry_t;

#ifndef LABCOMM_ENCODER_LINEAR_SEARCH
extern  labcomm_signature_t labcomm_first_signature;
extern  labcomm_signature_t labcomm_last_signature;
#endif

typedef struct labcomm_encoder_context {
#ifdef LABCOMM_ENCODER_LINEAR_SEARCH
  labcomm_sample_entry_t *sample;
  int index;
#else
  labcomm_sample_entry_t *by_section;
#endif
} labcomm_encoder_context_t;

typedef struct labcomm_decoder_context {
  labcomm_sample_entry_t *sample;
} labcomm_decoder_context_t;

void labcomm_register_error_handler_encoder(struct labcomm_encoder *encoder, labcomm_error_handler_callback callback)
{
 encoder->on_error = callback; 
 encoder->writer.on_error = callback; 
}

void labcomm_register_error_handler_decoder(struct labcomm_decoder *decoder, labcomm_error_handler_callback callback)
{
 decoder->on_error = callback; 
 decoder->reader.on_error = callback; 
}

/* Error strings. _must_ be the same order as in enum labcomm_error */
const char *labcomm_error_strings[] = { 
  "Enum begin guard. DO NO use this as an error.",
  "Encoder has no registration for this signature.",
  "Encoder is missing do_register",
  "Encoder is missing do_encode",
  "The labcomm buffer is full and it.",
  "Decoder is missing do_register",
  "Decoder is missing do_decode_one",
  "Decoder: Unknown datatype",
  "Decoder: index mismatch",
  "Decoder: type not found",
  "This function is not yet implemented.",
  "User defined error.",
  "Could not allocate memory.",
  "Enum end guard. DO NO use this as an error."
};

const char *labcomm_error_get_str(enum labcomm_error error_id)
{
  const char *error_str = NULL;
  // Check if this is a known error ID.
  if (error_id >= LABCOMM_ERROR_ENUM_BEGIN_GUARD && error_id <= LABCOMM_ERROR_ENUM_END_GUARD) {
    error_str = labcomm_error_strings[error_id];
  }
  return error_str;
}

void labcomm_decoder_register_new_datatype_handler(struct labcomm_decoder *d, labcomm_handle_new_datatype_callback on_new_datatype)
{
	d->on_new_datatype = on_new_datatype;
}

int on_new_datatype(labcomm_decoder_t *d, labcomm_signature_t *sig)
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


static labcomm_sample_entry_t *get_sample_by_signature_address(
  labcomm_sample_entry_t *head,
  labcomm_signature_t *signature)
{
  labcomm_sample_entry_t *p;
  for (p = head ; p && p->signature != signature ; p = p->next) {

  }
  return p;
}

static labcomm_sample_entry_t *get_sample_by_signature_value(
  labcomm_sample_entry_t *head,
  labcomm_signature_t *signature)
{
  labcomm_sample_entry_t *p;
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

static labcomm_sample_entry_t *get_sample_by_index(
  labcomm_sample_entry_t *head,
  int index)
{
  labcomm_sample_entry_t *p;
  for (p = head ; p && p->index != index ; p = p->next) {
  }
  return p;
}

#ifdef LABCOMM_ENCODER_LINEAR_SEARCH

static int get_encoder_index_by_search(
  labcomm_encoder_t *e,
  labcomm_signature_t *s)
{
  int result = 0;
  labcomm_encoder_context_t *context = e->context;
  labcomm_sample_entry_t *sample = context->sample;
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
  labcomm_encoder_t *e,
  labcomm_signature_t *s)
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
  labcomm_encoder_t *e,
  labcomm_signature_t *s)
{
#ifdef LABCOMM_ENCODER_LINEAR_SEARCH
  return get_encoder_index_by_search(e, s);
#else
  return get_encoder_index_by_section(e, s);
#endif
}

void labcomm_encoder_start(struct labcomm_encoder *e,
                           labcomm_signature_t *s) 
{
  int index = get_encoder_index(e, s);
  e->writer.write(&e->writer, labcomm_writer_start, index);
}

void labcomm_encoder_end(struct labcomm_encoder *e, 
                         labcomm_signature_t *s) 
{
  e->writer.write(&e->writer, labcomm_writer_end);
}

void labcomm_encode_type_index(labcomm_encoder_t *e, labcomm_signature_t *s)
{
  int index = get_encoder_index(e, s);
  labcomm_encode_packed32(e, index);
}

void labcomm_encode_signature(struct labcomm_encoder *e,
                              labcomm_signature_t *signature) 
{
  int i;
  e->writer.write(&e->writer, labcomm_writer_start_signature);
  labcomm_encode_packed32(e, signature->type);
  labcomm_encode_type_index(e, signature);
  labcomm_encode_string(e, signature->name);
  for (i = 0 ; i < signature->size ; i++) {
    if (e->writer.pos >= e->writer.count) {
      e->writer.write(&e->writer, labcomm_writer_continue);
    }
    e->writer.data[e->writer.pos] = signature->signature[i];
    e->writer.pos++;
  }
  e->writer.write(&e->writer, labcomm_writer_end_signature);
}

#ifdef LABCOMM_ENCODER_LINEAR_SEARCH
static int encoder_add_signature_by_search(struct labcomm_encoder *e,
					   labcomm_signature_t *signature,
					   labcomm_encode_typecast_t encode)
{
  int result;
  labcomm_encoder_context_t *context = e->context;
  labcomm_sample_entry_t *sample;

  sample = (labcomm_sample_entry_t*)malloc(sizeof(labcomm_sample_entry_t));
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
					    labcomm_signature_t *s,
					    labcomm_encode_typecast_t encode)
{
  int result = -ENOENT;
  
  if (&labcomm_first_signature <= s && s <= &labcomm_last_signature) {
    /* Signature is in right linker section */
    labcomm_encoder_context_t *context = e->context;
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
				  labcomm_signature_t *signature,
				  labcomm_encode_typecast_t encode)
{
  int index = -ENOENT;

#ifdef LABCOMM_ENCODER_LINEAR_SEARCH
  index = encoder_add_signature_by_search(e, signature, encode);
#else
  index = encoder_add_signature_by_section(e, signature, encode);
#endif
  return index;
}

static void do_encoder_register(struct labcomm_encoder *e,
				labcomm_signature_t *signature,
				labcomm_encode_typecast_t encode)
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

static labcomm_sample_entry_t *encoder_get_sample_by_signature_address(
  labcomm_encoder_t *encoder,
  labcomm_signature_t *s)
{
  labcomm_sample_entry_t *result = NULL;
  labcomm_encoder_context_t *context = encoder->context;
  
#ifndef LABCOMM_ENCODER_LINEAR_SEARCH
  if (&labcomm_first_signature <= s && s <= &labcomm_last_signature) {
    result = &context->by_section[s - &labcomm_first_signature];
  }
#else
  result = get_sample_by_signature_address(context->sample, s);
#endif
  return result;
}
						    
static void do_encode(
  labcomm_encoder_t *encoder,
  labcomm_signature_t *signature,
  labcomm_encode_typecast_t encode,
  void *value)
{
  labcomm_sample_entry_t *sample;
  sample = encoder_get_sample_by_signature_address(encoder, signature);
  (void)sample;

  labcomm_encoder_start(encoder, signature);
  labcomm_encode_type_index(encoder, signature);
  encode(encoder, value);
  labcomm_encoder_end(encoder, signature);
/*
  labcomm_sample_entry_t *sample;

  
  sample = encoder_get_sample_by_signature_address(encoder, signature);
  if (sample && sample->encode) {
    sample->encode(encoder, value);
  } else {
    encoder->on_error(LABCOMM_ERROR_ENC_NO_REG_SIGNATURE, 2, "No registration for %s.\n", signature->name);
  }
*/
}

labcomm_encoder_t *labcomm_encoder_new(
  int (*writer)(labcomm_writer_t *, labcomm_writer_action_t, ...),
  void *writer_context)
{
  labcomm_encoder_t *result = malloc(sizeof(labcomm_encoder_t));
  if (result) {
    labcomm_encoder_context_t *context;

    context = malloc(sizeof(labcomm_encoder_context_t));
#ifdef LABCOMM_ENCODER_LINEAR_SEARCH
    context->sample = NULL;
    context->index = LABCOMM_USER;
#else
    context->by_section = NULL;
#endif
    result->context = context;
    result->writer.context = writer_context;
    result->writer.data = 0;
    result->writer.data_size = 0;
    result->writer.count = 0;
    result->writer.pos = 0;
    result->writer.error = 0;
    result->writer.write = writer;
    result->writer.ioctl = NULL;
    result->writer.on_error = on_error_fprintf;
    result->do_register = do_encoder_register;
    result->do_encode = do_encode;
    result->on_error = on_error_fprintf;
    result->writer.write(&result->writer, labcomm_writer_alloc);
  }
  return result;
}

void labcomm_internal_encoder_register(
  labcomm_encoder_t *e,
  labcomm_signature_t *signature,
  labcomm_encode_typecast_t encode)
{
  // Will segfault if e == NULL.
  if (e->do_register) {
    e->do_register(e, signature, encode);
  } else {
    e->on_error(LABCOMM_ERROR_ENC_MISSING_DO_REG, 0);
  }
}

void labcomm_internal_encode(
  labcomm_encoder_t *e,
  labcomm_signature_t *signature,
  labcomm_encode_typecast_t encode,
  void *value)
{
  if (e->do_encode) {
    e->do_encode(e, signature, encode, value);
  } else {
    e->on_error(LABCOMM_ERROR_ENC_MISSING_DO_ENCODE, 0);
  }
}

void labcomm_encoder_free(labcomm_encoder_t* e)
{
  e->writer.write(&e->writer, labcomm_writer_free);
  labcomm_encoder_context_t *context = (labcomm_encoder_context_t *) e->context;

#ifdef LABCOMM_ENCODER_LINEAR_SEARCH
  labcomm_sample_entry_t *entry = context->sample;
  labcomm_sample_entry_t *entry_next;
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
  
  if (encoder->writer.ioctl != NULL) {
    va_list va;
    
    va_start(va, action);
    result = encoder->writer.ioctl(&encoder->writer, action, va);
    va_end(va);
  }
  return result;
}

static void collect_flat_signature(
  labcomm_decoder_t *decoder,
  labcomm_encoder_t *signature_writer)
{
  //int type = labcomm_decode_int(decoder); 
  int type = labcomm_decode_packed32(decoder); 
//  printf("%s: type=%x\n", __FUNCTION__, type);
  if (type >= LABCOMM_USER) {
    decoder->on_error(LABCOMM_ERROR_UNIMPLEMENTED_FUNC, 3,
			"Implement %s ... (1) for type 0x%x\n", __FUNCTION__, type);
  } else {
    //labcomm_encode_int(signature_writer, type); 
    labcomm_encode_packed32(signature_writer, type); 
    switch (type) {
      case LABCOMM_ARRAY: {
	int dimensions, i;

	dimensions = labcomm_decode_packed32(decoder); //labcomm_decode_int(decoder); //unpack32
	labcomm_encode_packed32(signature_writer, dimensions); //pack32
	for (i = 0 ; i < dimensions ; i++) {
	  int n = labcomm_decode_packed32(decoder); //labcomm_decode_int(decoder);
	  labcomm_encode_packed32(signature_writer, n); // labcomm_encode_int(signature_writer, n);
	}
	collect_flat_signature(decoder, signature_writer);
      } break;
      case LABCOMM_STRUCT: {
	int fields, i;
	//fields = labcomm_decode_int(decoder); 
	//labcomm_encode_int(signature_writer, fields); 
	fields = labcomm_decode_packed32(decoder); 
	labcomm_encode_packed32(signature_writer, fields); 
	for (i = 0 ; i < fields ; i++) {
	  char *name = labcomm_decode_string(decoder);
	  labcomm_encode_string(signature_writer, name);
	  free(name);
	  collect_flat_signature(decoder, signature_writer);
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

static void do_decoder_register(
  labcomm_decoder_t *decoder,
  labcomm_signature_t *signature,
  labcomm_decoder_typecast_t type_decoder,
  labcomm_handler_typecast_t handler,
  void *handler_context)
{

  labcomm_decoder_context_t *context = decoder->context;
  labcomm_sample_entry_t *sample;
  sample = get_sample_by_signature_address(context->sample,
					   signature);
  if (!sample) {
    sample = (labcomm_sample_entry_t*)malloc(sizeof(labcomm_sample_entry_t));
    sample->next = context->sample;
    context->sample = sample;
    sample->index = 0;
    sample->signature = signature;
  }
  sample->decoder = type_decoder;
  sample->handler = handler;
  sample->context = handler_context;
}

static int do_decode_one(labcomm_decoder_t *d)
{
  int result;

  do {
    result = d->reader.read(&d->reader, labcomm_reader_start);
    if (result > 0) {
      labcomm_decoder_context_t *context = d->context;

//      printf("do_decode_one: result = %x\n", result);
      result = labcomm_decode_packed32(d);
//      printf("do_decode_one: result(2) = %x\n", result);
      if (result == LABCOMM_TYPEDEF || result == LABCOMM_SAMPLE) {
	/* TODO: should the labcomm_dynamic_buffer_writer be 
	   a permanent part of labcomm_decoder? */
	labcomm_encoder_t *e = labcomm_encoder_new(
	  labcomm_dynamic_buffer_writer, 0);
	labcomm_signature_t signature;
	labcomm_sample_entry_t *entry = NULL;
	int index, err;

	index = labcomm_decode_packed32(d); //int
	signature.name = labcomm_decode_string(d);
	signature.type = result;
	e->writer.write(&e->writer, labcomm_writer_start);
	/* printf("do_decode_one: result = %x, index = %x, name=%s\n", 
	   result, index, signature.name); */
	collect_flat_signature(d, e);
	e->writer.write(&e->writer, labcomm_writer_end);
	err = labcomm_encoder_ioctl(e, LABCOMM_IOCTL_WRITER_GET_BYTES_WRITTEN,
				    &signature.size);
	if (err < 0) {
	  printf("Failed to get size: %s\n", strerror(-err));
	  goto free_signature_name;
	}
	err = labcomm_encoder_ioctl(e, LABCOMM_IOCTL_WRITER_GET_BYTE_POINTER,
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
	labcomm_encoder_free(e);
	if (!entry) {
	  // No handler for found type, bail out (after cleanup)
	  result = -ENOENT;
	}
      } else {
	labcomm_sample_entry_t *entry;

	entry = get_sample_by_index(context->sample, result);
	if (!entry) {
	  // printf("Error: %s: type not found (id=0x%x)\n",
		  //__FUNCTION__, result);
          d->on_error(LABCOMM_ERROR_DEC_TYPE_NOT_FOUND, 3, "%s(): type not found (id=0x%x)\n", __FUNCTION__, result);
	  result = -ENOENT;
	} else {
	  entry->decoder(d, entry->handler, entry->context);
	}
      }
    }
    d->reader.read(&d->reader, labcomm_reader_end);
    /* TODO: should we really loop, or is it OK to
       return after a typedef/sample */
  } while (result > 0 && result < LABCOMM_USER);
  return result;
}

labcomm_decoder_t *labcomm_decoder_new(
  int (*reader)(labcomm_reader_t *, labcomm_reader_action_t, ...),
  void *reader_context)
{
  labcomm_decoder_t *result = malloc(sizeof(labcomm_decoder_t));
  if (result) {
    labcomm_decoder_context_t *context =
      (labcomm_decoder_context_t*)malloc(sizeof(labcomm_decoder_context_t));
    context->sample = 0;
    result->context = context;
    result->reader.context = reader_context;
    result->reader.data = 0;
    result->reader.data_size = 0;
    result->reader.count = 0;
    result->reader.pos = 0;
    result->reader.read = reader;
    result->reader.ioctl = NULL;
    result->reader.on_error = on_error_fprintf;
    result->do_register = do_decoder_register;
    result->do_decode_one = do_decode_one;
    result->on_error = on_error_fprintf;
    result->on_new_datatype = on_new_datatype;
    result->reader.read(&result->reader, labcomm_reader_alloc, LABCOMM_VERSION);
  }
  return result;
}

void labcomm_internal_decoder_register(
  labcomm_decoder_t *d,
  labcomm_signature_t *signature,
  labcomm_decoder_typecast_t type_decoder,
  labcomm_handler_typecast_t handler,
  void *handler_context)
{
  // Will segfault if d == NULL
  if (d->do_register) {
    d->do_register(d, signature, type_decoder, handler, handler_context);
  } else {
    d->on_error(LABCOMM_ERROR_DEC_MISSING_DO_REG, 0);
  }
}

int labcomm_decoder_decode_one(labcomm_decoder_t *d)
{
  int result = -1;
  // Will segfault if decoder == NULL.
  if (d->do_decode_one)
  {
    result = d->do_decode_one(d);
  }
  else
  {
    d->on_error(LABCOMM_ERROR_DEC_MISSING_DO_DECODE_ONE, 0);
  }
  return result;
}

void labcomm_decoder_run(labcomm_decoder_t *d)
{
  while (labcomm_decoder_decode_one(d) > 0) {
  }
}

void labcomm_decoder_free(labcomm_decoder_t* d)
{
  d->reader.read(&d->reader, labcomm_reader_free);
  labcomm_decoder_context_t *context = (labcomm_decoder_context_t *) d->context;
  labcomm_sample_entry_t *entry = context->sample;
  labcomm_sample_entry_t *entry_next;

  while (entry != NULL) {
    entry_next = entry->next;
    free(entry);
    entry = entry_next;
  }

  free(d->context);
  free(d);
}
