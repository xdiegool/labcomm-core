#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "labcomm_private.h"
#include "decimating.h"
#include "gen/decimating_messages.h"

struct decimating_private {
  struct decimating decimating;
  struct labcomm_encoder *encoder;
  int encoder_initialized;
  struct labcomm_decoder *decoder;
  int decoder_initialized;
  struct orig_reader {
    void *context;
    const struct labcomm_reader_action *action;
  } orig_reader;
  struct orig_writer {
    void *context;
    const struct labcomm_writer_action *action;
  } orig_writer;
  LABCOMM_SIGNATURE_ARRAY_DEF(decimation, 
			      struct decimation {
				int n;
				int current;
			      });
};

static void set_decimation(
  decimating_messages_set_decimation *value,
  void * context)
{
  struct decimating_private *decimating = context;
  struct decimation *decimation;

  decimation = LABCOMM_SIGNATURE_ARRAY_REF(decimating->decimation, 
					   struct decimation, 
					   value->signature_index);
  decimation->n = value->decimation;
  decimation->current = 0;
}

static int wrap_reader_alloc(struct labcomm_reader *r, void *context, 
			     struct labcomm_decoder *decoder,
			     char *labcomm_version)
{
  int result;
  struct decimating_private *decimating = context;
  struct orig_reader *orig_reader = &decimating->orig_reader;

  /* Stash away decoder for later use */
  decimating->decoder = decoder;
  result = orig_reader->action->alloc(r, orig_reader->context, 
				      decoder, labcomm_version);
  return result;
}

static int wrap_reader_free(struct labcomm_reader *r, void *context)
{
  struct decimating_private *decimating = context;
  struct orig_reader *orig_reader = &decimating->orig_reader;

  return orig_reader->action->free(r, orig_reader->context);
}

static int wrap_reader_start(struct labcomm_reader *r, void *context)
{
  struct decimating_private *decimating = context;
  struct orig_reader *orig_reader = &decimating->orig_reader;

  if (! decimating->decoder_initialized) {
    decimating->decoder_initialized = 1;
    labcomm_decoder_register_decimating_messages_set_decimation(
      decimating->decoder, set_decimation, decimating);
  }
  return orig_reader->action->start(r, orig_reader->context);
}

static int wrap_reader_end(struct labcomm_reader *r, void *context)
{
  struct decimating_private *decimating = context;
  struct orig_reader *orig_reader = &decimating->orig_reader;

  return orig_reader->action->end(r, orig_reader->context);
}

static int wrap_reader_fill(struct labcomm_reader *r, void *context)
{
  struct decimating_private *decimating = context;
  struct orig_reader *orig_reader = &decimating->orig_reader;
  int result;

  result = orig_reader->action->fill(r, orig_reader->context);
  return result;
} 

static int wrap_reader_ioctl(struct labcomm_reader *r, void *context,
			     int signature_index,
			     struct labcomm_signature *signature, 
			     uint32_t action, va_list args)
{
  struct decimating_private *decimating = context;
  struct orig_reader *orig_reader = &decimating->orig_reader;

  if (action == SET_DECIMATION) {
    decimating_messages_set_decimation decimation;
    va_list va;

    va_copy(va, args);
    decimation.decimation = va_arg(va, int);
    decimation.signature_index = signature_index;
    va_end(va);
    return labcomm_encode_decimating_messages_set_decimation(
      decimating->encoder, &decimation);
  } else {
    return orig_reader->action->ioctl(r, orig_reader->context,
				      signature_index, signature, action, args);
  }
  
}

struct labcomm_reader_action decimating_reader_action = {
  .alloc = wrap_reader_alloc,
  .free = wrap_reader_free, 
  .start = wrap_reader_start,
  .end = wrap_reader_end,
  .fill = wrap_reader_fill,
  .ioctl = wrap_reader_ioctl
};

int wrap_writer_alloc(struct labcomm_writer *w, void *context, 
		      struct labcomm_encoder *encoder, char *labcomm_version)
{
  struct decimating_private *decimating = context;
  struct orig_writer *orig_writer = &decimating->orig_writer;

  /* Stash away encoder for later use */
  decimating->encoder = encoder;
  return orig_writer->action->alloc(w, orig_writer->context,
				    encoder, labcomm_version);
}

int wrap_writer_free(struct labcomm_writer *w, void *context)
{  
  struct decimating_private *decimating = context;
  struct orig_writer *orig_writer = &decimating->orig_writer;

  return orig_writer->action->free(w, orig_writer->context);
}

int wrap_writer_start(struct labcomm_writer *w, void *context,
		      struct labcomm_encoder *encoder,
		      int index, struct labcomm_signature *signature,
		      void *value)
{
  struct decimating_private *decimating = context;
  struct orig_writer *orig_writer = &decimating->orig_writer;
  struct decimation *decimation;

  if (! decimating->encoder_initialized) {
    decimating->encoder_initialized = 1;
    labcomm_encoder_register_decimating_messages_set_decimation(
      decimating->encoder);
  }
  decimation = LABCOMM_SIGNATURE_ARRAY_REF(decimating->decimation, 
					   struct decimation, index);
  decimation->current++;
  if (decimation->current < decimation->n) {
    return -EALREADY;
  } else {
    decimation->current = 0;
    return orig_writer->action->start(w, orig_writer->context,
				      encoder, index, signature, value);
  }
}

int wrap_writer_end(struct labcomm_writer *w, void *context)
{
  struct decimating_private *decimating = context;
  struct orig_writer *orig_writer = &decimating->orig_writer;

  return orig_writer->action->end(w, orig_writer->context);
}

int wrap_writer_flush(struct labcomm_writer *w, void *context)
{
  struct decimating_private *decimating = context;
  struct orig_writer *orig_writer = &decimating->orig_writer;

  return orig_writer->action->flush(w, orig_writer->context);
}

int wrap_writer_ioctl(struct labcomm_writer *w, void *context, 
		      int signature_index,
		      struct labcomm_signature *signature, 
		      uint32_t action, va_list args)
{
  struct decimating_private *decimating = context;
  struct orig_writer *orig_writer = &decimating->orig_writer;

  return orig_writer->action->ioctl(w, orig_writer->context,
				    signature_index, signature, action, args);
}


struct labcomm_writer_action decimating_writer_action = {
  .alloc = wrap_writer_alloc,
  .free = wrap_writer_free, 
  .start = wrap_writer_start,
  .end = wrap_writer_end,
  .flush = wrap_writer_flush,
  .ioctl = wrap_writer_ioctl
};

extern struct decimating *decimating_new(
  struct labcomm_reader *reader,
  struct labcomm_writer *writer,
  struct labcomm_lock *lock)
{
  struct decimating_private *result;

  result = malloc(sizeof(*result));
  if (result == NULL) {
    goto out_fail;
  }

  /* Wrap reader and writer */
  result->orig_reader.context = reader->context;
  result->orig_reader.action = reader->action;
  reader->context = result;
  reader->action = &decimating_reader_action;

  result->orig_writer.context = writer->context;
  result->orig_writer.action = writer->action;
  writer->context = result;
  writer->action = &decimating_writer_action;

  /* Init visible result struct */
  result->decimating.reader = reader;
  result->decimating.writer = writer;

  /* Init other fields */
  result->encoder = NULL;
  result->encoder_initialized = 0;
  result->decoder = NULL;
  result->decoder_initialized = 0;
  LABCOMM_SIGNATURE_ARRAY_INIT(result->decimation, struct decimation);

  goto out_ok;

out_fail:
  return NULL;

out_ok:
  return &result->decimating;
}


