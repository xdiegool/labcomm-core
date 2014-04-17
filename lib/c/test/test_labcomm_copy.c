#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <err.h>

#include "labcomm.h"
#include "labcomm_private.h"
#include "labcomm_default_error_handler.h"
#include "labcomm_default_memory.h"
#include "labcomm_default_scheduler.h"
#include "labcomm_fd_writer.h"
#include "labcomm_fd_reader.h"
#include "test/gen/generated_encoding.h"

#define DATA_FILE "copy_test.dat"

static void handle_s1(generated_encoding_S1 *v, void *context)
{
  labcomm_copy_generated_encoding_S1(labcomm_default_memory, context, v);
}

static void handle_b(generated_encoding_B *v, void *context)
{
  labcomm_copy_generated_encoding_B(labcomm_default_memory, context, v);
}

static void handle_i(generated_encoding_I *v, void *context)
{
  labcomm_copy_generated_encoding_I(labcomm_default_memory, context, v);
}

static void handle_p(generated_encoding_P *v, void *context)
{
  labcomm_copy_generated_encoding_P(labcomm_default_memory, context, v);
}

int main(int argc, char **argv)
{
  struct labcomm_encoder *encoder;
  struct labcomm_decoder *decoder;
  int fd;
  generated_encoding_S1 s1;
  generated_encoding_S1 cache_s1;
  generated_encoding_B b;
  generated_encoding_B cache_b;
  generated_encoding_I I;
  generated_encoding_I cache_I;
  generated_encoding_P p;
  generated_encoding_P cache_p;

  fd = open(DATA_FILE, O_RDWR | O_CREAT | O_TRUNC, 0644);
  if (fd == -1)
    err(1, "open()");
  encoder =
    labcomm_encoder_new(labcomm_fd_writer_new(labcomm_default_memory, fd, 0),
			labcomm_default_error_handler,
			labcomm_default_memory,
			labcomm_default_scheduler);

  labcomm_encoder_register_generated_encoding_S1(encoder);
  s1.i = 1;
  labcomm_encode_generated_encoding_S1(encoder, &s1);

  labcomm_encoder_register_generated_encoding_B(encoder);
  b = 2;
  labcomm_encode_generated_encoding_B(encoder, &b);

  labcomm_encoder_register_generated_encoding_I(encoder);
  I.n_0 = 3;
  I.a = calloc(I.n_0, sizeof(I.a[0]));
  I.a[0] = 4;
  I.a[1] = 5;
  I.a[2] = 6;
  labcomm_encode_generated_encoding_I(encoder, &I);

  labcomm_encoder_register_generated_encoding_P(encoder);
  p.n_0 = 7;
  p.a = calloc(p.n_0, sizeof(p.a[0]));
  for (int i = 0; i < p.n_0; i++)
    p.a[i].i = 8 + i;
  labcomm_encode_generated_encoding_P(encoder, &p);

  labcomm_encoder_free(encoder);
  encoder = NULL;
  lseek(fd, 0, SEEK_SET);
  decoder =
    labcomm_decoder_new(labcomm_fd_reader_new(labcomm_default_memory, fd, 0),
			labcomm_default_error_handler,
			labcomm_default_memory,
			labcomm_default_scheduler);

  labcomm_decoder_register_generated_encoding_S1(decoder, handle_s1, &cache_s1);
  labcomm_decoder_register_generated_encoding_B(decoder, handle_b, &cache_b);
  labcomm_decoder_register_generated_encoding_I(decoder, handle_i, &cache_I);
  labcomm_decoder_register_generated_encoding_P(decoder, handle_p, &cache_p);
  labcomm_decoder_decode_one(decoder); /* S1 */
  labcomm_decoder_decode_one(decoder);
  labcomm_decoder_decode_one(decoder); /* B */
  labcomm_decoder_decode_one(decoder);
  labcomm_decoder_decode_one(decoder); /* I */
  labcomm_decoder_decode_one(decoder);
  labcomm_decoder_decode_one(decoder); /* P */
  labcomm_decoder_decode_one(decoder);

  assert(cache_s1.i == s1.i);
  assert(cache_b == b);
  assert(cache_I.n_0 == I.n_0);
  assert(cache_I.a[0] == I.a[0]);
  assert(cache_I.a[1] == I.a[1]);
  assert(cache_I.a[2] == I.a[2]);
  free(I.a);
  for (int i = 0; i < p.n_0; i++)
    assert(cache_p.a[i].i == p.a[i].i);
  free(p.a);

  labcomm_decoder_free(decoder);
  close(fd);
  unlink(DATA_FILE);
  /* TODO: Implement labcomm_free_x */
}
