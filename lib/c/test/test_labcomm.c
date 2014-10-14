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
#include "labcomm_private.h"
#include "labcomm_default_error_handler.h"
#include "labcomm_default_memory.h"
#include "labcomm_default_scheduler.h"
#include "test/gen/test_sample.h"

static unsigned char buffer[512];

static int writer_alloc(struct labcomm_writer *w, 
			struct labcomm_writer_action_context *action_context, 
			char *labcomm_version)
{
  w->data = buffer;
  w->data_size = sizeof(buffer);
  w->count = sizeof(buffer);
  
  return 0;
}
static int writer_start(struct labcomm_writer *w, 
			 struct labcomm_writer_action_context *action_context,
			 int index, struct labcomm_signature *signature,
			 void *value)
{
  return 0;
}
const struct labcomm_writer_action writer_action = {
  .alloc = writer_alloc,
  .start = writer_start,
};
static struct labcomm_writer_action_context writer_action_context = {
  .next = NULL,
  .action = &writer_action,
  .context = NULL
}; 
static struct labcomm_writer writer =  {
  .action_context = &writer_action_context,
  .data = buffer,
  .data_size = sizeof(buffer),
  .count = sizeof(buffer),
  .pos = 0,
  .error = 0,
};

static int reader_alloc(struct labcomm_reader *r, 
			struct labcomm_reader_action_context *action_context, 
			char *labcomm_version)
{
  r->data = buffer;
  r->data_size = sizeof(buffer);
  r->count = 0;
  r->memory = labcomm_default_memory;
  
  return 0;
}
static int reader_fill(struct labcomm_reader *r, 
		       struct labcomm_reader_action_context *action_context)
{
  r->error = -ENOMEM;
  return r->error;
}
const struct labcomm_reader_action reader_action = {
  .alloc = reader_alloc,
  .fill = reader_fill,
};
static struct labcomm_reader_action_context reader_action_context = {
  .next = NULL,
  .action = &reader_action,
  .context = NULL
}; 
static struct labcomm_reader reader =  {
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
  .n_1 = 1,
  .a = encoder_data,
};
static int32_t decoder_data[256];
static test_sample_test_var decoder_var = {
  .n_0 = 1,
  .n_1 = 1,
  .a = decoder_data,
};;

void handle_test_var(test_sample_test_var *v, void *ctx)
{
  decoder_var.a[0] = v->a[0];  
}

int test_decode_one(struct labcomm_decoder *decoder)
{
  int result;

  for (reader.count = 0 ; reader.count < writer.pos ; reader.count++) {
    reader.error = 0;
    reader.pos = 0;
    result = labcomm_decoder_decode_one(decoder);
    if (result >= 0) {
      fprintf(stderr, "Got result from buffer with bogus length (%d)\n",
	      result);
      exit(1);
    }
  }
  reader.error = 0;
  reader.pos = 0;
  reader.count = writer.pos;
  result = labcomm_decoder_decode_one(decoder);
  if (result < 0) {
    fprintf(stderr, "Got result from buffer with correct length (%d)\n",
	    result);
    exit(1);
  }
  return result;
}

static void test_encode_decode(struct labcomm_encoder *encoder,
			       struct labcomm_decoder *decoder,
			       int expected, uint32_t n_0, uint32_t n_1)
{
  int err;

  writer.pos = 0;
  encoder_var.n_0 = n_0;
  encoder_var.n_1 = n_1;
  encoder_var.a[0] = 314;
  labcomm_encode_test_sample_test_var(encoder, &encoder_var);
  err = test_decode_one(decoder);
  fprintf(stderr, "decode of sample %u * %u -> size=%d err=%d\n", 
	  n_0, n_1, writer.pos, err);
  if (writer.pos != labcomm_sizeof_test_sample_test_var(&encoder_var)) {
    fprintf(stderr, "Incorrect sizeof %u * %u (%d != %d)\n",
	    n_0, n_1, 
	    writer.pos, labcomm_sizeof_test_sample_test_var(&encoder_var));
    exit(1);
  }
  if (writer.pos != expected) {
    fprintf(stderr, "Unexpected size %u * %u (%d != %d)\n",
	    n_0, n_1, 
	    writer.pos, expected);
    exit(1);
  }
}

