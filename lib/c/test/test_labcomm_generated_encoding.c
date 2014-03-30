/*
  test_labcomm_generated_encoding.c -- LabComm tests of generated encoding

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
#include "labcomm_private.h"
#include "labcomm_default_error_handler.h"
#include "labcomm_default_memory.h"
#include "labcomm_pthread_scheduler.h"
#include "test/gen/generated_encoding.h"

#define IOCTL_WRITER_ASSERT_BYTES 4096
#define IOCTL_WRITER_RESET 4097

static unsigned char buffer[128];
struct labcomm_writer *writer;

static int buf_writer_alloc(
  struct labcomm_writer *w, 
  struct labcomm_writer_action_context *action_context,
  char *labcomm_version)
{
  writer = w; /* Hack */
  w->data_size = sizeof(buffer);
  w->count = w->data_size;
  w->data = buffer;
  w->pos = 0;
  
  return 0;
}

static int buf_writer_free(
  struct labcomm_writer *w, 
  struct labcomm_writer_action_context *action_context)
{
  return 0;
}

static int buf_writer_start(
  struct labcomm_writer *w,
  struct labcomm_writer_action_context *action_context,
  int index,
  struct labcomm_signature *signature,
  void *value)
{
  return 0;
}

static int buf_writer_end(
  struct labcomm_writer *w, 
  struct labcomm_writer_action_context *action_context)
{
  return 0;
}

static int buf_writer_flush(
  struct labcomm_writer *w, 
  struct labcomm_writer_action_context *action_context)
{
  fprintf(stderr, "Should not come here %s:%d\n", __FILE__, __LINE__);
  exit(1);
  
  return 0;
}

static int buf_writer_ioctl(
  struct labcomm_writer *w, 
  struct labcomm_writer_action_context *action_context,
  int signature_index, struct labcomm_signature *signature,
  uint32_t action, va_list arg)
{
  int result = -ENOTSUP;
  switch (action) {
    case IOCTL_WRITER_ASSERT_BYTES: {
      int line = va_arg(arg, int);
      int count = va_arg(arg, int);
      int *expected = va_arg(arg, int *);
      int i, mismatch;

      if (w->pos != count) {
	fprintf(stderr, "Invalid length encoded %d != %d (%s:%d)\n", 
		w->pos, count, __FILE__, line);
	mismatch = 1;
      } 
      for (mismatch = 0, i = 0 ; i < count ; i++) {
	if (expected[i] >= 0 && expected[i] != buffer[i]) {
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
	    printf(".. ");
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

const struct labcomm_writer_action writer_action = {
  .alloc = buf_writer_alloc,
  .free = buf_writer_free,
  .start = buf_writer_start,
  .end = buf_writer_end,
  .flush = buf_writer_flush,
  .ioctl = buf_writer_ioctl
};

static struct labcomm_writer_action_context action_context = {
  .next = NULL,
  .action = &writer_action,
  .context = NULL
}; 
static struct labcomm_writer buffer_writer = {
  .action_context = &action_context,
  .data = buffer,
  .data_size = sizeof(buffer),
  .count = sizeof(buffer),
  .pos = 0,
  .error = 0,
};

void dump_encoder(struct labcomm_encoder *encoder)
{
  int i;
  
  for (i = 0 ; i < writer->pos ; i++) {
    printf("%2.2x ", writer->data[i]);
  }
  printf("\n");
}

#define EXPECT(...)							\
  {									\
    int expected[] = __VA_ARGS__;					\
    labcomm_encoder_ioctl(encoder, IOCTL_WRITER_ASSERT_BYTES,		\
			  __LINE__,					\
			  sizeof(expected)/sizeof(expected[0]),		\
			  expected);					\
  }

int main(void)
{
  generated_encoding_V V;
  generated_encoding_B B = 1;

  struct labcomm_encoder *encoder = labcomm_encoder_new(
    &buffer_writer, 
    labcomm_default_error_handler,
    labcomm_default_memory,
    labcomm_pthread_scheduler_new(labcomm_default_memory));

  labcomm_encoder_ioctl(encoder, IOCTL_WRITER_RESET);
  labcomm_encoder_register_generated_encoding_V(encoder);
  labcomm_encoder_register_generated_encoding_V(encoder);
  EXPECT({ 0x02, -1, 0x01, 'V', 0x11, 0x00 });

  labcomm_encoder_ioctl(encoder, IOCTL_WRITER_RESET);
  labcomm_encoder_register_generated_encoding_B(encoder);
  labcomm_encoder_register_generated_encoding_B(encoder);
  EXPECT({0x02, -1, 0x01, 'B', 0x21});

  labcomm_encoder_ioctl(encoder, IOCTL_WRITER_RESET);
  labcomm_encode_generated_encoding_V(encoder, &V);
  EXPECT({-1});

  labcomm_encoder_ioctl(encoder, IOCTL_WRITER_RESET);
  labcomm_encode_generated_encoding_B(encoder, &B);
  EXPECT({-1, 1});

  return 0;
}

