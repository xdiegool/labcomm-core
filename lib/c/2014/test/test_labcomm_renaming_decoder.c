#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <err.h>

#include "labcomm2014.h"
#include "labcomm2014_private.h"
#include "labcomm2014_default_error_handler.h"
#include "labcomm2014_default_memory.h"
#include "labcomm2014_default_scheduler.h"
#include "labcomm2014_fd_writer.h"
#include "labcomm2014_fd_reader.h"
#include "labcomm2014_renaming.h"
#include "labcomm2014_renaming_encoder.h"
#include "labcomm2014_renaming_decoder.h"
#include "test/gen/generated_encoding.h"

#define DATA_FILE "renaming_test.dat"

static void handle_r(generated_encoding_R *v, void *context)
{
  labcomm2014_copy_generated_encoding_R(labcomm2014_default_memory, context, v);
}

int main(int argc, char **argv)
{
  struct labcomm2014_encoder *encoder, *prefix_encoder, *suffix_encoder;
  struct labcomm2014_decoder *decoder, *prefix_decoder, *suffix_decoder;
  int fd;
  generated_encoding_R r;
  generated_encoding_R cache_r, prefix_cache_r, suffix_cache_r;

  fd = open(DATA_FILE, O_RDWR | O_CREAT | O_TRUNC, 0644);
  if (fd == -1) {
    err(1, "open()");
  }
  encoder = labcomm2014_encoder_new(
    labcomm2014_fd_writer_new(labcomm2014_default_memory, fd, 0),
    labcomm2014_default_error_handler,
    labcomm2014_default_memory,
    labcomm2014_default_scheduler);
  prefix_encoder = labcomm2014_renaming_encoder_new(
    encoder, labcomm2014_renaming_prefix, "p.");
  suffix_encoder = labcomm2014_renaming_encoder_new(
    prefix_encoder, labcomm2014_renaming_suffix, ".s");

  labcomm2014_encoder_register_generated_encoding_R(encoder);
  labcomm2014_encoder_register_generated_encoding_R(prefix_encoder);
  labcomm2014_encoder_register_generated_encoding_R(suffix_encoder);

  labcomm2014_encoder_sample_ref_register(
    encoder, labcomm2014_signature_generated_encoding_R);
  labcomm2014_encoder_sample_ref_register(
    prefix_encoder, labcomm2014_signature_generated_encoding_R);
  labcomm2014_encoder_sample_ref_register(
    suffix_encoder, labcomm2014_signature_generated_encoding_R);
  r.a[0] = labcomm2014_encoder_get_sample_ref(
    encoder, labcomm2014_signature_generated_encoding_R);
  r.a[1] = labcomm2014_encoder_get_sample_ref(
    prefix_encoder, labcomm2014_signature_generated_encoding_R);
  r.a[2] = labcomm2014_encoder_get_sample_ref(
    suffix_encoder, labcomm2014_signature_generated_encoding_R);

  r.a[3] = labcomm2014_encoder_get_sample_ref(
    encoder, labcomm2014_signature_generated_encoding_R);
  labcomm2014_encode_generated_encoding_R(encoder, &r);

  r.a[3] = labcomm2014_encoder_get_sample_ref(
    prefix_encoder, labcomm2014_signature_generated_encoding_R);
  labcomm2014_encode_generated_encoding_R(prefix_encoder, &r);

  r.a[3] = labcomm2014_encoder_get_sample_ref(
    suffix_encoder, labcomm2014_signature_generated_encoding_R);
  labcomm2014_encode_generated_encoding_R(suffix_encoder, &r);

  labcomm2014_encoder_free(suffix_encoder);
  labcomm2014_encoder_free(prefix_encoder);
  labcomm2014_encoder_free(encoder);
  encoder = NULL;

  lseek(fd, 0, SEEK_SET);
  decoder =
    labcomm2014_decoder_new(
      labcomm2014_fd_reader_new(labcomm2014_default_memory, fd, 0),
      labcomm2014_default_error_handler,
      labcomm2014_default_memory,
      labcomm2014_default_scheduler);
  prefix_decoder = labcomm2014_renaming_decoder_new(
    decoder, labcomm2014_renaming_prefix, "p.");
  suffix_decoder = labcomm2014_renaming_decoder_new(
    prefix_decoder, labcomm2014_renaming_suffix, ".s");

  labcomm2014_decoder_register_generated_encoding_R(
    decoder, handle_r, &cache_r);
  labcomm2014_decoder_register_generated_encoding_R(
    prefix_decoder, handle_r, &prefix_cache_r);
  labcomm2014_decoder_register_generated_encoding_R(
    suffix_decoder, handle_r, &suffix_cache_r);
  labcomm2014_decoder_sample_ref_register(
    decoder, labcomm2014_signature_generated_encoding_R);
  labcomm2014_decoder_sample_ref_register(
    prefix_decoder, labcomm2014_signature_generated_encoding_R);
  labcomm2014_decoder_sample_ref_register(
    suffix_decoder, labcomm2014_signature_generated_encoding_R);

  while (labcomm2014_decoder_decode_one(decoder) > 0) ;

  fprintf(stderr, "cache_r:\n");
  fprintf(stderr, "%p %p %p %p\n",
          cache_r.a[0],
          cache_r.a[1],
          cache_r.a[2],
          cache_r.a[3]);
          
  fprintf(stderr, "prefix_cache_r:\n");
  fprintf(stderr, "%p %p %p %p\n",
          prefix_cache_r.a[0],
          prefix_cache_r.a[1],
          prefix_cache_r.a[2],
          prefix_cache_r.a[3]);
          
  fprintf(stderr, "suffix_cache_r:\n");
  fprintf(stderr, "%p %p %p %p\n",
          suffix_cache_r.a[0],
          suffix_cache_r.a[1],
          suffix_cache_r.a[2],
          suffix_cache_r.a[3]);
          
  assert(cache_r.a[0] ==
         labcomm2014_decoder_get_sample_ref(
           decoder, labcomm2014_signature_generated_encoding_R));
  assert(cache_r.a[1] ==
         labcomm2014_decoder_get_sample_ref(
           prefix_decoder, labcomm2014_signature_generated_encoding_R));
  assert(cache_r.a[2] ==
         labcomm2014_decoder_get_sample_ref(
           suffix_decoder, labcomm2014_signature_generated_encoding_R));
  assert(cache_r.a[3] ==
         labcomm2014_decoder_get_sample_ref(
           decoder, labcomm2014_signature_generated_encoding_R));

  assert(prefix_cache_r.a[0] ==
         labcomm2014_decoder_get_sample_ref(
           decoder, labcomm2014_signature_generated_encoding_R));
  assert(prefix_cache_r.a[1] ==
         labcomm2014_decoder_get_sample_ref(
           prefix_decoder, labcomm2014_signature_generated_encoding_R));
  assert(prefix_cache_r.a[2] ==
         labcomm2014_decoder_get_sample_ref(
           suffix_decoder, labcomm2014_signature_generated_encoding_R));
  assert(prefix_cache_r.a[3] ==
         labcomm2014_decoder_get_sample_ref(
           prefix_decoder, labcomm2014_signature_generated_encoding_R));

  assert(suffix_cache_r.a[0] ==
         labcomm2014_decoder_get_sample_ref(
           decoder, labcomm2014_signature_generated_encoding_R));
  assert(suffix_cache_r.a[1] ==
         labcomm2014_decoder_get_sample_ref(
           prefix_decoder, labcomm2014_signature_generated_encoding_R));
  assert(suffix_cache_r.a[2] ==
         labcomm2014_decoder_get_sample_ref(
           suffix_decoder, labcomm2014_signature_generated_encoding_R));
  assert(suffix_cache_r.a[3] ==
         labcomm2014_decoder_get_sample_ref(
           suffix_decoder, labcomm2014_signature_generated_encoding_R));

  labcomm2014_decoder_free(decoder);
  close(fd);
  unlink(DATA_FILE);
}