int main(void)
{
  int err, i;
  struct labcomm_encoder *encoder = labcomm_encoder_new(
    &writer, 
    labcomm_default_error_handler,
    labcomm_default_memory,
    labcomm_default_scheduler);
  struct labcomm_decoder *decoder = labcomm_decoder_new(
    &reader,
    labcomm_default_error_handler,
    labcomm_default_memory,
    labcomm_default_scheduler);
  labcomm_decoder_register_test_sample_test_var(decoder,
						handle_test_var, 
						NULL);
  labcomm_encoder_register_test_sample_test_var(encoder);
  err = test_decode_one(decoder);
  fprintf(stderr, "decode of register -> index %d\n", err);
  test_encode_decode(encoder, decoder, 8, 1, 1);
  if (decoder_var.a[0] != encoder_var.a[0]) {
    fprintf(stderr, "Failed to decode correct value %d != %d\n", 
	    encoder_var.a[0], decoder_var.a[0]);
    exit(1);
  }
  test_encode_decode(encoder, decoder, 20, 2, 2);
  test_encode_decode(encoder, decoder, 4, 0, 0);
  for (i = 1 ; i <= 4 ; i++) {
    test_encode_decode(encoder, decoder, 3+i, 0, (1<<(7*i))-1);
    test_encode_decode(encoder, decoder, 4+i, 0, (1<<(7*i)));
  }
  test_encode_decode(encoder, decoder, 8, 0, 4294967295);
  return 0;
}

#if 0
#include <stdbool.h>
#include <stdlib.h>

#include <labcomm.h>
#include <labcomm_mem_writer.h>
#include <labcomm_mem_reader.h>
#include "test/testdata/gen/test_sample.h"

#define TEST_BUFFER_SIZE (50)

void test_error_handler(enum labcomm_error error_id, size_t nbr_va_args, ...);

int init_suit_labcomm()
{
	return 0;
}

int clean_suit_labcomm()
{
	return 0;
}

void setup_connected_encoder_decoder(struct labcomm_encoder **enc,
		labcomm_mem_writer_context_t *enc_ctx,
		struct labcomm_decoder **dec,
		labcomm_mem_reader_context_t *dec_ctx)
{
	enc_ctx->write_pos = 0;
	enc_ctx->buf = malloc(TEST_BUFFER_SIZE);
	enc_ctx->length = TEST_BUFFER_SIZE;

	*enc = labcomm_encoder_new(labcomm_mem_writer, enc_ctx);

	dec_ctx->size = 0;
	dec_ctx->enc_data = enc_ctx->buf;
	*dec = labcomm_decoder_new(labcomm_mem_reader, dec_ctx);

	labcomm_register_error_handler_decoder(*dec, test_error_handler);
	labcomm_register_error_handler_encoder(*enc, test_error_handler);
}

static bool in_error = false;
static enum labcomm_error in_error_id = LABCOMM_ERROR_ENUM_BEGIN_GUARD;
void test_error_handler(enum labcomm_error error_id, size_t nbr_va_args, ...)
{
	in_error = true;
	in_error_id = error_id;
}

static bool got_sample = false;
void test_decoder_handle_test_sample_test_var(test_sample_test_var *v, void *ctx)
{
	got_sample = true;
}

