/*
  labcomm_encoder.c -- handling encoding of labcomm samples.

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
#define CURRENT_VERSION "LabComm2014"

#include <errno.h>
#include "labcomm.h"
#include "labcomm_private.h"
#include "labcomm_ioctl.h"

struct labcomm_encoder {
  struct labcomm_writer *writer;
  struct labcomm_error_handler *error;
  struct labcomm_memory *memory;
  struct labcomm_scheduler *scheduler;
  LABCOMM_SIGNATURE_ARRAY_DEF(registered, int);
  LABCOMM_SIGNATURE_ARRAY_DEF(sample_ref, int);
};

struct labcomm_encoder *labcomm_encoder_new(
  struct labcomm_writer *writer,
  struct labcomm_error_handler *error,
  struct labcomm_memory *memory,
  struct labcomm_scheduler *scheduler)
{
  struct labcomm_encoder *result;

  result = labcomm_memory_alloc(memory, 0, sizeof(*result));
  if (result) {
    int length;

    result->writer = writer;
    result->writer->encoder = result;
    result->writer->data = NULL;
    result->writer->data_size = 0;
    result->writer->count = 0;
    result->writer->pos = 0;
    result->writer->error = 0;
    result->error = error;
    result->memory = memory;
    result->scheduler = scheduler;
    LABCOMM_SIGNATURE_ARRAY_INIT(result->registered, int);
    LABCOMM_SIGNATURE_ARRAY_INIT(result->sample_ref, int);
    labcomm_writer_alloc(result->writer,
			 result->writer->action_context);
    labcomm_writer_start(result->writer, 
                         result->writer->action_context, 
                         LABCOMM_VERSION, NULL, CURRENT_VERSION);
    labcomm_write_packed32(result->writer, LABCOMM_VERSION);
    length = (labcomm_size_packed32(LABCOMM_VERSION) +
              labcomm_size_string(CURRENT_VERSION));
    labcomm_write_packed32(result->writer, length);
    labcomm_write_string(result->writer, CURRENT_VERSION);
    labcomm_writer_end(result->writer, result->writer->action_context);
  }
  return result;
}

void labcomm_encoder_free(struct labcomm_encoder* e)
{
  struct labcomm_memory *memory = e->memory;

  labcomm_writer_free(e->writer, e->writer->action_context);
  LABCOMM_SIGNATURE_ARRAY_FREE(e->memory, e->registered, int);
  LABCOMM_SIGNATURE_ARRAY_FREE(e->memory, e->sample_ref, int);
  labcomm_memory_free(memory, 0, e);
}

int labcomm_internal_encoder_register(
  struct labcomm_encoder *e,
  const struct labcomm_signature *signature,
  labcomm_encoder_function encode)
{
  int result = -EINVAL;
  int index, *done, err, i, length;

  index = labcomm_get_local_index(signature);
  labcomm_scheduler_writer_lock(e->scheduler);
  if (index <= 0) { goto out; }
  done = LABCOMM_SIGNATURE_ARRAY_REF(e->memory, e->registered, int, index);
  if (*done) { goto out; }
  *done = 1;	
  err = labcomm_writer_start(e->writer, e->writer->action_context, 
			     index, signature, NULL);
  if (err == -EALREADY) { result = 0; goto out; }
  if (err != 0) { result = err; goto out; }
  labcomm_write_packed32(e->writer, LABCOMM_SAMPLE_DEF);
  length = (labcomm_size_packed32(index) +
            labcomm_size_string(signature->name) +
            labcomm_size_packed32(signature->size) +
            signature->size);
  labcomm_write_packed32(e->writer, length);
  labcomm_write_packed32(e->writer, index);
  labcomm_write_string(e->writer, signature->name);
  labcomm_write_packed32(e->writer, signature->size);
  for (i = 0 ; i < signature->size ; i++) {
    if (e->writer->pos >= e->writer->count) {
      labcomm_writer_flush(e->writer, e->writer->action_context);
    }
    e->writer->data[e->writer->pos] = signature->signature[i];
    e->writer->pos++;
  }
  labcomm_writer_end(e->writer, e->writer->action_context);
  result = e->writer->error;
out:
  labcomm_scheduler_writer_unlock(e->scheduler);
  return result;
}

int labcomm_internal_encode(
  struct labcomm_encoder *e,
  const struct labcomm_signature *signature,
  labcomm_encoder_function encode,
  void *value)
{
  int result, index, length;

  index = labcomm_get_local_index(signature);
  length = (signature->encoded_size(value));
  labcomm_scheduler_writer_lock(e->scheduler);
  result = labcomm_writer_start(e->writer, e->writer->action_context, 
				index, signature, value);
  if (result == -EALREADY) { result = 0; goto no_end; }
  if (result != 0) { goto out; }
  result = labcomm_write_packed32(e->writer, index);
  result = labcomm_write_packed32(e->writer, length);
  if (result != 0) { goto out; }
  result = encode(e->writer, value);
out:
  labcomm_writer_end(e->writer, e->writer->action_context);
no_end:
  labcomm_scheduler_writer_unlock(e->scheduler);
  return result;
}

int labcomm_encoder_sample_ref_register(
  struct labcomm_encoder *e,
  const struct labcomm_signature *signature)
{
  int result = -EINVAL;
  int index, *done, err, i, length;

  index = labcomm_get_local_index(signature);
  labcomm_scheduler_writer_lock(e->scheduler);
  if (index <= 0) { goto out; }

  done = LABCOMM_SIGNATURE_ARRAY_REF(e->memory, e->sample_ref, int, index);
  if (*done) { goto out; }
  *done = 1;	
  err = labcomm_writer_start(e->writer, e->writer->action_context, 
			     index, signature, NULL);
  if (err == -EALREADY) { result = 0; goto out; }
  if (err != 0) { result = err; goto out; }
  labcomm_write_packed32(e->writer, LABCOMM_SAMPLE_REF);
  length = (labcomm_size_packed32(index) +
            labcomm_size_string(signature->name) +
            labcomm_size_packed32(signature->size) +
            signature->size);
  labcomm_write_packed32(e->writer, length);
  labcomm_write_packed32(e->writer, index);
  labcomm_write_string(e->writer, signature->name);
  labcomm_write_packed32(e->writer, signature->size);
  for (i = 0 ; i < signature->size ; i++) {
    if (e->writer->pos >= e->writer->count) {
      labcomm_writer_flush(e->writer, e->writer->action_context);
    }
    e->writer->data[e->writer->pos] = signature->signature[i];
    e->writer->pos++;
  }
  labcomm_writer_end(e->writer, e->writer->action_context);
  result = e->writer->error;
out:
  labcomm_scheduler_writer_unlock(e->scheduler);
  return result;
}

int labcomm_encoder_ioctl(struct labcomm_encoder *encoder, 
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
  result = labcomm_writer_ioctl(encoder->writer, 
			       encoder->writer->action_context,
			       0, NULL, action, va);
  va_end(va);

out:
  return result;
}

int labcomm_internal_encoder_ioctl(struct labcomm_encoder *encoder, 
				   const struct labcomm_signature *signature,
				   uint32_t action, va_list va)
{
  int result = -ENOTSUP;
  int index;

  index = labcomm_get_local_index(signature);
  result = labcomm_writer_ioctl(encoder->writer, 
				encoder->writer->action_context, 
				index, signature, action, va);
  return result;
}

int labcomm_internal_encoder_signature_to_index(
  struct labcomm_encoder *e, const struct labcomm_signature *signature)
{
  /* writer_lock should be held at this point */
  int index = labcomm_get_local_index(signature);
  if (! LABCOMM_SIGNATURE_ARRAY_GET(e->sample_ref, int, index, 0)) {
    index = 0;
  }
  return index;
}

