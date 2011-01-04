/* LabComm declarations:
typedef int int_array_ss[1][_, 3, _][5];
sample struct {
  int aa;
  boolean bb;
  int_array_ss ias;
} struct_array_ss[1][_][_];
typedef int int_t;
typedef int int_array_t[1][2][3, 4];
typedef struct {
  int a;
} struct_t;
typedef struct {
  int a;
} struct_array_t[2];
typedef struct {
  struct {
    int a;
  } a;
} struct_struct_t;
sample int int_s;
sample int int_array_s[1][_, 3, _][5];
sample struct {
  int a;
  double bcd;
} struct_s;
sample struct {
  int a;
} struct_array_s[2];
sample struct {
  struct {
    int a;
  } a;
} struct_struct_s;
sample int_t int_t_s;
sample int_array_t int_array_t_s;
sample struct_t struct_t_s;
sample struct_array_t struct_array_t_s;
sample struct_struct_t struct_struct_t_s;
*/


#ifndef __LABCOMM_nested_H__
#define __LABCOMM_nested_H__

#include "labcomm.h"

#ifndef PREDEFINED_nested_int_array_ss
typedef struct {
  struct {
    int n_0;
    // n_1=3
    int n_2;
    struct {
      int a[5];
    } *a;
  } a[1];
} nested_int_array_ss;
#endif

#ifndef PREDEFINED_nested_struct_array_ss
typedef struct {
  struct {
    int n_0;
    struct {
      int n_0;
      struct {
        int aa;
        unsigned char bb;
        nested_int_array_ss ias;
      } *a;
    } *a;
  } a[1];
} nested_struct_array_ss;
#endif
void labcomm_decoder_register_nested_struct_array_ss(
  struct labcomm_decoder *d,
  void (*handler)(
    nested_struct_array_ss *v,
    void *context
  ),
  void *context
);
void labcomm_encoder_register_nested_struct_array_ss(
  struct labcomm_encoder *e);
void labcomm_encode_nested_struct_array_ss(
  struct labcomm_encoder *e,
  nested_struct_array_ss *v
);
extern int labcomm_sizeof_nested_struct_array_ss(nested_struct_array_ss *v);

#ifndef PREDEFINED_nested_int_t
typedef int nested_int_t;
#endif

#ifndef PREDEFINED_nested_int_array_t
typedef struct {
  struct {
    struct {
      int a[3][4];
    } a[2];
  } a[1];
} nested_int_array_t;
#endif

#ifndef PREDEFINED_nested_struct_t
typedef struct {
  int a;
} nested_struct_t;
#endif

#ifndef PREDEFINED_nested_struct_array_t
typedef struct {
  struct {
    int a;
  } a[2];
} nested_struct_array_t;
#endif

#ifndef PREDEFINED_nested_struct_struct_t
typedef struct {
  struct {
    int a;
  } a;
} nested_struct_struct_t;
#endif

#ifndef PREDEFINED_nested_int_s
typedef int nested_int_s;
#endif
void labcomm_decoder_register_nested_int_s(
  struct labcomm_decoder *d,
  void (*handler)(
    nested_int_s *v,
    void *context
  ),
  void *context
);
void labcomm_encoder_register_nested_int_s(
  struct labcomm_encoder *e);
void labcomm_encode_nested_int_s(
  struct labcomm_encoder *e,
  nested_int_s *v
);
extern int labcomm_sizeof_nested_int_s(nested_int_s *v);

#ifndef PREDEFINED_nested_int_array_s
typedef struct {
  struct {
    int n_0;
    // n_1=3
    int n_2;
    struct {
      int a[5];
    } *a;
  } a[1];
} nested_int_array_s;
#endif
void labcomm_decoder_register_nested_int_array_s(
  struct labcomm_decoder *d,
  void (*handler)(
    nested_int_array_s *v,
    void *context
  ),
  void *context
);
void labcomm_encoder_register_nested_int_array_s(
  struct labcomm_encoder *e);
void labcomm_encode_nested_int_array_s(
  struct labcomm_encoder *e,
  nested_int_array_s *v
);
extern int labcomm_sizeof_nested_int_array_s(nested_int_array_s *v);

#ifndef PREDEFINED_nested_struct_s
typedef struct {
  int a;
  double bcd;
} nested_struct_s;
#endif
void labcomm_decoder_register_nested_struct_s(
  struct labcomm_decoder *d,
  void (*handler)(
    nested_struct_s *v,
    void *context
  ),
  void *context
);
void labcomm_encoder_register_nested_struct_s(
  struct labcomm_encoder *e);
void labcomm_encode_nested_struct_s(
  struct labcomm_encoder *e,
  nested_struct_s *v
);
extern int labcomm_sizeof_nested_struct_s(nested_struct_s *v);

#ifndef PREDEFINED_nested_struct_array_s
typedef struct {
  struct {
    int a;
  } a[2];
} nested_struct_array_s;
#endif
void labcomm_decoder_register_nested_struct_array_s(
  struct labcomm_decoder *d,
  void (*handler)(
    nested_struct_array_s *v,
    void *context
  ),
  void *context
);
void labcomm_encoder_register_nested_struct_array_s(
  struct labcomm_encoder *e);
