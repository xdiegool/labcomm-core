#include <stdint.h>
#include <string.h>
#include "labcomm_private.h"

static int line;

int test_write(struct labcomm_writer *w, labcomm_writer_action_t a, ...)
{
  fprintf(stderr, "test_write should not be called\n");
  exit(1);
}

int test_read(struct labcomm_reader *r, labcomm_reader_action_t a, ...)
{
  fprintf(stderr, "test_read should not be called %s:%d\n", __FILE__, line);
  exit(1);
}

static unsigned char buffer[128];
static labcomm_encoder_t encoder = {
  .context = NULL,
  .writer =  {
    .context = NULL,
    .data = buffer,
    .data_size = sizeof(buffer),
    .count = sizeof(buffer),
    .pos = 0,
    .error = 0,
    .write = test_write,
    .ioctl = NULL,
    .on_error = NULL,
  },
  .do_register = NULL,
  .do_encode = NULL,
  .on_error = NULL,
};

static labcomm_decoder_t decoder = {
  .context = NULL,
  .reader =  {
    .context = NULL,
    .data = buffer,
    .data_size = sizeof(buffer),
    .count = 0,
    .pos = 0,
    .read = test_read,
    .ioctl = NULL,
    .on_error = NULL,
  },
  .do_register = NULL,
  .on_error = NULL,
};

typedef unsigned int packed32;
typedef unsigned char boolean;
typedef unsigned char byte;

