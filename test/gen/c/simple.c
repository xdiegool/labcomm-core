#include "labcomm.h"
#include "labcomm_private.h"
#include "gen/c/simple.h"

static unsigned char signature_bytes_an_int[] = {
0, 0, 0, 35, 
};
labcomm_signature_t labcomm_signature_simple_an_int = {
  LABCOMM_SAMPLE, "an_int",
  (int (*)(void *))labcomm_sizeof_simple_an_int,
  sizeof(signature_bytes_an_int),
  signature_bytes_an_int
 };
static unsigned char signature_bytes_a_fixed_int_array[] = {
// array [2]
0, 0, 0, 16, 
  0, 0, 0, 1, 
  0, 0, 0, 2, 
  0, 0, 0, 35, 
// }
};
labcomm_signature_t labcomm_signature_simple_a_fixed_int_array = {
  LABCOMM_SAMPLE, "a_fixed_int_array",
  (int (*)(void *))labcomm_sizeof_simple_a_fixed_int_array,
  sizeof(signature_bytes_a_fixed_int_array),
  signature_bytes_a_fixed_int_array
 };
static unsigned char signature_bytes_a_fixed_int_multi_array[] = {
// array [2, 2, 2]
0, 0, 0, 16, 
  0, 0, 0, 3, 
  0, 0, 0, 2, 
  0, 0, 0, 2, 
  0, 0, 0, 2, 
  0, 0, 0, 35, 
// }
};
labcomm_signature_t labcomm_signature_simple_a_fixed_int_multi_array = {
  LABCOMM_SAMPLE, "a_fixed_int_multi_array",
  (int (*)(void *))labcomm_sizeof_simple_a_fixed_int_multi_array,
  sizeof(signature_bytes_a_fixed_int_multi_array),
  signature_bytes_a_fixed_int_multi_array
 };
static unsigned char signature_bytes_a_fixed_int_array_array_array[] = {
// array [2]
0, 0, 0, 16, 
  0, 0, 0, 1, 
  0, 0, 0, 2, 
  // array [2]
  0, 0, 0, 16, 
    0, 0, 0, 1, 
    0, 0, 0, 2, 
    // array [2]
    0, 0, 0, 16, 
      0, 0, 0, 1, 
      0, 0, 0, 2, 
      0, 0, 0, 35, 
    // }
  // }
// }
};
labcomm_signature_t labcomm_signature_simple_a_fixed_int_array_array_array = {
  LABCOMM_SAMPLE, "a_fixed_int_array_array_array",
  (int (*)(void *))labcomm_sizeof_simple_a_fixed_int_array_array_array,
  sizeof(signature_bytes_a_fixed_int_array_array_array),
  signature_bytes_a_fixed_int_array_array_array
 };
static unsigned char signature_bytes_a_variable_int_array[] = {
// array [_]
0, 0, 0, 16, 
  0, 0, 0, 1, 
  0, 0, 0, 0, 
  0, 0, 0, 35, 
// }
};
labcomm_signature_t labcomm_signature_simple_a_variable_int_array = {
  LABCOMM_SAMPLE, "a_variable_int_array",
  (int (*)(void *))labcomm_sizeof_simple_a_variable_int_array,
  sizeof(signature_bytes_a_variable_int_array),
  signature_bytes_a_variable_int_array
 };
static unsigned char signature_bytes_a_variable_int_multi_array[] = {
// array [_, _, _]
0, 0, 0, 16, 
  0, 0, 0, 3, 
  0, 0, 0, 0, 
  0, 0, 0, 0, 
  0, 0, 0, 0, 
  0, 0, 0, 35, 
// }
};
labcomm_signature_t labcomm_signature_simple_a_variable_int_multi_array = {
  LABCOMM_SAMPLE, "a_variable_int_multi_array",
  (int (*)(void *))labcomm_sizeof_simple_a_variable_int_multi_array,
  sizeof(signature_bytes_a_variable_int_multi_array),
  signature_bytes_a_variable_int_multi_array
 };
