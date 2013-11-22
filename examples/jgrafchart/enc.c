#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <labcomm_fd_writer.h>
#include <labcomm_default_error_handler.h>
#include <labcomm_default_memory.h>
#include <labcomm_default_scheduler.h>
#include "jg.h"
#include <stdio.h>

struct labcomm_encoder *enc_init( int fd) {
  struct labcomm_encoder *encoder;

  encoder = labcomm_encoder_new(labcomm_fd_writer_new(
				  labcomm_default_memory, fd, 1), 
				labcomm_default_error_handler, 
				labcomm_default_memory,
				labcomm_default_scheduler);
  labcomm_encoder_register_jg_foo(encoder);
  return encoder;
}

void enc_run(struct labcomm_encoder * encoder) {
	jg_foo v;

	v.b = 17.17;
	v.c = 1742;
	v.d = 4217;
	v.e = "hello";
	v.f = 17;
	v.g = 42;
	v.h = 2;
	v.i = 42.42;

	labcomm_encode_jg_foo(encoder, &v);
	usleep(500000);
	v.f += 42;
	v.h += 42017040;
	labcomm_encode_jg_foo(encoder, &v);
}

void enc_cleanup(struct labcomm_encoder *encoder) {
	labcomm_encoder_free(encoder);
}
