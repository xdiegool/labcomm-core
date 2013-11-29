#include <labcomm_fd_writer.h>
#include <labcomm_default_error_handler.h>
#include <labcomm_default_memory.h>
#include <labcomm_default_scheduler.h>

#include "turtle1.h"

struct labcomm_encoder *turtle_enc_init(int fd) ;
void turtle_enc_run(struct labcomm_encoder *encoder, turtle1_velocity *v) ;
void turtle_enc_cleanup(struct labcomm_encoder *encoder) ;
