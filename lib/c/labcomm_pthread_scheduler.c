/*
  labcomm_pthread_scheduler.c -- labcomm pthread based task coordination

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
#include <errno.h>
#include <pthread.h>
#include "labcomm.h"
#include "labcomm_scheduler.h"
#include "labcomm_scheduler_private.h"
#include "labcomm_pthread_scheduler.h"

struct pthread_time {
  struct labcomm_time time;
  struct labcomm_memory *memory;
  struct timespec abstime;
};

struct pthread_deferred {
  struct pthread_deferred *next;
  struct pthread_deferred *prev;
  struct timespec when;
  void (*action)(void *context);
  void *context;
};

struct pthread_scheduler {
  struct labcomm_scheduler scheduler;
  struct labcomm_memory *memory;
  int wakeup;
  pthread_mutex_t writer_mutex;
  pthread_mutex_t data_mutex;
  pthread_cond_t data_cond;
  struct pthread_deferred deferred;
  struct pthread_deferred deferred_with_delay;
};

static struct labcomm_time_action time_action;

static int queue_empty(struct pthread_deferred *queue)
{
  return queue->next == queue;
}

static void timespec_add_usec(struct timespec *t, useconds_t usec)
{
  time_t sec = usec / 1000000;
  long nsec = (usec % 1000000) * 1000;
  
  t->tv_nsec += nsec;
  t->tv_sec += sec + t->tv_nsec / 1000000000;
  t->tv_nsec %= 1000000000;
}

static int timespec_compare(struct timespec *t1, struct timespec *t2)
{
  if (t1->tv_sec == t2->tv_sec && t1->tv_nsec == t2->tv_nsec) {
    return 0;
  } else if (t1->tv_sec == 0 && t1->tv_nsec == 0) {
    /* t1 is at end of time */
    return 1;
  } else  if (t2->tv_sec == 0 && t2->tv_nsec == 0) {
    /* t2 is at end of time */
    return -1;
  } else if (t1->tv_sec < t2->tv_sec) {
    return -1;
  } else if (t1->tv_sec == t2->tv_sec) {
    if (t1->tv_nsec < t2->tv_nsec) {
      return -1;
    } else if (t1->tv_nsec == t2->tv_nsec) {
      return 0;
    } else {
      return 1;
    }
  } else {
    return 1;
  }
}

static struct labcomm_time *time_new(struct labcomm_memory *memory)
{
  struct pthread_time *time;

  time = labcomm_memory_alloc(memory, 0, sizeof(*time));
  if (time == NULL) {
    return NULL;
  } else {
    time->time.action = &time_action;
    time->time.context = time;
    time->memory = memory;
    clock_gettime(CLOCK_REALTIME, &time->abstime);
    return &time->time;
  }
}

static int time_free(struct labcomm_time *t)
{
  struct pthread_time *time = t->context;
  struct labcomm_memory *memory = time->memory;
  
  labcomm_memory_free(memory, 0, time);
  
  return 0;
}
 
static int time_add_usec(struct labcomm_time *t, useconds_t usec)
{
  struct pthread_time *time = t->context;

  timespec_add_usec(&time->abstime, usec);

  return 0;
}

static struct labcomm_time_action time_action = {
  .free = time_free,
  .add_usec = time_add_usec
};

static int run_action(struct pthread_scheduler *scheduler,
		      struct pthread_deferred *element)
{
  element->prev->next = element->next;
  element->next->prev = element->prev;
  labcomm_scheduler_data_unlock(&scheduler->scheduler);
  element->action(element->context);
  labcomm_memory_free(scheduler->memory, 1, element);
  labcomm_scheduler_data_lock(&scheduler->scheduler);
  return 0;
}

static int run_deferred(struct pthread_scheduler *scheduler)
{
  while (!queue_empty(&scheduler->deferred)) {
    run_action(scheduler, scheduler->deferred.next);
  }
  if (!queue_empty(&scheduler->deferred_with_delay)) {
    struct timespec now;

    clock_gettime(CLOCK_REALTIME, &now);
    while (timespec_compare(&scheduler->deferred_with_delay.next->when,
			    &now) <= 0) {
      run_action(scheduler, scheduler->deferred_with_delay.next);
    }
  }
  return 0;
}

static int scheduler_free(struct labcomm_scheduler *s)
{
  struct pthread_scheduler *scheduler = s->context;
  struct labcomm_memory *memory = scheduler->memory;
  
  labcomm_memory_free(memory, 0, scheduler);

  return 0;
}
 
static int scheduler_writer_lock(struct labcomm_scheduler *s)
{
  struct pthread_scheduler *scheduler = s->context;

  labcomm_scheduler_data_lock(&scheduler->scheduler);
  run_deferred(scheduler);  /* Run deferred tasks before taking lock */
  labcomm_scheduler_data_unlock(&scheduler->scheduler);
  if (pthread_mutex_lock(&scheduler->writer_mutex) != 0) {
    return -errno;
  }
  return 0;
}
 
static int scheduler_writer_unlock(struct labcomm_scheduler *s)
{
  struct pthread_scheduler *scheduler = s->context;

  if (pthread_mutex_unlock(&scheduler->writer_mutex) != 0) {
    return -errno;
  }
  labcomm_scheduler_data_lock(&scheduler->scheduler);
  run_deferred(scheduler);  /* Run deferred tasks after releasing lock */
  labcomm_scheduler_data_unlock(&scheduler->scheduler);
  
  return 0;
}

