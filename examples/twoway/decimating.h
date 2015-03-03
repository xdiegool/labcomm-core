#ifndef __DECIMATING_H__
#define __DECIMATING_H__

#include <labcomm2006.h>
#include <labcomm2006_ioctl.h>

struct decimating {
  struct labcomm2006_reader *reader;
  struct labcomm2006_writer *writer;
};

extern struct decimating *decimating_new(
  struct labcomm2006_reader *reader,
  struct labcomm2006_writer *writer,
  struct labcomm2006_error_handler *error,
  struct labcomm2006_memory *memory,
  struct labcomm2006_scheduler *scheduler);

#define SET_DECIMATION LABCOMM_IOSW('d',0,int)

#endif
