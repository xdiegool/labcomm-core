/*
  test_labcomm2014_generated_encoding.c -- LabComm tests of generated encoding

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
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "labcomm2014_private.h"
#include "labcomm2014_default_error_handler.h"
#include "labcomm2014_default_memory.h"
#include "labcomm2014_pthread_scheduler.h"
#include "test/gen/generated_encoding.h"

#define IOCTL_WRITER_ASSERT_BYTES 4096
#define IOCTL_WRITER_RESET 4097

#define EXPECT(...)							\
  {                                                                     \
    int expected[] = __VA_ARGS__;					\
    labcomm2014_encoder_ioctl(encoder, IOCTL_WRITER_ASSERT_BYTES,       \
			  __LINE__,					\
			  sizeof(expected)/sizeof(expected[0]),		\
			  expected);					\
  }

#define VARIABLE(i) -(i + 1)
#define IS_VARIABLE(i) (i < 0)

static unsigned char buffer[128];
struct labcomm2014_writer *writer;
static int seen_variable[1024];

static int buf_writer_alloc(
  struct labcomm2014_writer *w, 
  struct labcomm2014_writer_action_context *action_context)
{
  writer = w; /* Hack */
  w->data_size = sizeof(buffer);
  w->count = w->data_size;
  w->data = buffer;
  w->pos = 0;
  
  return 0;
}

static int buf_writer_free(
  struct labcomm2014_writer *w, 
  struct labcomm2014_writer_action_context *action_context)
{
  return 0;
}

static int buf_writer_start(
  struct labcomm2014_writer *w,
  struct labcomm2014_writer_action_context *action_context,
  int index,
  const struct labcomm2014_signature *signature,
  void *value)
{
  return 0;
}

static int buf_writer_end(
  struct labcomm2014_writer *w, 
  struct labcomm2014_writer_action_context *action_context)
{
  return 0;
}

static int buf_writer_flush(
  struct labcomm2014_writer *w, 
  struct labcomm2014_writer_action_context *action_context)
{
  fprintf(stderr, "Should not come here %s:%d\n", __FILE__, __LINE__);
  exit(1);
  
  return 0;
}

static int buf_writer_ioctl(
  struct labcomm2014_writer *w, 
  struct labcomm2014_writer_action_context *action_context,
  int signature_index, const struct labcomm2014_signature *signature,
  uint32_t action, va_list arg)
{
  int result = -ENOTSUP;
  switch (action) {
    case IOCTL_WRITER_ASSERT_BYTES: {
      int line = va_arg(arg, int);
      int count = va_arg(arg, int);
      int *expected = va_arg(arg, int *);
      int i, mismatch;

      mismatch = 0;
      if (w->pos != count) {
	fprintf(stderr, "Invalid length detected %d != %d (%s:%d)\n", 
		w->pos, count, __FILE__, line);
	mismatch = 1;
      } 
      for (i = 0 ; i < count ; i++) {
        if (IS_VARIABLE(expected[i])) {
          if (seen_variable[VARIABLE(expected[i])] == -1) {
            seen_variable[VARIABLE(expected[i])] = buffer[i];
          }
          if (seen_variable[VARIABLE(expected[i])] != buffer[i]) {
            fprintf(stderr, "Unexpected variable (%d:  != %d)\n",
                    seen_variable[VARIABLE(expected[i])], buffer[i]);
            mismatch = 1;
          }
        } else if (expected[i] != buffer[i]) {
	  mismatch = 1;
	}
      }
      if (mismatch) {
	fprintf(stderr, "Encoder mismatch (%s:%d)\n",
		__FILE__, line);

	for (i = 0 ; i < w->pos ; i++) {
	  printf("%2.2x ", w->data[i]);
	}
	printf("\n");
	for (i = 0 ; i < count ; i++) {
	  if (expected[i] < 0) {
	    printf("v%d ", VARIABLE(expected[i]));
	  } else {
	    printf("%2.2x ", expected[i] );
	  }
	}
	printf("\n");
	exit(1);
      }
      result = 0;
    } break;
    case IOCTL_WRITER_RESET: {
      w->pos = 0;
      result = 0;
    }
  }
  return result;
}

const struct labcomm2014_writer_action writer_action = {
  .alloc = buf_writer_alloc,
  .free = buf_writer_free,
  .start = buf_writer_start,
  .end = buf_writer_end,
  .flush = buf_writer_flush,
  .ioctl = buf_writer_ioctl
};

static struct labcomm2014_writer_action_context action_context = {
  .next = NULL,
  .action = &writer_action,
  .context = NULL
}; 
static struct labcomm2014_writer buffer_writer = {
  .action_context = &action_context,
  .data = buffer,
  .data_size = sizeof(buffer),
  .count = sizeof(buffer),
  .pos = 0,
  .error = 0,
};

void dump_encoder(struct labcomm2014_encoder *encoder)
{
  int i;
  
  for (i = 0 ; i < writer->pos ; i++) {
    printf("%2.2x ", writer->data[i]);
  }
  printf("\n");
}

