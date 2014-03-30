#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <labcomm_fd_writer.h>
#include <labcomm_default_error_handler.h>
#include <labcomm_default_memory.h>
#include <labcomm_default_scheduler.h>
#include "turtle1.h"
#include <stdio.h>

struct labcomm_encoder *turtle_enc_init( int fd) {
  struct labcomm_encoder *encoder;

  encoder = labcomm_encoder_new(labcomm_fd_writer_new(
				  labcomm_default_memory, fd, 1), 
				labcomm_default_error_handler, 
				labcomm_default_memory,
				labcomm_default_scheduler);
  labcomm_encoder_register_turtle1_velocity(encoder);
  return encoder;
}

void turtle_enc_run(struct labcomm_encoder *encoder, turtle1_velocity *v) {
#if 0
	jg_foo v;

	v.b = 17.17;
	v.c = 1742;
	v.d = 4217;
	v.e = "hello";
	v.f = 17;
	v.g = 42;
	v.h = 2;
	v.i = 42.42;
#endif
 	printf("Encoding for turtle1\n");
	v->turtle_name="/turtlesim1/turtle1";
	labcomm_encode_turtle1_velocity(encoder, v);
	sleep(1);
 	printf("Encoding for turtle1\n");
	v->angular *= -1;
	labcomm_encode_turtle1_velocity(encoder, v);
	sleep(1);
	v->angular *= -1;
	//v->h += 42000000;
	labcomm_encode_turtle1_velocity(encoder, v);
}

void turtle_enc_cleanup(struct labcomm_encoder *encoder) {
	labcomm_encoder_free(encoder);
}
