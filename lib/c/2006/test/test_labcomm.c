/*
  test_labcomm.c -- Various labcomm tests

  Copyright 2013 Anders Blomdell <anders.blomdell@control.lth.se>

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

#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "labcomm2006_private.h"
#include "labcomm2006_default_error_handler.h"
#include "labcomm2006_default_memory.h"
#include "labcomm2006_default_scheduler.h"
#include "test/gen/test_sample.h"

static unsigned char buffer[512];

static int writer_alloc(struct labcomm2006_writer *w, 
			struct labcomm2006_writer_action_context *action_context)
{
  w->data = buffer;
  w->data_size = sizeof(buffer);
  w->count = sizeof(buffer);
  
  return 0;
}
static int writer_start(struct labcomm2006_writer *w, 
                        struct labcomm2006_writer_action_context *action_context,
                        int index, const struct labcomm2006_signature *signature,
                        void *value)
{
  return 0;
}
const struct labcomm2006_writer_action writer_action = {
  .alloc = writer_alloc,
  .start = writer_start,
};
static struct labcomm2006_writer_action_context writer_action_context = {
  .next = NULL,
  .action = &writer_action,
  .context = NULL
}; 
static struct labcomm2006_writer writer =  {
  .action_context = &writer_action_context,
  .data = buffer,
  .data_size = sizeof(buffer),
  .count = sizeof(buffer),
  .pos = 0,
  .error = 0,
};

static int reader_alloc(struct labcomm2006_reader *r, 
			struct labcomm2006_reader_action_context *action_context)
{
  r->data = buffer;
  r->data_size = sizeof(buffer);
  r->count = 0;
  r->memory = labcomm2006_default_memory;
  
  return 0;
}
static int reader_fill(struct labcomm2006_reader *r, 
		       struct labcomm2006_reader_action_context *action_context)
{
  r->error = -ENOMEM;
  return r->error;
}
const struct labcomm2006_reader_action reader_action = {
  .alloc = reader_alloc,
  .fill = reader_fill,
};
static struct labcomm2006_reader_action_context reader_action_context = {
  .next = NULL,
  .action = &reader_action,
  .context = NULL
}; 
static struct labcomm2006_reader reader =  {
  .action_context = &reader_action_context,
  .data = buffer,
  .data_size = sizeof(buffer),
  .count = 0,
  .pos = 0,
  .error = 0,
};

static int32_t encoder_data[256];
static test_sample_test_var encoder_var = {
  .n_0 = 1,
  .n_2 = 1,
  .a = encoder_data,
};
static int32_t decoder_data[256];
static test_sample_test_var decoder_var = {
  .n_0 = 1,
  .n_2 = 1,
  .a = decoder_data,
};;

void handle_test_var(test_sample_test_var *v, void *ctx)
{
  decoder_var.a[0] = v->a[0];  
}

int test_decode_one(struct labcomm2006_decoder *decoder)
{
  int result;

  for (reader.count = 0 ; reader.count < writer.pos ; reader.count++) {
    reader.error = 0;
    reader.pos = 0;
    result = labcomm2006_decoder_decode_one(decoder); 
    if (result >= 0 ) {
      fprintf(stderr, "Got result from buffer with bogus length (%d)\n",
	      result);
      exit(1);
    }
  }
  reader.error = 0;
  reader.pos = 0;
  reader.count = writer.pos;
  result = labcomm2006_decoder_decode_one(decoder);
  if (result < 0) {
    int i;

    fprintf(stderr, "Got result from buffer with correct length (%d, %d)\n",
	    result, writer.pos);
    for (i = 0 ; i < writer.pos ; i++) {
      fprintf(stderr, "%02x ", buffer[i]);
    }
    exit(1);
  }
  return result;
}

static void test_encode_decode(struct labcomm2006_encoder *encoder,
			       struct labcomm2006_decoder *decoder,
			       int expected, uint32_t n_0, uint32_t n_2)
{
  int err;

  writer.pos = 0;
  encoder_var.n_0 = n_0;
  encoder_var.n_2 = n_2;
  encoder_var.a[0] = 314;
  labcomm2006_encode_test_sample_test_var(encoder, &encoder_var);
  err = test_decode_one(decoder);
  fprintf(stderr, "decode of sample %u * 2 * %u -> size=%d err=%d\n", 
	  n_0, n_2, writer.pos, err);
  if (writer.pos != labcomm2006_sizeof_test_sample_test_var(&encoder_var)) {
    fprintf(stderr, "Incorrect sizeof %u * 2 * %u (%d != %d)\n",
	    n_0, n_2, 
	    writer.pos, labcomm2006_sizeof_test_sample_test_var(&encoder_var));
    exit(1);
  }
  if (writer.pos != expected) {
    fprintf(stderr, "Unexpected size %u * %u (%d != %d)\n",
	    n_0, n_2, 
	    writer.pos, expected);
    exit(1);
  }
}

int main(void)
{
  int err, i;
  struct labcomm2006_encoder *encoder = labcomm2006_encoder_new(
    &writer, 
    labcomm2006_default_error_handler,
    labcomm2006_default_memory,
    labcomm2006_default_scheduler);
  struct labcomm2006_decoder *decoder = labcomm2006_decoder_new(
    &reader,
    labcomm2006_default_error_handler,
    labcomm2006_default_memory,
    labcomm2006_default_scheduler);
  labcomm2006_decoder_register_test_sample_test_var(decoder,
						handle_test_var, 
						NULL);
  labcomm2006_encoder_register_test_sample_test_var(encoder);
  err = test_decode_one(decoder);
  fprintf(stderr, "decode of register -> index %d\n", err);
  test_encode_decode(encoder, decoder, 20, 1, 1);
  if (decoder_var.a[0] != encoder_var.a[0]) {
    fprintf(stderr, "Failed to decode correct value %d != %d\n", 
	    encoder_var.a[0], decoder_var.a[0]);
    exit(1);
  }
  test_encode_decode(encoder, decoder, 44, 2, 2);
  test_encode_decode(encoder, decoder, 12, 0, 0);
  for (i = 1 ; i <= 4 ; i++) {
    test_encode_decode(encoder, decoder, 12, 0, (1<<(7*i))-1);
    test_encode_decode(encoder, decoder, 12, 0, (1<<(7*i)));
  }
  test_encode_decode(encoder, decoder, 12, 0, 4294967295);
  return 0;
}
