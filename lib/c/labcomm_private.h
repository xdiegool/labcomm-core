#ifndef _LABCOMM_PRIVATE_H_
#define _LABCOMM_PRIVATE_H_

#ifdef LABCOMM_COMPAT
  #include LABCOMM_COMPAT
#else
  #include <endian.h>
  #include <stdio.h>
#endif

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "labcomm.h"

/*
 * Predeclared aggregate type indices
 */
#define LABCOMM_TYPEDEF  0x01
#define LABCOMM_SAMPLE   0x02
#define LABCOMM_ARRAY    0x10
#define LABCOMM_STRUCT   0x11

/*
 * Predeclared primitive type indices
 */
#define LABCOMM_BOOLEAN  0x20 
#define LABCOMM_BYTE     0x21
#define LABCOMM_SHORT    0x22
#define LABCOMM_INT      0x23
#define LABCOMM_LONG     0x24
#define LABCOMM_FLOAT    0x25
#define LABCOMM_DOUBLE   0x26
#define LABCOMM_STRING   0x27

/*
 * Start index for user defined types
 */
#define LABCOMM_USER     0x40

/*
 *
 */
#define LABCOMM_DECLARE_SIGNATURE(name) \
  labcomm_signature_t name __attribute__((section("labcomm")))

/*
 * Semi private decoder declarations
 */
typedef void (*labcomm_handler_typecast_t)(void *, void *);

typedef void (*labcomm_decoder_typecast_t)(
  struct labcomm_decoder *,
  labcomm_handler_typecast_t,
  void *);

typedef struct labcomm_decoder {
  void *context;
  labcomm_reader_t reader;
  void (*do_register)(struct labcomm_decoder *, 
		      labcomm_signature_t *, 
		      labcomm_decoder_typecast_t,
		      labcomm_handler_typecast_t,
		      void *context);
  int (*do_decode_one)(struct labcomm_decoder *decoder);
  labcomm_error_handler_callback on_error;
  labcomm_handle_new_datatype_callback on_new_datatype;
} labcomm_decoder_t;

/*
 * Non typesafe registration function to be called from
 * generated labcomm_decoder_register_* functions.
 */
void labcomm_internal_decoder_register(
  labcomm_decoder_t *, 
  labcomm_signature_t *, 
  labcomm_decoder_typecast_t,
  labcomm_handler_typecast_t,
  void *context);

#if __BYTE_ORDER == __LITTLE_ENDIAN

#define LABCOMM_DECODE(name, type)					\
  static inline type labcomm_read_##name(labcomm_reader_t *r) {		\
    type result; int i;							\
    for (i = sizeof(type) - 1 ; i >= 0 ; i--) {				\
      if (r->pos >= r->count) {						\
	r->read(r, labcomm_reader_continue);				\
      }									\
      ((unsigned char*)(&result))[i] = r->data[r->pos];			\
      r->pos++;								\
    }									\
    return result;							\
  }									\
  static inline type labcomm_decode_##name(labcomm_decoder_t *d) {	\
    return labcomm_read_##name(&d->reader);				\
  }

#else

#define LABCOMM_DECODE(name, type)					\
  static inline type labcomm_read_##name(labcomm_reader_t *r) {		\
    type result; int i;							\
    for (i = 0 ; i < sizeof(type) ; i++) {				\
      if (r->pos >= r->count) {						\
	r->read(r, labcomm_reader_continue);				\
      }									\
      ((unsigned char*)(&result))[i] = r->data[r->pos];			\
      r->pos++;								\
    }									\
    return result;							\
  }									\
  static inline type labcomm_decode_##name(labcomm_decoder_t *d) {	\
    return labcomm_read_##name(&d->reader);				\
  }

#endif

LABCOMM_DECODE(boolean, unsigned char)
LABCOMM_DECODE(byte, unsigned char)
LABCOMM_DECODE(short, short)
LABCOMM_DECODE(int, int)
LABCOMM_DECODE(long, long long)
LABCOMM_DECODE(float, float)
LABCOMM_DECODE(double, double)

#if 0
/* 
 * Unpack a 32 bit unsigned number from a sequence bytes, where the 
 * first byte is prefixed with a variable length bit pattern that
 * indicates the number of bytes used for encoding. The encoding
 * is inspired by the UTF-8 encoding.
 *
 * 0b0     - 1 byte  (0x00000000 - 0x0000007f)
 * 0b10    - 2 bytes (0x00000080 - 0x00003fff)
 * 0b110   - 3 bytes (0x00004000 - 0x001fffff)
 * 0b1110  - 4 bytes (0x00200000 - 0x0fffffff)
 * 0b11110 - 5 bytes (0x10000000 - 0xffffffff) [4 bits unused]
 */
