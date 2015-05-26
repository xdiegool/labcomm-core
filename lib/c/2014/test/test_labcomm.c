/*
  test_labcomm2014.c -- Various labcomm2014 tests

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
#include "labcomm2014_private.h"
#include "labcomm2014_default_error_handler.h"
#include "labcomm2014_default_memory.h"
#include "labcomm2014_default_scheduler.h"
#include "test/gen/test_sample.h"

static struct labcomm2014_writer writer;

static struct expect {
  char *description;
  int result;
  int (*trampoline)(struct labcomm2014_decoder *context);
  struct labcomm2014_decoder *context;
} *expect;

static void check_expect()
{
  struct expect *p = expect;

  {
    int i;
    for (i = 0 ; i < writer.pos ; i++) {
      fprintf(stderr, "%02x ", writer.data[i]);
    }
    fprintf(stderr, "\n");
  }
  if (p && p->trampoline) {
    int err;
    
    expect = p + 1;
    fprintf(stderr, "Checking '%s' expected=%d ", p->description, p->result);
    err = p->trampoline(p->context);
    fprintf(stderr, "actual=%d\n", err);
    if (p->result >= 0 && p->result != err) {
      fprintf(stderr, "FAILED\n");
      exit(1);
    } else if (err == 0) {
      fprintf(stderr, "FAILED (unexpected 0)\n");
      exit(1);
    }
    writer.pos = 0;
  }
}

static unsigned char buffer[512];

static int writer_alloc(struct labcomm2014_writer *w, 
			struct labcomm2014_writer_action_context *action_context)
{
  w->data = buffer;
  w->data_size = sizeof(buffer);
  w->count = sizeof(buffer);
  
  return 0;
}

static int writer_start(struct labcomm2014_writer *w, 
			 struct labcomm2014_writer_action_context *action_context,
			 int index, const struct labcomm2014_signature *signature,
			 void *value)
{
  return 0;
}

static int buf_writer_end(
  struct labcomm2014_writer *w, 
  struct labcomm2014_writer_action_context *action_context)
{
  check_expect();
  return 0;
}

const struct labcomm2014_writer_action writer_action = {
  .alloc = writer_alloc,
  .start = writer_start,
  .end = buf_writer_end,
};

static struct labcomm2014_writer_action_context writer_action_context = {
  .next = NULL,
  .action = &writer_action,
  .context = NULL
}; 

static struct labcomm2014_writer writer =  {
  .action_context = &writer_action_context,
  .data = buffer,
  .data_size = sizeof(buffer),
  .count = sizeof(buffer),
  .pos = 0,
  .error = 0,
};

static int reader_alloc(struct labcomm2014_reader *r, 
			struct labcomm2014_reader_action_context *action_context)
{
  r->data = buffer;
  r->data_size = sizeof(buffer);
  r->count = 0;
  r->memory = labcomm2014_default_memory;
  
  return 0;
}

static int reader_fill(struct labcomm2014_reader *r, 
		       struct labcomm2014_reader_action_context *action_context)
{
  r->error = -ENOMEM;
  return r->error;
}

const struct labcomm2014_reader_action reader_action = {
  .alloc = reader_alloc,
  .fill = reader_fill,
};

static struct labcomm2014_reader_action_context reader_action_context = {
  .next = NULL,
  .action = &reader_action,
  .context = NULL
}; 

static struct labcomm2014_reader reader =  {
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
  if (v->n_0 * v->n_2 > 0) {
    decoder_var.a[0] = v->a[0];
  }
}

int test_decode_one(struct labcomm2014_decoder *decoder)
{
  int result;

  for (reader.count = 0 ; reader.count < writer.pos ; reader.count++) {
    reader.error = 0;
    reader.pos = 0;
    result = labcomm2014_decoder_decode_one(decoder); 
    if (result >= 0 ) {
      fprintf(stderr,
              "Got result from buffer with bogus length (%d, %d != %d)\n",
	      result, reader.count, writer.pos);
      exit(1);
    }
  }
  reader.error = 0;
  reader.pos = 0;
  reader.count = writer.pos;
  result = labcomm2014_decoder_decode_one(decoder);
  if (result < 0) {
    fprintf(stderr, "Got no result from buffer with correct length (%d)\n",
	    result);
    exit(1);
  }
  return result;
}

static void test_encode_decode(struct labcomm2014_encoder *encoder,
			       struct labcomm2014_decoder *decoder,
			       int expected, uint32_t n_0, uint32_t n_2)
{
  int err;

  writer.pos = 0;
  encoder_var.n_0 = n_0;
  encoder_var.n_2 = n_2;
  encoder_var.a[0] = 314;
  labcomm2014_encode_test_sample_test_var(encoder, &encoder_var);
  err = test_decode_one(decoder);
  fprintf(stderr, "decode of sample %u * 2 * %u -> size=%d err=%d\n", 
	  n_0, n_2, writer.pos, err);
  if (writer.pos != labcomm2014_sizeof_test_sample_test_var(&encoder_var)) {
    fprintf(stderr, "Incorrect sizeof %u * 2 * %u (%d != %d)\n",
	    n_0, n_2, 
	    writer.pos, labcomm2014_sizeof_test_sample_test_var(&encoder_var));
    exit(1);
  }
  if (writer.pos != expected) {
    fprintf(stderr, "Unexpected size %u * 2 * %u (%d != %d)\n",
	    n_0, n_2, 
	    writer.pos, expected);
    exit(1);
  }
}

static int do_test(int argc, char *argv[])
{
  int i;
  struct labcomm2014_decoder *decoder = labcomm2014_decoder_new(
    &reader,
    labcomm2014_default_error_handler,
    labcomm2014_default_memory,
    labcomm2014_default_scheduler);
  struct expect expect_version[] = {
    { "Version", 1, test_decode_one, decoder },
    { 0, 0, 0 }
  };
  expect = expect_version;
  struct labcomm2014_encoder *encoder = labcomm2014_encoder_new(
    &writer, 
    labcomm2014_default_error_handler,
    labcomm2014_default_memory,
    labcomm2014_default_scheduler);
  labcomm2014_decoder_register_test_sample_test_var(decoder,
						handle_test_var, 
						NULL);
  struct expect expect_registration[] = {
    { "Sampledef", -1, test_decode_one, decoder },
#ifdef SHOULD_THIS_BE_REMOVED
    { "Typedef", -1, test_decode_one, decoder },
#endif
    { "Binding", -1, test_decode_one, decoder },
    { 0, 0, 0 }
  };
  expect = expect_registration;
  labcomm2014_encoder_register_test_sample_test_var(encoder);
  test_encode_decode(encoder, decoder, 12, 1, 1);
  if (decoder_var.a[0] != encoder_var.a[0]) {
    fprintf(stderr, "Failed to decode correct value %d != %d\n", 
	    encoder_var.a[0], decoder_var.a[0]);
    exit(1);
  }
  test_encode_decode(encoder, decoder, 36, 2, 2);
  test_encode_decode(encoder, decoder, 4, 0, 0);
  for (i = 1 ; i <= 4 ; i++) {
    test_encode_decode(encoder, decoder, 3+i, 0, (1<<(7*i))-1);
    test_encode_decode(encoder, decoder, 4+i, 0, (1<<(7*i)));
  }
  test_encode_decode(encoder, decoder, 8, 0, 4294967295);

  labcomm2014_encoder_free(encoder);
  labcomm2014_decoder_free(decoder);
  return 0;
}

int main(int argc, char *argv[])
{
  return do_test(argc, argv);
}
