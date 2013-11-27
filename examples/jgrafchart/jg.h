/* LabComm declarations:
sample struct {
  double b;
  int c;
  int d;
  string e;
  boolean f;
  short g;
  long h;
  float i;
} foo;
*/


#ifndef __LABCOMM_jg_H__
#define __LABCOMM_jg_H__

#include <stdint.h>
#include "labcomm.h"

#ifndef PREDEFINED_jg_foo
typedef struct {
  double b;
  int32_t c;
  int32_t d;
  char* e;
  uint8_t f;
  int16_t g;
  int64_t h;
  float i;
} jg_foo;
#endif
int labcomm_decoder_register_jg_foo(
  struct labcomm_decoder *d,
  void (*handler)(
    jg_foo *v,
    void *context
  ),
  void *context
);
int labcomm_decoder_ioctl_jg_foo(
  struct labcomm_decoder *d,
  int ioctl_action,
  ...
);
int labcomm_encoder_register_jg_foo(
  struct labcomm_encoder *e);
int labcomm_encode_jg_foo(
  struct labcomm_encoder *e,
  jg_foo *v
);
int labcomm_encoder_ioctl_jg_foo(
  struct labcomm_encoder *e,
  int ioctl_action,
  ...
);
extern int labcomm_sizeof_jg_foo(jg_foo *v);

void init_jg__signatures(void);
#define LABCOMM_FORALL_SAMPLES_jg(func, sep) \
  func(foo, jg_foo)
#endif