void labcomm_encode_nested_struct_array_s(
  struct labcomm_encoder *e,
  nested_struct_array_s *v
);
extern int labcomm_sizeof_nested_struct_array_s(nested_struct_array_s *v);

#ifndef PREDEFINED_nested_struct_struct_s
typedef struct {
  struct {
    int a;
  } a;
} nested_struct_struct_s;
#endif
void labcomm_decoder_register_nested_struct_struct_s(
  struct labcomm_decoder *d,
  void (*handler)(
    nested_struct_struct_s *v,
    void *context
  ),
  void *context
);
void labcomm_encoder_register_nested_struct_struct_s(
  struct labcomm_encoder *e);
void labcomm_encode_nested_struct_struct_s(
  struct labcomm_encoder *e,
  nested_struct_struct_s *v
);
extern int labcomm_sizeof_nested_struct_struct_s(nested_struct_struct_s *v);

#ifndef PREDEFINED_nested_int_t_s
typedef nested_int_t nested_int_t_s;
#endif
void labcomm_decoder_register_nested_int_t_s(
  struct labcomm_decoder *d,
  void (*handler)(
    nested_int_t_s *v,
    void *context
  ),
  void *context
);
void labcomm_encoder_register_nested_int_t_s(
  struct labcomm_encoder *e);
void labcomm_encode_nested_int_t_s(
  struct labcomm_encoder *e,
  nested_int_t_s *v
);
extern int labcomm_sizeof_nested_int_t_s(nested_int_t_s *v);

#ifndef PREDEFINED_nested_int_array_t_s
typedef nested_int_array_t nested_int_array_t_s;
#endif
void labcomm_decoder_register_nested_int_array_t_s(
  struct labcomm_decoder *d,
  void (*handler)(
    nested_int_array_t_s *v,
    void *context
  ),
  void *context
);
void labcomm_encoder_register_nested_int_array_t_s(
  struct labcomm_encoder *e);
void labcomm_encode_nested_int_array_t_s(
  struct labcomm_encoder *e,
  nested_int_array_t_s *v
);
extern int labcomm_sizeof_nested_int_array_t_s(nested_int_array_t_s *v);

#ifndef PREDEFINED_nested_struct_t_s
typedef nested_struct_t nested_struct_t_s;
#endif
void labcomm_decoder_register_nested_struct_t_s(
  struct labcomm_decoder *d,
  void (*handler)(
    nested_struct_t_s *v,
    void *context
  ),
  void *context
);
void labcomm_encoder_register_nested_struct_t_s(
  struct labcomm_encoder *e);
void labcomm_encode_nested_struct_t_s(
  struct labcomm_encoder *e,
  nested_struct_t_s *v
);
extern int labcomm_sizeof_nested_struct_t_s(nested_struct_t_s *v);

#ifndef PREDEFINED_nested_struct_array_t_s
typedef nested_struct_array_t nested_struct_array_t_s;
#endif
void labcomm_decoder_register_nested_struct_array_t_s(
  struct labcomm_decoder *d,
  void (*handler)(
    nested_struct_array_t_s *v,
    void *context
  ),
  void *context
);
void labcomm_encoder_register_nested_struct_array_t_s(
  struct labcomm_encoder *e);
void labcomm_encode_nested_struct_array_t_s(
  struct labcomm_encoder *e,
  nested_struct_array_t_s *v
);
extern int labcomm_sizeof_nested_struct_array_t_s(nested_struct_array_t_s *v);

#ifndef PREDEFINED_nested_struct_struct_t_s
typedef nested_struct_struct_t nested_struct_struct_t_s;
#endif
void labcomm_decoder_register_nested_struct_struct_t_s(
  struct labcomm_decoder *d,
  void (*handler)(
    nested_struct_struct_t_s *v,
    void *context
  ),
  void *context
);
void labcomm_encoder_register_nested_struct_struct_t_s(
  struct labcomm_encoder *e);
void labcomm_encode_nested_struct_struct_t_s(
  struct labcomm_encoder *e,
  nested_struct_struct_t_s *v
);
extern int labcomm_sizeof_nested_struct_struct_t_s(nested_struct_struct_t_s *v);

#define LABCOMM_FORALL_SAMPLES_nested(func, sep) \
  func(struct_array_ss, nested_struct_array_ss) sep \
  func(int_s, nested_int_s) sep \
  func(int_array_s, nested_int_array_s) sep \
  func(struct_s, nested_struct_s) sep \
  func(struct_array_s, nested_struct_array_s) sep \
  func(struct_struct_s, nested_struct_struct_s) sep \
  func(int_t_s, nested_int_t_s) sep \
  func(int_array_t_s, nested_int_array_t_s) sep \
  func(struct_t_s, nested_struct_t_s) sep \
  func(struct_array_t_s, nested_struct_array_t_s) sep \
  func(struct_struct_t_s, nested_struct_struct_t_s)
#endif
