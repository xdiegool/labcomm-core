#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include "labcomm_private.h"

static int line;

static unsigned char buffer[128];

static struct labcomm_writer writer =  {
  .context = NULL,
  .data = buffer,
  .data_size = sizeof(buffer),
  .count = sizeof(buffer),
  .pos = 0,
  .error = 0,
  .action = { NULL, NULL, NULL, NULL, NULL, NULL },
  .on_error = NULL,
};

static struct labcomm_reader reader =  {
  .context = NULL,
  .data = buffer,
  .data_size = sizeof(buffer),
  .count = 0,
  .pos = 0,
  .action = { NULL, NULL, NULL, NULL, NULL, NULL },
  .on_error = NULL,
};

typedef uint32_t packed32;

#define TEST_WRITE_READ(type, ltype, format, value, expect_count, expect_bytes) \
  {									\
    type decoded;							\
    line = __LINE__;							\
    writer.pos = 0;							\
    labcomm_write_##ltype(&writer, value);				\
    writer_assert(#ltype, expect_count, (uint8_t*)expect_bytes);	\
    reader.count = writer.pos;						\
    reader.pos = 0;							\
    decoded = labcomm_read_##ltype(&reader);				\
    if (decoded != value) {						\
      fprintf(stderr, "Decode error" format " != " format " @%s:%d \n", \
	      value, decoded, __FILE__, __LINE__);					\
      exit(1);								\
    }									\
  }

static void writer_assert(char *type,
			  int count,
			  uint8_t *bytes)
{
  if (writer.pos != count) {
    fprintf(stderr, 
	    "Wrong number of bytes written for '%s' (%d != %d) @%s:%d\n",
	    type, writer.pos, count, __FILE__, line);
    exit(1);
  }
  if (memcmp(writer.data, bytes, count) != 0) {
    int i;

    fprintf(stderr, "Wrong bytes written for '%s' ( ", type);
    for (i = 0 ; i < count ; i++) {
      fprintf(stderr, "%2.2x ", writer.data[i]);
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
  TEST_WRITE_READ(packed32, packed32, "%d", 0x0, 1, "\x00");
  TEST_WRITE_READ(packed32, packed32, "%d", 0x7f, 1, "\x7f");
  TEST_WRITE_READ(packed32, packed32, "%d", 0x80, 2, "\x81\x00");
  TEST_WRITE_READ(packed32, packed32, "%d", 0x3fff, 2, "\xff\x7f");
  TEST_WRITE_READ(packed32, packed32, "%d", 0x4000, 3, "\x81\x80\x00");
  TEST_WRITE_READ(packed32, packed32, "%d", 0x1fffff, 3, "\xff\xff\x7f");
  TEST_WRITE_READ(packed32, packed32, "%d", 0x200000, 4, "\x81\x80\x80\x00");
  TEST_WRITE_READ(packed32, packed32, "%d", 0xfffffff, 4, "\xff\xff\xff\x7f");
  TEST_WRITE_READ(packed32, packed32, "%d", 0x10000000, 5, "\x81\x80\x80\x80\x00");
  TEST_WRITE_READ(packed32, packed32, "%d", 0xffffffff, 5, "\x8f\xff\xff\xff\x7f");
  TEST_WRITE_READ(uint8_t, boolean, "%d", 0, 1, "\x00");
  TEST_WRITE_READ(uint8_t, boolean, "%d", 1, 1, "\x01");
  TEST_WRITE_READ(uint8_t, byte, "%d", 0, 1, "\x00");
  TEST_WRITE_READ(uint8_t, byte, "%d", 1, 1, "\x01");
  TEST_WRITE_READ(uint8_t, byte, "%d", 0xff, 1, "\xff");
  TEST_WRITE_READ(int16_t, short, "%d", 0, 2, "\x00\x00");
  TEST_WRITE_READ(int16_t, short, "%d", 0x7fff, 2, "\x7f\xff");
  TEST_WRITE_READ(int16_t, short, "%d", -1, 2, "\xff\xff");
  TEST_WRITE_READ(int32_t, int, "%d", 0, 4, "\x00\x00\x00\x00");
  TEST_WRITE_READ(int32_t, int, "%d", 0x7fffffff, 4, "\x7f\xff\xff\xff");
  TEST_WRITE_READ(int32_t, int, "%d", -1, 4, "\xff\xff\xff\xff");
  TEST_WRITE_READ(int64_t, long, "%" PRId64, INT64_C(0), 8, "\x00\x00\x00\x00\x00\x00\x00\x00");
  TEST_WRITE_READ(int64_t, long, "%" PRId64, INT64_C(0x7fffffffffffffff), 8, "\x7f\xff\xff\xff\xff\xff\xff\xff");
  TEST_WRITE_READ(int64_t, long, "%" PRId64, INT64_C(-1), 8, "\xff\xff\xff\xff\xff\xff\xff\xff");
  TEST_WRITE_READ(float, float, "%f", 0.0, 4, "\x00\x00\x00\x00");
  TEST_WRITE_READ(float, float, "%f", 1.0, 4, "\x3f\x80\x00\x00");
  TEST_WRITE_READ(float, float, "%f", 2.0, 4, "\x40\x00\x00\x00");
  TEST_WRITE_READ(float, float, "%f", 0.5, 4, "\x3f\x00\x00\x00");
  TEST_WRITE_READ(float, float, "%f", 0.25, 4, "\x3e\x80\x00\x00");
  TEST_WRITE_READ(float, float, "%f", -0.0, 4, "\x80\x00\x00\x00");
  TEST_WRITE_READ(float, float, "%f", -1.0, 4, "\xbf\x80\x00\x00");
  TEST_WRITE_READ(float, float, "%f", -2.0, 4, "\xc0\x00\x00\x00");
  TEST_WRITE_READ(float, float, "%f", -0.5, 4, "\xbf\x00\x00\x00");
  TEST_WRITE_READ(float, float, "%f", -0.25, 4, "\xbe\x80\x00\x00");
  TEST_WRITE_READ(double, double, "%f", 0.0, 8, "\x00\x00\x00\x00\x00\x00\x00\x00");
  TEST_WRITE_READ(double, double, "%f", 1.0, 8, "\x3f\xf0\x00\x00\x00\x00\x00\x00");
  TEST_WRITE_READ(double, double, "%f", 2.0, 8, "\x40\x00\x00\x00\x00\x00\x00\x00");
  TEST_WRITE_READ(double, double, "%f", 0.5, 8, "\x3f\xe0\x00\x00\x00\x00\x00\x00");
  TEST_WRITE_READ(double, double, "%f", 0.25, 8, "\x3f\xd0\x00\x00\x00\x00\x00\x00");
  TEST_WRITE_READ(double, double, "%f", -0.0, 8, "\x80\x00\x00\x00\x00\x00\x00\x00");
  TEST_WRITE_READ(double, double, "%f", -1.0, 8, "\xbf\xf0\x00\x00\x00\x00\x00\x00");
  TEST_WRITE_READ(double, double, "%f", -2.0, 8, "\xc0\x00\x00\x00\x00\x00\x00\x00");
  TEST_WRITE_READ(double, double, "%f", -0.5, 8, "\xbf\xe0\x00\x00\x00\x00\x00\x00");
  TEST_WRITE_READ(double, double, "%f", -0.25, 8, "\xbf\xd0\x00\x00\x00\x00\x00\x00");
  fprintf(stderr, "%s succeded\n", __FILE__);
  return 0;
}

