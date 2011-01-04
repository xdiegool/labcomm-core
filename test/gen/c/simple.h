/* LabComm declarations:
sample int an_int;
sample int a_fixed_int_array[2];
sample int a_fixed_int_multi_array[2, 2, 2];
sample int a_fixed_int_array_array_array[2][2][2];
sample int a_variable_int_array[_];
sample int a_variable_int_multi_array[_, _, _];
sample int a_variable_int_array_array_array[_][_][_];
sample struct {
  int a;
  int b;
} an_int_struct;
sample void a_void;
*/


#ifndef __LABCOMM_simple_H__
#define __LABCOMM_simple_H__

#include "labcomm.h"

#ifndef PREDEFINED_simple_an_int
typedef int simple_an_int;
#endif
void labcomm_decoder_register_simple_an_int(
  struct labcomm_decoder *d,
  void (*handler)(
    simple_an_int *v,
    void *context
  ),
  void *context
);
void labcomm_encoder_register_simple_an_int(
  struct labcomm_encoder *e);
void labcomm_encode_simple_an_int(
  struct labcomm_encoder *e,
  simple_an_int *v
);
extern int labcomm_sizeof_simple_an_int(simple_an_int *v);

#ifndef PREDEFINED_simple_a_fixed_int_array
typedef struct {
  int a[2];
} simple_a_fixed_int_array;
#endif
void labcomm_decoder_register_simple_a_fixed_int_array(
  struct labcomm_decoder *d,
  void (*handler)(
    simple_a_fixed_int_array *v,
    void *context
  ),
  void *context
);
void labcomm_encoder_register_simple_a_fixed_int_array(
  struct labcomm_encoder *e);
void labcomm_encode_simple_a_fixed_int_array(
  struct labcomm_encoder *e,
  simple_a_fixed_int_array *v
);
extern int labcomm_sizeof_simple_a_fixed_int_array(simple_a_fixed_int_array *v);

#ifndef PREDEFINED_simple_a_fixed_int_multi_array
typedef struct {
  int a[2][2][2];
} simple_a_fixed_int_multi_array;
#endif
void labcomm_decoder_register_simple_a_fixed_int_multi_array(
  struct labcomm_decoder *d,
  void (*handler)(
    simple_a_fixed_int_multi_array *v,
    void *context
  ),
  void *context
);
void labcomm_encoder_register_simple_a_fixed_int_multi_array(
  struct labcomm_encoder *e);
void labcomm_encode_simple_a_fixed_int_multi_array(
  struct labcomm_encoder *e,
  simple_a_fixed_int_multi_array *v
);
extern int labcomm_sizeof_simple_a_fixed_int_multi_array(simple_a_fixed_int_multi_array *v);

#ifndef PREDEFINED_simple_a_fixed_int_array_array_array
typedef struct {
  struct {
    struct {
      int a[2];
    } a[2];
  } a[2];
} simple_a_fixed_int_array_array_array;
#endif
void labcomm_decoder_register_simple_a_fixed_int_array_array_array(
  struct labcomm_decoder *d,
  void (*handler)(
    simple_a_fixed_int_array_array_array *v,
    void *context
  ),
  void *context
);
void labcomm_encoder_register_simple_a_fixed_int_array_array_array(
  struct labcomm_encoder *e);
void labcomm_encode_simple_a_fixed_int_array_array_array(
  struct labcomm_encoder *e,
  simple_a_fixed_int_array_array_array *v
);
extern int labcomm_sizeof_simple_a_fixed_int_array_array_array(simple_a_fixed_int_array_array_array *v);

#ifndef PREDEFINED_simple_a_variable_int_array
typedef struct {
  int n_0;
  int *a;
} simple_a_variable_int_array;
#endif
void labcomm_decoder_register_simple_a_variable_int_array(
  struct labcomm_decoder *d,
  void (*handler)(
    simple_a_variable_int_array *v,
    void *context
  ),
  void *context
);
void labcomm_encoder_register_simple_a_variable_int_array(
  struct labcomm_encoder *e);
