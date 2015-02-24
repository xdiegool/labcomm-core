#ifndef __DECIMATING_H__
#define __DECIMATING_H__

#include <labcomm2014.h>
#include <labcomm2014_ioctl.h>

struct decimating {
  struct labcomm2014_reader *reader;
  struct labcomm2014_writer *writer;
};

extern struct decimating *decimating_new(
  struct labcomm2014_reader *reader,
  struct labcomm2014_writer *writer,
  struct labcomm2014_error_handler *error,
  struct labcomm2014_memory *memory,
  struct labcomm2014_scheduler *scheduler);

#define SET_DECIMATION LABCOMM_IOSW('d',0,int)

#endif
