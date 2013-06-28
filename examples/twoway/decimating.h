#ifndef __DECIMATING_H__
#define __DECIMATING_H__

#include <labcomm.h>
#include <labcomm_ioctl.h>

struct decimating {
  struct labcomm_reader *reader;
  struct labcomm_writer *writer;
};

extern struct decimating *decimating_new(
  struct labcomm_reader *reader,
  struct labcomm_writer *writer,
  struct labcomm_error_handler *error,
  struct labcomm_memory *memory,
  struct labcomm_scheduler *scheduler);

#define SET_DECIMATION LABCOMM_IOSW('d',0,int)

#endif
