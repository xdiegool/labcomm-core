#include "labcomm_mem_writer.h"

#include <stddef.h>  // For size_t.
#include <stdarg.h>
#include <errno.h>

#include "labcomm.h"
#include "cppmacros.h"

#define BUFFER_SIZE 150 // Suitable size is at least the size of a fully encoded message. Found by inspecting size of file genreated from the labcomm_fs_reader_writer.c on the same message type.

// Put encdoded data directly in mcontext->mbuf or malloc new temporary memory.
// 1 == Allocate new memory.
// 2 == Use mcontext->buf directly. But _beware_; you can not then later change 
// mcontext->buf to something else since the writer gets a reference to this 
// buffer!
#if defined(MEM_WRITER_ENCODED_BUFFER) && (EMPTY(MEM_WRITER_ENCODED_BUFFER) != 1)
  #define ENCODED_BUFFER MEM_WRITER_ENCODED_BUFFER
#else
  #define ENCODED_BUFFER 1
#endif

static int get_writer_available(labcomm_writer_t *w, labcomm_mem_writer_context_t *mcontext);
static void copy_data(labcomm_writer_t *w, labcomm_mem_writer_context_t *mcontext, unsigned char *mbuf);

/*
 * Write encoded messages to memory. w->context is assumed to be a pointer to a
 * labcomm_mem_writer_context_t structure.
 */
int labcomm_mem_writer(labcomm_writer_t *w, labcomm_writer_action_t action)
{
  int result = 0;
  // Unwrap pointers for easy access.
  labcomm_mem_writer_context_t *mcontext = (labcomm_mem_writer_context_t *) w->context;
  unsigned char *mbuf = mcontext->buf;

  switch (action) {
  case labcomm_writer_alloc: {
#if (ENCODED_BUFFER == 1)
    w->data = malloc(BUFFER_SIZE); // Buffer that LabComm will use for putting the encoded data.
    if (w->data == NULL) {
      result = -ENOMEM;
      w->data_size = 0;
      w->count = 0;
      w->pos = 0;
    } else {
      w->data_size = BUFFER_SIZE;
      w->count = BUFFER_SIZE;
      w->pos = 0;
    }
#elif (ENCODED_BUFFER == 2)
    w->data = mbuf;
    int bytes_left = (mcontext->length - mcontext->write_pos);
    w->data_size = bytes_left;
    w->count = bytes_left;
    w->pos = mcontext->write_pos;
#endif
     } break;
  case labcomm_writer_free:{
#if (ENCODED_BUFFER == 1)
    free(w->data);
#endif
    w->data = 0;
    w->data_size = 0;
    w->count = 0;
    w->pos = 0;
   } break;
  case labcomm_writer_start:
    case labcomm_writer_start_signature: {
#if (ENCODED_BUFFER == 1)
    w->pos = 0;
#elif (ENCODED_BUFFER == 2)
    w->pos = mcontext->write_pos;
#endif
    } break;
  case labcomm_writer_continue:
  case labcomm_writer_continue_signature: { 
    // Encode-buffer(w->data) is full; empty/handle it. (w->pos == w->count) most likely.
#if (ENCODED_BUFFER == 1)
    copy_data(w, mcontext, mbuf);
    result = w->pos; // Assume result here should be number of bytes written.
    w->pos = 0;
#elif (ENCODED_BUFFER == 2)
    mcontext->write_pos = w->pos;
#endif
     result = 0;
    } break;
  case labcomm_writer_end:
  case labcomm_writer_end_signature:{ // Nothing more to encode, handle encode-buffer(w->data).
#if (ENCODED_BUFFER == 1)
    copy_data(w, mcontext, mbuf);
    result = w->pos;
    w->pos = 0;
#elif (ENCODED_BUFFER == 2)
    mcontext->write_pos = w->pos;
#endif
    result = 0;
    } break;
  }
  return result;
}

labcomm_mem_writer_context_t *labcomm_mem_writer_context_t_new(size_t init_pos, size_t length, unsigned char *buf)
{
  labcomm_mem_writer_context_t *mcontext = (labcomm_mem_writer_context_t *) malloc(sizeof(labcomm_mem_writer_context_t));
  if (mcontext == NULL) {
    //fprintf(stderr, "error: Can not allocate labcomm_mem_writer_context_t.\n");
  } else {
    mcontext->write_pos = init_pos;
    mcontext->length = length;
    mcontext->buf = buf;
  }
  return mcontext;
}

void labcomm_mem_writer_context_t_free(labcomm_mem_writer_context_t **mcontext)
{
  free(*mcontext);
  *mcontext = NULL;
}

// Get the number of available bytes in the mcontext->buf buffer.
static int get_writer_available(labcomm_writer_t *w, labcomm_mem_writer_context_t *mcontext)
{
  return (mcontext->length - mcontext->write_pos);
}

// Copy data from encoded buffer to mbuf.
static void copy_data(labcomm_writer_t *w, labcomm_mem_writer_context_t *mcontext, unsigned char *mbuf)
{
        int writer_available = get_writer_available(w, mcontext);
  if (( writer_available - w->pos) < 0) {
    w->on_error(LABCOMM_ERROR_ENC_BUF_FULL, 3, "labcomm_writer_t->pos=%i, but available in mcontext is %i", w->pos, writer_available); 
  } else {
    int i;
    for (i = 0; i < w->pos; ++i, mcontext->write_pos++) {
      mbuf[mcontext->write_pos] = w->data[i];
    }
  }
}

void test_copy_data(labcomm_writer_t *w, labcomm_mem_writer_context_t *mcontext, unsigned char *mbuf)
{
  copy_data(w, mcontext, mbuf); 
}
