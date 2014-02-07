/*
  test_default_scheduler.c -- LabComm default scheduler

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

#include <stdio.h>
#include <stdlib.h>
#include "labcomm_default_scheduler.h"
#include "labcomm_scheduler.h"
#include "labcomm_scheduler_private.h"

static int scheduler_free(struct labcomm_scheduler *s)
{
  fprintf(stderr, "%s:%d %s %s", __FILE__, __LINE__, __FUNCTION__,
	  "not implemented");
  exit(1);
  return 0;
}
 
static int scheduler_writer_lock(struct labcomm_scheduler *s)
{
  return 0;
}
 
static int scheduler_writer_unlock(struct labcomm_scheduler *s)
{
  return 0;
}

static int scheduler_data_lock(struct labcomm_scheduler *s)
{
  return 0;
}
 
static int scheduler_data_unlock(struct labcomm_scheduler *s)
{
  return 0;
}

static struct labcomm_time *scheduler_now(struct labcomm_scheduler *s)
{
  fprintf(stderr, "%s:%d %s %s", __FILE__, __LINE__, __FUNCTION__,
	  "not implemented");
  exit(1);
  return NULL;
}
 
static int scheduler_sleep(struct labcomm_scheduler *s,
			   struct labcomm_time *t)
{
  fprintf(stderr, "%s:%d %s %s", __FILE__, __LINE__, __FUNCTION__,
	  "not implemented");
  exit(1);
  return 0;
}

static int scheduler_wakeup(struct labcomm_scheduler *s)
{
  fprintf(stderr, "%s:%d %s %s", __FILE__, __LINE__, __FUNCTION__,
	  "not implemented");
  exit(1);
  return 0;
}

static int scheduler_enqueue(struct labcomm_scheduler *s,
			     uint32_t delay,
			     void (*deferred)(void *context),
			     void *context)
{
  fprintf(stderr, "%s:%d %s %s", __FILE__, __LINE__, __FUNCTION__,
	  "not implemented");
  exit(1);
  return 0;
}

static const struct labcomm_scheduler_action scheduler_action = {
  .free = scheduler_free,
  .writer_lock = scheduler_writer_lock,
  .writer_unlock = scheduler_writer_unlock,
  .data_lock = scheduler_data_lock,
  .data_unlock = scheduler_data_unlock,
  .now = scheduler_now,
  .sleep = scheduler_sleep,
  .wakeup = scheduler_wakeup,
  .enqueue = scheduler_enqueue  
};

static struct labcomm_scheduler scheduler = {
  .action = &scheduler_action,
  .context = NULL
};

struct labcomm_scheduler *labcomm_default_scheduler = &scheduler;
