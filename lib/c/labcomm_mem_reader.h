#ifndef LABCOMM_MEM_READER_H
#define LABCOMM_MEM_READER_H

#include "labcomm.h"

/* enc_data: The data to be decoded
 * size: the size of the data to be decoded
 */
typedef struct labcomm_mem_reader_context_t labcomm_mem_reader_context_t;
struct labcomm_mem_reader_context_t {
  size_t size;
  unsigned char *enc_data;
};

int labcomm_mem_reader( labcomm_reader_t *r, labcomm_reader_action_t action);

#endif
