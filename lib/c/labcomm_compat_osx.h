#ifndef __APPLE__
#error "__APPLE__" not defined
#endif

#ifndef LABCOMM_COMPAT_OSX
#define LABCOMM_COMPAT_OSX

#include <machine/endian.h>
#include <stdio.h>
#include <time.h>

#include <mach/clock.h>
#include <mach/mach.h>

#define CLOCK_REALTIME 0
static inline void clock_gettime(int garbage, struct timespec *ts)
{
  (void) garbage;
  clock_serv_t cclock;
  mach_timespec_t mts;
  host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
  clock_get_time(cclock, &mts);
  mach_port_deallocate(mach_task_self(), cclock);
  ts->tv_sec = mts.tv_sec;
  ts->tv_nsec = mts.tv_nsec;
}

#endif