#define TEST_WRITE_READ(type, format, value, expect_count, expect_bytes) \
  {									\
    type decoded;							\
    line = __LINE__;							\
    encoder.writer.pos = 0;						\
    labcomm_encode_##type(&encoder, value);				\
    writer_assert(#type, expect_count, (uint8_t*)expect_bytes);	        \
    decoder.reader.count = encoder.writer.pos;				\
    decoder.reader.pos = 0;						\
    decoded = labcomm_decode_##type(&decoder);				\
    if (decoded != value) {						\
      fprintf(stderr, "Decode error" format " != " format " @%s:%d \n", value, decoded, \
	      __FILE__, __LINE__);					\
      exit(1);								\
    }									\
  }

static void writer_assert(char *type,
			  int count,
			  uint8_t *bytes)
{
  if (encoder.writer.pos != count) {
    fprintf(stderr, 
	    "Wrong number of bytes written for '%s' (%d != %d) @%s:%d\n",
	   type, encoder.writer.pos, count, __FILE__, line);
    exit(1);
  }
  if (memcmp(encoder.writer.data, bytes, count) != 0) {
    int i;

    fprintf(stderr, "Wrong bytes written for '%s' ( ", type);
    for (i = 0 ; i < count ; i++) {
      fprintf(stderr, "%2.2x ", encoder.writer.data[i]);
    }
    fprintf(stderr, "!= ");
    for (i = 0 ; i < count ; i++) {
      fprintf(stderr, "%2.2x ", bytes[i]);
    }
    fprintf(stderr, ") @%s:%d\n", __FILE__, line);
    exit(1);
  }
}

int main(void)
{
  TEST_WRITE_READ(packed32, "%d", 0x0, 1, "\x00");
  TEST_WRITE_READ(packed32, "%d", 0x7f, 1, "\x7f");
  TEST_WRITE_READ(packed32, "%d", 0x80, 2, "\x81\x00");
  TEST_WRITE_READ(packed32, "%d", 0x3fff, 2, "\xff\x7f");
  TEST_WRITE_READ(packed32, "%d", 0x4000, 3, "\x81\x80\x00");
  TEST_WRITE_READ(packed32, "%d", 0x1fffff, 3, "\xff\xff\x7f");
  TEST_WRITE_READ(packed32, "%d", 0x200000, 4, "\x81\x80\x80\x00");
  TEST_WRITE_READ(packed32, "%d", 0xfffffff, 4, "\xff\xff\xff\x7f");
  TEST_WRITE_READ(packed32, "%d", 0x10000000, 5, "\x81\x80\x80\x80\x00");
  TEST_WRITE_READ(packed32, "%d", 0xffffffff, 5, "\x8f\xff\xff\xff\x7f");
  TEST_WRITE_READ(boolean, "%d", 0, 1, "\x00");
  TEST_WRITE_READ(boolean, "%d", 1, 1, "\x01");
  TEST_WRITE_READ(byte, "%d", 0, 1, "\x00");
  TEST_WRITE_READ(byte, "%d", 1, 1, "\x01");
  TEST_WRITE_READ(byte, "%d", 0xff, 1, "\xff");
  TEST_WRITE_READ(short, "%d", 0, 2, "\x00\x00");
  TEST_WRITE_READ(short, "%d", 0x7fff, 2, "\x7f\xff");
  TEST_WRITE_READ(short, "%d", -1, 2, "\xff\xff");
  TEST_WRITE_READ(int, "%d", 0, 4, "\x00\x00\x00\x00");
  TEST_WRITE_READ(int, "%d", 0x7fffffff, 4, "\x7f\xff\xff\xff");
  TEST_WRITE_READ(int, "%d", -1, 4, "\xff\xff\xff\xff");
  TEST_WRITE_READ(long, "%ld", 0L, 8, "\x00\x00\x00\x00\x00\x00\x00\x00");
  TEST_WRITE_READ(long, "%ld", 0x7fffffffffffffffL, 8, "\x7f\xff\xff\xff\xff\xff\xff\xff");
  TEST_WRITE_READ(long, "%ld", -1L, 8, "\xff\xff\xff\xff\xff\xff\xff\xff");
  TEST_WRITE_READ(float, "%f", 0.0, 4, "\x00\x00\x00\x00");
  TEST_WRITE_READ(float, "%f", 1.0, 4, "\x3f\x80\x00\x00");
  TEST_WRITE_READ(float, "%f", 2.0, 4, "\x40\x00\x00\x00");
  TEST_WRITE_READ(float, "%f", 0.5, 4, "\x3f\x00\x00\x00");
  TEST_WRITE_READ(float, "%f", 0.25, 4, "\x3e\x80\x00\x00");
  TEST_WRITE_READ(float, "%f", -0.0, 4, "\x80\x00\x00\x00");
  TEST_WRITE_READ(float, "%f", -1.0, 4, "\xbf\x80\x00\x00");
  TEST_WRITE_READ(float, "%f", -2.0, 4, "\xc0\x00\x00\x00");
  TEST_WRITE_READ(float, "%f", -0.5, 4, "\xbf\x00\x00\x00");
  TEST_WRITE_READ(float, "%f", -0.25, 4, "\xbe\x80\x00\x00");
  TEST_WRITE_READ(double, "%f", 0.0, 8, "\x00\x00\x00\x00\x00\x00\x00\x00");
  TEST_WRITE_READ(double, "%f", 1.0, 8, "\x3f\xf0\x00\x00\x00\x00\x00\x00");
  TEST_WRITE_READ(double, "%f", 2.0, 8, "\x40\x00\x00\x00\x00\x00\x00\x00");
  TEST_WRITE_READ(double, "%f", 0.5, 8, "\x3f\xe0\x00\x00\x00\x00\x00\x00");
  TEST_WRITE_READ(double, "%f", 0.25, 8, "\x3f\xd0\x00\x00\x00\x00\x00\x00");
  TEST_WRITE_READ(double, "%f", -0.0, 8, "\x80\x00\x00\x00\x00\x00\x00\x00");
  TEST_WRITE_READ(double, "%f", -1.0, 8, "\xbf\xf0\x00\x00\x00\x00\x00\x00");
  TEST_WRITE_READ(double, "%f", -2.0, 8, "\xc0\x00\x00\x00\x00\x00\x00\x00");
  TEST_WRITE_READ(double, "%f", -0.5, 8, "\xbf\xe0\x00\x00\x00\x00\x00\x00");
  TEST_WRITE_READ(double, "%f", -0.25, 8, "\xbf\xd0\x00\x00\x00\x00\x00\x00");
  fprintf(stderr, "%s succeded\n", __FILE__);
  return 0;
}
