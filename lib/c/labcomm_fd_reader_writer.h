#include "labcomm.h"

extern int labcomm_fd_reader(
  labcomm_reader_t *r, 
  labcomm_reader_action_t action);

extern int labcomm_fd_writer(
  labcomm_writer_t *r, 
  labcomm_writer_action_t action);
