/*
  labcomm_pthread_scheduler.h -- labcomm pthread based task coordination

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

#ifndef _LABCOMM_PTHREAD_SCHEDULER_H_
#define _LABCOMM_PTHREAD_SCHEDULER_H_

#include "labcomm.h"

struct labcomm_scheduler *labcomm_pthread_scheduler_new(
  struct labcomm_memory *memory);

#endif
