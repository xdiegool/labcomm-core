/*
  labcomm2014_encoder.c -- handling encoding of labcomm2014 samples.

  Copyright 2006-2013 Anders Blomdell <anders.blomdell@control.lth.se>

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
#define CURRENT_VERSION "LabComm2014.v1"

#include <errno.h>
#include "labcomm2014.h"
#include "labcomm2014_private.h"
#include "labcomm2014_ioctl.h"
#include "labcomm2014_dynamic_buffer_writer.h"

//define the following to disable encoding of typedefs
#undef LABCOMM_WITHOUT_TYPE_DEFS

struct encoder {
  struct labcomm2014_encoder encoder;
  LABCOMM_SIGNATURE_ARRAY_DEF(registered, int);
  LABCOMM_SIGNATURE_ARRAY_DEF(sample_ref, int);
  LABCOMM_SIGNATURE_ARRAY_DEF(typedefs, int);
};

static int do_sample_register(
  struct labcomm2014_encoder *e,
  const struct labcomm2014_signature *signature,
  labcomm2014_encoder_function encode)
{
  int result = -EINVAL;
  struct encoder *ie = e->context;
  int index, *done, err, i, length;

  index = labcomm2014_get_local_index(signature);
  labcomm2014_scheduler_writer_lock(e->scheduler);
  if (index <= 0) { goto out; }
  done = LABCOMM_SIGNATURE_ARRAY_REF(e->memory, ie->registered, int, index);
  if (*done) {
      goto out; }
  *done = 1;
  err = labcomm2014_writer_start(e->writer, e->writer->action_context,
			     index, signature, NULL);
  if (err == -EALREADY) { result = 0; goto out; }
  if (err != 0) { result = err; goto out; }
  labcomm2014_write_packed32(e->writer, LABCOMM_SAMPLE_DEF);
  length = (labcomm2014_size_packed32(index) +
            labcomm2014_size_string(signature->name) +
            labcomm2014_size_packed32(signature->size) +
            signature->size);
  labcomm2014_write_packed32(e->writer, length);
  labcomm2014_write_packed32(e->writer, index);
  labcomm2014_write_string(e->writer, signature->name);
  labcomm2014_write_packed32(e->writer, signature->size);
  for (i = 0 ; i < signature->size ; i++) {
    if (e->writer->pos >= e->writer->count) {
      labcomm2014_writer_flush(e->writer, e->writer->action_context);
    }
    e->writer->data[e->writer->pos] = signature->signature[i];
    e->writer->pos++;
  }
  labcomm2014_writer_end(e->writer, e->writer->action_context);
  result = e->writer->error;
out:
  labcomm2014_scheduler_writer_unlock(e->scheduler);
  return result;
}

static int do_encode(
  struct labcomm2014_encoder *e,
  const struct labcomm2014_signature *signature,
  labcomm2014_encoder_function encode,
  void *value)
{
  int result, index, length;
  struct encoder *ie = e->context;
  
  index = labcomm2014_get_local_index(signature);
  length = (signature->encoded_size(value));
  labcomm2014_scheduler_writer_lock(e->scheduler);
  if (! LABCOMM_SIGNATURE_ARRAY_GET(ie->registered, int, index, 0)) {
    result = -EINVAL;
    goto no_end;
  }
  result = labcomm2014_writer_start(e->writer, e->writer->action_context,
                                    index, signature, value);
  if (result == -EALREADY) { result = 0; goto no_end; }
  if (result != 0) { goto out; }
  result = labcomm2014_write_packed32(e->writer, index);
  result = labcomm2014_write_packed32(e->writer, length);
  if (result != 0) { goto out; }
  result = encode(e->writer, value);
out:
  labcomm2014_writer_end(e->writer, e->writer->action_context);
no_end:
  labcomm2014_scheduler_writer_unlock(e->scheduler);
  return result;
}

int labcomm2014_encoder_sample_ref_register(
  struct labcomm2014_encoder *e,
  const struct labcomm2014_signature *signature)
{
  return e->ref_register(e, signature);
}

static int do_ref_register(
  struct labcomm2014_encoder *e,
  const struct labcomm2014_signature *signature)
{
  int result = -EINVAL;
  struct encoder *ie = e->context;
  int index, *done, err, i, length;

  index = labcomm2014_get_local_index(signature);
  labcomm2014_scheduler_writer_lock(e->scheduler);
  if (index <= 0) { goto out; }

  done = LABCOMM_SIGNATURE_ARRAY_REF(e->memory, ie->sample_ref, int, index);
  if (*done) { goto out; }
  *done = 1;
  err = labcomm2014_writer_start(e->writer, e->writer->action_context,
			     index, signature, NULL);
  if (err == -EALREADY) { result = 0; goto out; }
  if (err != 0) { result = err; goto out; }
  labcomm2014_write_packed32(e->writer, LABCOMM_SAMPLE_REF);
  length = (labcomm2014_size_packed32(index) +
            labcomm2014_size_string(signature->name) +
            labcomm2014_size_packed32(signature->size) +
            signature->size);
  labcomm2014_write_packed32(e->writer, length);
  labcomm2014_write_packed32(e->writer, index);
  labcomm2014_write_string(e->writer, signature->name);
  labcomm2014_write_packed32(e->writer, signature->size);
  for (i = 0 ; i < signature->size ; i++) {
    if (e->writer->pos >= e->writer->count) {
      labcomm2014_writer_flush(e->writer, e->writer->action_context);
    }
    e->writer->data[e->writer->pos] = signature->signature[i];
    e->writer->pos++;
  }
  labcomm2014_writer_end(e->writer, e->writer->action_context);
  result = e->writer->error;
out:
  labcomm2014_scheduler_writer_unlock(e->scheduler);
  return result;
}

int labcomm2014_encoder_ioctl(struct labcomm2014_encoder *encoder,
                              uint32_t action,
                              ...)
{
  int result;
  va_list va;

  if (LABCOMM_IOC_SIG(action) != LABCOMM_IOC_NOSIG) {
    result = -EINVAL;
    goto out;
  }

  va_start(va, action);
  result = encoder->ioctl(encoder, NULL, action, va);
  va_end(va);

out:
  return result;
}

static int do_ioctl(
  struct labcomm2014_encoder *encoder,
  const struct labcomm2014_signature *signature,
  uint32_t action, va_list va)
{
  int result = -ENOTSUP;
  int index;

  index = labcomm2014_get_local_index(signature);
  result = labcomm2014_writer_ioctl(encoder->writer,
				encoder->writer->action_context,
				index, signature, action, va);
  return result;
}

static int do_sample_ref_to_index(
  struct labcomm2014_encoder *e,
  const struct labcomm2014_sample_ref *sample_ref)
{
  /* writer_lock should be held at this point */
  struct encoder *ie = e->context;
  int index = 0;
  if (sample_ref != NULL) {
    index = labcomm2014_get_local_index(
      (struct labcomm2014_signature *)sample_ref);
    if (! LABCOMM_SIGNATURE_ARRAY_GET(ie->sample_ref, int, index, 0)) {
      index = 0;
    }
  }
  return index;
}


