#ifndef _LABCOMM_DYNAMIC_BUFFER_READER_WRITER_H_
#define _LABCOMM_DYNAMIC_BUFFER_READER_WRITER_H_

#include "labcomm.h"

extern int labcomm_dynamic_buffer_writer(
  labcomm_writer_t *writer, 
  labcomm_writer_action_t action,
  ...);

#endif
