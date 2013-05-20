#ifndef _LABCOMM_FD_READER_H_
#define _LABCOMM_FD_READER_H_

#include "labcomm.h"

struct labcomm_reader *labcomm_fd_reader_new(int fd, int close_fd_on_free);

#endif

