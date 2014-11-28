/*
  test_labcomm_pthread_scheduler.c -- test labcomm pthread based task 
                                      coordination

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
#include "labcomm2006_default_memory.h"
#include "labcomm2006_scheduler.h"
#include "labcomm2006_pthread_scheduler.h"

#define TICK 100000
struct func_arg {
  struct labcomm2006_scheduler *scheduler;
  int i;
};

static void func(void *arg) 
{
  struct func_arg *func_arg = arg;
  
  printf("%p %d\n", arg, func_arg->i);
  if (func_arg->i == 999) {
    labcomm2006_scheduler_wakeup(func_arg->scheduler);
  }
}

void enqueue(struct labcomm2006_scheduler *scheduler, 
	     int first, int last)
{
  int i;
  
  for (i = first ; i <= last ; i++) {
    struct func_arg *tmp = malloc(sizeof(*tmp));
    
    tmp->scheduler = scheduler;
    tmp->i = i;
    labcomm2006_scheduler_enqueue(scheduler, i*TICK, func, tmp);
  }
}

int main(int argc, char *argv[])
{
  struct labcomm2006_scheduler *scheduler;
  struct labcomm2006_time *time;

  scheduler = labcomm2006_pthread_scheduler_new(labcomm2006_default_memory);
  enqueue(scheduler, 0, 5);
  enqueue(scheduler, 0, 1);
  enqueue(scheduler, 1, 3);
  enqueue(scheduler, 7, 10);
  {
    struct func_arg *tmp = malloc(sizeof(*tmp));
    
    tmp->scheduler = scheduler;
    tmp->i = 999;
    labcomm2006_scheduler_enqueue(scheduler, 6*TICK, func, tmp);
  }
  time = labcomm2006_scheduler_now(scheduler);
  labcomm2006_time_add_usec(time, 12*TICK);
  labcomm2006_scheduler_sleep(scheduler, NULL);
  labcomm2006_scheduler_sleep(scheduler, time);

  return 0;
}
