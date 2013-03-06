#include "labcomm.h"
#include "labcomm_private.h"
#include "example.h"

static unsigned char signature_bytes_log_message[] = {
// struct { 2 fields
17, 
  2, 
  // int 'sequence'
  8, 
  115, 101, 113, 117, 101, 110, 99, 101, 
  35, 
  // array [_] 'line'
  4, 
  108, 105, 110, 101, 
  // array [_]
  16, 
    1, 
    0, 
    // struct { 2 fields
    17, 
      2, 
      // boolean 'last'
      4, 
      108, 97, 115, 116, 
      32, 
      // string 'data'
      4, 
      100, 97, 116, 97, 
      39, 
    // }
  // }
// }
};
labcomm_signature_t labcomm_signature_example_log_message = {
  LABCOMM_SAMPLE, "log_message",
  (int (*)(void *))labcomm_sizeof_example_log_message,
  sizeof(signature_bytes_log_message),
  signature_bytes_log_message
 };
static unsigned char signature_bytes_data[] = {
37, 
};
labcomm_signature_t labcomm_signature_example_data = {
  LABCOMM_SAMPLE, "data",
  (int (*)(void *))labcomm_sizeof_example_data,
  sizeof(signature_bytes_data),
  signature_bytes_data
 };
static void decode_log_message(
  labcomm_decoder_t *d,
  void (*handle)(
    example_log_message *v,
    void *context
  ),
  void *context
)
{
  example_log_message v;
  v.sequence = labcomm_decode_int(d);
  v.line.n_0 = labcomm_decode_packed32(d);
  v.line.a = malloc(sizeof(v.line.a[0]) * v.line.n_0);
  {
    int i_0_0;
    for (i_0_0 = 0 ; i_0_0 < v.line.n_0 ; i_0_0++) {
      int i_0 = i_0_0;
      v.line.a[i_0].last = labcomm_decode_boolean(d);
      v.line.a[i_0].data = labcomm_decode_string(d);
    }
  }
  handle(&v, context);
  {
    {
      int i_0_0;
      for (i_0_0 = 0 ; i_0_0 < v.line.n_0 ; i_0_0++) {
        int i_0 = i_0_0;
        free(v.line.a[i_0].data);
      }
    }
    free(v.line.a);
  }
}
void labcomm_decoder_register_example_log_message(
  struct labcomm_decoder *d,
  void (*handler)(
    example_log_message *v,
    void *context
  ),
  void *context
)
{
  labcomm_internal_decoder_register(
    d,
    &labcomm_signature_example_log_message,
    (labcomm_decoder_typecast_t)decode_log_message,
    (labcomm_handler_typecast_t)handler,
    context
  );
}
static void encode_log_message(
  labcomm_encoder_t *e,
  example_log_message *v
)
{
  e->writer.write(&e->writer, labcomm_writer_start);
  labcomm_encode_type_index(e, &labcomm_signature_example_log_message);
  {
    labcomm_encode_int(e, (*v).sequence);
    labcomm_encode_packed32(e, (*v).line.n_0);
    {
      int i_0_0;
      for (i_0_0 = 0 ; i_0_0 < (*v).line.n_0 ; i_0_0++) {
        int i_0 = i_0_0;
        labcomm_encode_boolean(e, (*v).line.a[i_0].last);
        labcomm_encode_string(e, (*v).line.a[i_0].data);
      }
    }
  }
  e->writer.write(&e->writer, labcomm_writer_end);
}
void labcomm_encode_example_log_message(
labcomm_encoder_t *e,
example_log_message *v
)
{
labcomm_internal_encode(e, &labcomm_signature_example_log_message, v);
}
void labcomm_encoder_register_example_log_message(
  struct labcomm_encoder *e
)
{
  labcomm_internal_encoder_register(
    e,
    &labcomm_signature_example_log_message,
    (labcomm_encode_typecast_t)encode_log_message
  );
}
int labcomm_sizeof_example_log_message(example_log_message *v)
{
  int result = 4;
  {
    int i_0_0;
    for (i_0_0 = 0 ; i_0_0 < (*v).line.n_0 ; i_0_0++) {
      int i_0 = i_0_0;
      result += 4 + strlen((*v).line.a[i_0].data);
      result += 1;
    }
  }
  result += 4;
  return result;
}
static void decode_data(
  labcomm_decoder_t *d,
  void (*handle)(
    example_data *v,
    void *context
  ),
  void *context
)
{
  example_data v;
  v = labcomm_decode_float(d);
  handle(&v, context);
}
void labcomm_decoder_register_example_data(
  struct labcomm_decoder *d,
  void (*handler)(
    example_data *v,
    void *context
  ),
  void *context
)
{
  labcomm_internal_decoder_register(
    d,
    &labcomm_signature_example_data,
    (labcomm_decoder_typecast_t)decode_data,
    (labcomm_handler_typecast_t)handler,
    context
  );
}
static void encode_data(
  labcomm_encoder_t *e,
  example_data *v
)
{
  e->writer.write(&e->writer, labcomm_writer_start);
  labcomm_encode_type_index(e, &labcomm_signature_example_data);
  {
    labcomm_encode_float(e, (*v));
  }
  e->writer.write(&e->writer, labcomm_writer_end);
}
void labcomm_encode_example_data(
labcomm_encoder_t *e,
example_data *v
)
{
labcomm_internal_encode(e, &labcomm_signature_example_data, v);
}
void labcomm_encoder_register_example_data(
  struct labcomm_encoder *e
)
{
  labcomm_internal_encoder_register(
    e,
    &labcomm_signature_example_data,
    (labcomm_encode_typecast_t)encode_data
  );
}
int labcomm_sizeof_example_data(example_data *v)
{
  return 8;
}
