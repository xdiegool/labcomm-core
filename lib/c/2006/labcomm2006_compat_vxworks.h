#ifndef  __VXWORKS__
#error "__VXWORKS__" not defined
#endif

#if (CPU == PPC603)
  #undef _LITTLE_ENDIAN
#endif

#if (CPU == PENTIUM4)
  #undef _BIG_ENDIAN
#endif