static unsigned char signature_bytes_a_variable_int_array_array_array[] = {
// array [_]
0, 0, 0, 16, 
  0, 0, 0, 1, 
  0, 0, 0, 0, 
  // array [_]
  0, 0, 0, 16, 
    0, 0, 0, 1, 
    0, 0, 0, 0, 
    // array [_]
    0, 0, 0, 16, 
      0, 0, 0, 1, 
      0, 0, 0, 0, 
      0, 0, 0, 35, 
    // }
  // }
// }
};
labcomm_signature_t labcomm_signature_simple_a_variable_int_array_array_array = {
  LABCOMM_SAMPLE, "a_variable_int_array_array_array",
  (int (*)(void *))labcomm_sizeof_simple_a_variable_int_array_array_array,
  sizeof(signature_bytes_a_variable_int_array_array_array),
  signature_bytes_a_variable_int_array_array_array
 };
static unsigned char signature_bytes_an_int_struct[] = {
// struct { 2 fields
0, 0, 0, 17, 
  0, 0, 0, 2, 
  // int 'a'
  0, 0, 0, 1, 
  97, 
  0, 0, 0, 35, 
  // int 'b'
  0, 0, 0, 1, 
  98, 
  0, 0, 0, 35, 
// }
};
labcomm_signature_t labcomm_signature_simple_an_int_struct = {
  LABCOMM_SAMPLE, "an_int_struct",
  (int (*)(void *))labcomm_sizeof_simple_an_int_struct,
  sizeof(signature_bytes_an_int_struct),
  signature_bytes_an_int_struct
 };
static unsigned char signature_bytes_a_void[] = {
// void
0, 0, 0, 17, 
0, 0, 0, 0, 
};
labcomm_signature_t labcomm_signature_simple_a_void = {
  LABCOMM_SAMPLE, "a_void",
  (int (*)(void *))labcomm_sizeof_simple_a_void,
  sizeof(signature_bytes_a_void),
  signature_bytes_a_void
 };
