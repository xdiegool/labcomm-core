#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <labcomm2014.h>
#include <labcomm2014_default_memory.h>
#include <labcomm2014_fd_reader.h>
#include <labcomm2014_fd_writer.h>
#include "example.h"

int main(int argc, char *argv[]) {
  int fd;
  struct labcomm2014_encoder *encoder;
  struct labcomm2014_writer *labcomm2014_fd_writer;
  int i, j;

  fd = open("example.encoded", O_WRONLY|O_CREAT|O_TRUNC, 0644);
  labcomm2014_fd_writer = labcomm2014_fd_writer_new(labcomm2014_default_memory, fd, 1);
  encoder = labcomm2014_encoder_new(labcomm2014_fd_writer, NULL, 
                                labcomm2014_default_memory, NULL);
  labcomm2014_encoder_register_example_log_message(encoder);
  labcomm2014_encoder_register_example_data(encoder);
  for (i = 0 ; i < argc ; i++) {
    example_log_message message;

    message.sequence = i + 1;
    message.line.n_0 = i;
    message.line.a = malloc(message.line.n_0*sizeof(message.line));
    for (j = 0 ; j < i ; j++) {
      message.line.a[j].last = (j == message.line.n_0 - 1);
      message.line.a[j].data = argv[j + 1];
    }
    labcomm2014_encode_example_log_message(encoder, &message);
    free(message.line.a);
  }
  for (i = 0 ; i < argc ; i++) {
    float f = i;
    labcomm2014_encode_example_data(encoder, &f);
  }
  return 0;
}