/**********************************************************
 * Start of code related to sending (hierarchical)
 * typedefs. Define LABCOMM_WITHOUT_TYPEDEFS to disable
 **********************************************************/
#ifndef LABCOMM_WITHOUT_TYPE_DEFS

static void write_sig_tree_byte(char b, const struct labcomm2014_signature *signature,
                           void *context)
{
  struct labcomm2014_encoder *e = context;
  if(signature) {
    labcomm2014_write_packed32(e->writer, labcomm2014_get_local_index(signature));
  }else {
    if (e->writer->pos >= e->writer->count) {
     labcomm2014_writer_flush(e->writer, e->writer->action_context);
    }
    e->writer->data[e->writer->pos] = b;
    e->writer->pos++;
  }
}

static void do_write_signature(struct labcomm2014_encoder * e, 
                               const struct labcomm2014_signature *signature, 
                               unsigned char flatten)
{
  map_signature(write_sig_tree_byte, e, signature, flatten);
}

static void sig_size(char b, const struct labcomm2014_signature *signature,
                     void *context)
{
  int *result = context;
  int diff;
  if(signature) {
    int idx = labcomm2014_get_local_index(signature);
    diff = labcomm2014_size_packed32(idx);
  }else {
    diff = 1;
  }
    (*result)+=diff;
}

