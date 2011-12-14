#define PC_MODE
#ifdef PC_MODE
#include <stdio.h>
#define DISPLAY_ERR(s) 	perror(s);
#else
#define DISPLAY_ERR(s)  ;
#endif
