#include "test_labcomm_errors.h"

#include <stdlib.h>

#include <labcomm.h>
#include <labcomm_private.h>
#include <labcomm_mem_writer.h>
#include <labcomm_mem_reader.h>

static enum labcomm_error callback_error_id;

int assert_callback(enum labcomm_error expected, const char *name, const char *err_msg)
{
  int success;
  printf("----> %s()\n", name);
  if (callback_error_id == expected) {
    printf("Succeeded.\n");
    success = 1;
  } else {
    printf("Failed! %s\n", err_msg);
    success = 0;
  }
  return success;
}

/* Our callback that just logs which error_id that the library reported. */
void test_callback(enum labcomm_error error_id, size_t nbr_va_args, ...)
{
   va_list arg_pointer;
   va_start(arg_pointer, nbr_va_args);
   va_end(arg_pointer);
   callback_error_id = error_id;
}
 
void reset_callback_erro_id()
{
  callback_error_id = -128;
}

int encoded_size_mock(struct labcomm_signature *signature, void *voidp)
{
  return 0;
}

int test_enc_not_reg_encoder_sign()
{
  reset_callback_erro_id();
  unsigned char *buf = (unsigned char *) "a";
  labcomm_mem_writer_context_t *mcontext = labcomm_mem_writer_context_t_new(0, 1, buf);
  labcomm_encoder_t *encoder = labcomm_encoder_new(labcomm_mem_writer, mcontext);
  labcomm_register_error_handler_encoder(encoder, test_callback);
  
  labcomm_signature_t signature = {
    .type = 0, 
    .name = "test_signature", 
    .encoded_size = encoded_size_mock, 
    .size = 0, 
    .signature = (unsigned char *) "0"};
  encoder->do_encode(encoder, &signature, NULL);

  return assert_callback(LABCOMM_ERROR_ENC_NO_REG_SIGNATURE, __FUNCTION__, "");
}

int test_enc_missing_do_reg()
{
  reset_callback_erro_id();
  unsigned char *buf = (unsigned char *) "a";
  labcomm_mem_writer_context_t *mcontext = labcomm_mem_writer_context_t_new(0, 1, buf);
  labcomm_encoder_t *encoder = labcomm_encoder_new(labcomm_mem_writer, mcontext);
  labcomm_register_error_handler_encoder(encoder, test_callback);

  encoder->do_register = NULL;
  labcomm_internal_encoder_register(encoder, NULL, NULL);

  return assert_callback(LABCOMM_ERROR_ENC_MISSING_DO_REG, __FUNCTION__, "");
}

int test_enc_missing_do_encode()
{
  reset_callback_erro_id();
  unsigned char *buf = (unsigned char *) "a";
  labcomm_mem_writer_context_t *mcontext = labcomm_mem_writer_context_t_new(0, 1, buf);
  labcomm_encoder_t *encoder = labcomm_encoder_new(labcomm_mem_writer, mcontext);
  labcomm_register_error_handler_encoder(encoder, test_callback);

  encoder->do_encode = NULL;
  labcomm_internal_encode(encoder, NULL, NULL);

  return assert_callback(LABCOMM_ERROR_ENC_MISSING_DO_ENCODE, __FUNCTION__, "");
}

int test_enc_buf_full()
{
  reset_callback_erro_id();
  unsigned char *buf = (unsigned char *) "a";
  labcomm_mem_writer_context_t *mcontext = labcomm_mem_writer_context_t_new(0, 1, buf);
  labcomm_encoder_t *encoder = labcomm_encoder_new(labcomm_mem_writer, mcontext);
  labcomm_register_error_handler_encoder(encoder, test_callback);

  unsigned char *mbuf = mcontext->buf;
  labcomm_writer_t writer = encoder->writer;
  writer.data = malloc(1);
  writer.pos = 1;
  mcontext->write_pos = 1;
  test_copy_data(&writer, mcontext, mbuf);

  return assert_callback(LABCOMM_ERROR_ENC_BUF_FULL, __FUNCTION__, "");
}

void labcomm_decoder_typecast_t_mock(struct labcomm_decoder *decoder, labcomm_handler_typecast_t handler, void *voidp)
{
        ;
}

void labcomm_handler_typecast_t_mock(void *arg1, void *arg2)
{
        ;
}

int test_dec_missing_do_reg()
{
  reset_callback_erro_id();
  unsigned char *buf = (unsigned char *) "a";
  labcomm_mem_reader_context_t *mcontext = (labcomm_mem_reader_context_t *) malloc(sizeof(labcomm_mem_reader_context_t));
  labcomm_decoder_t *decoder = labcomm_decoder_new(labcomm_mem_reader, mcontext);
  labcomm_register_error_handler_decoder(decoder, test_callback);
  
  decoder->do_register = NULL;
  labcomm_internal_decoder_register(decoder, NULL, labcomm_decoder_typecast_t_mock, labcomm_handler_typecast_t_mock, buf);

  return assert_callback(LABCOMM_ERROR_DEC_MISSING_DO_REG, __FUNCTION__, "");
}

int test_dec_missing_do_decode_one()
{
  reset_callback_erro_id();
  labcomm_mem_reader_context_t *mcontext = (labcomm_mem_reader_context_t *) malloc(sizeof(labcomm_mem_reader_context_t));
  labcomm_decoder_t *decoder = labcomm_decoder_new(labcomm_mem_reader, mcontext);
  labcomm_register_error_handler_decoder(decoder, test_callback);
  
  decoder->do_decode_one = NULL;
  labcomm_decoder_decode_one(decoder);

  return assert_callback(LABCOMM_ERROR_DEC_MISSING_DO_DECODE_ONE, __FUNCTION__, "");
}

int main()
{
  printf("####> Begin tests.\n");
  unsigned int nbr_succeed = 0;
  unsigned int nbr_tests = 6; // Increment this when new tests are written.
  nbr_succeed += test_enc_not_reg_encoder_sign();
  nbr_succeed += test_enc_missing_do_reg();
  nbr_succeed += test_enc_missing_do_encode();
  nbr_succeed += test_enc_buf_full();
  nbr_succeed += test_dec_missing_do_reg();
  nbr_succeed += test_dec_missing_do_decode_one();

  // Too tedius to test really...
  //nbr_succeed += test_dec_unknown_datatype();
  //nbr_succeed += test_dec_index_mismatch();
  //nbr_succeed += test_dec_type_not_found();

  //nbr_succeed += test_unimplemented_func(); // This test will be obsolete in the future ;-)
  //nbr_succeed += test_user_def();           // There are no user defined errors in the library of course.

  printf("####> End tests.\nSummary: %u/%u tests succeed.\n", nbr_succeed, nbr_tests);
  if (nbr_succeed == nbr_tests) {
	  return EXIT_SUCCESS;
  } else {
  	  return EXIT_FAILURE;
  }
}
