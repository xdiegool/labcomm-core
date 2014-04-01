#ifndef _LABCOMM_COMPAT_VXWORKS_H_
#define _LABCOMM_COMPAT_VXWORKS_H_

#ifndef  __VXWORKS__
#error "__VXWORKS__" not defined
#endif

#include <types/vxTypes.h>
#include <selectLib.h>
#include <types.h>
#include <timers.h>
#include <stdio.h>

#ifdef __INT64_MAX__
#undef INT64_MAX
#define INT64_MAX __INT64_MAX__
#endif

#if (CPU == PPC603)
  #undef _LITTLE_ENDIAN
#endif

#if (CPU == PENTIUM4)
  #undef _BIG_ENDIAN
#endif

#endif