static inline unsigned int labcomm_read_unpacked32(labcomm_reader_t *r)
{
  unsigned int result = 0;
  int n, i;
  unsigned char tag;

  if (r->pos >= r->count) {	
    r->read(r, labcomm_reader_continue);
  }
  tag = r->data[r->pos];
  r->pos++;
  if (tag < 0x80) {
    n = 1;
    result = tag;
  } else if (tag < 0xc0) {
    n = 2;
    result = tag & 0x3f;
  } else if (tag < 0xe0) {
    n = 3;
    result = tag & 0x1f;
  } else if (tag < 0xf0) {
    n = 4;
    result = tag & 0x0f;
  } else {
    n = 5;
  }
  for (i = 1 ; i < n ; i++) {
    if (r->pos >= r->count) {	
      r->read(r, labcomm_reader_continue);
    }
    result = (result << 8) | r->data[r->pos];
    r->pos++;
  }
  return result;
}
#endif

static inline unsigned int labcomm_read_unpacked32(labcomm_reader_t *r)
{
  unsigned int result = 0;
  
  while (1) {
    unsigned char tmp;

    if (r->pos >= r->count) {	
      r->read(r, labcomm_reader_continue);
    }
    tmp = r->data[r->pos];
    r->pos++;
    result = (result << 7) | (tmp & 0x7f);
    if ((tmp & 0x80) == 0) { 
      break; 
    }
  }
  return result;
}
 
static inline unsigned int labcomm_decode_packed32(labcomm_decoder_t *d) 
{
  return labcomm_read_unpacked32(&d->reader);
}

static inline char *labcomm_read_string(labcomm_reader_t *r)
{
  char *result;
  int length, i; 
  
  length = labcomm_read_unpacked32(r);
  result = malloc(length + 1);
  for (i = 0 ; i < length ; i++) {
    if (r->pos >= r->count) {	
      r->read(r, labcomm_reader_continue);
    }
    result[i] = r->data[r->pos];
    r->pos++;
  }
  result[length] = 0;
  return result;
}

static inline char *labcomm_decode_string(labcomm_decoder_t *d)
{
  return labcomm_read_string(&d->reader);
}

/*
 * Semi private encoder declarations
 */
typedef int (*labcomm_encoder_function)(
  struct labcomm_encoder *,
  void *value);

typedef struct labcomm_encoder {
  void *context;
  labcomm_writer_t writer;
  void (*do_register)(struct labcomm_encoder *encoder, 
		      labcomm_signature_t *signature,
		      labcomm_encoder_function encode);
  int (*do_encode)(struct labcomm_encoder *encoder, 
		    labcomm_signature_t *signature, 
		    labcomm_encoder_function encode,
		    void *value);
  labcomm_error_handler_callback on_error;
} labcomm_encoder_t;

void labcomm_internal_encoder_register(
  labcomm_encoder_t *encoder, 
  labcomm_signature_t *signature, 
  labcomm_encoder_function encode);

int labcomm_internal_encode(
  labcomm_encoder_t *encoder, 
  labcomm_signature_t *signature, 
  labcomm_encoder_function encode,
  void *value);

#if __BYTE_ORDER == __LITTLE_ENDIAN

#define LABCOMM_ENCODE(name, type)					\
  static inline int labcomm_write_##name(labcomm_writer_t *w, type data) { \
    int i;								\
    for (i = sizeof(type) - 1 ; i >= 0 ; i--) {				\
      if (w->pos >= w->count) { /*buffer is full*/			\
        int err;							\
	err = w->write(w, labcomm_writer_continue);			\
	if (err != 0) { return err; }					\
      }									\
      w->data[w->pos] = ((unsigned char*)(&data))[i];			\
      w->pos++;								\
    }									\
    return 0;								\
  }									\
  static inline int labcomm_encode_##name(labcomm_encoder_t *e, type data) { \
    return labcomm_write_##name(&e->writer, data);			\
  }

#else

#define LABCOMM_ENCODE(name, type)					\
  static inline int labcomm_write_##name(labcomm_writer_t *w, type data) { \
    int i;								\
    for (i = 0 ; i < sizeof(type) ; i++) {				\
      if (w->pos >= w->count) {						\
        int err;							\
	err = w->write(w, labcomm_writer_continue);			\
	if (err != 0) { return err; }					\
      }									\
      w->data[w->pos] = ((unsigned char*)(&data))[i];			\
      w->pos++;								\
    }									\
    return 0;								\
  }									\
  static inline int labcomm_encode_##name(labcomm_encoder_t *e, type data) { \
    return labcomm_write_##name(&e->writer, data);			\
  }

