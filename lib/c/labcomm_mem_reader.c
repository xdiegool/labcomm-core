#include "labcomm_mem_reader.h"

#include <errno.h>

/* This implementation assumes labcomm will call end exactly once after each start
 * It is not allowed to save data in mcontext->enc_data,
 * this pointer will be set to NULL after decoding.
 */
/* NOTE!!!!
 * start will be run first, once a signature or a data section is decoded
 * end will be run and then start again. If end of encoded data is reached this
 * must be handled in start.
 */

// TODO make labcomm use result!
int labcomm_mem_reader(labcomm_reader_t *r, labcomm_reader_action_t action)
{
  int result = -EINVAL;
  labcomm_mem_reader_context_t *mcontext = (labcomm_mem_reader_context_t *) r->context;

  switch (action) {
    case labcomm_reader_alloc: {
      r->data = NULL;
      r->data_size = 0;
      r->pos = 0;
      r->count = 0;
      } break;
    case labcomm_reader_start: {
      if (r->data == NULL && mcontext->enc_data != NULL) {
        r->data = (unsigned char *) malloc(mcontext->size);
        if(r->data != NULL) {
          memcpy(r->data, mcontext->enc_data, mcontext->size);
          r->data_size = mcontext->size;
          r->count = mcontext->size;
          r->pos = 0;
          result = r->data_size;
        } else {
          r->data_size = 0;
          result = -ENOMEM;
        }
      } else if (r->data == NULL && mcontext->enc_data == NULL) {
        result = -1;
      } else {
        result = r->count - r->pos;
      }
    } break;
    case labcomm_reader_continue: {
      if (r->pos < r->count) {
        result = r->count - r->pos;
      } else {
        // TODO set some describing error here
        result = -1;
      }
    } break;
    case labcomm_reader_end: {
      if (r->pos >= r->count) {
        free(r->data);
        r->data = NULL;
        r->data_size = 0;
        mcontext->enc_data = NULL;
        mcontext->size = 0;
      }
      result = r->count - r->pos;
    } break;
    case labcomm_reader_free: {
      r->count = 0;
      r->pos = 0;
      result = 0;
    } break;
  }
  return result;
}