void labcomm_encode_simple_a_variable_int_array(
  struct labcomm_encoder *e,
  simple_a_variable_int_array *v
);
extern int labcomm_sizeof_simple_a_variable_int_array(simple_a_variable_int_array *v);

#ifndef PREDEFINED_simple_a_variable_int_multi_array
typedef struct {
  int n_0;
  int n_1;
  int n_2;
  int *a;
} simple_a_variable_int_multi_array;
#endif
void labcomm_decoder_register_simple_a_variable_int_multi_array(
  struct labcomm_decoder *d,
  void (*handler)(
    simple_a_variable_int_multi_array *v,
    void *context
  ),
  void *context
);
void labcomm_encoder_register_simple_a_variable_int_multi_array(
  struct labcomm_encoder *e);
void labcomm_encode_simple_a_variable_int_multi_array(
  struct labcomm_encoder *e,
  simple_a_variable_int_multi_array *v
);
extern int labcomm_sizeof_simple_a_variable_int_multi_array(simple_a_variable_int_multi_array *v);

#ifndef PREDEFINED_simple_a_variable_int_array_array_array
typedef struct {
  int n_0;
  struct {
    int n_0;
    struct {
      int n_0;
      int *a;
    } *a;
  } *a;
} simple_a_variable_int_array_array_array;
#endif
void labcomm_decoder_register_simple_a_variable_int_array_array_array(
  struct labcomm_decoder *d,
  void (*handler)(
    simple_a_variable_int_array_array_array *v,
    void *context
  ),
  void *context
);
void labcomm_encoder_register_simple_a_variable_int_array_array_array(
  struct labcomm_encoder *e);
void labcomm_encode_simple_a_variable_int_array_array_array(
  struct labcomm_encoder *e,
  simple_a_variable_int_array_array_array *v
);
extern int labcomm_sizeof_simple_a_variable_int_array_array_array(simple_a_variable_int_array_array_array *v);

#ifndef PREDEFINED_simple_an_int_struct
typedef struct {
  int a;
  int b;
} simple_an_int_struct;
#endif
void labcomm_decoder_register_simple_an_int_struct(
  struct labcomm_decoder *d,
  void (*handler)(
    simple_an_int_struct *v,
    void *context
  ),
  void *context
);
void labcomm_encoder_register_simple_an_int_struct(
  struct labcomm_encoder *e);
void labcomm_encode_simple_an_int_struct(
  struct labcomm_encoder *e,
  simple_an_int_struct *v
);
extern int labcomm_sizeof_simple_an_int_struct(simple_an_int_struct *v);

#ifndef PREDEFINED_simple_a_void
typedef char simple_a_void;
#endif
void labcomm_decoder_register_simple_a_void(
  struct labcomm_decoder *d,
  void (*handler)(
    simple_a_void *v,
    void *context
  ),
  void *context
);
void labcomm_encoder_register_simple_a_void(
  struct labcomm_encoder *e);
void labcomm_encode_simple_a_void(
  struct labcomm_encoder *e,
  simple_a_void *v
);
extern int labcomm_sizeof_simple_a_void(simple_a_void *v);

#define LABCOMM_FORALL_SAMPLES_simple(func, sep) \
  func(an_int, simple_an_int) sep \
  func(a_fixed_int_array, simple_a_fixed_int_array) sep \
  func(a_fixed_int_multi_array, simple_a_fixed_int_multi_array) sep \
  func(a_fixed_int_array_array_array, simple_a_fixed_int_array_array_array) sep \
  func(a_variable_int_array, simple_a_variable_int_array) sep \
  func(a_variable_int_multi_array, simple_a_variable_int_multi_array) sep \
  func(a_variable_int_array_array_array, simple_a_variable_int_array_array_array) sep \
  func(an_int_struct, simple_an_int_struct) sep \
  func(a_void, simple_a_void)
#endif
