#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <labcomm_fd_reader.h>
#include <labcomm_default_error_handler.h>
#include <labcomm_default_memory.h>
#include <labcomm_default_scheduler.h>
#include <stdio.h>
#include "jg.h"

static struct labcomm_encoder *encoder;

static void handle_foo(jg_foo *v, void *context) {
	printf("got foo: b=%f, e=%s, f=%d\n", v->b, v->e, v->f);
	v->d = v->d+1;
	labcomm_encode_jg_foo(encoder, v);
	usleep(500000);
	v->d = v->d+1;
	labcomm_encode_jg_foo(encoder, v);
}

struct labcomm_decoder *dec_init(int fd, struct labcomm_encoder *e) {
  struct labcomm_decoder *decoder;
  encoder = e;
  void  *context = NULL;

  decoder = labcomm_decoder_new(labcomm_fd_reader_new(
				  labcomm_default_memory, fd, 1), 
				labcomm_default_error_handler, 
				labcomm_default_memory,
				labcomm_default_scheduler);
  if (!decoder) { 
    printf("Failed to allocate decoder %s:%d\n", __FUNCTION__, __LINE__);
    return (void *)0;
  }

  labcomm_decoder_register_jg_foo(decoder, handle_foo, context);

  return decoder;
}


void dec_run(struct labcomm_decoder *decoder) {
  printf("Decoding:\n");
  labcomm_decoder_run(decoder);
  printf("--- End Of File ---:\n");
}

void dec_cleanup(struct labcomm_decoder *decoder) {
  labcomm_decoder_free(decoder);
}
