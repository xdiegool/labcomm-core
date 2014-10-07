#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <labcomm_fd_reader.h>
#include <labcomm_default_error_handler.h>
#include <labcomm_default_memory.h>
#include <labcomm_default_scheduler.h>
#include "gen/test.h"
#include <stdio.h>

static void handle_test_twoLines(test_twoLines *v,void *context) {
  printf("Got theTwoInts. (%d,%d) -> (%d,%d), (%d,%d) -> (%d,%d)\n", v->l1.start.x.val, v->l1.start.y.val, 
                                                                     v->l1.end.x.val, v->l1.end.y.val,    
                                                                     v->l2.start.x.val, v->l2.start.y.val,
                                                                     v->l2.end.x.val, v->l2.end.y.val);    
}

int main(int argc, char *argv[]) {
  int fd;
  struct labcomm_decoder *decoder;
  void  *context = NULL;

  char *filename = argv[1];
  printf("C decoder reading from %s\n", filename);
  fd = open(filename, O_RDONLY);
  decoder = labcomm_decoder_new(labcomm_fd_reader_new(
				  labcomm_default_memory, fd, 1), 
				labcomm_default_error_handler, 
				labcomm_default_memory,
				labcomm_default_scheduler);
  if (!decoder) { 
    printf("Failed to allocate decoder %s:%d\n", __FUNCTION__, __LINE__);
    return 1;
  }

  labcomm_decoder_register_test_twoLines(decoder, handle_test_twoLines, context);

  printf("Decoding:\n");
  labcomm_decoder_run(decoder);
  printf("--- End Of File ---:\n");
  labcomm_decoder_free(decoder);

  return 0;
}
