
#include "CUnit/Basic.h"
#include "CUnit/Console.h"
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
