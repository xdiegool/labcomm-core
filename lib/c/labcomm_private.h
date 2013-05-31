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
  struct labcomm_signature __attribute__((section("labcomm"))) name 

/*
 * Semi private lock declarations
 */
struct labcomm_lock_action {
  int (*alloc)(void *context);
  int (*free)(void *context);
  int (*read_lock)(void *context);
  int (*read_unlock)(void *context);
  int (*write_lock)(void *context);
  int (*write_unlock)(void *context);
};

struct labcomm_lock {
  const struct labcomm_lock_action action;
};

/*
 * Semi private decoder declarations
 */
typedef void (*labcomm_handler_function)(void *value, void *context);

typedef void (*labcomm_decoder_function)(
  struct labcomm_reader *r,
  labcomm_handler_function handler,
  void *context);

struct labcomm_reader_action {
  int (*alloc)(struct labcomm_reader *r, void *context, 
	       struct labcomm_decoder *decoder, char *labcomm_version);
  int (*free)(struct labcomm_reader *r, void *context);
  int (*start)(struct labcomm_reader *r, void *context);
  int (*end)(struct labcomm_reader *r, void *context);
  int (*fill)(struct labcomm_reader *r, void *context); 
  int (*ioctl)(struct labcomm_reader *r, void *context,
	       int action, struct labcomm_signature *signature, va_list args);
};

struct labcomm_reader {
  const struct labcomm_reader_action *action;
  void *context;
  unsigned char *data;
  int data_size;
  int count;
  int pos;
  int error;
};



/*
 * Non typesafe registration function to be called from
 * generated labcomm_decoder_register_* functions.
 */
void labcomm_internal_decoder_register(
  struct labcomm_decoder *d, 
  struct labcomm_signature *s, 
  labcomm_decoder_function decoder,
  labcomm_handler_function handler,
  void *context);

int labcomm_internal_decoder_ioctl(struct labcomm_decoder *decoder, 
				   int ioctl_action,
				   struct labcomm_signature *signature,
				   va_list args);

#if __BYTE_ORDER == __LITTLE_ENDIAN

#define LABCOMM_DECODE(name, type)					\
  static inline type labcomm_read_##name(struct labcomm_reader *r) {	\
    type result; int i;							\
    for (i = sizeof(type) - 1 ; i >= 0 ; i--) {				\
      if (r->pos >= r->count) {						\
	r->action->fill(r, r->context);					\
      }									\
      ((unsigned char*)(&result))[i] = r->data[r->pos];			\
      r->pos++;								\
    }									\
    return result;							\
  }

#else

#define LABCOMM_DECODE(name, type)					\
  static inline type labcomm_read_##name(struct labcomm_reader *r) {	\
    type result; int i;							\
    for (i = 0 ; i < sizeof(type) ; i++) {				\
      if (r->pos >= r->count) {						\
	r->action->fill(r, r->context);					\
      }									\
      ((unsigned char*)(&result))[i] = r->data[r->pos];			\
      r->pos++;								\
    }									\
    return result;							\
  }

#endif

LABCOMM_DECODE(boolean, unsigned char)
LABCOMM_DECODE(byte, unsigned char)
LABCOMM_DECODE(short, short)
LABCOMM_DECODE(int, int)
LABCOMM_DECODE(long, long long)
LABCOMM_DECODE(float, float)
LABCOMM_DECODE(double, double)

static inline unsigned int labcomm_read_packed32(struct labcomm_reader *r)
{
  unsigned int result = 0;
  
  while (1) {
    unsigned char tmp;

    if (r->pos >= r->count) {	
      r->action->fill(r, r->context);
      if (r->error != 0) {
	goto out;
      }
    }
    tmp = r->data[r->pos];
    r->pos++;
    result = (result << 7) | (tmp & 0x7f);
    if ((tmp & 0x80) == 0) { 
      break; 
    }
  }
out:
  return result;
}
 
