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
  struct labcomm_lock *lock,
  struct labcomm_memory *memory);

#define SET_DECIMATION LABCOMM_IOSW('d',0,int)

#endif