static int scheduler_data_lock(struct labcomm_scheduler *s)
{
  struct pthread_scheduler *scheduler = s->context;

  if (pthread_mutex_lock(&scheduler->data_mutex) != 0) {
    perror("Failed to lock data_mutex");
    exit(1);
  }
  return 0;
}
 
static int scheduler_data_unlock(struct labcomm_scheduler *s)
{
  struct pthread_scheduler *scheduler = s->context;

  if (pthread_mutex_unlock(&scheduler->data_mutex) != 0) {
    perror("Failed to unlock data_mutex");
    exit(1);
  }
  
  return 0;
}

static struct labcomm_time *scheduler_now(struct labcomm_scheduler *s)
{
  struct pthread_scheduler *scheduler = s->context;

  return time_new(scheduler->memory);
}
 
static int scheduler_sleep(struct labcomm_scheduler *s,
			   struct labcomm_time *t)
{
  struct pthread_scheduler *scheduler = s->context;
  struct pthread_time *time = t?t->context:NULL;

  labcomm_scheduler_data_lock(&scheduler->scheduler);
  while (1) {
    struct timespec *wakeup, now;

    /* Run deferred tasks before sleeping */
    run_deferred(scheduler);
    
    clock_gettime(CLOCK_REALTIME, &now);
    if (scheduler->wakeup ||
	(time && timespec_compare(&time->abstime, &now) <= 0)) {
      /* Done waiting */
      scheduler->wakeup = 0;
      break;
    }
    wakeup = NULL;
    if (!queue_empty(&scheduler->deferred_with_delay)) {
      wakeup = &scheduler->deferred_with_delay.next->when;
      if (time && timespec_compare(&time->abstime, wakeup) < 0) {
	wakeup = &time->abstime;
      }
    } else if (time) {
      wakeup = &time->abstime;
    }
 
    if (wakeup) {
      pthread_cond_timedwait(&scheduler->data_cond, 
			     &scheduler->data_mutex, 
			     wakeup);
    } else {
      pthread_cond_wait(&scheduler->data_cond, 
			&scheduler->data_mutex);
    }
  }
  labcomm_scheduler_data_unlock(&scheduler->scheduler);
  
  return 0;
}

static int scheduler_wakeup(struct labcomm_scheduler *s)
{
  struct pthread_scheduler *scheduler = s->context;

  labcomm_scheduler_data_lock(&scheduler->scheduler);
  scheduler->wakeup = 1;
  pthread_cond_signal(&scheduler->data_cond);
  labcomm_scheduler_data_unlock(&scheduler->scheduler);
  return 0;
}

static int scheduler_enqueue(struct labcomm_scheduler *s,
			     useconds_t delay,
			     void (*deferred)(void *context),
			     void *context)
{
  struct pthread_scheduler *scheduler = s->context;
  int result = 0;
  struct pthread_deferred *element, *insert_before;

  element = labcomm_memory_alloc(scheduler->memory, 1, sizeof(*element));
  if (element == NULL) {
    result = -ENOMEM;
    goto out;
  }
  
  element->action = deferred;
  element->context = context;
  labcomm_scheduler_data_lock(&scheduler->scheduler);
  if (delay == 0) {
    insert_before = &scheduler->deferred;
  } else {
    clock_gettime(CLOCK_REALTIME, &element->when);
    timespec_add_usec(&element->when, delay);
    for (insert_before = scheduler->deferred_with_delay.next ; 
	 timespec_compare(&element->when, &insert_before->when) >= 0 ;
	 insert_before = insert_before->next) {
    }
  }
  element->next = insert_before;
  element->prev = insert_before->prev;
  element->prev->next = element;
  element->next->prev = element;
  pthread_cond_signal(&scheduler->data_cond);
  labcomm_scheduler_data_unlock(&scheduler->scheduler);

out:
  return result;
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

struct labcomm_scheduler *labcomm_pthread_scheduler_new(
  struct labcomm_memory *memory)
{
  struct labcomm_scheduler *result = NULL;
  struct pthread_scheduler *scheduler;

  scheduler = labcomm_memory_alloc(memory, 0, sizeof(*scheduler));
  if (scheduler == NULL) {
    goto out;
  } else {
    scheduler->scheduler.action = &scheduler_action;
    scheduler->scheduler.context = scheduler;
    scheduler->wakeup = 0;
    scheduler->memory = memory;
    if (pthread_mutex_init(&scheduler->writer_mutex, NULL) != 0) {
      goto free_scheduler;
    }
     if (pthread_mutex_init(&scheduler->data_mutex, NULL) != 0) {
       goto destroy_writer_mutex;
    }
    if (pthread_cond_init(&scheduler->data_cond, NULL) != 0) {
      goto destroy_data_mutex;
    }
    scheduler->deferred.next = &scheduler->deferred;
    scheduler->deferred.prev = &scheduler->deferred;
    scheduler->deferred_with_delay.next = &scheduler->deferred_with_delay;
    scheduler->deferred_with_delay.prev = &scheduler->deferred_with_delay;
    scheduler->deferred_with_delay.when.tv_sec = 0;
    scheduler->deferred_with_delay.when.tv_nsec = 0;
    result = &scheduler->scheduler;
    goto out;
  }
destroy_data_mutex:
  pthread_mutex_destroy(&scheduler->data_mutex);
destroy_writer_mutex:
  pthread_mutex_destroy(&scheduler->writer_mutex);
free_scheduler:
  labcomm_memory_free(memory, 0, scheduler);
out:
  return result;
  
}

