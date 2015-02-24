#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <labcomm2014_fd_writer.h>
#include <labcomm2014_default_error_handler.h>
#include <labcomm2014_default_memory.h>
#include <labcomm2014_default_scheduler.h>
#include "gen/test.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
  int fd;
  struct labcomm2014_encoder *encoder;

  char *filename = argv[1];
  printf("C encoder writing to %s\n", filename);
  fd = open(filename, O_WRONLY|O_CREAT|O_TRUNC, 0644);
  encoder = labcomm2014_encoder_new(labcomm2014_fd_writer_new(
				  labcomm2014_default_memory, fd, 1), 
				labcomm2014_default_error_handler, 
				labcomm2014_default_memory,
				labcomm2014_default_scheduler);
  labcomm2014_encoder_register_test_twoLines(encoder);

  test_twoLines tl;

  tl.l1.start.x.val = 11;
  tl.l1.start.y.val = 13;
  tl.l1.end.x.val = 21;
  tl.l1.end.y.val = 23;
  tl.l2.start.x.val = 11;
  tl.l2.start.y.val = 13;
  tl.l2.end.x.val = 21;
  tl.l2.end.y.val = 23;
 
  printf("Encoding twoLines...\n");
  labcomm2014_encode_test_twoLines(encoder, &tl);

  return 0;
}
