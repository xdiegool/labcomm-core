#include <labcomm.h>
#include <labcomm_fd_reader_writer.h>
#include "gen/simple.h"

void simple_an_int_handler(simple_an_int *v,
		      void *context)
{
  struct labcomm_encoder *e = context;

  labcomm_encode_simple_an_int(e, v);
}

int main(int argc, char *argv[]) {
  struct labcomm_decoder *d;
  struct labcomm_encoder *e;

  labcomm_decoder_register_simple_an_int(d, simple_an_int_handler, e);
  return 0;
}
