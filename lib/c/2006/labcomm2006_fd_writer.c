/*
  labcomm2006_fd_writer.c -- LabComm writer for Unix file descriptors.

  Copyright 2006-2013 Anders Blomdell <anders.blomdell@control.lth.se>

  This file is part of LabComm.

  LabComm is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  LabComm is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "labcomm2006_private.h"
#include "labcomm2006_fd_writer.h"

#define BUFFER_SIZE 2048

struct labcomm2006_fd_writer {
  struct labcomm2006_writer writer;
  struct labcomm2006_writer_action_context action_context;
  int fd;
  int close_fd_on_free;
};

static int fd_flush(struct labcomm2006_writer *w, 
		    struct labcomm2006_writer_action_context *action_context);

static int fd_alloc(struct labcomm2006_writer *w, 
		    struct labcomm2006_writer_action_context *action_context, 
		    char *version)
{
  w->data = labcomm2006_memory_alloc(w->memory, 0, BUFFER_SIZE);
  if (! w->data) {
    w->error = -ENOMEM;
    w->data_size = 0;
    w->count = 0;
    w->pos = 0;
  } else {
    w->data_size = BUFFER_SIZE;
    w->count = BUFFER_SIZE;
    w->pos = 0;
    if (version && version[0]) {
#if 0 
      labcomm2006_write_string(w, version);
      fd_flush(w, action_context);
#endif
    }
  }

  return w->error;
}

static int fd_free(struct labcomm2006_writer *w, 
		   struct labcomm2006_writer_action_context *action_context)
{
  struct labcomm2006_fd_writer *fd_writer = action_context->context;
  struct labcomm2006_memory *memory = w->memory;

  labcomm2006_memory_free(memory, 0, w->data);
  w->data = 0;
  w->data_size = 0;
  w->count = 0;
  w->pos = 0;

  if (fd_writer->close_fd_on_free) {
    close(fd_writer->fd);
  }
  labcomm2006_memory_free(memory, 0, fd_writer);
  return 0;
}

static int fd_start(struct labcomm2006_writer *w, 
		    struct labcomm2006_writer_action_context *action_context,
		    int index,
		    struct labcomm2006_signature *signature,
		    void *value)
{
  w->pos = 0;
  
  return w->error;
}

static int fd_flush(struct labcomm2006_writer *w, 
		    struct labcomm2006_writer_action_context *action_context)
{
  struct labcomm2006_fd_writer *fd_context = action_context->context;
  int start, err;
  
  start = 0;
  err = 0;
  while (start < w->pos) {
    err = write(fd_context->fd, &w->data[start], w->pos - start);
    if (err <= 0) {
      break;
    }
    start = start + err;
  }
  if (err < 0) {
    w->error = -errno;
  } else if (err == 0) {
    w->error = -EINVAL;
  }
  w->pos = 0;
   
  return w->error;
}

static const struct labcomm2006_writer_action action = {
  .alloc = fd_alloc,
  .free = fd_free,
  .start = fd_start,
  .end = fd_flush,
  .flush = fd_flush,
  .ioctl = NULL
};

struct labcomm2006_writer *labcomm2006_fd_writer_new(struct labcomm2006_memory *memory,
					     int fd, int close_fd_on_free)
{
  struct labcomm2006_fd_writer *result;

  result = labcomm2006_memory_alloc(memory, 0, sizeof(*result));
  if (result == NULL) {
    return NULL;
  } else {
    result->action_context.next = NULL;
    result->action_context.action = &action;
    result->action_context.context = result;
    result->writer.action_context = &result->action_context;
    result->writer.memory = memory;
    result->fd = fd;
    result->close_fd_on_free = close_fd_on_free;
    return &result->writer;
  }
}