static void decode_an_int(
  labcomm_decoder_t *d,
  void (*handle)(
    simple_an_int *v,
    void *context
  ),
  void *context
)
{
  simple_an_int v;
  v = labcomm_decode_int(d);
  handle(&v, context);
}
void labcomm_decoder_register_simple_an_int(
  struct labcomm_decoder *d,
  void (*handler)(
    simple_an_int *v,
    void *context
  ),
  void *context
)
{
  labcomm_internal_decoder_register(
    d,
    &labcomm_signature_simple_an_int,
    (labcomm_decoder_typecast_t)decode_an_int,
    (labcomm_handler_typecast_t)handler,
    context
  );
}
static void encode_an_int(
  labcomm_encoder_t *e,
  simple_an_int *v
)
{
  e->writer.write(&e->writer, labcomm_writer_start);
  labcomm_encode_type_index(e, &labcomm_signature_simple_an_int);
  {
    labcomm_encode_int(e, (*v));
  }
  e->writer.write(&e->writer, labcomm_writer_end);
}
void labcomm_encode_simple_an_int(
labcomm_encoder_t *e,
simple_an_int *v
)
{
labcomm_internal_encode(e, &labcomm_signature_simple_an_int, v);
}
void labcomm_encoder_register_simple_an_int(
  struct labcomm_encoder *e
)
{
  labcomm_internal_encoder_register(
    e,
    &labcomm_signature_simple_an_int,
    (labcomm_encode_typecast_t)encode_an_int
  );
}
int labcomm_sizeof_simple_an_int(simple_an_int *v)
{
  return 8;
}
static void decode_a_fixed_int_array(
  labcomm_decoder_t *d,
  void (*handle)(
    simple_a_fixed_int_array *v,
    void *context
  ),
  void *context
)
{
  simple_a_fixed_int_array v;
  {
    int i_0_0;
    for (i_0_0 = 0 ; i_0_0 < 2 ; i_0_0++) {
      v.a[i_0_0] = labcomm_decode_int(d);
    }
  }
  handle(&v, context);
}
void labcomm_decoder_register_simple_a_fixed_int_array(
  struct labcomm_decoder *d,
  void (*handler)(
    simple_a_fixed_int_array *v,
    void *context
  ),
  void *context
)
{
  labcomm_internal_decoder_register(
    d,
    &labcomm_signature_simple_a_fixed_int_array,
    (labcomm_decoder_typecast_t)decode_a_fixed_int_array,
    (labcomm_handler_typecast_t)handler,
    context
  );
}
static void encode_a_fixed_int_array(
  labcomm_encoder_t *e,
  simple_a_fixed_int_array *v
)
{
  e->writer.write(&e->writer, labcomm_writer_start);
  labcomm_encode_type_index(e, &labcomm_signature_simple_a_fixed_int_array);
  {
    {
      int i_0_0;
      for (i_0_0 = 0 ; i_0_0 < 2 ; i_0_0++) {
        labcomm_encode_int(e, (*v).a[i_0_0]);
      }
    }
  }
  e->writer.write(&e->writer, labcomm_writer_end);
}
void labcomm_encode_simple_a_fixed_int_array(
labcomm_encoder_t *e,
simple_a_fixed_int_array *v
)
{
labcomm_internal_encode(e, &labcomm_signature_simple_a_fixed_int_array, v);
}
void labcomm_encoder_register_simple_a_fixed_int_array(
  struct labcomm_encoder *e
)
{
  labcomm_internal_encoder_register(
    e,
    &labcomm_signature_simple_a_fixed_int_array,
    (labcomm_encode_typecast_t)encode_a_fixed_int_array
  );
}
int labcomm_sizeof_simple_a_fixed_int_array(simple_a_fixed_int_array *v)
{
  return 12;
}
static void decode_a_fixed_int_multi_array(
  labcomm_decoder_t *d,
  void (*handle)(
    simple_a_fixed_int_multi_array *v,
    void *context
  ),
  void *context
)
{
  simple_a_fixed_int_multi_array v;
  {
    int i_0_0;
    int i_0_1;
    int i_0_2;
    for (i_0_0 = 0 ; i_0_0 < 2 ; i_0_0++) {
      for (i_0_1 = 0 ; i_0_1 < 2 ; i_0_1++) {
        for (i_0_2 = 0 ; i_0_2 < 2 ; i_0_2++) {
          v.a[i_0_0][i_0_1][i_0_2] = labcomm_decode_int(d);
        }
      }
    }
  }
  handle(&v, context);
}
void labcomm_decoder_register_simple_a_fixed_int_multi_array(
  struct labcomm_decoder *d,
  void (*handler)(
    simple_a_fixed_int_multi_array *v,
    void *context
  ),
  void *context
)
{
  labcomm_internal_decoder_register(
    d,
    &labcomm_signature_simple_a_fixed_int_multi_array,
    (labcomm_decoder_typecast_t)decode_a_fixed_int_multi_array,
    (labcomm_handler_typecast_t)handler,
    context
  );
}
static void encode_a_fixed_int_multi_array(
  labcomm_encoder_t *e,
  simple_a_fixed_int_multi_array *v
)
{
  e->writer.write(&e->writer, labcomm_writer_start);
  labcomm_encode_type_index(e, &labcomm_signature_simple_a_fixed_int_multi_array);
  {
    {
      int i_0_0;
      int i_0_1;
      int i_0_2;
      for (i_0_0 = 0 ; i_0_0 < 2 ; i_0_0++) {
        for (i_0_1 = 0 ; i_0_1 < 2 ; i_0_1++) {
          for (i_0_2 = 0 ; i_0_2 < 2 ; i_0_2++) {
            labcomm_encode_int(e, (*v).a[i_0_0][i_0_1][i_0_2]);
          }
        }
      }
    }
  }
  e->writer.write(&e->writer, labcomm_writer_end);
}
void labcomm_encode_simple_a_fixed_int_multi_array(
labcomm_encoder_t *e,
simple_a_fixed_int_multi_array *v
)
{
labcomm_internal_encode(e, &labcomm_signature_simple_a_fixed_int_multi_array, v);
}
void labcomm_encoder_register_simple_a_fixed_int_multi_array(
  struct labcomm_encoder *e
)
{
  labcomm_internal_encoder_register(
    e,
    &labcomm_signature_simple_a_fixed_int_multi_array,
    (labcomm_encode_typecast_t)encode_a_fixed_int_multi_array
  );
}
int labcomm_sizeof_simple_a_fixed_int_multi_array(simple_a_fixed_int_multi_array *v)
{
  return 36;
}
static void decode_a_fixed_int_array_array_array(
  labcomm_decoder_t *d,
  void (*handle)(
    simple_a_fixed_int_array_array_array *v,
    void *context
  ),
  void *context
)
{
  simple_a_fixed_int_array_array_array v;
  {
    int i_0_0;
    for (i_0_0 = 0 ; i_0_0 < 2 ; i_0_0++) {
      {
        int i_1_0;
        for (i_1_0 = 0 ; i_1_0 < 2 ; i_1_0++) {
          {
            int i_2_0;
            for (i_2_0 = 0 ; i_2_0 < 2 ; i_2_0++) {
              v.a[i_0_0].a[i_1_0].a[i_2_0] = labcomm_decode_int(d);
            }
          }
        }
      }
    }
  }
  handle(&v, context);
}
void labcomm_decoder_register_simple_a_fixed_int_array_array_array(
  struct labcomm_decoder *d,
  void (*handler)(
    simple_a_fixed_int_array_array_array *v,
    void *context
  ),
  void *context
)
{
  labcomm_internal_decoder_register(
    d,
    &labcomm_signature_simple_a_fixed_int_array_array_array,
    (labcomm_decoder_typecast_t)decode_a_fixed_int_array_array_array,
    (labcomm_handler_typecast_t)handler,
    context
  );
}
static void encode_a_fixed_int_array_array_array(
  labcomm_encoder_t *e,
  simple_a_fixed_int_array_array_array *v
)
{
  e->writer.write(&e->writer, labcomm_writer_start);
  labcomm_encode_type_index(e, &labcomm_signature_simple_a_fixed_int_array_array_array);
  {
    {
      int i_0_0;
      for (i_0_0 = 0 ; i_0_0 < 2 ; i_0_0++) {
        {
          int i_1_0;
          for (i_1_0 = 0 ; i_1_0 < 2 ; i_1_0++) {
            {
              int i_2_0;
              for (i_2_0 = 0 ; i_2_0 < 2 ; i_2_0++) {
                labcomm_encode_int(e, (*v).a[i_0_0].a[i_1_0].a[i_2_0]);
              }
            }
          }
        }
      }
    }
  }
  e->writer.write(&e->writer, labcomm_writer_end);
}
void labcomm_encode_simple_a_fixed_int_array_array_array(
labcomm_encoder_t *e,
simple_a_fixed_int_array_array_array *v
)
{
labcomm_internal_encode(e, &labcomm_signature_simple_a_fixed_int_array_array_array, v);
}
void labcomm_encoder_register_simple_a_fixed_int_array_array_array(
  struct labcomm_encoder *e
)
{
  labcomm_internal_encoder_register(
    e,
    &labcomm_signature_simple_a_fixed_int_array_array_array,
    (labcomm_encode_typecast_t)encode_a_fixed_int_array_array_array
  );
}
int labcomm_sizeof_simple_a_fixed_int_array_array_array(simple_a_fixed_int_array_array_array *v)
{
  return 36;
}
static void decode_a_variable_int_array(
  labcomm_decoder_t *d,
  void (*handle)(
    simple_a_variable_int_array *v,
    void *context
  ),
  void *context
)
{
  simple_a_variable_int_array v;
  v.n_0 = labcomm_decode_int(d);
  v.a = malloc(sizeof(v.a[0]) * v.n_0);
  {
    int i_0_0;
    for (i_0_0 = 0 ; i_0_0 < v.n_0 ; i_0_0++) {
      int i_0 = i_0_0;
      v.a[i_0] = labcomm_decode_int(d);
    }
  }
  handle(&v, context);
  {
    free(v.a);
  }
}
void labcomm_decoder_register_simple_a_variable_int_array(
  struct labcomm_decoder *d,
  void (*handler)(
    simple_a_variable_int_array *v,
    void *context
  ),
  void *context
)
{
  labcomm_internal_decoder_register(
    d,
    &labcomm_signature_simple_a_variable_int_array,
    (labcomm_decoder_typecast_t)decode_a_variable_int_array,
    (labcomm_handler_typecast_t)handler,
    context
  );
}
static void encode_a_variable_int_array(
  labcomm_encoder_t *e,
  simple_a_variable_int_array *v
)
{
  e->writer.write(&e->writer, labcomm_writer_start);
  labcomm_encode_type_index(e, &labcomm_signature_simple_a_variable_int_array);
  {
    labcomm_encode_int(e, (*v).n_0);
    {
      int i_0_0;
      for (i_0_0 = 0 ; i_0_0 < (*v).n_0 ; i_0_0++) {
        int i_0 = i_0_0;
        labcomm_encode_int(e, (*v).a[i_0]);
      }
    }
  }
  e->writer.write(&e->writer, labcomm_writer_end);
}
void labcomm_encode_simple_a_variable_int_array(
labcomm_encoder_t *e,
simple_a_variable_int_array *v
)
{
labcomm_internal_encode(e, &labcomm_signature_simple_a_variable_int_array, v);
}
void labcomm_encoder_register_simple_a_variable_int_array(
  struct labcomm_encoder *e
)
{
  labcomm_internal_encoder_register(
    e,
    &labcomm_signature_simple_a_variable_int_array,
    (labcomm_encode_typecast_t)encode_a_variable_int_array
  );
}
int labcomm_sizeof_simple_a_variable_int_array(simple_a_variable_int_array *v)
{
  int result = 4;
  result += 4 * (*v).n_0;
  return result;
}
static void decode_a_variable_int_multi_array(
  labcomm_decoder_t *d,
  void (*handle)(
    simple_a_variable_int_multi_array *v,
    void *context
  ),
  void *context
)
{
  simple_a_variable_int_multi_array v;
  v.n_0 = labcomm_decode_int(d);
  v.n_1 = labcomm_decode_int(d);
  v.n_2 = labcomm_decode_int(d);
  v.a = malloc(sizeof(v.a[0]) * v.n_0 * v.n_1 * v.n_2);
  {
    int i_0_0;
    int i_0_1;
    int i_0_2;
    for (i_0_0 = 0 ; i_0_0 < v.n_0 ; i_0_0++) {
      for (i_0_1 = 0 ; i_0_1 < v.n_1 ; i_0_1++) {
        for (i_0_2 = 0 ; i_0_2 < v.n_2 ; i_0_2++) {
          int i_0 = ((i_0_0) * v.n_1 + i_0_1) * v.n_2 + i_0_2;
          v.a[i_0] = labcomm_decode_int(d);
        }
      }
    }
  }
  handle(&v, context);
  {
    free(v.a);
  }
}
void labcomm_decoder_register_simple_a_variable_int_multi_array(
  struct labcomm_decoder *d,
  void (*handler)(
    simple_a_variable_int_multi_array *v,
    void *context
  ),
  void *context
)
{
  labcomm_internal_decoder_register(
    d,
    &labcomm_signature_simple_a_variable_int_multi_array,
    (labcomm_decoder_typecast_t)decode_a_variable_int_multi_array,
    (labcomm_handler_typecast_t)handler,
    context
  );
}
static void encode_a_variable_int_multi_array(
  labcomm_encoder_t *e,
  simple_a_variable_int_multi_array *v
)
{
  e->writer.write(&e->writer, labcomm_writer_start);
  labcomm_encode_type_index(e, &labcomm_signature_simple_a_variable_int_multi_array);
  {
    labcomm_encode_int(e, (*v).n_0);
    labcomm_encode_int(e, (*v).n_1);
    labcomm_encode_int(e, (*v).n_2);
    {
      int i_0_0;
      int i_0_1;
      int i_0_2;
      for (i_0_0 = 0 ; i_0_0 < (*v).n_0 ; i_0_0++) {
        for (i_0_1 = 0 ; i_0_1 < (*v).n_1 ; i_0_1++) {
          for (i_0_2 = 0 ; i_0_2 < (*v).n_2 ; i_0_2++) {
            int i_0 = ((i_0_0) * (*v).n_1 + i_0_1) * (*v).n_2 + i_0_2;
            labcomm_encode_int(e, (*v).a[i_0]);
          }
        }
      }
    }
  }
  e->writer.write(&e->writer, labcomm_writer_end);
}
void labcomm_encode_simple_a_variable_int_multi_array(
labcomm_encoder_t *e,
simple_a_variable_int_multi_array *v
)
{
labcomm_internal_encode(e, &labcomm_signature_simple_a_variable_int_multi_array, v);
}
void labcomm_encoder_register_simple_a_variable_int_multi_array(
  struct labcomm_encoder *e
)
{
  labcomm_internal_encoder_register(
    e,
    &labcomm_signature_simple_a_variable_int_multi_array,
    (labcomm_encode_typecast_t)encode_a_variable_int_multi_array
  );
}
int labcomm_sizeof_simple_a_variable_int_multi_array(simple_a_variable_int_multi_array *v)
{
  int result = 4;
  result += 4 * (*v).n_0 * (*v).n_1 * (*v).n_2;
  return result;
}
static void decode_a_variable_int_array_array_array(
  labcomm_decoder_t *d,
  void (*handle)(
    simple_a_variable_int_array_array_array *v,
    void *context
  ),
  void *context
)
{
  simple_a_variable_int_array_array_array v;
  v.n_0 = labcomm_decode_int(d);
  v.a = malloc(sizeof(v.a[0]) * v.n_0);
  {
    int i_0_0;
    for (i_0_0 = 0 ; i_0_0 < v.n_0 ; i_0_0++) {
      int i_0 = i_0_0;
      v.a[i_0].n_0 = labcomm_decode_int(d);
      v.a[i_0].a = malloc(sizeof(v.a[i_0].a[0]) * v.a[i_0].n_0);
      {
        int i_1_0;
        for (i_1_0 = 0 ; i_1_0 < v.a[i_0].n_0 ; i_1_0++) {
          int i_1 = i_1_0;
          v.a[i_0].a[i_1].n_0 = labcomm_decode_int(d);
          v.a[i_0].a[i_1].a = malloc(sizeof(v.a[i_0].a[i_1].a[0]) * v.a[i_0].a[i_1].n_0);
          {
            int i_2_0;
            for (i_2_0 = 0 ; i_2_0 < v.a[i_0].a[i_1].n_0 ; i_2_0++) {
              int i_2 = i_2_0;
              v.a[i_0].a[i_1].a[i_2] = labcomm_decode_int(d);
            }
          }
        }
      }
    }
  }
  handle(&v, context);
  {
    {
      int i_0_0;
      for (i_0_0 = 0 ; i_0_0 < v.n_0 ; i_0_0++) {
        int i_0 = i_0_0;
        {
          int i_1_0;
          for (i_1_0 = 0 ; i_1_0 < v.a[i_0].n_0 ; i_1_0++) {
            int i_1 = i_1_0;
            free(v.a[i_0].a[i_1].a);
          }
        }
        free(v.a[i_0].a);
      }
    }
    free(v.a);
  }
}
void labcomm_decoder_register_simple_a_variable_int_array_array_array(
  struct labcomm_decoder *d,
  void (*handler)(
    simple_a_variable_int_array_array_array *v,
    void *context
  ),
  void *context
)
{
  labcomm_internal_decoder_register(
    d,
    &labcomm_signature_simple_a_variable_int_array_array_array,
    (labcomm_decoder_typecast_t)decode_a_variable_int_array_array_array,
    (labcomm_handler_typecast_t)handler,
    context
  );
}
static void encode_a_variable_int_array_array_array(
  labcomm_encoder_t *e,
  simple_a_variable_int_array_array_array *v
)
{
  e->writer.write(&e->writer, labcomm_writer_start);
  labcomm_encode_type_index(e, &labcomm_signature_simple_a_variable_int_array_array_array);
  {
    labcomm_encode_int(e, (*v).n_0);
    {
      int i_0_0;
      for (i_0_0 = 0 ; i_0_0 < (*v).n_0 ; i_0_0++) {
        int i_0 = i_0_0;
        labcomm_encode_int(e, (*v).a[i_0].n_0);
        {
          int i_1_0;
          for (i_1_0 = 0 ; i_1_0 < (*v).a[i_0].n_0 ; i_1_0++) {
            int i_1 = i_1_0;
            labcomm_encode_int(e, (*v).a[i_0].a[i_1].n_0);
            {
              int i_2_0;
              for (i_2_0 = 0 ; i_2_0 < (*v).a[i_0].a[i_1].n_0 ; i_2_0++) {
                int i_2 = i_2_0;
                labcomm_encode_int(e, (*v).a[i_0].a[i_1].a[i_2]);
              }
            }
          }
        }
      }
    }
  }
  e->writer.write(&e->writer, labcomm_writer_end);
}
void labcomm_encode_simple_a_variable_int_array_array_array(
labcomm_encoder_t *e,
simple_a_variable_int_array_array_array *v
)
{
labcomm_internal_encode(e, &labcomm_signature_simple_a_variable_int_array_array_array, v);
}
void labcomm_encoder_register_simple_a_variable_int_array_array_array(
  struct labcomm_encoder *e
)
{
  labcomm_internal_encoder_register(
    e,
    &labcomm_signature_simple_a_variable_int_array_array_array,
    (labcomm_encode_typecast_t)encode_a_variable_int_array_array_array
  );
}
int labcomm_sizeof_simple_a_variable_int_array_array_array(simple_a_variable_int_array_array_array *v)
{
  int result = 4;
  {
    int i_0_0;
    for (i_0_0 = 0 ; i_0_0 < (*v).n_0 ; i_0_0++) {
      int i_0 = i_0_0;
      {
        int i_1_0;
        for (i_1_0 = 0 ; i_1_0 < (*v).a[i_0].n_0 ; i_1_0++) {
          int i_1 = i_1_0;
          result += 4 * (*v).a[i_0].a[i_1].n_0;
        }
      }
    }
  }
  return result;
}
static void decode_an_int_struct(
  labcomm_decoder_t *d,
  void (*handle)(
    simple_an_int_struct *v,
    void *context
  ),
  void *context
)
{
  simple_an_int_struct v;
  v.a = labcomm_decode_int(d);
  v.b = labcomm_decode_int(d);
  handle(&v, context);
}
void labcomm_decoder_register_simple_an_int_struct(
  struct labcomm_decoder *d,
  void (*handler)(
    simple_an_int_struct *v,
    void *context
  ),
  void *context
)
{
  labcomm_internal_decoder_register(
    d,
    &labcomm_signature_simple_an_int_struct,
    (labcomm_decoder_typecast_t)decode_an_int_struct,
    (labcomm_handler_typecast_t)handler,
    context
  );
}
static void encode_an_int_struct(
  labcomm_encoder_t *e,
  simple_an_int_struct *v
)
{
  e->writer.write(&e->writer, labcomm_writer_start);
  labcomm_encode_type_index(e, &labcomm_signature_simple_an_int_struct);
  {
    labcomm_encode_int(e, (*v).a);
    labcomm_encode_int(e, (*v).b);
  }
  e->writer.write(&e->writer, labcomm_writer_end);
}
void labcomm_encode_simple_an_int_struct(
labcomm_encoder_t *e,
simple_an_int_struct *v
)
{
labcomm_internal_encode(e, &labcomm_signature_simple_an_int_struct, v);
}
void labcomm_encoder_register_simple_an_int_struct(
  struct labcomm_encoder *e
)
{
  labcomm_internal_encoder_register(
    e,
    &labcomm_signature_simple_an_int_struct,
    (labcomm_encode_typecast_t)encode_an_int_struct
  );
}
int labcomm_sizeof_simple_an_int_struct(simple_an_int_struct *v)
{
  return 12;
}
static void decode_a_void(
  labcomm_decoder_t *d,
  void (*handle)(
    simple_a_void *v,
    void *context
  ),
  void *context
)
{
  simple_a_void v;
  handle(&v, context);
}
void labcomm_decoder_register_simple_a_void(
  struct labcomm_decoder *d,
  void (*handler)(
    simple_a_void *v,
    void *context
  ),
  void *context
)
{
  labcomm_internal_decoder_register(
    d,
    &labcomm_signature_simple_a_void,
    (labcomm_decoder_typecast_t)decode_a_void,
    (labcomm_handler_typecast_t)handler,
    context
  );
}
static void encode_a_void(
  labcomm_encoder_t *e,
  simple_a_void *v
)
{
  e->writer.write(&e->writer, labcomm_writer_start);
  labcomm_encode_type_index(e, &labcomm_signature_simple_a_void);
  {
  }
  e->writer.write(&e->writer, labcomm_writer_end);
}
void labcomm_encode_simple_a_void(
labcomm_encoder_t *e,
simple_a_void *v
)
{
labcomm_internal_encode(e, &labcomm_signature_simple_a_void, v);
}
void labcomm_encoder_register_simple_a_void(
  struct labcomm_encoder *e
)
{
  labcomm_internal_encoder_register(
    e,
    &labcomm_signature_simple_a_void,
    (labcomm_encode_typecast_t)encode_a_void
  );
}
int labcomm_sizeof_simple_a_void(simple_a_void *v)
{
  return 4;
}
