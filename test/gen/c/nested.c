#include "labcomm.h"
#include "labcomm_private.h"
#include "gen/c/nested.h"

static unsigned char signature_bytes_struct_array_ss[] = {
// array [1]
0, 0, 0, 16, 
  0, 0, 0, 1, 
  0, 0, 0, 1, 
  // array [_]
  0, 0, 0, 16, 
    0, 0, 0, 1, 
    0, 0, 0, 0, 
    // array [_]
    0, 0, 0, 16, 
      0, 0, 0, 1, 
      0, 0, 0, 0, 
      // struct { 3 fields
      0, 0, 0, 17, 
        0, 0, 0, 3, 
        // int 'aa'
        0, 0, 0, 2, 
        97, 97, 
        0, 0, 0, 35, 
        // boolean 'bb'
        0, 0, 0, 2, 
        98, 98, 
        0, 0, 0, 32, 
        // int_array_ss 'ias'
        0, 0, 0, 3, 
        105, 97, 115, 
        // array [1]
        0, 0, 0, 16, 
          0, 0, 0, 1, 
          0, 0, 0, 1, 
          // array [_, 3, _]
          0, 0, 0, 16, 
            0, 0, 0, 3, 
            0, 0, 0, 0, 
            0, 0, 0, 3, 
            0, 0, 0, 0, 
            // array [5]
            0, 0, 0, 16, 
              0, 0, 0, 1, 
              0, 0, 0, 5, 
              0, 0, 0, 35, 
            // }
          // }
        // }
      // }
    // }
  // }
// }
};
labcomm_signature_t labcomm_signature_nested_struct_array_ss = {
  LABCOMM_SAMPLE, "struct_array_ss",
  (int (*)(void *))labcomm_sizeof_nested_struct_array_ss,
  sizeof(signature_bytes_struct_array_ss),
  signature_bytes_struct_array_ss
 };
static unsigned char signature_bytes_int_s[] = {
0, 0, 0, 35, 
};
labcomm_signature_t labcomm_signature_nested_int_s = {
  LABCOMM_SAMPLE, "int_s",
  (int (*)(void *))labcomm_sizeof_nested_int_s,
  sizeof(signature_bytes_int_s),
  signature_bytes_int_s
 };
static unsigned char signature_bytes_int_array_s[] = {
// array [1]
0, 0, 0, 16, 
  0, 0, 0, 1, 
  0, 0, 0, 1, 
  // array [_, 3, _]
  0, 0, 0, 16, 
    0, 0, 0, 3, 
    0, 0, 0, 0, 
    0, 0, 0, 3, 
    0, 0, 0, 0, 
    // array [5]
    0, 0, 0, 16, 
      0, 0, 0, 1, 
      0, 0, 0, 5, 
      0, 0, 0, 35, 
    // }
  // }
// }
};
labcomm_signature_t labcomm_signature_nested_int_array_s = {
  LABCOMM_SAMPLE, "int_array_s",
  (int (*)(void *))labcomm_sizeof_nested_int_array_s,
  sizeof(signature_bytes_int_array_s),
  signature_bytes_int_array_s
 };
static unsigned char signature_bytes_struct_s[] = {
// struct { 2 fields
0, 0, 0, 17, 
  0, 0, 0, 2, 
  // int 'a'
  0, 0, 0, 1, 
  97, 
  0, 0, 0, 35, 
  // double 'bcd'
  0, 0, 0, 3, 
  98, 99, 100, 
  0, 0, 0, 38, 
// }
};
labcomm_signature_t labcomm_signature_nested_struct_s = {
  LABCOMM_SAMPLE, "struct_s",
  (int (*)(void *))labcomm_sizeof_nested_struct_s,
  sizeof(signature_bytes_struct_s),
  signature_bytes_struct_s
 };
static unsigned char signature_bytes_struct_array_s[] = {
// array [2]
0, 0, 0, 16, 
  0, 0, 0, 1, 
  0, 0, 0, 2, 
  // struct { 1 fields
  0, 0, 0, 17, 
    0, 0, 0, 1, 
    // int 'a'
    0, 0, 0, 1, 
    97, 
    0, 0, 0, 35, 
  // }
// }
};
labcomm_signature_t labcomm_signature_nested_struct_array_s = {
  LABCOMM_SAMPLE, "struct_array_s",
  (int (*)(void *))labcomm_sizeof_nested_struct_array_s,
  sizeof(signature_bytes_struct_array_s),
  signature_bytes_struct_array_s
 };
static unsigned char signature_bytes_struct_struct_s[] = {
// struct { 1 fields
0, 0, 0, 17, 
  0, 0, 0, 1, 
  // struct 'a'
  0, 0, 0, 1, 
  97, 
  // struct { 1 fields
  0, 0, 0, 17, 
    0, 0, 0, 1, 
    // int 'a'
    0, 0, 0, 1, 
    97, 
    0, 0, 0, 35, 
  // }
// }
};
labcomm_signature_t labcomm_signature_nested_struct_struct_s = {
  LABCOMM_SAMPLE, "struct_struct_s",
  (int (*)(void *))labcomm_sizeof_nested_struct_struct_s,
  sizeof(signature_bytes_struct_struct_s),
  signature_bytes_struct_struct_s
 };
