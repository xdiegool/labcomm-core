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

#define LABCOMM_VERSION "LabComm2006"

#include <stdarg.h>

#ifdef LABCOMM_COMPAT
  #include LABCOMM_COMPAT
#else
  #include <stdint.h>
  #include <unistd.h>
#endif
#include "labcomm2006_error.h"
#include "labcomm2006_scheduler.h"

/* Forward declaration */
struct labcomm2006_encoder;
struct labcomm2006_decoder;

/*
 * Signature entry
 */
struct labcomm2006_signature {
  char *name;
  int (*encoded_size)(void *); /* void* refers to sample_data */
  int size;
  unsigned char *signature; 
  int index;
#ifdef LABCOMM_EXPERIMENTAL_CACHED_ENCODED_SIZE
  int cached_encoded_size; // -1 if not initialized or type is variable size
#endif
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
typedef void (*labcomm2006_error_handler_callback)(enum labcomm2006_error error_id, 
					       size_t nbr_va_args, ...); 

/* Default error handler, prints message to stderr. 
 * Extra info about the error can be supplied as char* as VA-args. Especially user defined errors should supply a describing string. if nbr_va_args > 1 the first variable argument must be a printf format string and the possibly following arguments are passed as va_args to vprintf. 
 */
void on_error_fprintf(enum labcomm2006_error error_id, size_t nbr_va_args, ...);

/* Register a callback for the error handler for this encoder. */
void labcomm2006_register_error_handler_encoder(struct labcomm2006_encoder *encoder, labcomm2006_error_handler_callback callback);

/* Register a callback for the error handler for this decoder. */
void labcomm2006_register_error_handler_decoder(struct labcomm2006_decoder *decoder, labcomm2006_error_handler_callback callback);

/* Get a string describing the supplied standrad labcomm error. */
const char *labcomm2006_error_get_str(enum labcomm2006_error error_id);

typedef int (*labcomm2006_handle_new_datatype_callback)(
  struct labcomm2006_decoder *decoder,
  struct labcomm2006_signature *sig);

void labcomm2006_decoder_register_new_datatype_handler(struct labcomm2006_decoder *d,
		labcomm2006_handle_new_datatype_callback on_new_datatype);

/*
 * Dynamic memory handling
 *   lifetime == 0     memory that will live for as long as the 
 *                     encoder/decoder or that are allocated/deallocated 
 *                     during the communication setup phase
 *   otherwise         memory will live for approximately this number of
 *                     sent/received samples
 */
struct labcomm2006_memory;

void *labcomm2006_memory_alloc(struct labcomm2006_memory *m, int lifetime, size_t size);
void *labcomm2006_memory_realloc(struct labcomm2006_memory *m, int lifetime, 
			     void *ptr, size_t size);
void labcomm2006_memory_free(struct labcomm2006_memory *m, int lifetime, void *ptr);

/*
 * Decoder
 */
struct labcomm2006_reader;

struct labcomm2006_decoder *labcomm2006_decoder_new(
  struct labcomm2006_reader *reader,
  struct labcomm2006_error_handler *error,
  struct labcomm2006_memory *memory,
  struct labcomm2006_scheduler *scheduler);
void labcomm2006_decoder_free(
  struct labcomm2006_decoder *decoder);
int labcomm2006_decoder_decode_one(
  struct labcomm2006_decoder *decoder);
void labcomm2006_decoder_run(
  struct labcomm2006_decoder *decoder);

/* See labcomm2006_ioctl.h for predefined ioctl_action values */
int labcomm2006_decoder_ioctl(struct labcomm2006_decoder *decoder, 
			  uint32_t ioctl_action,
			  ...);

/*
 * Encoder
 */
struct labcomm2006_writer;

struct labcomm2006_encoder *labcomm2006_encoder_new(
  struct labcomm2006_writer *writer,
  struct labcomm2006_error_handler *error,
  struct labcomm2006_memory *memory,
  struct labcomm2006_scheduler *scheduler);
void labcomm2006_encoder_free(
  struct labcomm2006_encoder *encoder);

/* See labcomm2006_ioctl.h for predefined ioctl_action values */
int labcomm2006_encoder_ioctl(struct labcomm2006_encoder *encoder, 
			  uint32_t ioctl_action,
			  ...);

#define LABCOMM_VOID ((void*)1)

#endif
