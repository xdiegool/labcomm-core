/*
  labcomm2014.h -- user interface for handling encoding and decoding of
               labcomm2014 samples.

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

#ifndef __LABCOMM2014_H__
#define __LABCOMM2014_H__

#include <stdarg.h>

#ifdef LABCOMM_COMPAT
  #include LABCOMM_COMPAT
#else
  #include <stdint.h>
  #include <unistd.h>
#endif

#include "labcomm2014_error.h"
#include "labcomm2014_scheduler.h"

/* Forward declaration */
struct labcomm2014_encoder;
struct labcomm2014_decoder;

#include "labcomm2014_type_signature.h"
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
typedef void (*labcomm2014_error_handler_callback)(enum labcomm2014_error error_id, 
					       size_t nbr_va_args, ...); 

/* Default error handler, prints message to stderr. 
 * Extra info about the error can be supplied as char* as VA-args. Especially user defined errors should supply a describing string. if nbr_va_args > 1 the first variable argument must be a printf format string and the possibly following arguments are passed as va_args to vprintf. 
 */
void labcomm20142014_on_error_fprintf(enum labcomm2014_error error_id, size_t nbr_va_args, ...);

/* Register a callback for the error handler for this encoder. */
void labcomm2014_register_error_handler_encoder(struct labcomm2014_encoder *encoder, labcomm2014_error_handler_callback callback);

/* Register a callback for the error handler for this decoder. */
void labcomm2014_register_error_handler_decoder(struct labcomm2014_decoder *decoder, labcomm2014_error_handler_callback callback);

/* Get a string describing the supplied standrad labcomm2014 error. */
const char *labcomm2014_error_get_str(enum labcomm2014_error error_id);

typedef int (*labcomm2014_handle_new_datatype_callback)(
  struct labcomm2014_decoder *decoder,
  struct labcomm2014_signature *sig);

/*
 * Dynamic memory handling
 *   lifetime == 0     memory that will live for as long as the 
 *                     encoder/decoder or that are allocated/deallocated 
 *                     during the communication setup phase
 *   otherwise         memory will live for approximately this number of
 *                     sent/received samples
 */
struct labcomm2014_memory;

void *labcomm2014_memory_alloc(struct labcomm2014_memory *m, int lifetime, size_t size);
void *labcomm2014_memory_realloc(struct labcomm2014_memory *m, int lifetime, 
			     void *ptr, size_t size);
void labcomm2014_memory_free(struct labcomm2014_memory *m, int lifetime, void *ptr);

/*
 * Decoder
 */
struct labcomm2014_reader;

struct labcomm2014_decoder *labcomm2014_decoder_new(
  struct labcomm2014_reader *reader,
  struct labcomm2014_error_handler *error,
  struct labcomm2014_memory *memory,
  struct labcomm2014_scheduler *scheduler);
void labcomm2014_decoder_free(
  struct labcomm2014_decoder *decoder);
int labcomm2014_decoder_decode_one(
  struct labcomm2014_decoder *decoder);
void labcomm2014_decoder_run(
  struct labcomm2014_decoder *decoder);
int labcomm2014_decoder_sample_ref_register(
  struct labcomm2014_decoder *decoder,
  const struct labcomm2014_signature *signature);

/* See labcomm2014_ioctl.h for predefined ioctl_action values */
int labcomm2014_decoder_ioctl(struct labcomm2014_decoder *decoder, 
                              uint32_t ioctl_action,
                              ...);

const struct labcomm2014_signature *labcomm2014_decoder_get_ref_signature(
  struct labcomm2014_decoder *decoder,
  const struct labcomm2014_signature *signature);

/*
 * Encoder
 */
struct labcomm2014_writer;

struct labcomm2014_encoder *labcomm2014_encoder_new(
  struct labcomm2014_writer *writer,
  struct labcomm2014_error_handler *error,
  struct labcomm2014_memory *memory,
  struct labcomm2014_scheduler *scheduler);
void labcomm2014_encoder_free(
  struct labcomm2014_encoder *encoder);
int labcomm2014_encoder_sample_ref_register(
  struct labcomm2014_encoder *encoder,
  const struct labcomm2014_signature *signature);

/* See labcomm2014_ioctl.h for predefined ioctl_action values */
int labcomm2014_encoder_ioctl(struct labcomm2014_encoder *encoder, 
                              uint32_t ioctl_action,
                              ...);

const struct labcomm2014_signature *labcomm2014_encoder_get_sample_ref(
  struct labcomm2014_encoder *encoder,
  const struct labcomm2014_signature *signature);

#endif
