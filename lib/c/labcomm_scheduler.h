/*
  labcomm_scheduler.h -- labcomm task coordination

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

#ifndef _LABCOMM_SCHEDULER_H_
#define _LABCOMM_SCHEDULER_H_

#include <unistd.h>
#include <stdint.h>

struct labcomm_time;

int labcomm_time_free(struct labcomm_time *t);
int labcomm_time_add_usec(struct labcomm_time *t, uint32_t usec);

struct labcomm_scheduler;

int labcomm_scheduler_free(struct labcomm_scheduler *s);

/* Lock and event handling */
int labcomm_scheduler_writer_lock(struct labcomm_scheduler *s);
int labcomm_scheduler_writer_unlock(struct labcomm_scheduler *s);
int labcomm_scheduler_data_lock(struct labcomm_scheduler *s);
int labcomm_scheduler_data_unlock(struct labcomm_scheduler *s);

/* Time handling */
struct labcomm_time *labcomm_scheduler_now(struct labcomm_scheduler *s);
int labcomm_scheduler_sleep(struct labcomm_scheduler *s,
			    struct labcomm_time *wakeup);
int labcomm_scheduler_wakeup(struct labcomm_scheduler *s);

/* Deferred action handling */
int labcomm_scheduler_enqueue(struct labcomm_scheduler *s,
			      uint32_t delay,
			      void (*deferred)(void *context),
			      void *context);

#endif

