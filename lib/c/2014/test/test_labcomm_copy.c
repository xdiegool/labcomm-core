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
#include "test/gen/generated_encoding.h"
#include "test/gen/test_sample.h"
#include "test/gen/more_types.h"

#define DATA_FILE "copy_test.dat"

static void handle_s1(generated_encoding_S1 *v, void *context)
{
  labcomm2014_copy_generated_encoding_S1(labcomm2014_default_memory, context, v);
}

static void handle_b(generated_encoding_B *v, void *context)
{
  labcomm2014_copy_generated_encoding_B(labcomm2014_default_memory, context, v);
}

static void handle_i(generated_encoding_I *v, void *context)
{
  labcomm2014_copy_generated_encoding_I(labcomm2014_default_memory, context, v);
}

static void handle_p(generated_encoding_P *v, void *context)
{
  labcomm2014_copy_generated_encoding_P(labcomm2014_default_memory, context, v);
}

static void handle_r(generated_encoding_R *v, void *context)
{
  labcomm2014_copy_generated_encoding_R(labcomm2014_default_memory, context, v);
}

static void handle_test_var(test_sample_test_var *v, void *context)
{
  labcomm2014_copy_test_sample_test_var(labcomm2014_default_memory, context, v);
}

static void handle_a(more_types_A *v, void *context)
{
  labcomm2014_copy_more_types_A(labcomm2014_default_memory, context, v);
}

static void handle_s(more_types_S *v, void *context)
{
  labcomm2014_copy_more_types_S(labcomm2014_default_memory, context, v);
}

static void handle_ns(more_types_NS *v, void *context)
{
  labcomm2014_copy_more_types_NS(labcomm2014_default_memory, context, v);
}

static void handle_as(more_types_AS *v, void *context)
{
  labcomm2014_copy_more_types_AS(labcomm2014_default_memory, context, v);
}

