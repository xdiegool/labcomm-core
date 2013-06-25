/*
  labcomm.h -- user interface for handling encoding and decoding of
               labcomm samples.

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

#ifndef _LABCOMM_H_
#define _LABCOMM_H_

#include <stdarg.h>
#include <stdint.h>
#include <unistd.h>
#include <labcomm_error.h>

/* Forward declaration */
struct labcomm_encoder;
struct labcomm_decoder;

/*
 * Signature entry
 */
struct labcomm_signature {
  int type;
  char *name;
  int (*encoded_size)(struct labcomm_signature *, void *); // void * == encoded_sample *
  int size;
  unsigned char *signature; 
  int cached_encoded_size; // -1 if not initialized or type is variable size
};

/*
 * Error handling.
 */

/* The callback prototype for error handling.
 * First parameter is the error ID.
 * The second paramters is the number of va_args that comes after this 
 * one. If none it should be 0.
 * Optionaly other paramters can be supplied depending on what is needed 
 * for this error ID.
 */
typedef void (*labcomm_error_handler_callback)(enum labcomm_error error_id, 
					       size_t nbr_va_args, ...); 

/* Default error handler, prints message to stderr. 
 * Extra info about the error can be supplied as char* as VA-args. Especially user defined errors should supply a describing string. if nbr_va_args > 1 the first variable argument must be a printf format string and the possibly following arguments are passed as va_args to vprintf. 
 */
void on_error_fprintf(enum labcomm_error error_id, size_t nbr_va_args, ...);

/* Register a callback for the error handler for this encoder. */
void labcomm_register_error_handler_encoder(struct labcomm_encoder *encoder, labcomm_error_handler_callback callback);

/* Register a callback for the error handler for this decoder. */
void labcomm_register_error_handler_decoder(struct labcomm_decoder *decoder, labcomm_error_handler_callback callback);

/* Get a string describing the supplied standrad labcomm error. */
const char *labcomm_error_get_str(enum labcomm_error error_id);

typedef int (*labcomm_handle_new_datatype_callback)(
  struct labcomm_decoder *decoder,
  struct labcomm_signature *sig);

void labcomm_decoder_register_new_datatype_handler(struct labcomm_decoder *d,
		labcomm_handle_new_datatype_callback on_new_datatype);

/*
 * Locking support (optional), if not used only a single thread
 * may access an encoder or decoder at the same time.
 */
struct labcomm_lock;

int labcomm_lock_free(struct labcomm_lock *lock);
int labcomm_lock_acquire(struct labcomm_lock *lock);
int labcomm_lock_release(struct labcomm_lock *lock);
int labcomm_lock_wait(struct labcomm_lock *lock, useconds_t usec);
int labcomm_lock_notify(struct labcomm_lock *lock);

/*
 * Dynamic memory handling
 *   lifetime == 0     memory that will live for as long as the 
 *                     encoder/decoder or that are allocated/deallocated 
 *                     during the communication setup phase
 *   otherwise         memory will live for approximately this number of
 *                     sent/received samples
 */
struct labcomm_memory;

void *labcomm_memory_alloc(struct labcomm_memory *m, int lifetime, size_t size);
void *labcomm_memory_realloc(struct labcomm_memory *m, int lifetime, 
			     void *ptr, size_t size);
void labcomm_memory_free(struct labcomm_memory *m, int lifetime, void *ptr);

extern struct labcomm_memory *labcomm_default_memory;

/*
 * Decoder
 */
struct labcomm_reader;

struct labcomm_decoder *labcomm_decoder_new(
  struct labcomm_reader *reader,
  struct labcomm_lock *lock,
  struct labcomm_memory *memory);
int labcomm_decoder_decode_one(
  struct labcomm_decoder *decoder);
void labcomm_decoder_run(
  struct labcomm_decoder *decoder);
void labcomm_decoder_free(
  struct labcomm_decoder *decoder);

/* See labcomm_ioctl.h for predefined ioctl_action values */
int labcomm_decoder_ioctl(struct labcomm_decoder *decoder, 
			  uint32_t ioctl_action,
			  ...);

/*
 * Encoder
 */
struct labcomm_writer;

struct labcomm_encoder *labcomm_encoder_new(
  struct labcomm_writer *writer,
  struct labcomm_lock *lock,
  struct labcomm_memory *memory);
void labcomm_encoder_free(
  struct labcomm_encoder *encoder);

/* See labcomm_ioctl.h for predefined ioctl_action values */
int labcomm_encoder_ioctl(struct labcomm_encoder *encoder, 
			  uint32_t ioctl_action,
			  ...);

#define LABCOMM_VOID ((void*)1)

#endif
