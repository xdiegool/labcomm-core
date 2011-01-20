#ifndef _LABCOMM_PRIVATE_H_
#define _LABCOMM_PRIVATE_H_

#include <endian.h>
#include <stdio.h>
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
#define LABCOMM_USER     0x80

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
static inline char *labcomm_read_string(labcomm_reader_t *r)
{
  char *result;
  int length, i; 
  
  length = labcomm_read_int(r);
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

static inline int labcomm_buffer_read(struct labcomm_reader *r, 
				      labcomm_reader_action_t action)
{
  // If this gets called, it is an error, 
  // so note error and let producer proceed
  r->context = r;
  r->pos = 0;
  return 0;
}

static inline int labcomm_buffer_reader_error(struct labcomm_reader *r) 
{
  return r->context != NULL;
} 

static inline void labcomm_buffer_reader_setup(
  labcomm_reader_t *r,
  void *data,
  int length)
{
  r->context = NULL; // Used as errer flag
  r->data = data;  
  r->data_size = length;
  r->count = length;
  r->pos = 0;
  r->read = labcomm_buffer_read;
}

/*
 * Semi private encoder declarations
 */
typedef void (*labcomm_encode_typecast_t)(
  struct labcomm_encoder *,
  void *value);

typedef struct labcomm_encoder {
  void *context;
  labcomm_writer_t writer;
  void (*do_register)(struct labcomm_encoder *encoder, 
		      labcomm_signature_t *signature,
		      labcomm_encode_typecast_t);
  void (*do_encode)(struct labcomm_encoder *encoder, 
		    labcomm_signature_t *signature, 
		    void *value);
} labcomm_encoder_t;

void labcomm_internal_encoder_register(
  labcomm_encoder_t *encoder, 
  labcomm_signature_t *signature, 
  labcomm_encode_typecast_t encode);

void labcomm_internal_encode(
  labcomm_encoder_t *encoder, 
  labcomm_signature_t *signature, 
  void *value);

#define LABCOMM_USER_ACTION(i) (i + 100)
void labcomm_internal_encoder_user_action(struct labcomm_encoder *encoder, 
					  int action);

#if __BYTE_ORDER == __LITTLE_ENDIAN

#define LABCOMM_ENCODE(name, type)					\
  static inline void labcomm_write_##name(labcomm_writer_t *w, type data) { \
    int i;								\
    for (i = sizeof(type) - 1 ; i >= 0 ; i--) {				\
      if (w->pos >= w->count) {						\
	w->write(w, labcomm_writer_continue);				\
      }									\
      w->data[w->pos] = ((unsigned char*)(&data))[i];			\
      w->pos++;								\
    }									\
  }									\
  static inline void labcomm_encode_##name(labcomm_encoder_t *e, type data) { \
    labcomm_write_##name(&e->writer, data);				\
  }

#else

#define LABCOMM_ENCODE(name, type)					\
  static inline void labcomm_write_##name(labcomm_writer_t *w, type data) { \
    int i;								\
    for (i = 0 ; i < sizeof(type) ; i++) {				\
      if (w->pos >= w->count) {						\
	w->write(w, labcomm_writer_continue);			\
      }									\
      w->data[w->pos] = ((unsigned char*)(&data))[i];			\
      w->pos++;								\
    }									\
  }									\
  static inline void labcomm_encode_##name(labcomm_encoder_t *e, type data) { \
    labcomm_write_##name(&e->writer, data);				\
  }

#endif

LABCOMM_ENCODE(boolean, unsigned char)
LABCOMM_ENCODE(byte, unsigned char)
LABCOMM_ENCODE(short, short)
LABCOMM_ENCODE(int, int)
LABCOMM_ENCODE(long, long long)
LABCOMM_ENCODE(float, float)
LABCOMM_ENCODE(double, double)
static inline void labcomm_write_string(labcomm_writer_t *w, char *s)
{
  int length, i; 

  length = strlen((char*)s);
  labcomm_write_int(w, length);
  for (i = 0 ; i < length ; i++) {
    if (w->pos >= w->count) {	
      w->write(w, labcomm_writer_continue);
    }
    w->data[w->pos] = s[i];
    w->pos++;
  }
}

static inline void labcomm_encode_string(labcomm_encoder_t *e, 
					 char *s)
{
  labcomm_write_string(&e->writer, s);
}

void labcomm_encode_type_index(labcomm_encoder_t *e, labcomm_signature_t *s);

static inline int labcomm_buffer_write(struct labcomm_writer *w, 
                                       labcomm_writer_action_t action)
{
  // If this gets called, it is an error, 
  // so note error and let producer proceed
  w->context = w;
  w->pos = 0;
  return 0;
}


static inline int labcomm_buffer_writer_error(struct labcomm_writer *w) 
{
  return w->context != NULL;
} 


static inline void labcomm_buffer_writer_setup(struct labcomm_writer *w,
                                               void *data,
                                               int length)
{
  w->context = NULL; // Used as error flag
  w->data = data;
  w->data_size = length;
  w->count = length;
  w->pos = 0;
  w->write = labcomm_buffer_write;
}

#endif
