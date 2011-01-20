#ifndef _LABCOMM_H_
#define _LABCOMM_H_

#include <endian.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Signature entry
 */
typedef struct {
  int type;
  char *name;
  int (*encoded_size)(void *);
  int size;
  unsigned char *signature; 
} labcomm_signature_t;


/*
 * Decoder
 */
struct labcomm_decoder;

typedef enum { 
  labcomm_reader_alloc, 
  labcomm_reader_free,
  labcomm_reader_start, 
  labcomm_reader_continue, 
  labcomm_reader_end
} labcomm_reader_action_t;

typedef struct labcomm_reader {
  void *context;
  unsigned char *data;
  int data_size;
  int count;
  int pos;
  int (*read)(struct labcomm_reader *, labcomm_reader_action_t);
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
struct labcomm_encoder;

typedef enum { 
  labcomm_writer_alloc, 
  labcomm_writer_free,
  labcomm_writer_start, 
  labcomm_writer_continue, 
  labcomm_writer_end, 
  labcomm_writer_available,
} labcomm_writer_action_t;

typedef struct labcomm_writer {
  void *context;
  unsigned char *data;
  int data_size;
  int count;
  int pos;
  int (*write)(struct labcomm_writer *, labcomm_writer_action_t);
} labcomm_writer_t;

struct labcomm_encoder;
struct labcomm_encoder *labcomm_encoder_new(
  int (*writer)(labcomm_writer_t *, labcomm_writer_action_t),
  void *writer_context);
void labcomm_encoder_free(
  struct labcomm_encoder *encoder);

#endif
