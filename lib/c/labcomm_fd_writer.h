#ifndef _LABCOMM_FD_WRITER_H_
#define _LABCOMM_FD_WRITER_H_

#include "labcomm.h"

struct labcomm_writer *labcomm_fd_writer_new(int fd, int close_on_free);

#endif

