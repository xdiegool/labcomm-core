/*
  labcomm2014_renaming_decoder.c -- a stacked decoder that renames samples

  Copyright 2015 Anders Blomdell <anders.blomdell@control.lth.se>

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

#include <errno.h>
#include "labcomm2014_renaming_decoder.h"
#include "labcomm2014.h"
#include "labcomm2014_private.h"

struct decoder {
  struct labcomm2014_decoder decoder;
  struct labcomm2014_decoder *next;
  char *(*rename)(struct labcomm2014_memory *m, char *name, void *context);
  void *context;
  LABCOMM_SIGNATURE_ARRAY_DEF(renamed, struct labcomm2014_signature *);
};

static struct labcomm2014_signature *get_renamed(
  struct labcomm2014_decoder *d,
  const struct labcomm2014_signature *signature)
{
  struct labcomm2014_signature *result;
  struct decoder *id = d->context;
  int index;

  index = labcomm2014_get_local_index(signature);
  labcomm2014_scheduler_data_lock(d->scheduler);
  result = LABCOMM_SIGNATURE_ARRAY_GET(id->renamed,
                                      struct labcomm2014_signature *,
                                      index, NULL);
  labcomm2014_scheduler_data_unlock(d->scheduler);
  return result;
}
  
static struct labcomm2014_signature *set_renamed(
  struct labcomm2014_decoder *d,
  const struct labcomm2014_signature *signature)
{
  struct labcomm2014_signature *result;

  result = get_renamed(d, signature);
  if (result == NULL) {
    /* create a renamed sample */
    struct decoder *id = d->context;
    int index;
    struct labcomm2014_signature **renamed;
  
    index = labcomm2014_get_local_index(signature);
    if (index <= 0) { goto out; /*result already NULL */}
    labcomm2014_scheduler_data_lock(d->scheduler);
    renamed = LABCOMM_SIGNATURE_ARRAY_REF(d->memory, id->renamed,
                                          struct labcomm2014_signature *,
                                          index);
    if (renamed == NULL) {
      labcomm2014_error_warning(d->error,
                                LABCOMM2014_ERROR_MEMORY,
                                "Could not allocate rename slot: %s\n",
                                signature->name);
      goto unlock;
    }
    if (*renamed != NULL) {
      /* Somebody beat as to allocation, this should never happen */
      goto unlock;
    }
    result = labcomm2014_memory_alloc(d->memory, 0, sizeof(*result));
    if (result == NULL) {
      labcomm2014_error_warning(d->error,
                                LABCOMM2014_ERROR_MEMORY,
                                "Could not allocate rename signature: %s\n",
                                signature->name);
      goto unlock;
    }
    result->name = id->rename(d->memory, signature->name, id->context);
    if (result->name == NULL) {
      labcomm2014_error_warning(d->error,
                                LABCOMM2014_ERROR_MEMORY,
                                "Could not allocate rename name: %s\n",
                                signature->name);
      goto unlock_free_result;
    }
    result->encoded_size = signature->encoded_size;
    result->size = signature->size;
    result->signature = signature->signature; 
    result->index = 0;
#ifndef LABCOMM_NO_TYPEDECL
    result->tdsize = signature->tdsize;
    result->treedata = signature->treedata;
#endif  
    labcomm2014_set_local_index(result);
    *renamed = result;
    goto unlock;
  unlock_free_result:
    labcomm2014_memory_free(d->memory, 0, result);
    result = NULL;
  unlock:
    labcomm2014_scheduler_data_unlock(d->scheduler);
  out:
    ;
  }
  return result;
}

static int do_sample_register(struct labcomm2014_decoder *d, 
                         const struct labcomm2014_signature *s, 
                         labcomm2014_decoder_function decoder,
                         labcomm2014_handler_function handler,
                         void *context)
{
  struct decoder *id = d->context;
  
  return id->next->sample_register(id->next, set_renamed(d, s), decoder,
				   handler, context);
}

static int do_ref_register(struct labcomm2014_decoder *d, 
                      const struct labcomm2014_signature *signature)
{
  struct decoder *id = d->context;
  
  return id->next->ref_register(id->next, set_renamed(d, signature));
}

static int do_ioctl(struct labcomm2014_decoder *d, 
                    const struct labcomm2014_signature *signature,
                    uint32_t ioctl_action, va_list args)
{
  struct decoder *id = d->context;
  
  return id->next->ioctl(id->next, get_renamed(d, signature),
                         ioctl_action, args);
}

static int do_decode_one(struct labcomm2014_decoder *d)
{
  struct decoder *id = d->context;

  return id->next->decode_one(id->next);
}

static const struct labcomm2014_sample_ref *do_index_to_sample_ref(
  struct labcomm2014_decoder *d, int index)
{
  struct decoder *id = d->context;
  
  return id->next->index_to_sample_ref(id->next, index);
}

static const struct labcomm2014_sample_ref *do_ref_get(
  struct labcomm2014_decoder *d,
  const struct labcomm2014_signature *signature)
{
  const struct labcomm2014_signature *renamed;
  struct decoder *id = d->context;

  renamed = get_renamed(d, signature);
  if (renamed == NULL) {
    return id->next->ref_get(id->next, signature);
  } else {
    return id->next->ref_get(id->next, renamed);
  }
}

static void do_free(struct labcomm2014_decoder *d)
{
  struct decoder *id = d->context;
  int i;

  LABCOMM_SIGNATURE_ARRAY_FOREACH(id->renamed, struct labcomm2014_signature *,
                                  i) {
    struct labcomm2014_signature *s;
    s = LABCOMM_SIGNATURE_ARRAY_GET(id->renamed,
                                    struct labcomm2014_signature *, i, NULL);
    if (s) {
      labcomm2014_memory_free(d->memory, 0, s->name);
      labcomm2014_memory_free(d->memory, 0, s);
    }
  }
  LABCOMM_SIGNATURE_ARRAY_FREE(d->memory, id->renamed,
                               struct labcomm2014_signature *);
  labcomm2014_memory_free(d->memory, 0, id);
}

struct labcomm2014_decoder *labcomm2014_renaming_decoder_new(
  struct labcomm2014_decoder *d,
  char *(*rename)(struct labcomm2014_memory *m, char *name, void *context),
  void *context)
{
  struct decoder *result;
  result = labcomm2014_memory_alloc(d->memory, 0, sizeof(*result));
  if (!result) {
    return NULL;
  } else {
      result->decoder.context = result;
      result->decoder.reader = d->reader;
      result->decoder.error = d->error;
      result->decoder.memory = d->memory;
      result->decoder.scheduler = d->scheduler;
      result->decoder.free = do_free;
      result->decoder.decode_one = do_decode_one;
      result->decoder.sample_register = do_sample_register;
      result->decoder.ref_register = do_ref_register;
      result->decoder.ioctl = do_ioctl;
      result->decoder.index_to_sample_ref = do_index_to_sample_ref;
      result->decoder.ref_get = do_ref_get;
      result->next = d;
      result->rename = rename;
      result->context = context;
      LABCOMM_SIGNATURE_ARRAY_INIT(result->renamed,
                                   struct labcomm2014_signature *);
      return &(result->decoder);
  }
}