void test_decoder_decode_sig()
{
	labcomm_mem_writer_context_t enc_ctx;
	struct labcomm_encoder *encoder;
	labcomm_mem_reader_context_t dec_ctx;
	struct labcomm_decoder *decoder;
	setup_connected_encoder_decoder(&encoder, &enc_ctx, &decoder, &dec_ctx);

	labcomm_encoder_register_test_sample_test_var(encoder);
	dec_ctx.size = enc_ctx.write_pos;

	labcomm_decoder_register_test_sample_test_var(decoder,
			test_decoder_handle_test_sample_test_var, NULL);
	labcomm_decoder_decode_one(decoder);

	CU_ASSERT_FALSE(in_error);
	enc_ctx.write_pos = 0;
	test_sample_test_var var = 1;
	labcomm_encode_test_sample_test_var(encoder, &var);
	dec_ctx.size = enc_ctx.write_pos;
	labcomm_decoder_decode_one(decoder);

	CU_ASSERT_FALSE(in_error);
	CU_ASSERT_FALSE(got_sample);

	labcomm_decoder_free(decoder);
	labcomm_encoder_free(encoder);
	free(enc_ctx.buf);

	in_error = false;
	in_error_id = LABCOMM_ERROR_ENUM_BEGIN_GUARD;
	got_sample = false;
}

static bool got_new_datatype = false;
static labcomm_signature_t new_sig;
int test_new_datatype(struct labcomm_decoder *decoder,
		labcomm_signature_t *sig)
{
	got_new_datatype = true;
	memcpy(&new_sig, sig, sizeof(labcomm_signature_t));
	return 0;
}

void test_decode_unreg_signature_handle()
{
	labcomm_mem_writer_context_t enc_ctx;
	struct labcomm_encoder *encoder;
	labcomm_mem_reader_context_t dec_ctx;
	struct labcomm_decoder *decoder;
	setup_connected_encoder_decoder(&encoder, &enc_ctx, &decoder, &dec_ctx);

	labcomm_encoder_register_test_sample_test_var(encoder);
	dec_ctx.size = enc_ctx.write_pos;
	labcomm_decoder_register_new_datatype_handler(decoder, test_new_datatype);
	labcomm_decoder_decode_one(decoder);

	CU_ASSERT_TRUE(got_new_datatype);
	CU_ASSERT_EQUAL(
			memcmp(new_sig.signature, dec_ctx.enc_data, dec_ctx.size), 0);

	got_new_datatype = false;
	labcomm_decoder_free(decoder);
	labcomm_encoder_free(encoder);
	free(enc_ctx.buf);
}

void test_decode_unreg_signature_error()
{
	labcomm_mem_writer_context_t enc_ctx;
	struct labcomm_encoder *encoder;
	labcomm_mem_reader_context_t dec_ctx;
	struct labcomm_decoder *decoder;
	setup_connected_encoder_decoder(&encoder, &enc_ctx, &decoder, &dec_ctx);

	labcomm_encoder_register_test_sample_test_var(encoder);
	dec_ctx.size = enc_ctx.write_pos;

	labcomm_decoder_decode_one(decoder);

	CU_ASSERT_TRUE(in_error);
	CU_ASSERT_EQUAL(in_error_id, LABCOMM_ERROR_DEC_UNKNOWN_DATATYPE);
	got_new_datatype = false;
	labcomm_decoder_free(decoder);
	labcomm_encoder_free(encoder);
	free(enc_ctx.buf);
}
int main()
{
	CU_pSuite suite_decoder = NULL;

	// Initialize CUnit test registry.
	if (CUE_SUCCESS != CU_initialize_registry()) {
		return CU_get_error();
	}

	// Add our test suites.
	suite_decoder = CU_add_suite("transport_enc_dec",
			init_suit_labcomm, clean_suit_labcomm);
	if (suite_decoder == NULL) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	if (
			(CU_add_test(suite_decoder, "test_decoder_decode_sig",
					test_decoder_decode_sig) == NULL)
			||
			(CU_add_test(suite_decoder, "test_decode_unreg_signature_handle",
					test_decode_unreg_signature_handle) == NULL)
			||
			(CU_add_test(suite_decoder, "test_decode_unreg_signature_error",
					test_decode_unreg_signature_error) == NULL)
		) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	// Set verbosity.
	CU_basic_set_mode(CU_BRM_VERBOSE);
	/*CU_console_run_tests();*/

	// Run all test suites.
	CU_basic_run_tests();

	// Clean up.
	CU_cleanup_registry();

	return CU_get_error();
}
#endif
