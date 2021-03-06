/*
  labcomm2014_scheduler.h -- labcomm2014 task coordination, semi-private part

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

#ifndef __LABCOMM2014_SCHEDULER_PRIVATE_H__
#define __LABCOMM2014_SCHEDULER_PRIVATE_H__

#ifdef LABCOMM_COMPAT
  #include LABCOMM_COMPAT
#else
  #include <unistd.h>
#endif

#include "labcomm2014_scheduler.h"

struct labcomm2014_time {
  const struct labcomm2014_time_action {
    int (*free)(struct labcomm2014_time *t);
    int (*add_usec)(struct labcomm2014_time *t, uint32_t usec);
  } *action;
  void *context;
};

struct labcomm2014_scheduler {
  const struct labcomm2014_scheduler_action {
    int (*free)(struct labcomm2014_scheduler *s);
    int (*writer_lock)(struct labcomm2014_scheduler *s);
    int (*writer_unlock)(struct labcomm2014_scheduler *s);
    int (*data_lock)(struct labcomm2014_scheduler *s);
    int (*data_unlock)(struct labcomm2014_scheduler *s);
    struct labcomm2014_time *(*now)(struct labcomm2014_scheduler *s);
    int (*sleep)(struct labcomm2014_scheduler *s,
		 struct labcomm2014_time *wakeup);
    int (*wakeup)(struct labcomm2014_scheduler *s);
    int (*enqueue)(struct labcomm2014_scheduler *s,
		   uint32_t delay,
		   void (*deferred)(void *context),
		   void *context);
  } *action;
  void *context;
};

#endif