int do_test(int argc, char **argv)
{
  struct labcomm2014_encoder *encoder;
  struct labcomm2014_decoder *decoder;
  int fd;
  generated_encoding_S1 s1;
  generated_encoding_S1 cache_s1;
  generated_encoding_B b;
  generated_encoding_B cache_b;
  generated_encoding_I I;
  generated_encoding_I cache_I;
  generated_encoding_P p;
  generated_encoding_P cache_p;
  test_sample_test_var test_var;
  test_sample_test_var cache_test_var;
  more_types_A a;
  more_types_A cache_a;
  more_types_S s;
  more_types_S cache_s = NULL;
  more_types_NS ns;
  more_types_NS cache_ns;
  more_types_AS as;
  more_types_AS cache_as;
  generated_encoding_R r;
  generated_encoding_R cache_r;

  fd = open(DATA_FILE, O_RDWR | O_CREAT | O_TRUNC, 0644);
  if (fd == -1)
    err(1, "open()");
  encoder =
    labcomm2014_encoder_new(labcomm2014_fd_writer_new(labcomm2014_default_memory, fd, 0),
			labcomm2014_default_error_handler,
			labcomm2014_default_memory,
			labcomm2014_default_scheduler);

  labcomm2014_encoder_register_generated_encoding_S1(encoder);
  s1.i = 1;
  labcomm2014_encode_generated_encoding_S1(encoder, &s1);

  labcomm2014_encoder_register_generated_encoding_B(encoder);
  b = 2;
  labcomm2014_encode_generated_encoding_B(encoder, &b);

  labcomm2014_encoder_register_generated_encoding_I(encoder);
  I.n_0 = 3;
  I.a = calloc(I.n_0, sizeof(I.a[0]));
  I.a[0] = 4;
  I.a[1] = 5;
  I.a[2] = 6;
  labcomm2014_encode_generated_encoding_I(encoder, &I);

  labcomm2014_encoder_register_generated_encoding_P(encoder);
  p.n_0 = 7;
  p.a = calloc(p.n_0, sizeof(p.a[0]));
  for (int i = 0; i < p.n_0; i++)
    p.a[i].i = 8 + i;
  labcomm2014_encode_generated_encoding_P(encoder, &p);

  labcomm2014_encoder_register_test_sample_test_var(encoder);
  test_var.n_0 = 2;
  test_var.n_2 = 7;
  test_var.a = calloc(test_var.n_0 * 2 * test_var.n_2, sizeof(*test_var.a));
  for (int i = 0; i < test_var.n_0; i++)
    for (int j = 0; j < 2; j++)
      for (int k = 0; k < test_var.n_2; k++) {
        test_var.a[(((i) * 2 + j) * test_var.n_2) + k] = 100 * i + 10 * j + k;
      }
  labcomm2014_encode_test_sample_test_var(encoder, &test_var);

  labcomm2014_encoder_register_more_types_A(encoder);
  for (int i = 0; i < sizeof(a.a) / sizeof(a.a[0]); i++)
    a.a[i] = i;
  labcomm2014_encode_more_types_A(encoder, &a);

  labcomm2014_encoder_register_more_types_S(encoder);
  s = "this is a string";
  labcomm2014_encode_more_types_S(encoder, &s);

  labcomm2014_encoder_register_more_types_NS(encoder);
  ns.s1 = "this is a string";
  ns.s2 = "this is a another string";
  labcomm2014_encode_more_types_NS(encoder, &ns);

  labcomm2014_encoder_register_more_types_AS(encoder);
  as.n_0 = 3;
  as.a = calloc(as.n_0, sizeof(as.a[0]));
  as.a[0] = "string 0";
  as.a[1] = "string 1";
  as.a[2] = "string 2";
  labcomm2014_encode_more_types_AS(encoder, &as);

  labcomm2014_encoder_register_generated_encoding_R(encoder);
  labcomm2014_encoder_sample_ref_register(encoder, labcomm2014_signature_generated_encoding_V);
  labcomm2014_encoder_sample_ref_register(encoder, 
                                      labcomm2014_signature_generated_encoding_UnusedD);
  labcomm2014_encoder_sample_ref_register(encoder, labcomm2014_signature_generated_encoding_R);
  r.a[0] = labcomm2014_encoder_get_sample_ref(
    encoder, labcomm2014_signature_generated_encoding_V);
  r.a[1] = labcomm2014_encoder_get_sample_ref(
    encoder, labcomm2014_signature_generated_encoding_UnusedE);
  r.a[2] = labcomm2014_encoder_get_sample_ref(
    encoder, labcomm2014_signature_generated_encoding_UnusedD);
  r.a[3] = labcomm2014_encoder_get_sample_ref(
    encoder, labcomm2014_signature_generated_encoding_R);
  labcomm2014_encode_generated_encoding_R(encoder, &r);

  labcomm2014_encoder_free(encoder);
  encoder = NULL;
  lseek(fd, 0, SEEK_SET);
  decoder =
    labcomm2014_decoder_new(labcomm2014_fd_reader_new(labcomm2014_default_memory, fd, 0),
			labcomm2014_default_error_handler,
			labcomm2014_default_memory,
			labcomm2014_default_scheduler);

  labcomm2014_decoder_register_generated_encoding_S1(decoder, handle_s1, &cache_s1);
  labcomm2014_decoder_register_generated_encoding_B(decoder, handle_b, &cache_b);
  labcomm2014_decoder_register_generated_encoding_I(decoder, handle_i, &cache_I);
  labcomm2014_decoder_register_generated_encoding_P(decoder, handle_p, &cache_p);
  labcomm2014_decoder_register_test_sample_test_var(decoder, handle_test_var,
						&cache_test_var);
  labcomm2014_decoder_register_more_types_A(decoder, handle_a, &cache_a);
  labcomm2014_decoder_register_more_types_S(decoder, handle_s, &cache_s);
  labcomm2014_decoder_register_more_types_NS(decoder, handle_ns, &cache_ns);
  labcomm2014_decoder_register_more_types_AS(decoder, handle_as, &cache_as);
  labcomm2014_decoder_register_generated_encoding_R(decoder, handle_r, &cache_r);
  labcomm2014_decoder_sample_ref_register(decoder, labcomm2014_signature_generated_encoding_V);
  labcomm2014_decoder_sample_ref_register(decoder, 
                                      labcomm2014_signature_generated_encoding_UnusedE);
  labcomm2014_decoder_sample_ref_register(decoder, labcomm2014_signature_generated_encoding_R);

  while (labcomm2014_decoder_decode_one(decoder) > 0) ;

  printf("cache_s1.i = %d, s1.i = %d\n", cache_s1.i, s1.i);
  assert(cache_s1.i == s1.i);
  puts("S1 copied ok");

  assert(cache_b == b);
  puts("B copied ok");

  assert(cache_I.n_0 == I.n_0);
  assert(cache_I.a[0] == I.a[0]);
  assert(cache_I.a[1] == I.a[1]);
  assert(cache_I.a[2] == I.a[2]);
  free(I.a);
  puts("I copied ok");

  assert(cache_p.n_0 == p.n_0);
  for (int i = 0; i < p.n_0; i++)
    assert(cache_p.a[i].i == p.a[i].i);
  free(p.a);
  puts("P copied ok");

  assert(cache_test_var.n_0 == test_var.n_0);
  assert(cache_test_var.n_2 == test_var.n_2);
  for (int i = 0; i < test_var.n_0; i++)
    for (int j = 0; j < 2; j++)
      for (int k = 0; k < test_var.n_2; k++) {
        assert(cache_test_var.a[(((i) * 2 + j) * test_var.n_2) + k] == 
                     test_var.a[(((i) * 2 + j) * test_var.n_2) + k]);
        assert(cache_test_var.a[(((i) * 2 + j) * test_var.n_2) + k] == 
               100 * i + 10 * j + k);
      }
  free(test_var.a);
  puts("test_var copied ok");

  for (int i = 0; i < sizeof(a.a) / sizeof(a.a[0]); i++)
    assert(cache_a.a[i] == a.a[i]);
  puts("A copied ok");

  assert(!strcmp(cache_s, s));
  puts("S copied ok");

  assert(!strcmp(cache_ns.s1, ns.s1));
  assert(!strcmp(cache_ns.s2, ns.s2));
  puts("NS copied ok");

  for (int i = 0; i < as.n_0; i++)
    assert(!strcmp(cache_as.a[i], as.a[i]));
  free(as.a);
  puts("AS copied ok");

  fprintf(stderr, "%p %p\n", r.a[0], cache_r.a[0]);
  fprintf(stderr, "%p %p\n", r.a[1], cache_r.a[1]);
  fprintf(stderr, "%p %p\n", r.a[2], cache_r.a[2]);
  fprintf(stderr, "%p %p\n", r.a[3], cache_r.a[3]);
  assert(cache_r.a[0] == r.a[0]);
  assert(cache_r.a[1] == NULL); /* UnusedE */
  assert(cache_r.a[2] == NULL); /* UnusedD */
  assert(cache_r.a[3] == r.a[3]);
  puts("R copied ok");

  labcomm2014_decoder_free(decoder);
  close(fd);
  unlink(DATA_FILE);

  labcomm2014_copy_free_generated_encoding_S1(labcomm2014_default_memory, &cache_s1);
  puts("S1 deallocated ok");
  labcomm2014_copy_free_generated_encoding_B(labcomm2014_default_memory, &cache_b);
  puts("B deallocated ok");
  labcomm2014_copy_free_generated_encoding_I(labcomm2014_default_memory, &cache_I);
  puts("I deallocated ok");
  labcomm2014_copy_free_generated_encoding_P(labcomm2014_default_memory, &cache_p);
  puts("P deallocated ok");
  labcomm2014_copy_free_test_sample_test_var(labcomm2014_default_memory, &cache_test_var);
  puts("test_var deallocated ok");
  labcomm2014_copy_free_more_types_A(labcomm2014_default_memory, &cache_a);
  puts("A deallocated ok");
  labcomm2014_copy_free_more_types_S(labcomm2014_default_memory, &cache_s);
  puts("S deallocated ok");
  labcomm2014_copy_free_more_types_NS(labcomm2014_default_memory, &cache_ns);
  puts("NS deallocated ok");
  labcomm2014_copy_free_more_types_AS(labcomm2014_default_memory, &cache_as);
  puts("AS deallocated ok");

  labcomm2014_copy_free_generated_encoding_R(labcomm2014_default_memory,
                                             &cache_r);
  puts("R deallocated ok");

  return 0;
}

int main(int argc, char **argv)
{
  return do_test(argc, argv);
}
