/*
  labcomm_private.h -- semi private declarations for handling encoding and 
                       decoding of labcomm samples.

  Copyright 2006-2013 Anders Blomdell <anders.blomdell@control.lth.se>

  This file is part of LabComm.

  LabComm is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  LabComm is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _LABCOMM_PRIVATE_H_
#define _LABCOMM_PRIVATE_H_

#ifdef LABCOMM_COMPAT
  #include LABCOMM_COMPAT
#else
  #include <endian.h>
  #include <stdio.h>
#endif

#include <stdint.h>
//#include <stdlib.h>
#include <string.h>
#include <unistd.h>
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
 * Macro to automagically call constructors in modules compiled 
 * with the labcomm compiler. If __attribute__((constructor)) is
 * not supported, these calls has to be done first in main program.
 */
#ifndef LABCOMM_CONSTRUCTOR
#define LABCOMM_CONSTRUCTOR __attribute__((constructor))
#endif

/*
 * Semi private dynamic memory declarations
 */

struct labcomm_memory {
  void *(*alloc)(struct labcomm_memory *m, int lifetime, size_t size);
  void *(*realloc)(struct labcomm_memory *m, int lifetime, 
		   void *ptr, size_t size);
  void (*free)(struct labcomm_memory *m, int lifetime, void *ptr);
  void *context;
};

/*
 * Semi private decoder declarations
 */
typedef void (*labcomm_handler_function)(void *value, void *context);

typedef void (*labcomm_decoder_function)(
  struct labcomm_reader *r,
  labcomm_handler_function handler,
  void *context);

struct labcomm_reader_action_context;

struct labcomm_reader_action {
  /* 'alloc' is called at the first invocation of 'labcomm_decoder_decode_one' 
     on the decoder containing the reader. If 'labcomm_version' != NULL
     and non-empty the transport layer may use it to ensure that
     compatible versions are used.

     Returned value:
       >  0    Number of bytes allocated for buffering
       <= 0    Error
  */
  int (*alloc)(struct labcomm_reader *r, 
	       struct labcomm_reader_action_context *action_context, 
	       char *labcomm_version);
  /* 'free' returns the resources claimed by 'alloc' and might have other
     reader specific side-effects as well.

     Returned value:
       == 0    Success
       != 0    Error
  */
  int (*free)(struct labcomm_reader *r, 
	      struct labcomm_reader_action_context *action_context);
  /* 'start' is called at the following instances:
     1. When a sample is registered 
          (local_index != 0, remote_index == 0, value == NULL)
     2. When a sample definition is received 
          (local_index != 0, remote_index != 0, value == NULL)
     3. When a sample is received
          (local_index != 0, remote_index != 0, value != NULL)
   */
  int (*start)(struct labcomm_reader *r, 
	       struct labcomm_reader_action_context *action_context,
	       int local_index, int remote_index,
	       struct labcomm_signature *signature,
	       void *value);
  int (*end)(struct labcomm_reader *r, 
	     struct labcomm_reader_action_context *action_context);
  int (*fill)(struct labcomm_reader *r, 
	      struct labcomm_reader_action_context *action_context);
  int (*ioctl)(struct labcomm_reader *r, 
	       struct labcomm_reader_action_context *action_context,
	       int local_index, int remote_index,
	       struct labcomm_signature *signature, 
	       uint32_t ioctl_action, va_list args);
};

struct labcomm_reader_action_context {
  struct labcomm_reader_action_context *next;
  const struct labcomm_reader_action *action;
  void *context;  
};

struct labcomm_reader {
  struct labcomm_reader_action_context *action_context;
  struct labcomm_memory *memory;
  /* The following fields are initialized by labcomm_decoder_new */
  struct labcomm_decoder *decoder;
  unsigned char *data;
  int data_size;
  int count;
  int pos;
  int error;
};

int labcomm_reader_alloc(struct labcomm_reader *r, 
			 struct labcomm_reader_action_context *action_context, 
			 char *labcomm_version);
int labcomm_reader_free(struct labcomm_reader *r, 
			struct labcomm_reader_action_context *action_context);
int labcomm_reader_start(struct labcomm_reader *r, 
			 struct labcomm_reader_action_context *action_context,
			 int local_index, int remote_index,
			 struct labcomm_signature *signature,
			 void *value);
int labcomm_reader_end(struct labcomm_reader *r, 
		       struct labcomm_reader_action_context *action_context);
int labcomm_reader_fill(struct labcomm_reader *r, 
			struct labcomm_reader_action_context *action_context);
