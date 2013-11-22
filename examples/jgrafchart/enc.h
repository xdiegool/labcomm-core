#include <labcomm_fd_writer.h>
#include <labcomm_default_error_handler.h>
#include <labcomm_default_memory.h>
#include <labcomm_default_scheduler.h>

struct labcomm_encoder *enc_init(int fd) ;
void enc_run(struct labcomm_encoder *encoder) ;
void enc_cleanup(struct labcomm_encoder *encoder) ;
