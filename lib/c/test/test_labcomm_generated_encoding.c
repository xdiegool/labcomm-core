#include <stdint.h>
#include <string.h>
#include <errno.h>
#include "labcomm_private.h"
#include "labcomm_private.h"
#include "test/gen/generated_encoding.h"

int test_write(struct labcomm_writer *w, labcomm_writer_action_t a, ...)
{
  fprintf(stderr, "test_write should not be called\n");
  exit(1);
}

int test_read(struct labcomm_reader *r, labcomm_reader_action_t a, ...)
{
  fprintf(stderr, "test_read should not be called\n");
  exit(1);
}

#define IOCTL_WRITER_ASSERT_BYTES 4096
#define IOCTL_WRITER_RESET 4097

static unsigned char buffer[128];

static int buffer_writer_ioctl(
  struct labcomm_writer *w, int action, va_list arg)
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

static int buffer_writer(
  labcomm_writer_t *w, 
  labcomm_writer_action_t action,
  ...)
{
  switch (action) {
    case labcomm_writer_alloc: {
      w->data_size = sizeof(buffer);
      w->count = w->data_size;
      w->data = buffer;
      w->pos = 0;
      w->ioctl = buffer_writer_ioctl;
    } break;
    case labcomm_writer_start: 
    case labcomm_writer_start_signature: {
    } break;
    case labcomm_writer_continue: 
    case labcomm_writer_continue_signature: {
      fprintf(stderr, "Should not come here %s:%d\n", __FILE__, __LINE__);
      exit(1);
    } break;
    case labcomm_writer_end: 
    case labcomm_writer_end_signature: {
    } break;
    case labcomm_writer_free: {
      w->data = 0;
      w->data_size = 0;
      w->count = 0;
      w->pos = 0;
    } break;
  }
  return w->error;
}

void dump_encoder(labcomm_encoder_t *encoder)
{
  int i;
  
  for (i = 0 ; i < encoder->writer.pos ; i++) {
    printf("%2.2x ", encoder->writer.data[i]);
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

  labcomm_encoder_t *encoder = labcomm_encoder_new(buffer_writer, buffer);

  labcomm_encoder_ioctl(encoder, IOCTL_WRITER_RESET);
  labcomm_encoder_register_generated_encoding_V(encoder);
  EXPECT({ 0x02, -1, 0x01, 'V', 0x11, 0x00 });

  labcomm_encoder_ioctl(encoder, IOCTL_WRITER_RESET);
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