static unsigned char signature_bytes_int_t_s[] = {
0, 0, 0, 35, 
};
labcomm_signature_t labcomm_signature_nested_int_t_s = {
  LABCOMM_SAMPLE, "int_t_s",
  (int (*)(void *))labcomm_sizeof_nested_int_t_s,
  sizeof(signature_bytes_int_t_s),
  signature_bytes_int_t_s
 };
static unsigned char signature_bytes_int_array_t_s[] = {
// array [1]
0, 0, 0, 16, 
  0, 0, 0, 1, 
  0, 0, 0, 1, 
  // array [2]
  0, 0, 0, 16, 
    0, 0, 0, 1, 
    0, 0, 0, 2, 
    // array [3, 4]
    0, 0, 0, 16, 
      0, 0, 0, 2, 
      0, 0, 0, 3, 
      0, 0, 0, 4, 
      0, 0, 0, 35, 
    // }
  // }
// }
};
labcomm_signature_t labcomm_signature_nested_int_array_t_s = {
  LABCOMM_SAMPLE, "int_array_t_s",
  (int (*)(void *))labcomm_sizeof_nested_int_array_t_s,
  sizeof(signature_bytes_int_array_t_s),
  signature_bytes_int_array_t_s
 };
static unsigned char signature_bytes_struct_t_s[] = {
// struct { 1 fields
0, 0, 0, 17, 
  0, 0, 0, 1, 
  // int 'a'
  0, 0, 0, 1, 
  97, 
  0, 0, 0, 35, 
// }
};
labcomm_signature_t labcomm_signature_nested_struct_t_s = {
  LABCOMM_SAMPLE, "struct_t_s",
  (int (*)(void *))labcomm_sizeof_nested_struct_t_s,
  sizeof(signature_bytes_struct_t_s),
  signature_bytes_struct_t_s
 };
static unsigned char signature_bytes_struct_array_t_s[] = {
// array [2]
0, 0, 0, 16, 
  0, 0, 0, 1, 
  0, 0, 0, 2, 
  // struct { 1 fields
  0, 0, 0, 17, 
    0, 0, 0, 1, 
    // int 'a'
    0, 0, 0, 1, 
    97, 
    0, 0, 0, 35, 
  // }
// }
};
labcomm_signature_t labcomm_signature_nested_struct_array_t_s = {
  LABCOMM_SAMPLE, "struct_array_t_s",
  (int (*)(void *))labcomm_sizeof_nested_struct_array_t_s,
  sizeof(signature_bytes_struct_array_t_s),
  signature_bytes_struct_array_t_s
 };
static unsigned char signature_bytes_struct_struct_t_s[] = {
// struct { 1 fields
0, 0, 0, 17, 
  0, 0, 0, 1, 
  // struct 'a'
  0, 0, 0, 1, 
  97, 
  // struct { 1 fields
  0, 0, 0, 17, 
    0, 0, 0, 1, 
    // int 'a'
    0, 0, 0, 1, 
    97, 
    0, 0, 0, 35, 
  // }
// }
};
labcomm_signature_t labcomm_signature_nested_struct_struct_t_s = {
  LABCOMM_SAMPLE, "struct_struct_t_s",
  (int (*)(void *))labcomm_sizeof_nested_struct_struct_t_s,
  sizeof(signature_bytes_struct_struct_t_s),
  signature_bytes_struct_struct_t_s
 };
