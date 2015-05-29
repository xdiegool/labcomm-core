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
#include "labcomm2014_renaming_private.h"

struct decoder {
  struct labcomm2014_decoder decoder;
  struct labcomm2014_decoder *next;
  struct labcomm2014_renaming_registry *registry;
  char *(*rename_func)(struct labcomm2014_memory *m, char *name, void *context);
  void *context;
  LABCOMM_SIGNATURE_ARRAY_DEF(renamed,
                              struct labcomm2014_renaming_rename *);
};

static struct labcomm2014_renaming_rename *get_renamed(
  struct labcomm2014_decoder *d,
  const struct labcomm2014_signature *signature)
{
  struct labcomm2014_renaming_rename *result;
  struct decoder *id = d->context;
  int index;

  index = labcomm2014_get_local_index(signature);
  labcomm2014_scheduler_data_lock(d->scheduler);
  result = LABCOMM_SIGNATURE_ARRAY_GET(id->renamed,
                                      struct labcomm2014_renaming_rename *,
                                      index, NULL);
  labcomm2014_scheduler_data_unlock(d->scheduler);
  return result;
}
  
static struct labcomm2014_renaming_rename *set_renamed(
  struct labcomm2014_decoder *d,
  const struct labcomm2014_signature *signature)
{
  struct labcomm2014_renaming_rename *result = NULL;

  result = get_renamed(d, signature);
  if (result == NULL) {
    struct decoder *id = d->context;
    struct labcomm2014_renaming_rename **renamed;
    struct labcomm2014_renaming_rename *entry = NULL;
    int index;

    index = labcomm2014_get_local_index(signature);
    entry = labcomm2014_renaming_rename_new(id->registry, signature,
                                            id->rename_func, id->context);
    if (entry == NULL) { goto out; }
    labcomm2014_scheduler_data_lock(d->scheduler);
    renamed = LABCOMM_SIGNATURE_ARRAY_REF(d->memory, id->renamed,
                                          struct labcomm2014_renaming_rename *,
                                          index);
    if (renamed == NULL) { goto free_unlock; }
    if (*renamed != NULL) { result = *renamed; goto free_unlock; }
    *renamed = entry;
    result = entry;
    goto unlock;
  free_unlock:
    labcomm2014_renaming_rename_free(id->registry, entry);
  unlock:
    labcomm2014_scheduler_data_unlock(d->scheduler);
  out:
    ;
  }
  return result;
}

static int do_sample_register(struct labcomm2014_decoder *d, 
                         const struct labcomm2014_signature *signature, 
                         labcomm2014_decoder_function decoder,
                         labcomm2014_handler_function handler,
                         void *context)
{
  const struct labcomm2014_renaming_rename *renamed;
  struct decoder *id = d->context;
  
  renamed = set_renamed(d, signature);  
  return id->next->sample_register(
    id->next, labcomm2014_renaming_rename_signature(renamed),
    decoder, handler, context);
}

static int do_ref_register(struct labcomm2014_decoder *d, 
                      const struct labcomm2014_signature *signature)
{
  const struct labcomm2014_renaming_rename *renamed;
  struct decoder *id = d->context;

  renamed = set_renamed(d, signature);  
  return id->next->ref_register(
    id->next, labcomm2014_renaming_rename_signature(renamed));
}

static int do_ioctl(struct labcomm2014_decoder *d, 
                    const struct labcomm2014_signature *signature,
                    uint32_t ioctl_action, va_list args)
{
  const struct labcomm2014_renaming_rename *renamed;
  struct decoder *id = d->context;
  
  renamed = get_renamed(d, signature);
  return id->next->ioctl(
    id->next, labcomm2014_renaming_rename_signature(renamed),
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
  const struct labcomm2014_renaming_rename *renamed;
  struct decoder *id = d->context;

  renamed = get_renamed(d, signature);
  if (renamed == NULL) {
    return id->next->ref_get(id->next, signature);
  } else {
    return id->next->ref_get(id->next,
                             labcomm2014_renaming_rename_signature(renamed));
  }
}

static void do_free(struct labcomm2014_decoder *d)
{
  struct decoder *id = d->context;
  int i;

  LABCOMM_SIGNATURE_ARRAY_FOREACH(id->renamed,
                                  struct labcomm2014_renaming_rename *,
                                  i) {
    struct labcomm2014_renaming_rename *r;
    r = LABCOMM_SIGNATURE_ARRAY_GET(id->renamed,
                                    struct labcomm2014_renaming_rename *,
                                    i, NULL);
    if (r) {
      labcomm2014_renaming_rename_free(id->registry, r);
    }
  }
  LABCOMM_SIGNATURE_ARRAY_FREE(d->memory, id->renamed,
                               struct labcomm2014_renaming_rename *);
  labcomm2014_memory_free(d->memory, 0, id);
}

struct labcomm2014_decoder *labcomm2014_renaming_decoder_new(
  struct labcomm2014_decoder *d,
  struct labcomm2014_renaming_registry *registry,
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
      result->registry = registry;
      result->rename_func = rename;
      result->context = context;
      LABCOMM_SIGNATURE_ARRAY_INIT(result->renamed,
                                   struct labcomm2014_renaming_rename *);
      return &(result->decoder);
  }
}
