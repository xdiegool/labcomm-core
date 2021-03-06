#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <labcomm_fd_writer.h>
#include <labcomm_default_error_handler.h>
#include <labcomm_default_memory.h>
#include <labcomm_default_scheduler.h>
#include "gen06/simple.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
  int fd;
  struct labcomm_encoder *encoder;

  char *filename = argv[1];
  printf("C encoder writing to %s\n", filename);
  fd = open(filename, O_WRONLY|O_CREAT|O_TRUNC, 0644);
  encoder = labcomm_encoder_new(labcomm_fd_writer_new(
				  labcomm_default_memory, fd, 1), 
				labcomm_default_error_handler, 
				labcomm_default_memory,
				labcomm_default_scheduler);
  labcomm_encoder_register_simple_theTwoInts(encoder);
  labcomm_encoder_register_simple_anotherTwoInts(encoder);
  labcomm_encoder_register_simple_IntString(encoder);
  simple_IntString is;
  is.x = 24;
  is.s = "Hello, LabComm!";
  printf("Encoding IntString, x=%d, s=%s\n", is.x, is.s);
  labcomm_encode_simple_IntString(encoder, &is);

  simple_theTwoInts ti;
  ti.a = 13;
  ti.b = 37;
  printf("Encoding theTwoInts, a=%d, b=%d\n", ti.a, ti.b);
  labcomm_encode_simple_theTwoInts(encoder, &ti);

  simple_anotherTwoInts ati;
  ati.a = 23;
  ati.b = 47;
  printf("Encoding anotherTwoInts, a=%d, b=%d\n", ati.a, ati.b);
  labcomm_encode_simple_anotherTwoInts(encoder, &ati);

  int foo[20];

  labcomm_encoder_register_simple_TwoArrays(encoder);

  simple_TwoArrays ta;
  ta.fixed.a[0] = 17;
  ta.fixed.a[1] = 42;
  ta.variable.n_1 = 10;
  ta.variable.a = foo;
  
  int k;
  for(k=0; k<20; k++) {
	foo[k] = k;
  }

  printf("Encoding TwoArrays...\n");
  labcomm_encode_simple_TwoArrays(encoder, &ta);

  ti.a = 23;
  ti.b = 47;
  printf("Encoding theTwoInts, a=%d, b=%d\n", ti.a, ti.b);
  labcomm_encode_simple_theTwoInts(encoder, &ti);


  simple_TwoFixedArrays tfa;

  tfa.a.a[0] = 41;
  tfa.a.a[1] = 42;

  tfa.b.a[0][0] = 51;
  tfa.b.a[0][1] = 52;
  tfa.b.a[0][2] = 53;
  tfa.b.a[1][0] = 61;
  tfa.b.a[1][1] = 62;
  tfa.b.a[1][2] = 63;

  printf("Encoding TwoFixedArrays...\n");
  labcomm_encoder_register_simple_TwoFixedArrays(encoder);
  labcomm_encode_simple_TwoFixedArrays(encoder, &tfa);

  return 0;
}
