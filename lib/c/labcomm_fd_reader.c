/*
  labcomm_fd_reader.c -- LabComm reader for Unix file descriptors.

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
#include <stdlib.h>
#include <string.h>
#include "labcomm_private.h"
#include "labcomm_fd_reader.h"

#define BUFFER_SIZE 2048

struct labcomm_fd_reader {
  struct labcomm_reader reader;
  struct labcomm_reader_action_context action_context;
  int fd;
  int close_fd_on_free;
};

static int fd_alloc(struct labcomm_reader *r,
		    struct labcomm_reader_action_context *action_context, 
		    struct labcomm_decoder *decoder,
		    char *version)
{
  int result = 0;
  
  r->count = 0;
  r->pos = 0;
  r->data = labcomm_memory_alloc(r->memory, 0, BUFFER_SIZE);
  if (! r->data) {
    r->data_size = 0;
    result = -ENOMEM;
  } else {

    r->data_size = BUFFER_SIZE;
    result = r->data_size;
    if (version && version[0]) {
      char *tmp;
      
      tmp = labcomm_read_string(r);
      if (strcmp(tmp, version) != 0) {
	result = -EINVAL;
      } else {
	result = r->data_size;
      }
      labcomm_memory_free(r->memory, 1, tmp);
    }
  }
  return result;
}

static int fd_free(struct labcomm_reader *r, 
		   struct labcomm_reader_action_context *action_context)
{
  struct labcomm_fd_reader *fd_reader = action_context->context;
  struct labcomm_memory *memory = r->memory;

  labcomm_memory_free(memory, 0, r->data);
  r->data = 0;
  r->data_size = 0;
  r->count = 0;
  r->pos = 0;

  if (fd_reader->close_fd_on_free) {
    close(fd_reader->fd);
  }
  labcomm_memory_free(memory, 0, fd_reader);

  return 0;
}

static int fd_fill(struct labcomm_reader *r, 
		   struct labcomm_reader_action_context *action_context)
{
  int result = 0;
  struct labcomm_fd_reader *fd_reader = action_context->context;

  if (r->pos < r->count) {
    result = r->count - r->pos;
  } else {
    int err;
    
    r->pos = 0;
    err = read(fd_reader->fd, r->data, r->data_size);
    if (err <= 0) {
      r->count = 0;
      r->error = -EPIPE;
      result = -EPIPE;
    } else {
      r->count = err;
      result = r->count - r->pos;
    }
  }
  return result;
}

static const struct labcomm_reader_action action = {
  .alloc = fd_alloc,
  .free = fd_free,
  .start = NULL,
  .fill = fd_fill,
  .end = NULL,
  .ioctl = NULL
};

struct labcomm_reader *labcomm_fd_reader_new(struct labcomm_memory *memory,
					     int fd, int close_fd_on_free)
{
  struct labcomm_fd_reader *result;

  result = labcomm_memory_alloc(memory, 0, sizeof(*result));
  if (result == NULL) {
    return NULL;
  } else {
    result->action_context.next = NULL;
    result->action_context.action = &action;
    result->action_context.context = result;
    result->reader.action_context = &result->action_context;
    result->reader.memory = memory;
    result->fd = fd;
    result->close_fd_on_free = close_fd_on_free;
    return &result->reader;
  }
}
