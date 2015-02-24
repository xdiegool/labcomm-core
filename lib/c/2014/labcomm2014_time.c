/*
  labcomm2014_time.c -- labcomm2014 time handling

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

#include <errno.h>
#include "labcomm2014_scheduler_private.h"

#define TIME_time(time, ...) time
#define TIME(func, ...)						\
  if (TIME_time(__VA_ARGS__) &&				\
      TIME_time(__VA_ARGS__)->action->func) {			\
    return TIME_time(__VA_ARGS__)->action->func(__VA_ARGS__);	\
  }									\
  return -ENOSYS;

int labcomm2014_time_free(struct labcomm2014_time *s)
{
  TIME(free, s);
}

int labcomm2014_time_add_usec(struct labcomm2014_time *s, uint32_t usec)
{
  TIME(add_usec, s, usec);
}