int main(void)
{
  generated_encoding_B B = 1;
  generated_encoding_R R;
  struct labcomm2014_encoder *encoder;
  int i;

  for (i = 0 ; i < sizeof(seen_variable)/sizeof(seen_variable[0]) ; i++) {
    seen_variable[i] = -1;
  }

  encoder = labcomm2014_encoder_new(
    &buffer_writer, 
    labcomm2014_default_error_handler,
    labcomm2014_default_memory,
    labcomm2014_pthread_scheduler_new(labcomm2014_default_memory));
  EXPECT({ 0x01, 0x0c, 0x0b, 
           'L', 'a', 'b', 'C', 'o', 'm', 'm', '2','0', '1', '4' });

  labcomm2014_encoder_ioctl(encoder, IOCTL_WRITER_RESET);
  /* Register twice to make sure that only one registration gets encoded */
  labcomm2014_encoder_register_generated_encoding_V(encoder);
  labcomm2014_encoder_register_generated_encoding_V(encoder);
  EXPECT({ 0x02, 0x08, VARIABLE(0), 0x01, 0x00, 0x01, 'V', 0x02, 0x11, 0x00,
           0x04, 0x0a, VARIABLE(1), 0x01, 0x00, 0x03, 'v', '_', 't', 0x02, 0x11, 0x00,
           0x04, 0x07, VARIABLE(2), 0x01, 0x00, 0x01, 'V', 0x01, VARIABLE(1),
           0x05, 0x02, VARIABLE(0), VARIABLE(2) });

  labcomm2014_encoder_ioctl(encoder, IOCTL_WRITER_RESET);
  /* Register twice to make sure that only one registration gets encoded */
  labcomm2014_encoder_register_generated_encoding_B(encoder);
  labcomm2014_encoder_register_generated_encoding_B(encoder);
  EXPECT({ 0x02, 0x07, VARIABLE(3),0x01, 0x00,  0x01, 'B', 0x01, 0x21,
           0x05, 0x02, VARIABLE(3), LABCOMM_BIND_SELF });

  labcomm2014_encoder_ioctl(encoder, IOCTL_WRITER_RESET);
  /* Register twice to make sure that only one registration gets encoded */
  labcomm2014_encoder_register_generated_encoding_R(encoder);
  labcomm2014_encoder_register_generated_encoding_R(encoder);
  EXPECT({ 0x02, 0x0a, VARIABLE(4), 0x01, 0x00, 0x01, 'R', 0x04,  0x10, 0x01, 0x04, 0x28,
           0x05, 0x02, VARIABLE(4), LABCOMM_BIND_SELF });

  labcomm2014_encoder_ioctl(encoder, IOCTL_WRITER_RESET);
  /* Register twice to make sure that only one registration gets encoded */
  labcomm2014_encoder_sample_ref_register(encoder, 
                                      labcomm2014_signature_generated_encoding_V);
  labcomm2014_encoder_sample_ref_register(encoder, 
                                      labcomm2014_signature_generated_encoding_V);
  EXPECT({0x03, 0x08, VARIABLE(5), 0x01, 0x00, 0x01, 'V', 0x02, 0x11, 0x00});

  labcomm2014_encoder_ioctl(encoder, IOCTL_WRITER_RESET);
  /* Register twice to make sure that only one registration gets encoded */
  labcomm2014_encoder_sample_ref_register(encoder, 
                                      labcomm2014_signature_generated_encoding_B);
  labcomm2014_encoder_sample_ref_register(encoder, 
                                      labcomm2014_signature_generated_encoding_B);
  EXPECT({0x03, 0x07, VARIABLE(6), 0x01, 0x00, 0x01, 'B', 0x01, 0x21});

  labcomm2014_encoder_ioctl(encoder, IOCTL_WRITER_RESET);
  /* Register twice to make sure that only one registration gets encoded */
  labcomm2014_encoder_sample_ref_register(encoder, 
                                      labcomm2014_signature_generated_encoding_R);
  labcomm2014_encoder_sample_ref_register(encoder, 
                                      labcomm2014_signature_generated_encoding_R);
  EXPECT({0x03, 0x0a, VARIABLE(7), 0x01, 0x00, 0x01, 'R', 0x04, 0x10, 0x01, 0x04, 0x28});

  labcomm2014_encoder_ioctl(encoder, IOCTL_WRITER_RESET);
  // was: labcomm2014_encode_generated_encoding_V(encoder, &V);
  labcomm2014_encode_generated_encoding_V(encoder);
  EXPECT({VARIABLE(10), 0x00 });

  labcomm2014_encoder_ioctl(encoder, IOCTL_WRITER_RESET);
  labcomm2014_encode_generated_encoding_B(encoder, &B);
  EXPECT({VARIABLE(20), 0x01, 1});

  labcomm2014_encoder_ioctl(encoder, IOCTL_WRITER_RESET);
  R.a[0] = labcomm2014_encoder_get_sample_ref(
    encoder, labcomm2014_signature_generated_encoding_V);
  R.a[1] = labcomm2014_encoder_get_sample_ref(
    encoder, labcomm2014_signature_generated_encoding_B);
  R.a[2] = labcomm2014_encoder_get_sample_ref(
    encoder, labcomm2014_signature_generated_encoding_UnusedE);
  R.a[3] = labcomm2014_encoder_get_sample_ref(
    encoder, labcomm2014_signature_generated_encoding_R);
  labcomm2014_encode_generated_encoding_R(encoder, &R);
  EXPECT({VARIABLE(4), 0x10, 0x00, 0x00, 0x00, VARIABLE(5),
                             0x00, 0x00, 0x00, VARIABLE(6),
                             0x00, 0x00, 0x00, 0x00,
                             0x00, 0x00, 0x00, VARIABLE(7)});
  return 0;
}

