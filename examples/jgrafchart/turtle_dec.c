#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <labcomm_fd_reader.h>
#include <labcomm_default_error_handler.h>
#include <labcomm_default_memory.h>
#include <labcomm_default_scheduler.h>
#include <stdio.h>
#include "turtle1.h"

static struct labcomm_encoder *encoder;

static void handle_pose(turtle1_pose *v, void *context) {
#if 1
	printf("got pose\n");
#else
	turtle1_velocity vel;
	sleep(1);

	labcomm_encode_turtle1_velocity(encoder, vel);
	vel.angular = -1;
	sleep(1);

	labcomm_encode_turtle1_velocity(encoder, vel);
	sleep(1);

	vel.angular = -1;
	labcomm_encode_turtle1_velocity(encoder, vel);
#endif
}

struct labcomm_decoder *turtle_dec_init(int fd, struct labcomm_encoder *e) {
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

  labcomm_decoder_register_turtle1_pose(decoder, handle_pose, context);

  return decoder;
}


void turtle_dec_run(struct labcomm_decoder *decoder) {
  printf("Decoding:\n");
  labcomm_decoder_run(decoder);
  printf("--- End Of File ---:\n");
}

void turtle_dec_cleanup(struct labcomm_decoder *decoder) {
  labcomm_decoder_free(decoder);
}
