#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <labcomm_fd_reader_writer.h>
#include "gen/simple.h"

static void handle_simple_TwoInts(simple_TwoInts *v,void *context) {
  printf("Got TwoInts. a=%d, b=%d\n", v->a, v->b);
}

static void handle_simple_IntString(simple_IntString *v,void *context) {
  printf("Got IntString. x=%d, s=%s\n", v->x, v->s);
}

int main(int argc, char *argv[]) {
  int fd;
  struct labcomm_decoder *decoder;
  void  *context = NULL;
  int i, j;

  char *filename = argv[1];
  printf("C decoder reading from %s\n", filename);
  fd = open(filename, O_RDONLY);
  decoder = labcomm_decoder_new(labcomm_fd_reader, &fd);
  if (!decoder) { 
    printf("Failed to allocate decoder %s:%d\n", __FUNCTION__, __LINE__);
    return 1;
  }

  labcomm_decoder_register_simple_TwoInts(decoder, handle_simple_TwoInts, context);
  labcomm_decoder_register_simple_IntString(decoder, handle_simple_IntString, context);

  printf("Decoding:\n");
  labcomm_decoder_run(decoder);
  printf("--- End Of File ---:\n");
  labcomm_decoder_free(decoder);
}
