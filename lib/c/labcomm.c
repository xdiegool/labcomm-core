#include <errno.h>
#include <string.h>
#include <stdio.h>
#ifndef __VXWORKS__
#include <strings.h>
#endif
#include <stdlib.h>
#ifdef __VXWORKS__
#if (CPU == PPC603)
#undef _LITTLE_ENDIAN
#endif
#if (CPU == PENTIUM4)
#undef _BIG_ENDIAN
#endif
#endif
#include "labcomm.h"
#include "labcomm_private.h"

typedef struct labcomm_sample_entry {
  struct labcomm_sample_entry *next;
  int index;
  labcomm_signature_t *signature;
  labcomm_decoder_typecast_t decoder;
  labcomm_handler_typecast_t handler;
  labcomm_encode_typecast_t encode;
  void *context;
} labcomm_sample_entry_t;

typedef struct labcomm_encoder_context {
  labcomm_sample_entry_t *sample;
  int index;
} labcomm_encoder_context_t;

typedef struct labcomm_decoder_context {
  labcomm_sample_entry_t *sample;
} labcomm_decoder_context_t;

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

static int get_encoder_index(
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

static void do_encoder_register(struct labcomm_encoder *e,
				labcomm_signature_t *signature,
				labcomm_encode_typecast_t encode)
{
  if (signature->type == LABCOMM_SAMPLE) {
    if (get_encoder_index(e, signature) == 0) {
      int i;
      labcomm_encoder_context_t *context = e->context;
      labcomm_sample_entry_t *sample =
	(labcomm_sample_entry_t*)malloc(sizeof(labcomm_sample_entry_t));
      sample->next = context->sample;
      sample->index = context->index;
      sample->signature = signature;
      sample->encode = encode;
      context->index++;
      context->sample = sample;

      e->writer.write(&e->writer, labcomm_writer_start);
      labcomm_encode_int(e, signature->type);
      labcomm_encode_type_index(e, signature);
      labcomm_encode_string(e, signature->name);
      for (i = 0 ; i < signature->size ; i++) {
	if (e->writer.pos >= e->writer.count) {
	  e->writer.write(&e->writer, labcomm_writer_continue);
	}
	e->writer.data[e->writer.pos] = signature->signature[i];
	e->writer.pos++;
      }
      e->writer.write(&e->writer, labcomm_writer_end);
    }
  }
}

static void do_encode(
  labcomm_encoder_t *encoder,
  labcomm_signature_t *signature,
  void *value)
{
  labcomm_encoder_context_t *context = encoder->context;
  labcomm_sample_entry_t *sample;
  sample = get_sample_by_signature_address(context->sample,
					   signature);
  if (sample && sample->encode) {
    sample->encode(encoder, value);
  } else {
    printf("Encoder has no registration for %s\n", signature->name);
  }
}

labcomm_encoder_t *labcomm_encoder_new(
  int (*writer)(labcomm_writer_t *, labcomm_writer_action_t),
  void *writer_context)
{
  labcomm_encoder_t *result = malloc(sizeof(labcomm_encoder_t));
  if (result) {
    labcomm_encoder_context_t *context;

    context = malloc(sizeof(labcomm_encoder_context_t));
    context->sample = 0;
    context->index = LABCOMM_USER;
    result->context = context;
    result->writer.context = writer_context;
    result->writer.data = 0;
    result->writer.data_size = 0;
    result->writer.count = 0;
    result->writer.pos = 0;
    result->writer.write = writer;
    result->writer.write(&result->writer, labcomm_writer_alloc);
    result->do_register = do_encoder_register;
    result->do_encode = do_encode;
  }
  return result;
}

void labcomm_internal_encoder_register(
  labcomm_encoder_t *e,
  labcomm_signature_t *signature,
  labcomm_encode_typecast_t encode)
{
  if (e && e->do_register) {
    e->do_register(e, signature, encode);
  } else {
    printf("Encoder is missing do_register\n");
  }
}

void labcomm_internal_encode(
  labcomm_encoder_t *e,
  labcomm_signature_t *signature,
  void *value)
{
  if (e && e->do_encode) {
    e->do_encode(e, signature, value);
  } else {
    printf("Encoder is missing do_encode\n");
  }
}

void labcomm_internal_encoder_user_action(labcomm_encoder_t *e,
					  int action)
{
  e->writer.write(&e->writer, action);
}

void labcomm_encoder_free(labcomm_encoder_t* e)
{

  e->writer.write(&e->writer, labcomm_writer_free);
  free(e);
}

void labcomm_encode_type_index(labcomm_encoder_t *e, labcomm_signature_t *s)
{
  int index = get_encoder_index(e, s);
  labcomm_encode_int(e, index);
}

static int signature_writer(
  labcomm_writer_t *w,
  labcomm_writer_action_t action)
{
  switch (action) {
    case labcomm_writer_alloc: {
      w->data_size = 1000;
      w->count = w->data_size;
      w->data = malloc(w->data_size);
      w->pos = 0;
    } break;
    case labcomm_writer_start: {
      w->data_size = 1000;
      w->count = w->data_size;
      w->data = realloc(w->data, w->data_size);
      w->pos = 0;
    } break;
    case labcomm_writer_continue: {
      w->data_size += 1000;
      w->count = w->data_size;
      w->data = realloc(w->data, w->data_size);
    } break;
    case labcomm_writer_end: {
    } break;
    case labcomm_writer_free: {
      free(w->data);
      w->data = 0;
      w->data_size = 0;
      w->count = 0;
      w->pos = 0;
    } break;
    case labcomm_writer_available: {
    } break;
  }
  return 0;

}

static void collect_flat_signature(
  labcomm_decoder_t *decoder,
  labcomm_encoder_t *signature_writer)
{
  int type = labcomm_decode_int(decoder);
  if (type >= LABCOMM_USER) {
    printf("Implement %s ...\n", __FUNCTION__);
  } else {
    labcomm_encode_int(signature_writer, type);
    switch (type) {
      case LABCOMM_ARRAY: {
	int dimensions, i;

	dimensions = labcomm_decode_int(decoder);
	labcomm_encode_int(signature_writer, dimensions);
	for (i = 0 ; i < dimensions ; i++) {
	  int n = labcomm_decode_int(decoder);
	  labcomm_encode_int(signature_writer, n);
	}
	collect_flat_signature(decoder, signature_writer);
      } break;
      case LABCOMM_STRUCT: {
	int fields, i;
	fields = labcomm_decode_int(decoder);
	labcomm_encode_int(signature_writer, fields);
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
	printf("Implement %s ...\n", __FUNCTION__);
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

      result = labcomm_decode_int(d);
      if (result == LABCOMM_TYPEDEF || result == LABCOMM_SAMPLE) {
	labcomm_encoder_t *e = labcomm_encoder_new(signature_writer, 0);
	labcomm_signature_t signature;
	labcomm_sample_entry_t *entry;
	int index;

	e->writer.write(&e->writer, labcomm_writer_start);
	signature.type = result;
	index = labcomm_decode_int(d);
	signature.name = labcomm_decode_string(d);
	collect_flat_signature(d, e);
	signature.size = e->writer.pos;
	signature.signature = e->writer.data;
	entry = get_sample_by_signature_value(context->sample, &signature);
	if (! entry) {
	  // Unknown datatype, bail out
	  fprintf(stderr,	"%s: unknown datatype '%s' (id=0x%x)\n",
		  __FUNCTION__, signature.name, index);
	} else if (entry->index && entry->index != index) {
	  fprintf(stderr,	"%s: index mismatch '%s' (id=0x%x != 0x%x)\n",
		  __FUNCTION__, signature.name, entry->index, index);
	} else {
	  entry->index = index;
	}
	free(signature.name);
	e->writer.write(&e->writer, labcomm_writer_end);
	if (!entry) {
	  // No handler for found type, bail out (after cleanup)
	  result = -ENOENT;
	}
	labcomm_encoder_free(e);
      } else {
	labcomm_sample_entry_t *entry;

	entry = get_sample_by_index(context->sample, result);
	if (!entry) {
	  fprintf(stderr,	"%s: type not found (id=0x%x)\n",
		  __FUNCTION__, result);
	  result = -ENOENT;
	} else {
	  entry->decoder(d, entry->handler, entry->context);
	}
      }
    }
    d->reader.read(&d->reader, labcomm_reader_end);
  } while (result > 0 && result < LABCOMM_USER);
  return result;
}

labcomm_decoder_t *labcomm_decoder_new(
  int (*reader)(labcomm_reader_t *, labcomm_reader_action_t),
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
    result->reader.read(&result->reader, labcomm_reader_alloc);
    result->do_register = do_decoder_register;
    result->do_decode_one = do_decode_one;
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
  if (d && d->do_register) {
    d->do_register(d, signature, type_decoder, handler, handler_context);
  } else {
    printf("Decoder is missing do_register\n");
  }
}

int labcomm_decoder_decode_one(labcomm_decoder_t *d)
{
  int result = -1;
  if (d && d->do_decode_one)
  {
    printf("labcomm decode 1\n");
    result = d->do_decode_one(d);
  }
  else
  {
    printf("Decoder is missing do_decode_one\n");
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
  free(d);
}
