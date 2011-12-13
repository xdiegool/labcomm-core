#include <errno.h>
#include <unistd.h>
#include "labcomm.h"
#include "ThrottleDrv/throttle_drv.h"

#define BUFFER_SIZE 2048

int labcomm_thr_reader(labcomm_reader_t *r, labcomm_reader_action_t action)
{
  int result = -EINVAL;
  struct thr_chn_t *ctx = r->context;

  switch (action)
  {
    case labcomm_reader_alloc:
    {
      r->data = malloc(BUFFER_SIZE);
      if (r->data)
      {
        r->data_size = BUFFER_SIZE;
        result = r->data_size;
      }
      else
      {
        r->data_size = 0;
        result = -ENOMEM;
      }
      r->count = 0;
      r->pos = 0;
    }
    break;

    case labcomm_reader_start:
    case labcomm_reader_continue:
    {
       printf("labcomm_reader_start or continue...\n");
      if (r->pos < r->count)
      {
        result = r->count - r->pos;
      }
      else
      {
        int err;
        r->pos = 0;
        printf("labcomm_thr_reader_writer. calling thr_read: %x, %x, %d\n", ctx, r->data, r->data_size);
        err = thr_read(ctx, r->data, r->data_size);
        if (err <= 0)
        {
          r->count = 0;
          result = -1;
        } else
        {
          r->count = err;
          result = r->count - r->pos;
        }
      }
    }
    break;

    case labcomm_reader_end:
    {
      result = 0;
    }
    break;

    case labcomm_reader_free:
    {
      free(r->data);
      r->data = 0;
      r->data_size = 0;
      r->count = 0;
      r->pos = 0;
      result = 0;
    }
    break;
  }
  return result;
}

int labcomm_thr_writer(labcomm_writer_t *w, labcomm_writer_action_t action)
{
  int result = 0;
  struct thr_chn_t *ctx = w->context;

  switch (action)
  {
    case labcomm_writer_alloc:
    {
      w->data = malloc(BUFFER_SIZE);
      if (! w->data)
      {
        result = -ENOMEM;
        w->data_size = 0;
        w->count = 0;
        w->pos = 0;
      }
      else
      {
        w->data_size = BUFFER_SIZE;
        w->count = BUFFER_SIZE;
        w->pos = 0;
      }
    } break;
    case labcomm_writer_free:
    {
      free(w->data);
      w->data = 0;
      w->data_size = 0;
      w->count = 0;
      w->pos = 0;
    }
    break;

    case labcomm_writer_start:
    {
      w->pos = 0;
    }
    break;

    case labcomm_writer_continue:
    {
      result = thr_send(ctx, w->data, w->pos);
      w->pos = 0;
    }
    break;

    case labcomm_writer_end:
    {
      result = thr_send(ctx, w->data, w->pos);
      w->pos = 0;
    }
    break;

    case labcomm_writer_available:
    {
      result = w->count - w->pos;
    }
    break;
  }
  return result;
}
