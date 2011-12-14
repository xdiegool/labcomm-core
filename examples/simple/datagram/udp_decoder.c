#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <labcomm_udp_reader_writer.h>
#include "gen/simple.h"
//#include "/home/sven/slask/UDP/udp_hack.h"
#include "udp_hack.h"

static void handle_simple_TwoInts(simple_TwoInts *v,void *context) {
  printf("Got TwoInts. a=%d, b=%d\n", v->a, v->b);
}

static void handle_simple_IntString(simple_IntString *v,void *context) {
  printf("Got IntString. x=%d, s=%s\n", v->x, v->s);
}

int main(int argc, char *argv[]) {
  server_context_t *fd;
  struct labcomm_decoder *decoder;
  void  *context = NULL;
  int i, j;

  char *filename = argv[1];
  fd = server_init();
  decoder = labcomm_decoder_new(labcomm_udp_reader, fd);
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
  server_exit(fd);
}
