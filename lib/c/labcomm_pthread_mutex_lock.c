#include <errno.h>
#include <pthread.h>
#include "labcomm.h"
#include "labcomm_private.h"

struct labcomm_pthread_mutex_lock {
  struct labcomm_lock lock;
  pthread_mutex_t mutex;
  pthread_cond_t cond;
};

int do_free(struct labcomm_lock *l)
{
  struct labcomm_pthread_mutex_lock *lock = l->context;
  
  pthread_cond_destroy(&lock->cond);
  pthread_mutex_destroy(&lock->mutex);
  free(lock);
  return 0;
}

int do_acquire(struct labcomm_lock *l)
{
  struct labcomm_pthread_mutex_lock *lock = l->context;
  
  if (pthread_mutex_lock(&lock->mutex) != 0) {
    return -errno;
  }
  return 0;
}

int do_release(struct labcomm_lock *l)
{
  struct labcomm_pthread_mutex_lock *lock = l->context;
  
  if (pthread_mutex_unlock(&lock->mutex) != 0) {
    return -errno;
  }
  return 0;
}

int do_wait(struct labcomm_lock *l, useconds_t usec)
{
  struct labcomm_pthread_mutex_lock *lock = l->context;
  
  if (usec <= 0) {
    if (pthread_cond_wait(&lock->cond, &lock->mutex) != 0) {
      return -errno;
    }
  } else {
    struct timespec abstime;
    time_t sec = usec / 1000000;
    long nsec = (usec % 1000000) * 1000;

    clock_gettime(CLOCK_REALTIME, &abstime);
    abstime.tv_nsec += nsec;
    abstime.tv_sec += sec + abstime.tv_nsec / 1000000000;
    abstime.tv_nsec %= 1000000000;
    if (pthread_cond_timedwait(&lock->cond, &lock->mutex, &abstime) != 0) {
      return -errno;
    }
  }
  return 0;
}

int do_notify(struct labcomm_lock *l)
{
  struct labcomm_pthread_mutex_lock *lock = l->context;
  if (pthread_cond_broadcast(&lock->cond) != 0) {
    return -errno;
  }
  return 0;
}

static struct labcomm_lock_action action = {
  .free = do_free,
  .acquire = do_acquire,
  .release = do_release,
  .wait = do_wait,
  .notify = do_notify
};

struct labcomm_lock *labcomm_pthread_mutex_lock_new()
{
  struct labcomm_lock *result = NULL;
  struct labcomm_pthread_mutex_lock *lock;

  lock = malloc(sizeof(*lock));
  if (lock == NULL) {
    goto out;
  }
  if (pthread_mutex_init(&lock->mutex, NULL) != 0) {
    goto free_lock;
  }
  if (pthread_cond_init(&lock->cond, NULL) != 0) {
    goto destroy_mutex;
  }
  lock->lock.action = &action;
  lock->lock.context = lock;
  result = &lock->lock;
  goto out;
destroy_mutex:
  pthread_mutex_destroy(&lock->mutex);
free_lock:
  free(lock);
out:
  return result;

}

