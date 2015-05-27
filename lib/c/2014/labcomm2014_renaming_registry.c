/*
  labcomm2014_renaming_registry.c -- renaming registry

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

#include "labcomm2014.h"
#include "labcomm2014_private.h"

struct alias {
  int usecount;
  struct alias *next;
  char *name;
};

struct registry {
  struct registry *base; /* NULL if this is the base type */
  const struct labcomm2014_signature *signature;
  struct labcomm2014_renaming_rename *rename;
};

struct labcomm2014_renaming_rename {
  struct labcomm2014_renaming_rename *next;
  int use_count;
  struct registry *base;
  struct labcomm2014_signature signature;
  struct labcomm2014_signature_data s_treedata[2];
};

struct labcomm2014_renaming_registry {
  struct labcomm2014_error_handler *error;
  struct labcomm2014_memory *memory;
  struct labcomm2014_scheduler *scheduler;
  LABCOMM_SIGNATURE_ARRAY_DEF(registry, struct registry *);
};

struct labcomm2014_renaming_registry *labcomm2014_renaming_registry_new(
  struct labcomm2014_error_handler *error,
  struct labcomm2014_memory *memory,
  struct labcomm2014_scheduler *scheduler)
{
  struct labcomm2014_renaming_registry *result;
  result = labcomm2014_memory_alloc(memory, 0, sizeof(*result));
  if (! result) {
    return NULL;
  } else {
    result->error = error;
    result->memory = memory;
    result->scheduler = scheduler;
    LABCOMM_SIGNATURE_ARRAY_INIT(result->registry, struct registry *);
    return result;
  }
}

static struct registry *registry_new(
  struct labcomm2014_renaming_registry *r,
  const struct labcomm2014_signature *signature,
  struct registry *base)
{
  /* Called with registry locked */
  struct registry *result = NULL;
  struct registry **registry;
  int index;

  index = labcomm2014_get_local_index(signature);
  if (index <= 0) {
    labcomm2014_error_warning(r->error,
                              LABCOMM2014_ERROR_MEMORY,
                              "Signature has no index: %s\n",
                              signature->name);
    goto out;
  }
  registry = LABCOMM_SIGNATURE_ARRAY_REF(r->memory, r->registry,
                                          struct registry *, index);
  if (registry == NULL) { goto out; }
  if (*registry != NULL) {
    result = *registry;
  } else {
    /* Add previosly unknown sample to registry */
    registry = LABCOMM_SIGNATURE_ARRAY_REF(r->memory, r->registry,
                                          struct registry *, index);
    if (registry == NULL) { goto out; }
    result = labcomm2014_memory_alloc(r->memory, 0, sizeof(*result));
    if (result == NULL) { goto out; }
    result->base = base;
    result->signature = signature;
    result->rename = NULL;
    *registry = result;
  }
out:
  return result;
}

struct labcomm2014_renaming_rename *labcomm2014_renaming_rename_new(
  struct labcomm2014_renaming_registry *r,
  const struct labcomm2014_signature *signature,
  char *(*rename)(struct labcomm2014_memory *m, char *name, void *context),
  void *context)
{
  struct labcomm2014_renaming_rename *result = NULL;

  labcomm2014_scheduler_data_lock(r->scheduler);
  {
    char *new_name = NULL;
    static struct registry *base, *renamed;
    struct labcomm2014_renaming_rename **l;

    base = registry_new(r, signature, NULL);
    if (base == NULL) { goto out; }
    if (base->base) {
      base = base->base;
    }

    /* Find if renamed entry already exists */
    new_name = rename(r->memory, signature->name, context);
    if (new_name == NULL) { goto out; }
    for (l = &base->rename ; *l ; l = &(*l)->next) {
      if (strcmp((*l)->signature.name, new_name) == 0) { break; }
    }
    if ((*l) == NULL) {
      /* Create a new rename entry */
      struct labcomm2014_renaming_rename *entry = NULL;

      entry = labcomm2014_memory_alloc(r->memory, 0, sizeof(*entry));
      if (entry == NULL) { goto out; }
      entry->next = NULL;
      entry->use_count = 0;
      entry->base = base;
      entry->signature.name = new_name;
      entry->signature.encoded_size = signature->encoded_size;
      entry->signature.size = signature->size;
      entry->signature.signature = signature->signature;
      entry->signature.index = 0;
#ifndef LABCOMM_NO_TYPEDECL
      struct labcomm2014_signature_data s_treedata[2] = {
        LABCOMM_SIGDEF_SIGNATURE(*signature),
        LABCOMM_SIGDEF_END
      };
      entry->s_treedata[0] = s_treedata[0];
      entry->s_treedata[1] = s_treedata[1];
      entry->signature.tdsize = sizeof(result->s_treedata);
      entry->signature.treedata = entry->s_treedata;
#endif
      labcomm2014_set_local_index(&entry->signature);
      renamed = registry_new(r, &entry->signature, base);
      if (renamed == NULL) {
        /* Failed to create registry entry */
        labcomm2014_memory_free(r->memory, 0, entry);
        goto out;
      } else {
        (*l) = entry;
      }
    }
    result = *l;
    if (result) {
      result->use_count++;
    }
  out:
    ;
  }
  labcomm2014_scheduler_data_unlock(r->scheduler);
  return result;
}

const struct labcomm2014_signature *labcomm2014_renaming_rename_signature(
  struct labcomm2014_renaming_rename *rename)
{
  return &rename->signature;
}
