#ifndef __DECIMATING_H__
#define __DECIMATING_H__

#include <labcomm.h>
#include <labcomm_ioctl.h>
#include <labcomm_fd_reader.h>
#include <labcomm_fd_writer.h>

struct decimating {
  struct labcomm_reader *reader;
  struct labcomm_writer *writer;
};

extern struct decimating *decimating_new(
  struct labcomm_reader *reader,
  struct labcomm_writer *writer,
  struct labcomm_lock *lock);

#define SET_DECIMATION LABCOMM_IOSW('d',0,int)
#define GET_DECIMATION LABCOMM_IOSR('d',1,int)

#endif
