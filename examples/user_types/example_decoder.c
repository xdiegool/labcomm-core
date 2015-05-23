#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <labcomm2014_fd_reader.h>
#include <labcomm2014_default_error_handler.h>
#include <labcomm2014_default_memory.h>
#include <labcomm2014_default_scheduler.h>
#include "gen/test.h"
#include <stdio.h>

static void handle_test_doavoid(test_doavoid *v,void *context) {
  printf("Got a void.\n"); 
}

static void handle_test_intAndRef(test_intAndRef *v,void *context) {
  printf("Got intAndRef. (%d : %p) \n", v->x, v->reference); 
}

static void handle_test_twoInts(test_twoInts *v,void *context) {
  printf("Got twoInts. (%d,%d) \n", v->a, v->b); 
}

static void handle_test_theFirstInt(int *v,void *context) {
  printf("Got theFirstInt. (%d) \n", *v); 
}

static void handle_test_theSecondInt(int *v,void *context) {
  printf("Got theSecondInt. (%d) \n", *v); 
}

static void handle_type_def(struct labcomm2014_raw_type_def *v,void *context) {
  printf("Got type_def. (0x%x) %s\n", v->index, v->name); 
}

static void handle_type_binding(struct labcomm2014_type_binding *v,void *context) {
  printf("Got type binding. 0x%x --> 0x%x\n", v->sample_index, v->type_index); 
}

static void handle_test_twoLines(test_twoLines *v,void *context) {
  printf("Got twoLines. (%d,%d) -> (%d,%d), (%d,%d) -> (%d,%d)\n", v->l1.start.x.val, v->l1.start.y.val, 
                                                                     v->l1.end.x.val, v->l1.end.y.val,    
                                                                     v->l2.start.x.val, v->l2.start.y.val,
                                                                     v->l2.end.x.val, v->l2.end.y.val);    
}

int main(int argc, char *argv[]) {
  int fd;
  struct labcomm2014_decoder *decoder;
  void  *context = NULL;

  char *filename = argv[1];
  printf("C decoder reading from %s\n", filename);
  fd = open(filename, O_RDONLY);
  decoder = labcomm2014_decoder_new(labcomm2014_fd_reader_new(
				  labcomm2014_default_memory, fd, 1), 
				labcomm2014_default_error_handler, 
				labcomm2014_default_memory,
				labcomm2014_default_scheduler);
  if (!decoder) { 
    printf("Failed to allocate decoder %s:%d\n", __FUNCTION__, __LINE__);
    return 1;
  }

  labcomm2014_decoder_register_test_doavoid(decoder, handle_test_doavoid, context);
  labcomm2014_decoder_register_test_intAndRef(decoder, handle_test_intAndRef, context);
  labcomm2014_decoder_sample_ref_register(decoder,labcomm2014_signature_test_doavoid );

  labcomm2014_decoder_register_test_twoInts(decoder, handle_test_twoInts, context);
  labcomm2014_decoder_register_test_theFirstInt(decoder, handle_test_theFirstInt, context);
  labcomm2014_decoder_register_test_theSecondInt(decoder, handle_test_theSecondInt, context);
  labcomm2014_decoder_register_test_twoLines(decoder, handle_test_twoLines, context);
  labcomm2014_decoder_register_labcomm2014_type_def(decoder, handle_type_def, context);
  labcomm2014_decoder_register_labcomm2014_type_binding(decoder, handle_type_binding, context);

  printf("Decoding:\n");
  labcomm2014_decoder_run(decoder);
  printf("--- End Of File ---:\n");
  labcomm2014_decoder_free(decoder);

  return 0;
}