static int calc_sig_encoded_size(struct labcomm2014_encoder *e,
                                 const struct labcomm2014_signature *sig)
{
  int result=0;
  map_signature(sig_size, &result, sig, LABCOMM2014_FALSE);
  return result;
}

static int internal_reg_type(
  struct labcomm2014_encoder *e,
  const struct labcomm2014_signature *signature,
  labcomm2014_bool flatten)
{
  int result = -EINVAL;
  int index, *done, err;
  struct encoder *ie = e->context;
  
  index = labcomm2014_get_local_index(signature);
  labcomm2014_scheduler_writer_lock(e->scheduler);
  if (index <= 0) { goto out; }
  done = LABCOMM_SIGNATURE_ARRAY_REF(e->memory, ie->typedefs, int, index);
  if (*done) { goto out; }
  *done = 1;
  err = labcomm2014_writer_start(e->writer, e->writer->action_context,
                                 index, signature, NULL);
  if (err == -EALREADY) { result = 0; goto out; }
  if (err != 0) { result = err; goto out; }

  int sig_size = calc_sig_encoded_size(e, signature);
  int len =  labcomm2014_size_packed32(index) +
             labcomm2014_size_string(signature->name) +
             labcomm2014_size_packed32(sig_size) +
              sig_size;

  labcomm2014_write_packed32(e->writer, LABCOMM_TYPE_DEF);
  labcomm2014_write_packed32(e->writer, len);
  labcomm2014_write_packed32(e->writer, index);
  labcomm2014_write_string(e->writer, signature->name);
  labcomm2014_write_packed32(e->writer, sig_size);
  do_write_signature(e, signature, LABCOMM2014_FALSE);

  labcomm2014_writer_end(e->writer, e->writer->action_context);
  result = e->writer->error;
out:
  labcomm2014_scheduler_writer_unlock(e->scheduler);
  return result;
}
#endif

static int do_type_register(
  struct labcomm2014_encoder *e,
  const struct labcomm2014_signature *signature)
{
#ifndef LABCOMM_WITHOUT_TYPE_DEFS
  return internal_reg_type(e, signature, LABCOMM2014_FALSE);
#else
  return 0;
#endif
}

static int do_type_bind(
  struct labcomm2014_encoder *e,
  const struct labcomm2014_signature *signature,
  char has_deps)
{
#ifndef LABCOMM_WITHOUT_TYPE_DEFS
  int result = -EINVAL;
  int err;
  int sindex = labcomm2014_get_local_index(signature);
  int tindex = has_deps ? labcomm2014_get_local_type_index(signature) : LABCOMM_BIND_SELF;
  labcomm2014_scheduler_writer_lock(e->scheduler);
  if(sindex <= 0 || (has_deps && tindex <= 0)) {goto out;}
  err = labcomm2014_writer_start(e->writer, e->writer->action_context,
			     LABCOMM_TYPE_BINDING, signature, NULL);
  if (err == -EALREADY) { result = 0; goto out; }
  if (err != 0) { result = err; goto out; }
  int length = (labcomm2014_size_packed32(sindex) +
                labcomm2014_size_packed32(tindex));
  labcomm2014_write_packed32(e->writer, LABCOMM_TYPE_BINDING);
  labcomm2014_write_packed32(e->writer, length);
  labcomm2014_write_packed32(e->writer, sindex);
  labcomm2014_write_packed32(e->writer, tindex);
  labcomm2014_writer_end(e->writer, e->writer->action_context);
  result = e->writer->error;

out:
  labcomm2014_scheduler_writer_unlock(e->scheduler);
  return result;
#else
  return 0;
#endif
}

