#ifndef LABCOMM_MEM_WRITER_H
#define LABCOMM_MEM_WRITER_H

#include "labcomm.h"

/* Wrapper structure for the memory buffer including a writer position. */
typedef struct labcomm_mem_writer_context_t labcomm_mem_writer_context_t;
struct labcomm_mem_writer_context_t {
  size_t write_pos;  // Position where next write should be.
  size_t length;  // Length of the buffer.
  unsigned char *buf;  // Allocated destination buffer.
};

int labcomm_mem_writer(labcomm_writer_t *w, labcomm_writer_action_t action);

/* Wrapper the internal static function copy_data. This is needed so that the exceptions can be unit tested. */
void test_copy_data(labcomm_writer_t *w, labcomm_mem_writer_context_t *mcontext, unsigned char *mbuf);

/* Allocate new labcomm_mem_writer_context_t. */
labcomm_mem_writer_context_t *labcomm_mem_writer_context_t_new(size_t init_pos, size_t length, unsigned char *buf);

/* Deallocate mcontext. */
void labcomm_mem_writer_context_t_free(labcomm_mem_writer_context_t **mcontext);

#endif
