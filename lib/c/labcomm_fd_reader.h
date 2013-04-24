#ifndef _LABCOMM_FD_READER_H_
#define _LABCOMM_FD_READER_H_

#include "labcomm.h"

extern int labcomm_fd_reader(
  labcomm_reader_t *reader, 
  labcomm_reader_action_t action,
  ...);

#endif

