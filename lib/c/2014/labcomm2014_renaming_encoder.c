/*
  labcomm2014_renaming_encoder.c -- a stacked encoder that renames samples

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
#include "labcomm2014_renaming_encoder.h"
#include "labcomm2014.h"
#include "labcomm2014_private.h"
#include "labcomm2014_renaming_private.h"

struct encoder {
  struct labcomm2014_encoder encoder;
  struct labcomm2014_encoder *next;
  struct labcomm2014_renaming_registry *registry;
  char *(*rename_func)(struct labcomm2014_memory *m, char *name, void *context);
  void *context;
  LABCOMM_SIGNATURE_ARRAY_DEF(renamed,
                              struct labcomm2014_renaming_rename *);
};

static struct labcomm2014_renaming_rename *get_renamed(
  struct labcomm2014_encoder *e,
  const struct labcomm2014_signature *signature)
{
  struct labcomm2014_renaming_rename *result;
  struct encoder *ie = e->context;
  int index;

  index = labcomm2014_get_local_index(signature);
  labcomm2014_scheduler_writer_lock(e->scheduler);
  result = LABCOMM_SIGNATURE_ARRAY_GET(ie->renamed,
                                      struct labcomm2014_renaming_rename *,
                                      index, NULL);
  labcomm2014_scheduler_writer_unlock(e->scheduler);
  return result;
}
  
static struct labcomm2014_renaming_rename *set_renamed(
  struct labcomm2014_encoder *e,
  const struct labcomm2014_signature *signature)
{
  struct labcomm2014_renaming_rename *result = NULL;

  result = get_renamed(e, signature);
  if (result == NULL) {
    struct encoder *ie = e->context;
    struct labcomm2014_renaming_rename **renamed;
    struct labcomm2014_renaming_rename *entry = NULL;
    int index;

    index = labcomm2014_get_local_index(signature);
    entry = labcomm2014_renaming_rename_new(ie->registry, signature,
                                            ie->rename_func, ie->context);
    if (entry == NULL) { goto out; }
    labcomm2014_scheduler_writer_lock(e->scheduler);
    renamed = LABCOMM_SIGNATURE_ARRAY_REF(e->memory, ie->renamed,
                                          struct labcomm2014_renaming_rename *,
                                          index);
    if (renamed == NULL) { goto free_unlock; }
    if (*renamed != NULL) { result = *renamed; goto free_unlock; }
    *renamed = entry;
    result = entry;
    goto unlock;
  free_unlock:
    labcomm2014_renaming_rename_free(ie->registry, entry);
  unlock:
    labcomm2014_scheduler_writer_unlock(e->scheduler);
  out:
    ;
  }
  return result;
}
  
static int do_type_register(struct labcomm2014_encoder *e,
                            const struct labcomm2014_signature *signature)
{
  const struct labcomm2014_renaming_rename *renamed;
  struct encoder *ie = e->context;

  renamed = get_renamed(e, signature);
  if (renamed) {
    /* Register base type and renamed type */
    ie->next->type_register(ie->next, signature);
    return ie->next->type_register(
      ie->next, labcomm2014_renaming_rename_signature(renamed));
  } else {
    return ie->next->type_register(ie->next, signature);
  }
}

static int do_type_bind(struct labcomm2014_encoder *e,
                        const struct labcomm2014_signature *signature,
                        char has_deps)
{
  const struct labcomm2014_renaming_rename *renamed;
  struct encoder *ie = e->context;

  renamed = get_renamed(e, signature);
  if (renamed) {
    return ie->next->type_bind(
      ie->next, labcomm2014_renaming_rename_signature(renamed), 1);
  } else {
    return ie->next->type_bind(ie->next, signature, has_deps);
  }
}

static int do_sample_register(struct labcomm2014_encoder *e, 
                         const struct labcomm2014_signature *signature, 
                         labcomm2014_encoder_function encode)
{
  const struct labcomm2014_renaming_rename *renamed;
  struct encoder *ie = e->context;

  renamed = set_renamed(e, signature);  
  return ie->next->sample_register(
    ie->next, labcomm2014_renaming_rename_signature(renamed), encode);
}

