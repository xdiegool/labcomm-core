#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <labcomm_udp_reader_writer.h>
#include "gen/simple.h"
//#include "/home/sven/slask/UDP/udp_hack.h"
#include "udp_hack.h"

int main(int argc, char *argv[]) {
  server_context_t *fd;
  struct labcomm_encoder *encoder;
  int i, j;

  char *filename = argv[1];
  fd = client_init();
  encoder = labcomm_encoder_new(labcomm_udp_writer, fd);
  labcomm_encoder_register_simple_TwoInts(encoder);
  labcomm_encoder_register_simple_IntString(encoder);
  simple_IntString is;
  is.x = 24;
  is.s = "Hello, LabComm!";
  printf("Encoding IntString, x=%d, s=%s\n", is.x, is.s);
  labcomm_encode_simple_IntString(encoder, &is);

  simple_TwoInts ti;
  ti.a = 13;
  ti.b = 37;
  printf("Encoding TwoInts, a=%d, b=%d\n", ti.a, ti.b);
  labcomm_encode_simple_TwoInts(encoder, &ti);

  client_exit(fd);
}
