/*
  introspecting.h -- LabComm example of a twoway stacked introspection 
                     reader/writer.

  Copyright 2013 Anders Blomdell <anders.blomdell@control.lth.se>

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

#ifndef __INTROSPECTING_H__
#define __INTROSPECTING_H__

#include <labcomm2006.h>
#include <labcomm2006_ioctl.h>
#include <labcomm2006_fd_reader.h>
#include <labcomm2006_fd_writer.h>

struct introspecting {
  struct labcomm2006_reader *reader;
  struct labcomm2006_writer *writer;
};

extern struct introspecting *introspecting_new(
  struct labcomm2006_reader *reader,
  struct labcomm2006_writer *writer,
  struct labcomm2006_error_handler *error,
  struct labcomm2006_memory *memory,
  struct labcomm2006_scheduler *scheduler);

#define HAS_SIGNATURE LABCOMM_IOS('i',2)
enum introspecting_status { introspecting_unknown, 
			    introspecting_unhandled, 
			    introspecting_unregistered, 
			    introspecting_registered };
#endif