#endif

LABCOMM_ENCODE(boolean, unsigned char)
LABCOMM_ENCODE(byte, unsigned char)
LABCOMM_ENCODE(short, short)
LABCOMM_ENCODE(int, int)
LABCOMM_ENCODE(long, long long)
LABCOMM_ENCODE(float, float)
LABCOMM_ENCODE(double, double)

#if 0
/* 
 * Pack the 32 bit unsigned number data as a sequence bytes, where the 
 * first byte is prefixed with a variable length bit pattern that
 * indicates the number of bytes used for encoding. The encoding
 * is inspired by the UTF-8 encoding.
 *
 * 0b0     - 1 byte  (0x00000000 - 0x0000007f)
 * 0b10    - 2 bytes (0x00000080 - 0x00003fff)
 * 0b110   - 3 bytes (0x00004000 - 0x001fffff)
 * 0b1110  - 4 bytes (0x00200000 - 0x0fffffff)
 * 0b11110 - 5 bytes (0x10000000 - 0xffffffff) [4 bits unused]
 */
static inline int labcomm_write_packed32(labcomm_writer_t *w, 
					 unsigned int data)
{
  int n;
  unsigned char tag;
  unsigned char tmp[4] = { (data >> 24) & 0xff, 
			   (data >> 16) & 0xff, 
			   (data >>  8) & 0xff, 
			   (data      ) & 0xff }; 
  if (data < 0x80) {
    n = 1;
    tag = 0x00;
  } else if (data < 0x4000) { 
    n = 2;
    tag = 0x80;
  } else if (data < 0x200000) { 
    n = 3;
    tag = 0xc0;
  } else if (data < 0x10000000) { 
    n = 4;
    tag = 0xe0;
  } else  {
    n = 5;
    tag = 0xf0;
  }
  /* TODO: maybe?
    if (w->pos + n - 1 >= w->count) {	
    w->write(w, labcomm_writer_continue, n);
    }
  */
  switch (n) {
    case 5: { 
      if (w->pos >= w->count) {	
	int err;
	err = w->write(w, labcomm_writer_continue);
	if (err != 0) { return err; }
      }
      w->data[w->pos++] = tag; tag = 0;
    }
    case 4: { 
      if (w->pos >= w->count) {					
	int err;
	err = w->write(w, labcomm_writer_continue);
	if (err != 0) { return err; }
      }
      w->data[w->pos++] = tmp[0] | tag; tag = 0;
    }
    case 3: { 
      if (w->pos >= w->count) {					
	int err;
	err = w->write(w, labcomm_writer_continue);
	if (err != 0) { return err; }
      }
      w->data[w->pos++] = tmp[1] | tag; tag = 0;
    }
    case 2: { 
      if (w->pos >= w->count) {					
	int err;
	err = w->write(w, labcomm_writer_continue);
	if (err != 0) { return err; }
      }
      w->data[w->pos++] = tmp[2] | tag; tag = 0;
    }
    case 1: { 
      if (w->pos >= w->count) {					
	int err;
	err = w->write(w, labcomm_writer_continue);
	if (err != 0) { return err; }
      }
      w->data[w->pos++] = tmp[3] | tag;
    }
  }
  return 0;
}
#endif

static inline int labcomm_write_packed32(labcomm_writer_t *w, 
					 unsigned int data)
{
  unsigned char tmp[5];
  int i;
  
  for (i = 0 ; i == 0 || data ; i++, data = (data >> 7)) {
    tmp[i] = data & 0x7f;
  }
  for (i = i - 1 ; i >= 0 ; i--) {
    if (w->pos >= w->count) {					
      int err;
      err = w->write(w, labcomm_writer_continue);
      if (err != 0) { return err; }
    }
    w->data[w->pos++] = tmp[i] | (i?0x80:0x00);
  }
  return 0;
}


static inline int labcomm_encode_packed32(labcomm_encoder_t *e, 
					   unsigned int data) 
{ 
  return labcomm_write_packed32(&e->writer, data);				
}


static inline int labcomm_write_string(labcomm_writer_t *w, char *s)
{
  int length, i, err; 

  length = strlen((char*)s);
  err = labcomm_write_packed32(w, length);
  if (err != 0) { return err; }
  for (i = 0 ; i < length ; i++) {
    if (w->pos >= w->count) {	
      int err;
      err = w->write(w, labcomm_writer_continue);
      if (err != 0) { return err; }
    }
    w->data[w->pos] = s[i];
    w->pos++;
  }
  return 0;
}

static inline int labcomm_encode_string(labcomm_encoder_t *e, 
					 char *s)
{
  return labcomm_write_string(&e->writer, s);
}

#endif
