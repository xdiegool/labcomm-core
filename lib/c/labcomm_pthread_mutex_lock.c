#include "labcomm.h"
#include "labcomm_private.h"

struct labcomm_lock *labcomm_pthread_mutex_lock_new()
{
  struct labcomm_lock *result;

  result = malloc(sizeof(*result));
  if (result == NULL) {
    goto out;
  }
  
out:
  return result;

}

