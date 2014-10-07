#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <labcomm2006_fd_reader.h>
#include <labcomm2006_default_error_handler.h>
#include <labcomm2006_default_memory.h>
#include <labcomm2006_default_scheduler.h>
#include "gen06/simple.h"
#include <stdio.h>

static void handle_simple_theTwoInts(simple_TwoInts *v,void *context) {
  printf("Got theTwoInts. a=%d, b=%d\n", v->a, v->b);
}

static void handle_simple_anotherTwoInts(simple_TwoInts *v,void *context) {
  printf("Got anotherTwoInts. a=%d, b=%d\n", v->a, v->b);
}

static void handle_simple_IntString(simple_IntString *v,void *context) {
  printf("Got IntString. x=%d, s=%s\n", v->x, v->s);
}

static void handle_simple_TwoArrays(simple_TwoArrays *d,void *context) {
  printf("Got TwoArrays:");
  int i,j;
    for(i=0; i<2; i++) {
        printf("%d ",d->fixed.a[i]);
    }
    printf("\n");
    for(i=0; i<2; i++) {
      for(j=0; j<d->variable.n_1; j++) {
	printf("%d ",d->variable.a[i *d->variable.n_1 + j]);
      }
      printf("\n");
    }
    printf("\n");
}

static void handle_simple_TwoFixedArrays(simple_TwoFixedArrays *d,void *context) {
  printf("Got TwoFixedArrays:");
  int i,j;
    for(i=0; i<2; i++) {
        printf("%d ",d->a.a[i]);
    }
    printf("\n");
    for(i=0; i<2; i++) {
      for(j=0; j<3; j++) {
	printf("%d ",d->b.a[i][j]);
      }
      printf("\n");
    }
}

int main(int argc, char *argv[]) {
  int fd;
  struct labcomm2006_decoder *decoder;
  void  *context = NULL;

  char *filename = argv[1];
  printf("C decoder reading from %s\n", filename);
  fd = open(filename, O_RDONLY);
  decoder = labcomm2006_decoder_new(labcomm2006_fd_reader_new(
				labcomm2006_default_memory, fd, 1), 
				labcomm2006_default_error_handler, 
				labcomm2006_default_memory,
				labcomm2006_default_scheduler);
  if (!decoder) { 
    printf("Failed to allocate decoder %s:%d\n", __FUNCTION__, __LINE__);
    return 1;
  }

  labcomm2006_decoder_register_simple_theTwoInts(decoder, handle_simple_theTwoInts, context);
  labcomm2006_decoder_register_simple_anotherTwoInts(decoder, handle_simple_anotherTwoInts, context);
  labcomm2006_decoder_register_simple_IntString(decoder, handle_simple_IntString, context);
  labcomm2006_decoder_register_simple_TwoArrays(decoder, handle_simple_TwoArrays, context);
  labcomm2006_decoder_register_simple_TwoFixedArrays(decoder, handle_simple_TwoFixedArrays, context);

  printf("Decoding:\n");
  labcomm2006_decoder_run(decoder);
  printf("--- End Of File ---:\n");
  labcomm2006_decoder_free(decoder);

  return 0;
}