static const struct labcomm2014_sample_ref *do_ref_get(
  struct labcomm2014_encoder *e,
  const struct labcomm2014_signature *signature)
{
  return (const struct labcomm2014_sample_ref *) signature;
}

void labcomm2014_encoder_free(struct labcomm2014_encoder* e)
{
  e->free(e);
}

static void do_free(struct labcomm2014_encoder* e)
{
  struct encoder *ie = e->context;
  struct labcomm2014_memory *memory = e->memory;

  labcomm2014_writer_free(e->writer, e->writer->action_context);
  LABCOMM_SIGNATURE_ARRAY_FREE(e->memory, ie->registered, int);
  LABCOMM_SIGNATURE_ARRAY_FREE(e->memory, ie->sample_ref, int);
  LABCOMM_SIGNATURE_ARRAY_FREE(e->memory, ie->typedefs, int);
  labcomm2014_memory_free(memory, 0, ie);
}

static struct labcomm2014_encoder *internal_encoder_new(
  struct labcomm2014_writer *writer,
  struct labcomm2014_error_handler *error,
  struct labcomm2014_memory *memory,
  struct labcomm2014_scheduler *scheduler,
  labcomm2014_bool outputVer)
{
  struct encoder *result;

  result = labcomm2014_memory_alloc(memory, 0, sizeof(*result));
  if (result) {
    int length;

    result->encoder.context = result;
    result->encoder.writer = writer;
    result->encoder.writer->encoder = &result->encoder;
    result->encoder.writer->data = NULL;
    result->encoder.writer->data_size = 0;
    result->encoder.writer->count = 0;
    result->encoder.writer->pos = 0;
    result->encoder.writer->error = 0;
    result->encoder.error = error;
    result->encoder.memory = memory;
    result->encoder.scheduler = scheduler;
    result->encoder.free = do_free;
    result->encoder.type_register = do_type_register;
    result->encoder.type_bind = do_type_bind;
    result->encoder.sample_register = do_sample_register;
    result->encoder.ref_register = do_ref_register;
    result->encoder.encode = do_encode;
    result->encoder.ioctl = do_ioctl;
    result->encoder.sample_ref_to_index = do_sample_ref_to_index;
    result->encoder.ref_get = do_ref_get;
    LABCOMM_SIGNATURE_ARRAY_INIT(result->registered, int);
    LABCOMM_SIGNATURE_ARRAY_INIT(result->sample_ref, int);
    LABCOMM_SIGNATURE_ARRAY_INIT(result->typedefs, int);
    labcomm2014_writer_alloc(result->encoder.writer,
                             result->encoder.writer->action_context);
    if(outputVer) {
        labcomm2014_writer_start(result->encoder.writer,
                                 result->encoder.writer->action_context,
                                 LABCOMM_VERSION, NULL, CURRENT_VERSION);
        labcomm2014_write_packed32(result->encoder.writer, LABCOMM_VERSION);
        length = labcomm2014_size_string(CURRENT_VERSION);
        labcomm2014_write_packed32(result->encoder.writer, length);
        labcomm2014_write_string(result->encoder.writer, CURRENT_VERSION);
        labcomm2014_writer_end(result->encoder.writer,
                               result->encoder.writer->action_context);
    }
  }
  return &(result->encoder);
}

struct labcomm2014_encoder *labcomm2014_encoder_new(
  struct labcomm2014_writer *writer,
  struct labcomm2014_error_handler *error,
  struct labcomm2014_memory *memory,
  struct labcomm2014_scheduler *scheduler)
{
    return internal_encoder_new(writer,error,memory,scheduler,LABCOMM2014_TRUE);
}


const struct labcomm2014_sample_ref *labcomm2014_encoder_get_sample_ref(
  struct labcomm2014_encoder *encoder,
  const struct labcomm2014_signature *signature)
{
  return encoder->ref_get(encoder, signature);
}
  
