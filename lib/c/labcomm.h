#ifndef _LABCOMM_H_
#define _LABCOMM_H_

#include <stdarg.h>
#include <unistd.h>

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
  struct labcomm_signature *sig);

void labcomm_decoder_register_new_datatype_handler(struct labcomm_decoder *d,
		labcomm_handle_new_datatype_callback on_new_datatype);

/*
 * Locking support (optional)
 */

struct labcomm_lock_action {
  int (*alloc)(void *context);
  int (*free)(void *context);
  int (*read_lock)(void *context);
  int (*read_unlock)(void *context);
  int (*write_lock)(void *context);
  int (*write_unlock)(void *context);
};

/*
 * Decoder
 */
struct labcomm_reader;

struct labcomm_decoder *labcomm_decoder_new(
  struct labcomm_reader *reader,
  const struct labcomm_lock_action *lock,
  void *lock_context);
int labcomm_decoder_decode_one(
  struct labcomm_decoder *decoder);
void labcomm_decoder_run(
  struct labcomm_decoder *decoder);
void labcomm_decoder_free(
  struct labcomm_decoder *decoder);

/* See labcomm_ioctl.h for predefined ioctl_action values */
int labcomm_decoder_ioctl(struct labcomm_decoder *decoder, 
			  int ioctl_action,
			  ...);

/*
 * Encoder
 */
struct labcomm_writer;

struct labcomm_encoder *labcomm_encoder_new(
  struct labcomm_writer *writer,
  const struct labcomm_lock_action *lock,
  void *lock_context);
void labcomm_encoder_free(
  struct labcomm_encoder *encoder);

/* See labcomm_ioctl.h for predefined ioctl_action values */
int labcomm_encoder_ioctl(struct labcomm_encoder *encoder, 
			  int ioctl_action,
			  ...);

#endif
