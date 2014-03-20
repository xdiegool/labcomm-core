#ifndef _LABCOMM_COMPAT_VXWORKS_H_
#define _LABCOMM_COMPAT_VXWORKS_H_

#ifndef  __VXWORKS__
#error "__VXWORKS__" not defined
#endif

#include <types/vxTypes.h>
#include <stdio.h>

#if (CPU == PPC603)
  #undef _LITTLE_ENDIAN
#endif

#if (CPU == PENTIUM4)
  #undef _BIG_ENDIAN
#endif

#endif

