#define PC_MODE
#ifdef PC_MODE
#define DISPLAY_ERR(s) 	perror(s);

// Some projects can not use stdio.h.
#ifndef LABCOMM_NO_STDIO
  #include <stdio.h>
#endif

#else
#define DISPLAY_ERR(s)  ;
#endif
