#ifndef _LABCOMM_FD_WRITER_H_
#define _LABCOMM_FD_WRITER_H_

#include "labcomm.h"

extern int labcomm_fd_writer(
  labcomm_writer_t *writer, 
  labcomm_writer_action_t action,
  ...);

#endif