static inline char *labcomm_read_string(struct labcomm_reader *r)
{
  char *result;
  int length, i; 
  
  length = labcomm_read_packed32(r);
  result = malloc(length + 1);
  for (i = 0 ; i < length ; i++) {
    if (r->pos >= r->count) {	
      r->action->fill(r, r->context);
    }
    result[i] = r->data[r->pos];
    r->pos++;
  }
  result[length] = 0;
  return result;
}

/*
 * Semi private encoder declarations
 */
typedef int (*labcomm_encoder_function)(
  struct labcomm_writer *,
  void *value);

struct labcomm_writer;

struct labcomm_writer_action {
  int (*alloc)(struct labcomm_writer *w, void *context, 
	       struct labcomm_encoder *encoder, char *labcomm_version);
  int (*free)(struct labcomm_writer *w, void *context);
  int (*start)(struct labcomm_writer *w, void *context,
	       struct labcomm_encoder *encoder,
	       int index, struct labcomm_signature *signature,
	       void *value);
  int (*end)(struct labcomm_writer *w, void *context);
  int (*flush)(struct labcomm_writer *w, void *context); 
  int (*ioctl)(struct labcomm_writer *w, void *context, 
	       int index, struct labcomm_signature *, 
	       va_list);
};

struct labcomm_writer {
  const struct labcomm_writer_action *action;
  void *context;
  unsigned char *data;
  int data_size;
  int count;
  int pos;
  int error;
};

void labcomm_internal_encoder_register(
  struct labcomm_encoder *encoder, 
  struct labcomm_signature *signature, 
  labcomm_encoder_function encode);

int labcomm_internal_encode(
  struct labcomm_encoder *encoder, 
  struct labcomm_signature *signature, 
  labcomm_encoder_function encode,
  void *value);


int labcomm_internal_encoder_ioctl(struct labcomm_encoder *encoder, 
				   int ioctl_action,
				   struct labcomm_signature *signature,
				   va_list args);

#if __BYTE_ORDER == __LITTLE_ENDIAN

#define LABCOMM_ENCODE(name, type)					\
  static inline int labcomm_write_##name(struct labcomm_writer *w, type data) { \
    int i;								\
    for (i = sizeof(type) - 1 ; i >= 0 ; i--) {				\
      if (w->pos >= w->count) { /*buffer is full*/			\
        int err;							\
	err = w->action->flush(w, w->context);				\
	if (err != 0) { return err; }					\
      }									\
      w->data[w->pos] = ((unsigned char*)(&data))[i];			\
      w->pos++;								\
    }									\
    return 0;								\
  }

#else

#define LABCOMM_ENCODE(name, type)					\
  static inline int labcomm_write_##name(struct labcomm_writer *w, type data) { \
    int i;								\
    for (i = 0 ; i < sizeof(type) ; i++) {				\
      if (w->pos >= w->count) {						\
        int err;							\
	err = w->action->flush(w, w->context);					\
	if (err != 0) { return err; }					\
      }									\
      w->data[w->pos] = ((unsigned char*)(&data))[i];			\
      w->pos++;								\
    }									\
    return 0;								\
  }

#endif

LABCOMM_ENCODE(boolean, unsigned char)
LABCOMM_ENCODE(byte, unsigned char)
LABCOMM_ENCODE(short, short)
LABCOMM_ENCODE(int, int)
LABCOMM_ENCODE(long, long long)
LABCOMM_ENCODE(float, float)
LABCOMM_ENCODE(double, double)

static inline int labcomm_write_packed32(struct labcomm_writer *w, 
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
      err = w->action->flush(w, w->context);
      if (err != 0) { return err; }
    }
    w->data[w->pos++] = tmp[i] | (i?0x80:0x00);
  }
  return 0;
}

static inline int labcomm_write_string(struct labcomm_writer *w, char *s)
{
  int length, i, err; 

  length = strlen((char*)s);
  err = labcomm_write_packed32(w, length);
  if (err != 0) { return err; }
  for (i = 0 ; i < length ; i++) {
    if (w->pos >= w->count) {	
      int err;
      err = w->action->flush(w, w->context);
      if (err != 0) { return err; }
    }
    w->data[w->pos] = s[i];
    w->pos++;
  }
  return 0;
}

#endif