static void decode_struct_array_ss(
  labcomm_decoder_t *d,
  void (*handle)(
    nested_struct_array_ss *v,
    void *context
  ),
  void *context
)
{
  nested_struct_array_ss v;
  {
    int i_0_0;
    for (i_0_0 = 0 ; i_0_0 < 1 ; i_0_0++) {
      v.a[i_0_0].n_0 = labcomm_decode_int(d);
      v.a[i_0_0].a = malloc(sizeof(v.a[i_0_0].a[0]) * v.a[i_0_0].n_0);
      {
        int i_1_0;
        for (i_1_0 = 0 ; i_1_0 < v.a[i_0_0].n_0 ; i_1_0++) {
          int i_1 = i_1_0;
          v.a[i_0_0].a[i_1].n_0 = labcomm_decode_int(d);
          v.a[i_0_0].a[i_1].a = malloc(sizeof(v.a[i_0_0].a[i_1].a[0]) * v.a[i_0_0].a[i_1].n_0);
          {
            int i_2_0;
            for (i_2_0 = 0 ; i_2_0 < v.a[i_0_0].a[i_1].n_0 ; i_2_0++) {
              int i_2 = i_2_0;
              v.a[i_0_0].a[i_1].a[i_2].aa = labcomm_decode_int(d);
              v.a[i_0_0].a[i_1].a[i_2].bb = labcomm_decode_boolean(d);
              {
                int i_3_0;
                for (i_3_0 = 0 ; i_3_0 < 1 ; i_3_0++) {
                  v.a[i_0_0].a[i_1].a[i_2].ias.a[i_3_0].n_0 = labcomm_decode_int(d);
                  v.a[i_0_0].a[i_1].a[i_2].ias.a[i_3_0].n_2 = labcomm_decode_int(d);
                  v.a[i_0_0].a[i_1].a[i_2].ias.a[i_3_0].a = malloc(sizeof(v.a[i_0_0].a[i_1].a[i_2].ias.a[i_3_0].a[0]) * v.a[i_0_0].a[i_1].a[i_2].ias.a[i_3_0].n_0 * 3 * v.a[i_0_0].a[i_1].a[i_2].ias.a[i_3_0].n_2);
                  {
                    int i_4_0;
                    int i_4_1;
                    int i_4_2;
                    for (i_4_0 = 0 ; i_4_0 < v.a[i_0_0].a[i_1].a[i_2].ias.a[i_3_0].n_0 ; i_4_0++) {
                      for (i_4_1 = 0 ; i_4_1 < 3 ; i_4_1++) {
                        for (i_4_2 = 0 ; i_4_2 < v.a[i_0_0].a[i_1].a[i_2].ias.a[i_3_0].n_2 ; i_4_2++) {
                          int i_4 = ((i_4_0) * 3 + i_4_1) * v.a[i_0_0].a[i_1].a[i_2].ias.a[i_3_0].n_2 + i_4_2;
                          {
                            int i_5_0;
                            for (i_5_0 = 0 ; i_5_0 < 5 ; i_5_0++) {
                              v.a[i_0_0].a[i_1].a[i_2].ias.a[i_3_0].a[i_4].a[i_5_0] = labcomm_decode_int(d);
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
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
      for (i_0_0 = 0 ; i_0_0 < 1 ; i_0_0++) {
        {
          int i_1_0;
          for (i_1_0 = 0 ; i_1_0 < v.a[i_0_0].n_0 ; i_1_0++) {
            int i_1 = i_1_0;
            {
              int i_2_0;
              for (i_2_0 = 0 ; i_2_0 < v.a[i_0_0].a[i_1].n_0 ; i_2_0++) {
                int i_2 = i_2_0;
                {
                  int i_3_0;
                  for (i_3_0 = 0 ; i_3_0 < 1 ; i_3_0++) {
                    free(v.a[i_0_0].a[i_1].a[i_2].ias.a[i_3_0].a);
                  }
                }
              }
            }
            free(v.a[i_0_0].a[i_1].a);
          }
        }
        free(v.a[i_0_0].a);
      }
    }
  }
}
void labcomm_decoder_register_nested_struct_array_ss(
  struct labcomm_decoder *d,
  void (*handler)(
    nested_struct_array_ss *v,
    void *context
  ),
  void *context
)
{
  labcomm_internal_decoder_register(
    d,
    &labcomm_signature_nested_struct_array_ss,
    (labcomm_decoder_typecast_t)decode_struct_array_ss,
    (labcomm_handler_typecast_t)handler,
    context
  );
}
static void encode_struct_array_ss(
  labcomm_encoder_t *e,
  nested_struct_array_ss *v
)
{
  e->writer.write(&e->writer, labcomm_writer_start);
  labcomm_encode_type_index(e, &labcomm_signature_nested_struct_array_ss);
  {
    {
      int i_0_0;
      for (i_0_0 = 0 ; i_0_0 < 1 ; i_0_0++) {
        labcomm_encode_int(e, (*v).a[i_0_0].n_0);
        {
          int i_1_0;
          for (i_1_0 = 0 ; i_1_0 < (*v).a[i_0_0].n_0 ; i_1_0++) {
            int i_1 = i_1_0;
            labcomm_encode_int(e, (*v).a[i_0_0].a[i_1].n_0);
            {
              int i_2_0;
              for (i_2_0 = 0 ; i_2_0 < (*v).a[i_0_0].a[i_1].n_0 ; i_2_0++) {
                int i_2 = i_2_0;
                labcomm_encode_int(e, (*v).a[i_0_0].a[i_1].a[i_2].aa);
                labcomm_encode_boolean(e, (*v).a[i_0_0].a[i_1].a[i_2].bb);
                {
                  int i_3_0;
                  for (i_3_0 = 0 ; i_3_0 < 1 ; i_3_0++) {
                    labcomm_encode_int(e, (*v).a[i_0_0].a[i_1].a[i_2].ias.a[i_3_0].n_0);
                    labcomm_encode_int(e, (*v).a[i_0_0].a[i_1].a[i_2].ias.a[i_3_0].n_2);
                    {
                      int i_4_0;
                      int i_4_1;
                      int i_4_2;
                      for (i_4_0 = 0 ; i_4_0 < (*v).a[i_0_0].a[i_1].a[i_2].ias.a[i_3_0].n_0 ; i_4_0++) {
                        for (i_4_1 = 0 ; i_4_1 < 3 ; i_4_1++) {
                          for (i_4_2 = 0 ; i_4_2 < (*v).a[i_0_0].a[i_1].a[i_2].ias.a[i_3_0].n_2 ; i_4_2++) {
                            int i_4 = ((i_4_0) * 3 + i_4_1) * (*v).a[i_0_0].a[i_1].a[i_2].ias.a[i_3_0].n_2 + i_4_2;
                            {
                              int i_5_0;
                              for (i_5_0 = 0 ; i_5_0 < 5 ; i_5_0++) {
                                labcomm_encode_int(e, (*v).a[i_0_0].a[i_1].a[i_2].ias.a[i_3_0].a[i_4].a[i_5_0]);
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  e->writer.write(&e->writer, labcomm_writer_end);
}
void labcomm_encode_nested_struct_array_ss(
labcomm_encoder_t *e,
nested_struct_array_ss *v
)
{
labcomm_internal_encode(e, &labcomm_signature_nested_struct_array_ss, v);
}
void labcomm_encoder_register_nested_struct_array_ss(
  struct labcomm_encoder *e
)
{
  labcomm_internal_encoder_register(
    e,
    &labcomm_signature_nested_struct_array_ss,
    (labcomm_encode_typecast_t)encode_struct_array_ss
  );
}
int labcomm_sizeof_nested_struct_array_ss(nested_struct_array_ss *v)
{
  int result = 4;
  {
    int i_0_0;
    for (i_0_0 = 0 ; i_0_0 < 1 ; i_0_0++) {
      {
        int i_1_0;
        for (i_1_0 = 0 ; i_1_0 < (*v).a[i_0_0].n_0 ; i_1_0++) {
          int i_1 = i_1_0;
          {
            int i_2_0;
            for (i_2_0 = 0 ; i_2_0 < (*v).a[i_0_0].a[i_1].n_0 ; i_2_0++) {
              int i_2 = i_2_0;
              {
                int i_3_0;
                for (i_3_0 = 0 ; i_3_0 < 1 ; i_3_0++) {
                  result += 20 * (*v).a[i_0_0].a[i_1].a[i_2].ias.a[i_3_0].n_0 * 3 * (*v).a[i_0_0].a[i_1].a[i_2].ias.a[i_3_0].n_2;
                }
              }
              result += 5;
            }
          }
        }
      }
    }
  }
  return result;
}
static void decode_int_s(
  labcomm_decoder_t *d,
  void (*handle)(
    nested_int_s *v,
    void *context
  ),
  void *context
)
{
  nested_int_s v;
  v = labcomm_decode_int(d);
  handle(&v, context);
}
void labcomm_decoder_register_nested_int_s(
  struct labcomm_decoder *d,
  void (*handler)(
    nested_int_s *v,
    void *context
  ),
  void *context
)
{
  labcomm_internal_decoder_register(
    d,
    &labcomm_signature_nested_int_s,
    (labcomm_decoder_typecast_t)decode_int_s,
    (labcomm_handler_typecast_t)handler,
    context
  );
}
static void encode_int_s(
  labcomm_encoder_t *e,
  nested_int_s *v
)
{
  e->writer.write(&e->writer, labcomm_writer_start);
  labcomm_encode_type_index(e, &labcomm_signature_nested_int_s);
  {
    labcomm_encode_int(e, (*v));
  }
  e->writer.write(&e->writer, labcomm_writer_end);
}
void labcomm_encode_nested_int_s(
labcomm_encoder_t *e,
nested_int_s *v
)
{
labcomm_internal_encode(e, &labcomm_signature_nested_int_s, v);
}
void labcomm_encoder_register_nested_int_s(
  struct labcomm_encoder *e
)
{
  labcomm_internal_encoder_register(
    e,
    &labcomm_signature_nested_int_s,
    (labcomm_encode_typecast_t)encode_int_s
  );
}
int labcomm_sizeof_nested_int_s(nested_int_s *v)
{
  return 8;
}
static void decode_int_array_s(
  labcomm_decoder_t *d,
  void (*handle)(
    nested_int_array_s *v,
    void *context
  ),
  void *context
)
{
  nested_int_array_s v;
  {
    int i_0_0;
    for (i_0_0 = 0 ; i_0_0 < 1 ; i_0_0++) {
      v.a[i_0_0].n_0 = labcomm_decode_int(d);
      v.a[i_0_0].n_2 = labcomm_decode_int(d);
      v.a[i_0_0].a = malloc(sizeof(v.a[i_0_0].a[0]) * v.a[i_0_0].n_0 * 3 * v.a[i_0_0].n_2);
      {
        int i_1_0;
        int i_1_1;
        int i_1_2;
        for (i_1_0 = 0 ; i_1_0 < v.a[i_0_0].n_0 ; i_1_0++) {
          for (i_1_1 = 0 ; i_1_1 < 3 ; i_1_1++) {
            for (i_1_2 = 0 ; i_1_2 < v.a[i_0_0].n_2 ; i_1_2++) {
              int i_1 = ((i_1_0) * 3 + i_1_1) * v.a[i_0_0].n_2 + i_1_2;
              {
                int i_2_0;
                for (i_2_0 = 0 ; i_2_0 < 5 ; i_2_0++) {
                  v.a[i_0_0].a[i_1].a[i_2_0] = labcomm_decode_int(d);
                }
              }
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
      for (i_0_0 = 0 ; i_0_0 < 1 ; i_0_0++) {
        free(v.a[i_0_0].a);
      }
    }
  }
}
void labcomm_decoder_register_nested_int_array_s(
  struct labcomm_decoder *d,
  void (*handler)(
    nested_int_array_s *v,
    void *context
  ),
  void *context
)
{
  labcomm_internal_decoder_register(
    d,
    &labcomm_signature_nested_int_array_s,
    (labcomm_decoder_typecast_t)decode_int_array_s,
    (labcomm_handler_typecast_t)handler,
    context
  );
}
static void encode_int_array_s(
  labcomm_encoder_t *e,
  nested_int_array_s *v
)
{
  e->writer.write(&e->writer, labcomm_writer_start);
  labcomm_encode_type_index(e, &labcomm_signature_nested_int_array_s);
  {
    {
      int i_0_0;
      for (i_0_0 = 0 ; i_0_0 < 1 ; i_0_0++) {
        labcomm_encode_int(e, (*v).a[i_0_0].n_0);
        labcomm_encode_int(e, (*v).a[i_0_0].n_2);
        {
          int i_1_0;
          int i_1_1;
          int i_1_2;
          for (i_1_0 = 0 ; i_1_0 < (*v).a[i_0_0].n_0 ; i_1_0++) {
            for (i_1_1 = 0 ; i_1_1 < 3 ; i_1_1++) {
              for (i_1_2 = 0 ; i_1_2 < (*v).a[i_0_0].n_2 ; i_1_2++) {
                int i_1 = ((i_1_0) * 3 + i_1_1) * (*v).a[i_0_0].n_2 + i_1_2;
                {
                  int i_2_0;
                  for (i_2_0 = 0 ; i_2_0 < 5 ; i_2_0++) {
                    labcomm_encode_int(e, (*v).a[i_0_0].a[i_1].a[i_2_0]);
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  e->writer.write(&e->writer, labcomm_writer_end);
}
void labcomm_encode_nested_int_array_s(
labcomm_encoder_t *e,
nested_int_array_s *v
)
{
labcomm_internal_encode(e, &labcomm_signature_nested_int_array_s, v);
}
void labcomm_encoder_register_nested_int_array_s(
  struct labcomm_encoder *e
)
{
  labcomm_internal_encoder_register(
    e,
    &labcomm_signature_nested_int_array_s,
    (labcomm_encode_typecast_t)encode_int_array_s
  );
}
int labcomm_sizeof_nested_int_array_s(nested_int_array_s *v)
{
  int result = 4;
  {
    int i_0_0;
    for (i_0_0 = 0 ; i_0_0 < 1 ; i_0_0++) {
      result += 20 * (*v).a[i_0_0].n_0 * 3 * (*v).a[i_0_0].n_2;
    }
  }
  return result;
}
static void decode_struct_s(
  labcomm_decoder_t *d,
  void (*handle)(
    nested_struct_s *v,
    void *context
  ),
  void *context
)
{
  nested_struct_s v;
  v.a = labcomm_decode_int(d);
  v.bcd = labcomm_decode_double(d);
  handle(&v, context);
}
void labcomm_decoder_register_nested_struct_s(
  struct labcomm_decoder *d,
  void (*handler)(
    nested_struct_s *v,
    void *context
  ),
  void *context
)
{
  labcomm_internal_decoder_register(
    d,
    &labcomm_signature_nested_struct_s,
    (labcomm_decoder_typecast_t)decode_struct_s,
    (labcomm_handler_typecast_t)handler,
    context
  );
}
static void encode_struct_s(
  labcomm_encoder_t *e,
  nested_struct_s *v
)
{
  e->writer.write(&e->writer, labcomm_writer_start);
  labcomm_encode_type_index(e, &labcomm_signature_nested_struct_s);
  {
    labcomm_encode_int(e, (*v).a);
    labcomm_encode_double(e, (*v).bcd);
  }
  e->writer.write(&e->writer, labcomm_writer_end);
}
void labcomm_encode_nested_struct_s(
labcomm_encoder_t *e,
nested_struct_s *v
)
{
labcomm_internal_encode(e, &labcomm_signature_nested_struct_s, v);
}
void labcomm_encoder_register_nested_struct_s(
  struct labcomm_encoder *e
)
{
  labcomm_internal_encoder_register(
    e,
    &labcomm_signature_nested_struct_s,
    (labcomm_encode_typecast_t)encode_struct_s
  );
}
int labcomm_sizeof_nested_struct_s(nested_struct_s *v)
{
  return 16;
}
static void decode_struct_array_s(
  labcomm_decoder_t *d,
  void (*handle)(
    nested_struct_array_s *v,
    void *context
  ),
  void *context
)
{
  nested_struct_array_s v;
  {
    int i_0_0;
    for (i_0_0 = 0 ; i_0_0 < 2 ; i_0_0++) {
      v.a[i_0_0].a = labcomm_decode_int(d);
    }
  }
  handle(&v, context);
}
void labcomm_decoder_register_nested_struct_array_s(
  struct labcomm_decoder *d,
  void (*handler)(
    nested_struct_array_s *v,
    void *context
  ),
  void *context
)
{
  labcomm_internal_decoder_register(
    d,
    &labcomm_signature_nested_struct_array_s,
    (labcomm_decoder_typecast_t)decode_struct_array_s,
    (labcomm_handler_typecast_t)handler,
    context
  );
}
static void encode_struct_array_s(
  labcomm_encoder_t *e,
  nested_struct_array_s *v
)
{
  e->writer.write(&e->writer, labcomm_writer_start);
  labcomm_encode_type_index(e, &labcomm_signature_nested_struct_array_s);
  {
    {
      int i_0_0;
      for (i_0_0 = 0 ; i_0_0 < 2 ; i_0_0++) {
        labcomm_encode_int(e, (*v).a[i_0_0].a);
      }
    }
  }
  e->writer.write(&e->writer, labcomm_writer_end);
}
void labcomm_encode_nested_struct_array_s(
labcomm_encoder_t *e,
nested_struct_array_s *v
)
{
labcomm_internal_encode(e, &labcomm_signature_nested_struct_array_s, v);
}
void labcomm_encoder_register_nested_struct_array_s(
  struct labcomm_encoder *e
)
{
  labcomm_internal_encoder_register(
    e,
    &labcomm_signature_nested_struct_array_s,
    (labcomm_encode_typecast_t)encode_struct_array_s
  );
}
int labcomm_sizeof_nested_struct_array_s(nested_struct_array_s *v)
{
  return 12;
}
static void decode_struct_struct_s(
  labcomm_decoder_t *d,
  void (*handle)(
    nested_struct_struct_s *v,
    void *context
  ),
  void *context
)
{
  nested_struct_struct_s v;
  v.a.a = labcomm_decode_int(d);
  handle(&v, context);
}
void labcomm_decoder_register_nested_struct_struct_s(
  struct labcomm_decoder *d,
  void (*handler)(
    nested_struct_struct_s *v,
    void *context
  ),
  void *context
)
{
  labcomm_internal_decoder_register(
    d,
    &labcomm_signature_nested_struct_struct_s,
    (labcomm_decoder_typecast_t)decode_struct_struct_s,
    (labcomm_handler_typecast_t)handler,
    context
  );
}
static void encode_struct_struct_s(
  labcomm_encoder_t *e,
  nested_struct_struct_s *v
)
{
  e->writer.write(&e->writer, labcomm_writer_start);
  labcomm_encode_type_index(e, &labcomm_signature_nested_struct_struct_s);
  {
    labcomm_encode_int(e, (*v).a.a);
  }
  e->writer.write(&e->writer, labcomm_writer_end);
}
void labcomm_encode_nested_struct_struct_s(
labcomm_encoder_t *e,
nested_struct_struct_s *v
)
{
labcomm_internal_encode(e, &labcomm_signature_nested_struct_struct_s, v);
}
void labcomm_encoder_register_nested_struct_struct_s(
  struct labcomm_encoder *e
)
{
  labcomm_internal_encoder_register(
    e,
    &labcomm_signature_nested_struct_struct_s,
    (labcomm_encode_typecast_t)encode_struct_struct_s
  );
}
int labcomm_sizeof_nested_struct_struct_s(nested_struct_struct_s *v)
{
  return 8;
}
static void decode_int_t_s(
  labcomm_decoder_t *d,
  void (*handle)(
    nested_int_t_s *v,
    void *context
  ),
  void *context
)
{
  nested_int_t_s v;
  v = labcomm_decode_int(d);
  handle(&v, context);
}
void labcomm_decoder_register_nested_int_t_s(
  struct labcomm_decoder *d,
  void (*handler)(
    nested_int_t_s *v,
    void *context
  ),
  void *context
)
{
  labcomm_internal_decoder_register(
    d,
    &labcomm_signature_nested_int_t_s,
    (labcomm_decoder_typecast_t)decode_int_t_s,
    (labcomm_handler_typecast_t)handler,
    context
  );
}
static void encode_int_t_s(
  labcomm_encoder_t *e,
  nested_int_t_s *v
)
{
  e->writer.write(&e->writer, labcomm_writer_start);
  labcomm_encode_type_index(e, &labcomm_signature_nested_int_t_s);
  {
    labcomm_encode_int(e, (*v));
  }
  e->writer.write(&e->writer, labcomm_writer_end);
}
void labcomm_encode_nested_int_t_s(
labcomm_encoder_t *e,
nested_int_t_s *v
)
{
labcomm_internal_encode(e, &labcomm_signature_nested_int_t_s, v);
}
void labcomm_encoder_register_nested_int_t_s(
  struct labcomm_encoder *e
)
{
  labcomm_internal_encoder_register(
    e,
    &labcomm_signature_nested_int_t_s,
    (labcomm_encode_typecast_t)encode_int_t_s
  );
}
int labcomm_sizeof_nested_int_t_s(nested_int_t_s *v)
{
  return 8;
}
static void decode_int_array_t_s(
  labcomm_decoder_t *d,
  void (*handle)(
    nested_int_array_t_s *v,
    void *context
  ),
  void *context
)
{
  nested_int_array_t_s v;
  {
    int i_0_0;
    for (i_0_0 = 0 ; i_0_0 < 1 ; i_0_0++) {
      {
        int i_1_0;
        for (i_1_0 = 0 ; i_1_0 < 2 ; i_1_0++) {
          {
            int i_2_0;
            int i_2_1;
            for (i_2_0 = 0 ; i_2_0 < 3 ; i_2_0++) {
              for (i_2_1 = 0 ; i_2_1 < 4 ; i_2_1++) {
                v.a[i_0_0].a[i_1_0].a[i_2_0][i_2_1] = labcomm_decode_int(d);
              }
            }
          }
        }
      }
    }
  }
  handle(&v, context);
}
void labcomm_decoder_register_nested_int_array_t_s(
  struct labcomm_decoder *d,
  void (*handler)(
    nested_int_array_t_s *v,
    void *context
  ),
  void *context
)
{
  labcomm_internal_decoder_register(
    d,
    &labcomm_signature_nested_int_array_t_s,
    (labcomm_decoder_typecast_t)decode_int_array_t_s,
    (labcomm_handler_typecast_t)handler,
    context
  );
}
static void encode_int_array_t_s(
  labcomm_encoder_t *e,
  nested_int_array_t_s *v
)
{
  e->writer.write(&e->writer, labcomm_writer_start);
  labcomm_encode_type_index(e, &labcomm_signature_nested_int_array_t_s);
  {
    {
      int i_0_0;
      for (i_0_0 = 0 ; i_0_0 < 1 ; i_0_0++) {
        {
          int i_1_0;
          for (i_1_0 = 0 ; i_1_0 < 2 ; i_1_0++) {
            {
              int i_2_0;
              int i_2_1;
              for (i_2_0 = 0 ; i_2_0 < 3 ; i_2_0++) {
                for (i_2_1 = 0 ; i_2_1 < 4 ; i_2_1++) {
                  labcomm_encode_int(e, (*v).a[i_0_0].a[i_1_0].a[i_2_0][i_2_1]);
                }
              }
            }
          }
        }
      }
    }
  }
  e->writer.write(&e->writer, labcomm_writer_end);
}
void labcomm_encode_nested_int_array_t_s(
labcomm_encoder_t *e,
nested_int_array_t_s *v
)
{
labcomm_internal_encode(e, &labcomm_signature_nested_int_array_t_s, v);
}
void labcomm_encoder_register_nested_int_array_t_s(
  struct labcomm_encoder *e
)
{
  labcomm_internal_encoder_register(
    e,
    &labcomm_signature_nested_int_array_t_s,
    (labcomm_encode_typecast_t)encode_int_array_t_s
  );
}
int labcomm_sizeof_nested_int_array_t_s(nested_int_array_t_s *v)
{
  return 100;
}
static void decode_struct_t_s(
  labcomm_decoder_t *d,
  void (*handle)(
    nested_struct_t_s *v,
    void *context
  ),
  void *context
)
{
  nested_struct_t_s v;
  v.a = labcomm_decode_int(d);
  handle(&v, context);
}
void labcomm_decoder_register_nested_struct_t_s(
  struct labcomm_decoder *d,
  void (*handler)(
    nested_struct_t_s *v,
    void *context
  ),
  void *context
)
{
  labcomm_internal_decoder_register(
    d,
    &labcomm_signature_nested_struct_t_s,
    (labcomm_decoder_typecast_t)decode_struct_t_s,
    (labcomm_handler_typecast_t)handler,
    context
  );
}
static void encode_struct_t_s(
  labcomm_encoder_t *e,
  nested_struct_t_s *v
)
{
  e->writer.write(&e->writer, labcomm_writer_start);
  labcomm_encode_type_index(e, &labcomm_signature_nested_struct_t_s);
  {
    labcomm_encode_int(e, (*v).a);
  }
  e->writer.write(&e->writer, labcomm_writer_end);
}
void labcomm_encode_nested_struct_t_s(
labcomm_encoder_t *e,
nested_struct_t_s *v
)
{
labcomm_internal_encode(e, &labcomm_signature_nested_struct_t_s, v);
}
void labcomm_encoder_register_nested_struct_t_s(
  struct labcomm_encoder *e
)
{
  labcomm_internal_encoder_register(
    e,
    &labcomm_signature_nested_struct_t_s,
    (labcomm_encode_typecast_t)encode_struct_t_s
  );
}
int labcomm_sizeof_nested_struct_t_s(nested_struct_t_s *v)
{
  return 8;
}
static void decode_struct_array_t_s(
  labcomm_decoder_t *d,
  void (*handle)(
    nested_struct_array_t_s *v,
    void *context
  ),
  void *context
)
{
  nested_struct_array_t_s v;
  {
    int i_0_0;
    for (i_0_0 = 0 ; i_0_0 < 2 ; i_0_0++) {
      v.a[i_0_0].a = labcomm_decode_int(d);
    }
  }
  handle(&v, context);
}
void labcomm_decoder_register_nested_struct_array_t_s(
  struct labcomm_decoder *d,
  void (*handler)(
    nested_struct_array_t_s *v,
    void *context
  ),
  void *context
)
{
  labcomm_internal_decoder_register(
    d,
    &labcomm_signature_nested_struct_array_t_s,
    (labcomm_decoder_typecast_t)decode_struct_array_t_s,
    (labcomm_handler_typecast_t)handler,
    context
  );
}
static void encode_struct_array_t_s(
  labcomm_encoder_t *e,
  nested_struct_array_t_s *v
)
{
  e->writer.write(&e->writer, labcomm_writer_start);
  labcomm_encode_type_index(e, &labcomm_signature_nested_struct_array_t_s);
  {
    {
      int i_0_0;
      for (i_0_0 = 0 ; i_0_0 < 2 ; i_0_0++) {
        labcomm_encode_int(e, (*v).a[i_0_0].a);
      }
    }
  }
  e->writer.write(&e->writer, labcomm_writer_end);
}
void labcomm_encode_nested_struct_array_t_s(
labcomm_encoder_t *e,
nested_struct_array_t_s *v
)
{
labcomm_internal_encode(e, &labcomm_signature_nested_struct_array_t_s, v);
}
void labcomm_encoder_register_nested_struct_array_t_s(
  struct labcomm_encoder *e
)
{
  labcomm_internal_encoder_register(
    e,
    &labcomm_signature_nested_struct_array_t_s,
    (labcomm_encode_typecast_t)encode_struct_array_t_s
  );
}
int labcomm_sizeof_nested_struct_array_t_s(nested_struct_array_t_s *v)
{
  return 12;
}
static void decode_struct_struct_t_s(
  labcomm_decoder_t *d,
  void (*handle)(
    nested_struct_struct_t_s *v,
    void *context
  ),
  void *context
)
{
  nested_struct_struct_t_s v;
  v.a.a = labcomm_decode_int(d);
  handle(&v, context);
}
void labcomm_decoder_register_nested_struct_struct_t_s(
  struct labcomm_decoder *d,
  void (*handler)(
    nested_struct_struct_t_s *v,
    void *context
  ),
  void *context
)
{
  labcomm_internal_decoder_register(
    d,
    &labcomm_signature_nested_struct_struct_t_s,
    (labcomm_decoder_typecast_t)decode_struct_struct_t_s,
    (labcomm_handler_typecast_t)handler,
    context
  );
}
static void encode_struct_struct_t_s(
  labcomm_encoder_t *e,
  nested_struct_struct_t_s *v
)
{
  e->writer.write(&e->writer, labcomm_writer_start);
  labcomm_encode_type_index(e, &labcomm_signature_nested_struct_struct_t_s);
  {
    labcomm_encode_int(e, (*v).a.a);
  }
  e->writer.write(&e->writer, labcomm_writer_end);
}
void labcomm_encode_nested_struct_struct_t_s(
labcomm_encoder_t *e,
nested_struct_struct_t_s *v
)
{
labcomm_internal_encode(e, &labcomm_signature_nested_struct_struct_t_s, v);
}
void labcomm_encoder_register_nested_struct_struct_t_s(
  struct labcomm_encoder *e
)
{
  labcomm_internal_encoder_register(
    e,
    &labcomm_signature_nested_struct_struct_t_s,
    (labcomm_encode_typecast_t)encode_struct_struct_t_s
  );
}
int labcomm_sizeof_nested_struct_struct_t_s(nested_struct_struct_t_s *v)
{
  return 8;
}
