/*
  labcomm_scheduler.c -- labcomm task coordination

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
#include "labcomm_scheduler_private.h"

#define SCHEDULER_scheduler(scheduler, ...) scheduler
#define SCHEDULER(func, ...)						\
  if (SCHEDULER_scheduler(__VA_ARGS__) &&				\
      SCHEDULER_scheduler(__VA_ARGS__)->action->func) {			\
    return SCHEDULER_scheduler(__VA_ARGS__)->action->func(__VA_ARGS__);	\
  }									\
  return -ENOSYS;

int labcomm_scheduler_free(struct labcomm_scheduler *s)
{
  SCHEDULER(free, s);
}

int labcomm_scheduler_writer_lock(struct labcomm_scheduler *s)
{
  SCHEDULER(writer_lock, s);
}

int labcomm_scheduler_writer_unlock(struct labcomm_scheduler *s)
{
  SCHEDULER(writer_unlock, s);
}

int labcomm_scheduler_data_lock(struct labcomm_scheduler *s)
{
  SCHEDULER(data_lock, s);
}

int labcomm_scheduler_data_unlock(struct labcomm_scheduler *s)
{
  SCHEDULER(data_unlock, s);
}

struct labcomm_time *labcomm_scheduler_now(struct labcomm_scheduler *s)
{
  if (s && s->action->now) {
    return s->action->now(s); 
  } 
  return NULL;
}

int labcomm_scheduler_sleep(struct labcomm_scheduler *s,
			    struct labcomm_time *wakeup)
{
  SCHEDULER(sleep, s, wakeup);
}

int labcomm_scheduler_wakeup(struct labcomm_scheduler *s)
{
  SCHEDULER(wakeup, s);
}

int labcomm_scheduler_enqueue(struct labcomm_scheduler *s,
			      useconds_t delay,
			      void (*func)(void *context),
			      void *context)
{
  SCHEDULER(enqueue, s, delay, func, context);
}


