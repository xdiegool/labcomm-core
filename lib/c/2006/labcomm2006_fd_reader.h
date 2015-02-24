
/*
  labcomm2006_fd_reader.c -- a reader for unix style file-descriptors

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

#ifndef __LABCOMM2006_FD_READER_H__
#define __LABCOMM2006_FD_READER_H__

#include "labcomm2006.h"

struct labcomm2006_reader *labcomm2006_fd_reader_new(struct labcomm2006_memory *memory,
					     int fd, int close_fd_on_free);

#endif

