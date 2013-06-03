#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "labcomm_private.h"
#include "decimating.h"
#include "gen/decimating_messages.h"

struct decimating_private {
  struct decimating decimating;
  struct labcomm_encoder *encoder;
  struct labcomm_decoder *decoder;
  struct orig_reader {
    void *context;
    const struct labcomm_reader_action *action;
  } orig_reader;
  struct orig_writer {
    void *context;
    const struct labcomm_writer_action *action;
  } orig_writer;
};

static void set_decimation(
  decimating_messages_set_decimation *value,
  void * context)
{
  fprintf(stderr, "%s\n", __FUNCTION__);
}

static int wrap_reader_alloc(struct labcomm_reader *r, void *context, 
			     struct labcomm_decoder *decoder,
			     char *labcomm_version)
{
  int result;
  struct decimating_private *decimating = context;
  struct orig_reader *orig_reader = &decimating->orig_reader;

  fprintf(stderr, "%s\n", __FUNCTION__);
  result = orig_reader->action->alloc(r, orig_reader->context, 
				      decoder, labcomm_version);
  labcomm_decoder_register_decimating_messages_set_decimation(decoder,
							      set_decimation,
							      decimating);
  return result;
}

static int wrap_reader_free(struct labcomm_reader *r, void *context)
{
  struct decimating_private *decimating = context;
  struct orig_reader *orig_reader = &decimating->orig_reader;

  fprintf(stderr, "%s\n", __FUNCTION__);
  return orig_reader->action->free(r, orig_reader->context);
}

static int wrap_reader_start(struct labcomm_reader *r, void *context)
{
  struct decimating_private *decimating = context;
  struct orig_reader *orig_reader = &decimating->orig_reader;

  fprintf(stderr, "%s\n", __FUNCTION__);
  return orig_reader->action->start(r, orig_reader->context);
}

static int wrap_reader_end(struct labcomm_reader *r, void *context)
{
  struct decimating_private *decimating = context;
  struct orig_reader *orig_reader = &decimating->orig_reader;

  fprintf(stderr, "%s\n", __FUNCTION__);
  return orig_reader->action->end(r, orig_reader->context);
}

static int wrap_reader_fill(struct labcomm_reader *r, void *context)
{
  struct decimating_private *decimating = context;
  struct orig_reader *orig_reader = &decimating->orig_reader;
  int result;

  fprintf(stderr, "%s\n", __FUNCTION__);
  fprintf(stderr, "%d\n", decimating->decimating.reader->pos);
  result = orig_reader->action->fill(r, orig_reader->context);
  fprintf(stderr, "%d %d\n", decimating->decimating.reader->pos, result);
  return result;
} 

static int wrap_reader_ioctl(struct labcomm_reader *r, void *context,
			     int action, 
			     struct labcomm_signature *signature, va_list args)
{
  struct decimating_private *decimating = context;
  struct orig_reader *orig_reader = &decimating->orig_reader;

  fprintf(stderr, "%s\n", __FUNCTION__);
  if (orig_reader->action->ioctl == NULL) {
    return -ENOTSUP;
  } else {
    return orig_reader->action->ioctl(r, orig_reader->context,
				      action, signature, args);
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

  result->orig_reader.context = reader->context;
  result->orig_reader.action = reader->action;
  reader->context = result;
  reader->action = &decimating_reader_action;
  result->decimating.reader = reader;

  goto out_ok;

out_fail:
  return NULL;

out_ok:
  return &result->decimating;
}


