#ifndef _LABCOMM_H_
#define _LABCOMM_H_

#ifdef ARM_CORTEXM3_CODESOURCERY
  #include <machine/endian.h>
#else
  #include <endian.h>
#endif

// Some projects can not use stdio.h.
#ifndef LABCOMM_NO_STDIO
  #include <stdio.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

/* Forward declaration */
struct labcomm_encoder;
struct labcomm_decoder;

/*
 * Signature entry
 */
typedef struct labcomm_signature{
  int type;
  char *name;
  int (*encoded_size)(struct labcomm_signature *, void *); // void * == encoded_sample *
  int size;
  unsigned char *signature; 
  int cached_encoded_size; // -1 if not initialized or type is variable size
} labcomm_signature_t;

/*
 * Error handling.
 */

/* Error IDs */
enum labcomm_error {
  LABCOMM_ERROR_ENUM_BEGIN_GUARD,	// _must_ be the first enum element. labcomm_error_get_str() depends on this.
  LABCOMM_ERROR_ENC_NO_REG_SIGNATURE, 	
  LABCOMM_ERROR_ENC_MISSING_DO_REG,
  LABCOMM_ERROR_ENC_MISSING_DO_ENCODE,
  LABCOMM_ERROR_ENC_BUF_FULL,
  LABCOMM_ERROR_DEC_MISSING_DO_REG,
  LABCOMM_ERROR_DEC_MISSING_DO_DECODE_ONE,
  LABCOMM_ERROR_DEC_UNKNOWN_DATATYPE,
  LABCOMM_ERROR_DEC_INDEX_MISMATCH,
  LABCOMM_ERROR_DEC_TYPE_NOT_FOUND,
  LABCOMM_ERROR_UNIMPLEMENTED_FUNC,
  LABCOMM_ERROR_MEMORY,
  LABCOMM_ERROR_USER_DEF,			
  LABCOMM_ERROR_ENUM_END_GUARD		// _must_ be the last enum element. labcomm_error_get_str() depends on this.
};

/* Error strings. _must_ be the same order as in enum labcomm_error */
extern const char *labcomm_error_strings[];

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
  labcomm_signature_t *sig);

void labcomm_decoder_register_new_datatype_handler(struct labcomm_decoder *d,
		labcomm_handle_new_datatype_callback on_new_datatype);


/*
 * Decoder
 */

typedef enum { 
  labcomm_reader_alloc, 
  labcomm_reader_free,
  labcomm_reader_start, 
  labcomm_reader_continue, 
  labcomm_reader_end,
  labcomm_reader_ioctl
} labcomm_reader_action_t;

typedef struct labcomm_reader {
  void *context;
  unsigned char *data;
  int data_size;
  int count;
  int pos;
  int (*read)(struct labcomm_reader *, labcomm_reader_action_t);
  int (*ioctl)(struct labcomm_reader *, int, va_list);
  labcomm_error_handler_callback on_error;
}  labcomm_reader_t;

struct labcomm_decoder *labcomm_decoder_new(
  int (*reader)(labcomm_reader_t *, labcomm_reader_action_t),
  void *reader_context);
int labcomm_decoder_decode_one(
  struct labcomm_decoder *decoder);
void labcomm_decoder_run(
  struct labcomm_decoder *decoder);
void labcomm_decoder_free(
  struct labcomm_decoder *decoder);

/*
 * Encoder
 */

typedef enum { 
  labcomm_writer_alloc,              /* Allocate all neccessary data */
  labcomm_writer_free,               /* Free all allocated data */
  labcomm_writer_start,              /* Start writing an ordinary sample */
  labcomm_writer_continue,           /* Buffer full during ordinary sample */
  labcomm_writer_end,                /* End writing ordinary sample */
  labcomm_writer_start_signature,    /* Start writing signature */
  labcomm_writer_continue_signature, /* Buffer full during signature */
  labcomm_writer_end_signature,      /* End writing signature */
} labcomm_writer_action_t;

typedef struct labcomm_writer {
  void *context;
  unsigned char *data;
  int data_size;
  int count;
  int pos;
  int error;
  int (*write)(struct labcomm_writer *, labcomm_writer_action_t, ...);
  int (*ioctl)(struct labcomm_writer *, int, va_list);
  labcomm_error_handler_callback on_error;
} labcomm_writer_t;

struct labcomm_encoder *labcomm_encoder_new(
  int (*writer)(labcomm_writer_t *, labcomm_writer_action_t, ...),
  void *writer_context);
void labcomm_encoder_free(
  struct labcomm_encoder *encoder);
/* See labcomm_ioctl.h for predefined ioctl_action values */
int labcomm_encoder_ioctl(struct labcomm_encoder *encoder, 
			  int ioctl_action,
			  ...);

void labcomm_encoder_start(struct labcomm_encoder *e,
                           labcomm_signature_t *s) ;

//HERE BE DRAGONS: is the signature_t* needed here?
void labcomm_encoder_end(struct labcomm_encoder *e,
                           labcomm_signature_t *s) ;
#endif
