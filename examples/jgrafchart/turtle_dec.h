#include <labcomm_fd_reader.h>
#include <labcomm_default_error_handler.h>
#include <labcomm_default_memory.h>
#include <labcomm_default_scheduler.h>
#include <stdio.h>

struct labcomm_decoder * turtle_dec_init(int fd, struct labcomm_encoder *e) ;
void turtle_dec_run(struct labcomm_decoder *decoder) ;
void turtle_dec_cleanup(struct labcomm_decoder *decoder) ;
