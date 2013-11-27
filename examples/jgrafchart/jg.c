#include "labcomm.h"
#include "labcomm_private.h"
#include "jg.h"

static unsigned char signature_bytes_jg_foo[] = {
// struct { 8 fields
17, 
  8, 
  // double 'b'
  1, 
  98, 
  38, 
  // int 'c'
  1, 
  99, 
  35, 
  // int 'd'
  1, 
  100, 
  35, 
  // string 'e'
  1, 
  101, 
  39, 
  // boolean 'f'
  1, 
  102, 
  32, 
  // short 'g'
  1, 
  103, 
  34, 
  // long 'h'
  1, 
  104, 
  36, 
  // float 'i'
  1, 
  105, 
  37, 
// }
};
struct labcomm_signature labcomm_signature_jg_foo = {
  LABCOMM_SAMPLE, "foo",
  (int (*)(struct labcomm_signature *, void *))labcomm_sizeof_jg_foo,
  sizeof(signature_bytes_jg_foo),
  signature_bytes_jg_foo,
  0
 };
static void decode_jg_foo(
  struct labcomm_reader *r,
  void (*handle)(
    jg_foo *v,
    void *context
  ),
  void *context
)
{
  jg_foo v;
  v.b = labcomm_read_double(r);
  v.c = labcomm_read_int(r);
  v.d = labcomm_read_int(r);
  v.e = labcomm_read_string(r);
  v.f = labcomm_read_boolean(r);
  v.g = labcomm_read_short(r);
  v.h = labcomm_read_long(r);
  v.i = labcomm_read_float(r);
  handle(&v, context);
  {
    labcomm_memory_free(r->memory, 1, v.e);
  }
}
int labcomm_decoder_register_jg_foo(
  struct labcomm_decoder *d,
  void (*handler)(
    jg_foo *v,
    void *context
  ),
  void *context
)
{
  return labcomm_internal_decoder_register(
    d,
    &labcomm_signature_jg_foo,
    (labcomm_decoder_function)decode_jg_foo,
    (labcomm_handler_function)handler,
    context
  );
}
int labcomm_decoder_ioctl_jg_foo(
  struct labcomm_decoder *d,
  int ioctl_action,
  ...
)
{
  int result;
  va_list va;
  va_start(va, ioctl_action);
  result = labcomm_internal_decoder_ioctl(
    d, &labcomm_signature_jg_foo, 
    ioctl_action, va);
  va_end(va);
  return result;
}
static int encode_jg_foo(
  struct labcomm_writer *w,
  jg_foo *v
)
{
  int result = 0;
  result = labcomm_write_double(w, (*v).b);
  if (result != 0) { return result; }
  result = labcomm_write_int(w, (*v).c);
  if (result != 0) { return result; }
  result = labcomm_write_int(w, (*v).d);
  if (result != 0) { return result; }
  result = labcomm_write_string(w, (*v).e);
  if (result != 0) { return result; }
  result = labcomm_write_boolean(w, (*v).f);
  if (result != 0) { return result; }
  result = labcomm_write_short(w, (*v).g);
  if (result != 0) { return result; }
  result = labcomm_write_long(w, (*v).h);
  if (result != 0) { return result; }
  result = labcomm_write_float(w, (*v).i);
  if (result != 0) { return result; }
  return result;
}
int labcomm_encode_jg_foo(
struct labcomm_encoder *e,
jg_foo *v
)
{
return labcomm_internal_encode(e, &labcomm_signature_jg_foo, (labcomm_encoder_function)encode_jg_foo, v);
}
int labcomm_encoder_register_jg_foo(
  struct labcomm_encoder *e
)
{
  return labcomm_internal_encoder_register(
    e,
    &labcomm_signature_jg_foo,
    (labcomm_encoder_function)encode_jg_foo
  );
}
int labcomm_encoder_ioctl_jg_foo(
  struct labcomm_encoder *e,
  int ioctl_action,
  ...
)
{
  int result;
  va_list va;
  va_start(va, ioctl_action);
  result = labcomm_internal_encoder_ioctl(
    e, &labcomm_signature_jg_foo, 
    ioctl_action, va);
  va_end(va);
  return result;
}
int labcomm_sizeof_jg_foo(jg_foo *v)
{
  int result = labcomm_size_packed32(labcomm_signature_jg_foo.index);
  result += 0 + strlen((*v).e);
  result += 31;
  return result;
}
LABCOMM_CONSTRUCTOR void init_jg__signatures(void)
{
  static int initialized = 0;
  if (initialized == 0) {
    initialized = 1;
    labcomm_set_local_index(&labcomm_signature_jg_foo);
  }
}
