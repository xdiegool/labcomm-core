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

struct renamed {
  struct labcomm2014_signature signature;
  struct labcomm2014_signature renamed;
  struct labcomm2014_signature_data treedata[2]; 
};

struct encoder {
  struct labcomm2014_encoder encoder;
  struct labcomm2014_encoder *next;
  char *(*rename)(struct labcomm2014_memory *m, char *name, void *context);
  void *context;
  LABCOMM_SIGNATURE_ARRAY_DEF(renamed, struct renamed *);
};

static struct renamed *get_renamed(
  struct labcomm2014_encoder *e,
  const struct labcomm2014_signature *signature)
{
  struct renamed *result;
  struct encoder *ie = e->context;
  int index;

  index = labcomm2014_get_local_index(signature);
  labcomm2014_scheduler_writer_lock(e->scheduler);
  result = LABCOMM_SIGNATURE_ARRAY_GET(ie->renamed,
                                      struct renamed *,
                                      index, NULL);
  labcomm2014_scheduler_writer_unlock(e->scheduler);
  return result;
}
  
static struct renamed *set_renamed(
  struct labcomm2014_encoder *e,
  const struct labcomm2014_signature *signature)
{
  struct renamed *result;

  result = get_renamed(e, signature);
  if (result == NULL) {
    /* create a renamed sample */
    struct encoder *ie = e->context;
    int index;
    struct renamed **renamed;
  
    index = labcomm2014_get_local_index(signature);
    if (index <= 0) { goto out; /*result already NULL */}
    labcomm2014_scheduler_writer_lock(e->scheduler);
    renamed = LABCOMM_SIGNATURE_ARRAY_REF(e->memory, ie->renamed,
                                          struct renamed *,
                                          index);
    if (renamed == NULL) {
      labcomm2014_error_warning(e->error,
                                LABCOMM2014_ERROR_MEMORY,
                                "Could not allocate rename slot: %s\n",
                                signature->name);
      goto unlock;
    }
    if (*renamed != NULL) {
      /* Somebody beat as to allocation, this should never happen */
      goto unlock;
    }
    result = labcomm2014_memory_alloc(e->memory, 0, sizeof(*result));
    if (result == NULL) {
      labcomm2014_error_warning(e->error,
                                LABCOMM2014_ERROR_MEMORY,
                                "Could not allocate rename signature: %s\n",
                                signature->name);
      goto unlock;
    }
    result->signature.name = ie->rename(
      e->memory, signature->name, ie->context);
    if (result->signature.name == NULL) {
      labcomm2014_error_warning(e->error,
                                LABCOMM2014_ERROR_MEMORY,
                                "Could not allocate rename name: %s\n",
                                signature->name);
      goto unlock_free_result;
    }
    result->signature.encoded_size = signature->encoded_size;
    result->signature.size = signature->size;
    result->signature.signature = signature->signature; 
    result->signature.index = 0;
#ifndef LABCOMM_NO_TYPEDECL
    result->renamed.name = result->signature.name;
    result->renamed.encoded_size = NULL;
    result->renamed.size = 0;
    result->renamed.signature = NULL;
    result->renamed.index = 0;
    result->renamed.tdsize = signature->tdsize;
    result->renamed.treedata = signature->treedata;
    struct labcomm2014_signature_data treedata[2] = { 
      LABCOMM_SIGDEF_SIGNATURE(result->renamed),
      LABCOMM_SIGDEF_END
    };
    result->treedata[0] = treedata[0];
    result->treedata[1] = treedata[1];
    result->signature.tdsize = sizeof(result->treedata);
    result->signature.treedata = result->treedata;
#endif  
    labcomm2014_set_local_index(&result->signature);
    labcomm2014_set_local_index(&result->renamed);
    *renamed = result;
    goto unlock;
  unlock_free_result:
    labcomm2014_memory_free(e->memory, 0, result);
    result = NULL;
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
  const struct renamed *renamed;
  struct encoder *ie = e->context;

  renamed = get_renamed(e, signature);
  if (renamed) {
    /* Register renaming type */
    ie->next->type_register(ie->next, &renamed->renamed);
  }
  return ie->next->type_register(ie->next, signature);
}

static int do_type_bind(struct labcomm2014_encoder *e,
                        const struct labcomm2014_signature *signature,
                        char has_deps)
{
  const struct renamed *renamed;
  struct encoder *ie = e->context;

  renamed = get_renamed(e, signature);
  if (renamed) {
    return ie->next->type_bind(ie->next, &renamed->signature, 1);
  } else {
    return ie->next->type_bind(ie->next, signature, has_deps);
  }
}

static int do_sample_register(struct labcomm2014_encoder *e, 
                         const struct labcomm2014_signature *signature, 
                         labcomm2014_encoder_function encode)
{
  const struct renamed *renamed;
  struct encoder *ie = e->context;

  renamed = set_renamed(e, signature);  
  return ie->next->sample_register(ie->next, &renamed->signature, encode);
}

static int do_ref_register(struct labcomm2014_encoder *e, 
                      const struct labcomm2014_signature *signature)
{
  const struct renamed *renamed;
  struct encoder *ie = e->context;

  renamed = set_renamed(e, signature);  
  return ie->next->ref_register(ie->next, &renamed->signature);
}

static int do_encode(struct labcomm2014_encoder *e, 
                     const struct labcomm2014_signature *signature, 
                     labcomm2014_encoder_function encode,
                     void *value)
{
  const struct renamed *renamed;
  struct encoder *ie = e->context;

  renamed = get_renamed(e, signature);
  if (renamed == NULL) {
    return -EINVAL;
  } else {
    return ie->next->encode(ie->next, &renamed->signature, encode, value);
  }
}

static int do_ioctl(struct labcomm2014_encoder *e, 
                    const struct labcomm2014_signature *signature,
                    uint32_t ioctl_action, va_list args)
{
  const struct renamed *renamed;
  struct encoder *ie = e->context;

  renamed = get_renamed(e, signature);
  if (renamed != NULL) {
    signature = &renamed->signature;
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
  const struct renamed *renamed;
  struct encoder *ie = e->context;

  renamed = get_renamed(e, signature);
  if (renamed == NULL) {
    return ie->next->ref_get(ie->next, signature);
  } else {
    return ie->next->ref_get(ie->next, &renamed->signature);
  }
}

static void do_free(struct labcomm2014_encoder *e)
{
  struct encoder *ie = e->context;
  int i;

  LABCOMM_SIGNATURE_ARRAY_FOREACH(ie->renamed, struct renamed *, i) {
    struct renamed *r;
    r = LABCOMM_SIGNATURE_ARRAY_GET(ie->renamed, struct renamed *, i, NULL);
    if (r) {
      labcomm2014_memory_free(e->memory, 0, r->signature.name);
      labcomm2014_memory_free(e->memory, 0, r);
    }
  }
  LABCOMM_SIGNATURE_ARRAY_FREE(e->memory, ie->renamed, struct renamed *);
  labcomm2014_memory_free(e->memory, 0, ie);
}

struct labcomm2014_encoder *labcomm2014_renaming_encoder_new(
  struct labcomm2014_encoder *e,
  char *(*rename)(struct labcomm2014_memory *m, char *name, void *context),
  void *context)
{
  struct encoder *result;
  result = labcomm2014_memory_alloc(e->memory, 0, sizeof(*result));
  if (!result) {
    return NULL;
  } else {
      result->encoder.context = result;
      result->encoder.writer = NULL;
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
      result->rename = rename;
      result->context = context;
      LABCOMM_SIGNATURE_ARRAY_INIT(result->renamed,
                                   struct renamed *);
      return &(result->encoder);
  }
}