int labcomm_reader_ioctl(struct labcomm_reader *r, 
			 struct labcomm_reader_action_context *action_context,
			 int local_index, int remote_index,
			 struct labcomm_signature *signature, 
			 uint32_t ioctl_action, va_list args);

/*
 * Non typesafe registration function to be called from
 * generated labcomm_decoder_register_* functions.
 */
int labcomm_internal_decoder_register(
  struct labcomm_decoder *d, 
  struct labcomm_signature *s, 
  labcomm_decoder_function decoder,
  labcomm_handler_function handler,
  void *context);

int labcomm_internal_decoder_ioctl(struct labcomm_decoder *decoder, 
				   struct labcomm_signature *signature,
				   uint32_t ioctl_action, va_list args);

#if __BYTE_ORDER == __LITTLE_ENDIAN

#define LABCOMM_DECODE(name, type)					\
  static inline type labcomm_read_##name(struct labcomm_reader *r) {	\
    type result; int i;							\
    for (i = sizeof(type) - 1 ; i >= 0 ; i--) {				\
      if (r->pos >= r->count) {						\
	labcomm_reader_fill(r, r->action_context);			\
	if (r->error < 0) {						\
	  return 0;							\
	}								\
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
	labcomm_reader_fille(r, r->action_context);			\
	if (r->error < 0) {						\
	  return 0;							\
	}								\
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

//compatibility with 2013 version
#define labcomm_read_packed32 labcomm_read_int

#if 0
static inline unsigned int labcomm_read_packed32(struct labcomm_reader *r)
{
  unsigned int result = 0;
  
  while (1) {
    unsigned char tmp;

    if (r->pos >= r->count) {	
      labcomm_reader_fill(r, r->action_context);
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
#endif 

static inline char *labcomm_read_string(struct labcomm_reader *r)
{
  char *result = NULL;
  int length, pos; 
  
  length = labcomm_read_packed32(r);
  result = labcomm_memory_alloc(r->memory, 1, length + 1);
  for (pos = 0 ; pos < length ; pos++) {
    if (r->pos >= r->count) {	
      labcomm_reader_fill(r, r->action_context);
      if (r->error < 0) {
	goto out;
      }
    }
    result[pos] = r->data[r->pos];
    r->pos++;
  }
out:
  result[pos] = 0;
  return result;
}

/*
 * Semi private encoder declarations
 */
typedef int (*labcomm_encoder_function)(struct labcomm_writer *,
					void *value);
struct labcomm_writer_action_context;

struct labcomm_writer_action {
  int (*alloc)(struct labcomm_writer *w, 
	       struct labcomm_writer_action_context *action_context, 
	       char *labcomm_version);
  int (*free)(struct labcomm_writer *w, 
	      struct labcomm_writer_action_context *action_context);
  /* 'start' is called right before a sample is to be sent. In the 
     case of a sample or typedef, 'value' == NULL.

     Returned value:
       == 0          Success -> continue sending the sample
       == -EALREADY  Success -> silently skip sending the sample,
                                'end' will not be called
       < 0           Error
   */
  int (*start)(struct labcomm_writer *w, 
	       struct labcomm_writer_action_context *action_context,
	       int index, struct labcomm_signature *signature,
	       void *value);
  int (*end)(struct labcomm_writer *w, 
	     struct labcomm_writer_action_context *action_context);
  int (*flush)(struct labcomm_writer *w, 
	       struct labcomm_writer_action_context *action_context); 
  int (*ioctl)(struct labcomm_writer *w, 
	       struct labcomm_writer_action_context *action_context, 
	       int index, struct labcomm_signature *signature, 
	       uint32_t ioctl_action, va_list args);
};

struct labcomm_writer_action_context {
  struct labcomm_writer_action_context *next;
  const struct labcomm_writer_action *action;
  void *context;  
};

struct labcomm_writer {
  struct labcomm_writer_action_context *action_context;
  struct labcomm_memory *memory;
  /* The following fields are initialized by labcomm_encoder_new */
  struct labcomm_encoder *encoder;
  unsigned char *data;
  int data_size;
  int count;
  int pos;
  int error;
};

int labcomm_writer_alloc(struct labcomm_writer *w, 
			 struct labcomm_writer_action_context *action_context, 
			 char *labcomm_version);
int labcomm_writer_free(struct labcomm_writer *w, 
			struct labcomm_writer_action_context *action_context);
int labcomm_writer_start(struct labcomm_writer *w, 
			 struct labcomm_writer_action_context *action_context,
			 int index, struct labcomm_signature *signature,
			 void *value);
int labcomm_writer_end(struct labcomm_writer *w, 
		       struct labcomm_writer_action_context *action_context);
int labcomm_writer_flush(struct labcomm_writer *w, 
			 struct labcomm_writer_action_context *action_context); 
int labcomm_writer_ioctl(struct labcomm_writer *w, 
			 struct labcomm_writer_action_context *action_context, 
			 int index, struct labcomm_signature *signature, 
			 uint32_t ioctl_action, va_list args);

int labcomm_internal_encoder_register(
  struct labcomm_encoder *encoder, 
  struct labcomm_signature *signature, 
  labcomm_encoder_function encode);

int labcomm_internal_encode(
  struct labcomm_encoder *encoder, 
  struct labcomm_signature *signature, 
  labcomm_encoder_function encode,
  void *value);

int labcomm_internal_encoder_ioctl(struct labcomm_encoder *encoder, 
				   struct labcomm_signature *signature,
				   uint32_t ioctl_action, va_list args);

#if __BYTE_ORDER == __LITTLE_ENDIAN

#define LABCOMM_ENCODE(name, type)					\
  static inline int labcomm_write_##name(struct labcomm_writer *w, type data) { \
    int i;								\
    for (i = sizeof(type) - 1 ; i >= 0 ; i--) {				\
      if (w->pos >= w->count) { /*buffer is full*/			\
        int err;							\
	err = labcomm_writer_flush(w, w->action_context);		\
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
	err = labcomm_writer_flush(w, w->action_context);		\
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

#define labcomm_write_packed32 labcomm_write_int

#if 0
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
      err = labcomm_writer_flush(w, w->action_context);	
      if (err != 0) { return err; }
    }
    w->data[w->pos++] = tmp[i] | (i?0x80:0x00);
  }
  return 0;
}
#endif

static inline int labcomm_write_string(struct labcomm_writer *w, char *s)
{
  int length, i, err; 

  length = strlen((char*)s);
  err = labcomm_write_packed32(w, length);
  if (err != 0) { return err; }
  for (i = 0 ; i < length ; i++) {
    if (w->pos >= w->count) {	
      int err;
      err = labcomm_writer_flush(w, w->action_context);	
      if (err != 0) { return err; }
    }
    w->data[w->pos] = s[i];
    w->pos++;
  }
  return 0;
}

/* Size of packed32 variable is 4 as we use int*/
static inline int labcomm_size_packed32(unsigned int data)
{
  return 4;
}

/*
 * Macros for handling arrays indexed by signature index
 */

#define LABCOMM_SIGNATURE_ARRAY_DEF(name, kind)	\
  struct {					\
    int first;					\
    int last;					\
    kind *data;					\
  } name

#define LABCOMM_SIGNATURE_ARRAY_DEF_INIT(name, kind)		\
  LABCOMM_SIGNATURE_ARRAY_DEF(name, kind) = { 0, 0, NULL }

#define LABCOMM_SIGNATURE_ARRAY_INIT(name, kind)		\
  name.first = 0; name.last = 0; name.data = NULL;		\
  name.data = (kind *)name.data; /* typechecking no-op */

#define LABCOMM_SIGNATURE_ARRAY_FREE(memory, name, kind)	\
  if (name.data) { labcomm_memory_free(memory, 0, name.data); }	\
  name.data = (kind *)NULL; /* typechecking */

void *labcomm_signature_array_ref(struct labcomm_memory * memory,
				  int *first, int *last, void **data,
				  int size, int index);
/*
 * NB: the pointer returned by LABCOMM_SIGNATURE_ARRAY_REF might be
 *     rendered invalid by a subsequent call to LABCOMM_SIGNATURE_ARRAY_REF
 *     on the same SIGNATURE_ARRAY, so make sure not to use the result if 
 *     any other code might have made a call to LABCOMM_SIGNATURE_ARRAY_REF
 *     on the same SIGNATURE_ARRAY.
 */
#define LABCOMM_SIGNATURE_ARRAY_REF(memory, name, kind, index)		\
  (name.data = (kind *)name.data, /* typechecking no-op */		\
   (kind *)(labcomm_signature_array_ref(memory,				\
					&name.first, &name.last,	\
					(void **)&name.data,		\
					sizeof(kind), index)))

#define LABCOMM_SIGNATURE_ARRAY_FOREACH(name, kind, var)		\
  for (name.data = (kind *)name.data, /* typechecking no-op */		\
       var = name.first ; var < name.last ; var++)

/* Give signature a free local index, this may not be used concurrently */
void labcomm_set_local_index(struct labcomm_signature *signature);

/* Get the local index for a signature */
int labcomm_get_local_index(struct labcomm_signature *s);

#endif
