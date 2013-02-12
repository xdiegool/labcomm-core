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
  labcomm_encoder_register_simple_TwoArrays(encoder);
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

  int foo[10];

  simple_TwoArrays ta;
  ta.fixed.a[0] = 17;
  ta.fixed.a[1] = 42;
  ta.variable.n_0 = 10;
  ta.variable.a = foo;

  ti.a = 23;
  ti.b = 47;
  printf("Encoding TwoInts, a=%d, b=%d\n", ti.a, ti.b);
  labcomm_encode_simple_TwoInts(encoder, &ti);

  labcomm_encode_simple_TwoArrays(encoder, &ta);

}
