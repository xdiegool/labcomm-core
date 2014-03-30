/*
  labcomm_scheduler.h -- labcomm task coordination, semi-private part

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

#ifndef _LABCOMM_SCHEDULER_PRIVATE_H_
#define _LABCOMM_SCHEDULER_PRIVATE_H_

#include <unistd.h>
#include "labcomm_scheduler.h"

struct labcomm_time {
  const struct labcomm_time_action {
    int (*free)(struct labcomm_time *t);
    int (*add_usec)(struct labcomm_time *t, uint32_t usec);
  } *action;
  void *context;
};

struct labcomm_scheduler {
  const struct labcomm_scheduler_action {
    int (*free)(struct labcomm_scheduler *s);
    int (*writer_lock)(struct labcomm_scheduler *s);
    int (*writer_unlock)(struct labcomm_scheduler *s);
    int (*data_lock)(struct labcomm_scheduler *s);
    int (*data_unlock)(struct labcomm_scheduler *s);
    struct labcomm_time *(*now)(struct labcomm_scheduler *s);
    int (*sleep)(struct labcomm_scheduler *s,
		 struct labcomm_time *wakeup);
    int (*wakeup)(struct labcomm_scheduler *s);
    int (*enqueue)(struct labcomm_scheduler *s,
		   uint32_t delay,
		   void (*deferred)(void *context),
		   void *context);
  } *action;
  void *context;
};

#endif
