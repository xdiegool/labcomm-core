/* LabComm declarations:
sample struct {
  int sequence;
  struct {
    boolean last;
    string data;
  } line[_];
} log_message;
sample float data;
*/


#ifndef __LABCOMM_example_H__
#define __LABCOMM_example_H__

#include "labcomm.h"

#ifndef PREDEFINED_example_log_message
typedef struct {
  int sequence;
  struct {
    int n_0;
    struct {
      unsigned char last;
      char* data;
    } *a;
  } line;
} example_log_message;
#endif
void labcomm_decoder_register_example_log_message(
  struct labcomm_decoder *d,
  void (*handler)(
    example_log_message *v,
    void *context
  ),
  void *context
);
void labcomm_encoder_register_example_log_message(
  struct labcomm_encoder *e);
void labcomm_encode_example_log_message(
  struct labcomm_encoder *e,
  example_log_message *v
);
extern int labcomm_sizeof_example_log_message(example_log_message *v);

#ifndef PREDEFINED_example_data
typedef float example_data;
#endif
void labcomm_decoder_register_example_data(
  struct labcomm_decoder *d,
  void (*handler)(
    example_data *v,
    void *context
  ),
  void *context
);
void labcomm_encoder_register_example_data(
  struct labcomm_encoder *e);
void labcomm_encode_example_data(
  struct labcomm_encoder *e,
  example_data *v
);
extern int labcomm_sizeof_example_data(example_data *v);

#define LABCOMM_FORALL_SAMPLES_example(func, sep) \
  func(log_message, example_log_message) sep \
  func(data, example_data)
#endif
