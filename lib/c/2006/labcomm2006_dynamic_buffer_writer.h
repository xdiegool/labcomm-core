/*
  labcomm_dynamic_buffer_writer.h -- LabComm dynamic memory writer.

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

#ifndef _LABCOMM_DYNAMIC_BUFFER_READER_WRITER_H_
#define _LABCOMM_DYNAMIC_BUFFER_READER_WRITER_H_

#include "labcomm.h"

extern const struct labcomm_writer_action *labcomm_dynamic_buffer_writer_action;

struct labcomm_writer *labcomm_dynamic_buffer_writer_new(
  struct labcomm_memory *memory);

#endif
