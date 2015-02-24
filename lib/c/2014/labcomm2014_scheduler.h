/*
  labcomm2014_scheduler.h -- labcomm2014 task coordination

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

#ifndef __LABCOMM2014_SCHEDULER_H__
#define __LABCOMM2014_SCHEDULER_H__

#ifdef LABCOMM_COMPAT
  #include LABCOMM_COMPAT
#else
  #include <unistd.h>
  #include <stdint.h>
#endif

struct labcomm2014_time;

int labcomm2014_time_free(struct labcomm2014_time *t);
int labcomm2014_time_add_usec(struct labcomm2014_time *t, uint32_t usec);

struct labcomm2014_scheduler;

int labcomm2014_scheduler_free(struct labcomm2014_scheduler *s);

/* Lock and event handling */
int labcomm2014_scheduler_writer_lock(struct labcomm2014_scheduler *s);
int labcomm2014_scheduler_writer_unlock(struct labcomm2014_scheduler *s);
int labcomm2014_scheduler_data_lock(struct labcomm2014_scheduler *s);
int labcomm2014_scheduler_data_unlock(struct labcomm2014_scheduler *s);

/* Time handling */
struct labcomm2014_time *labcomm2014_scheduler_now(struct labcomm2014_scheduler *s);
int labcomm2014_scheduler_sleep(struct labcomm2014_scheduler *s,
			    struct labcomm2014_time *wakeup);
int labcomm2014_scheduler_wakeup(struct labcomm2014_scheduler *s);

/* Deferred action handling */
int labcomm2014_scheduler_enqueue(struct labcomm2014_scheduler *s,
			      uint32_t delay,
			      void (*deferred)(void *context),
			      void *context);

#endif