static int do_ref_register(struct labcomm2014_encoder *e, 
                      const struct labcomm2014_signature *signature)
{
  const struct labcomm2014_renaming_rename *renamed;
  struct encoder *ie = e->context;

  renamed = set_renamed(e, signature);  
  return ie->next->ref_register(ie->next,
                                labcomm2014_renaming_rename_signature(renamed));
}

static int do_encode(struct labcomm2014_encoder *e, 
                     const struct labcomm2014_signature *signature, 
                     labcomm2014_encoder_function encode,
                     void *value)
{
  const struct labcomm2014_renaming_rename *renamed;
  struct encoder *ie = e->context;

  renamed = get_renamed(e, signature);
  if (renamed == NULL) {
    return -EINVAL;
  } else {
    return ie->next->encode(ie->next,
                            labcomm2014_renaming_rename_signature(renamed),
                            encode, value);
  }
}

static int do_ioctl(struct labcomm2014_encoder *e, 
                    const struct labcomm2014_signature *signature,
                    uint32_t ioctl_action, va_list args)
{
  const struct labcomm2014_renaming_rename *renamed;
  struct encoder *ie = e->context;

  renamed = get_renamed(e, signature);
  if (renamed != NULL) {
    signature = labcomm2014_renaming_rename_signature(renamed);
  }
  return ie->next->ioctl(ie->next, signature, ioctl_action, args);
}

static int do_sample_ref_to_index(
  struct labcomm2014_encoder *e,
  const struct labcomm2014_sample_ref *sample_ref)
{
  struct encoder *ie = e->context;

  return ie->next->sample_ref_to_index(ie->next,sample_ref);
}

static const struct labcomm2014_sample_ref *do_ref_get(
  struct labcomm2014_encoder *e,
  const struct labcomm2014_signature *signature)
{
  const struct labcomm2014_renaming_rename *renamed;
  struct encoder *ie = e->context;

  renamed = get_renamed(e, signature);
  if (renamed == NULL) {
    return ie->next->ref_get(ie->next, signature);
  } else {
    return ie->next->ref_get(
      ie->next, labcomm2014_renaming_rename_signature(renamed));
  }
}

static void do_free(struct labcomm2014_encoder *e)
{
  struct encoder *ie = e->context;
  int i;

  LABCOMM_SIGNATURE_ARRAY_FOREACH(ie->renamed,
                                  struct labcomm2014_renaming_rename *, i) {
    struct labcomm2014_renaming_rename *r;
    r = LABCOMM_SIGNATURE_ARRAY_GET(ie->renamed,
                                    struct labcomm2014_renaming_rename *, i,
                                    NULL);
    if (r) {
      labcomm2014_renaming_rename_free(ie->registry, r);
    }
  }
  LABCOMM_SIGNATURE_ARRAY_FREE(e->memory, ie->renamed, struct labcomm2014_renaming_rename *);
  labcomm2014_memory_free(e->memory, 0, ie);
}

struct labcomm2014_encoder *labcomm2014_renaming_encoder_new(
  struct labcomm2014_encoder *e,
  struct labcomm2014_renaming_registry *registry,
  char *(*rename)(struct labcomm2014_memory *m, char *name, void *context),
  void *context)
{
  struct encoder *result;
  result = labcomm2014_memory_alloc(e->memory, 0, sizeof(*result));
  if (!result) {
    return NULL;
  } else {
      result->encoder.context = result;
      result->encoder.writer = e->writer;
      result->encoder.error = e->error;
      result->encoder.memory = e->memory;
      result->encoder.scheduler = e->scheduler;
      result->encoder.free = do_free;
      result->encoder.type_register = do_type_register;
      result->encoder.type_bind = do_type_bind;
      result->encoder.sample_register = do_sample_register;
      result->encoder.ref_register = do_ref_register;
      result->encoder.encode = do_encode;
      result->encoder.ioctl = do_ioctl;
      result->encoder.sample_ref_to_index = do_sample_ref_to_index;
      result->encoder.ref_get = do_ref_get;
      result->next = e;
      result->registry = registry;
      result->rename_func = rename;
      result->context = context;
      LABCOMM_SIGNATURE_ARRAY_INIT(result->renamed,
                                   struct labcomm2014_renaming_rename *);
      return &(result->encoder);
  }
}
