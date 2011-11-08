#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <labcomm_fd_reader_writer.h>
#include "gen/simple.h"

int main(int argc, char *argv[]) {
  int fd;
  struct labcomm_encoder *encoder;
  int i, j;

  char *filename = argv[1];
  printf("C encoder writing to %s\n", filename);
  fd = open(filename, O_WRONLY|O_CREAT|O_TRUNC, 0644);
  encoder = labcomm_encoder_new(labcomm_fd_writer, &fd);
  labcomm_encoder_register_simple_TwoInts(encoder);
  labcomm_encoder_register_simple_IntString(encoder);
  simple_IntString is;
  is.x = 24;
  is.s = "Hello, LabComm!";
  labcomm_encode_simple_IntString(encoder, &is);

  simple_TwoInts ti;
  ti.a = 13;
  ti.b = 37;
  labcomm_encode_simple_TwoInts(encoder, &ti);
}
