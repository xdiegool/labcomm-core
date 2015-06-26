#ifndef __LABCOMM2014_TYPE_SIGNATURE_H__
#define __LABCOMM2014_TYPE_SIGNATURE_H__

//XXX move to common.h
#ifndef labcomm2014_bool
#define labcomm2014_bool char
#define LABCOMM2014_TRUE 1
#define LABCOMM2014_FALSE 0
#endif

/*
 * Signature entry
 */
#ifndef LABCOMM_NO_TYPEDECL
#ifdef USE_UNIONS

/* Useful for C99 and up (or GCC without -pedantic) */ 

#define LABCOMM_SIGDEF_BYTES_OR_SIGNATURE          \
  union {                                   \
    char *bytes;                            \
    struct labcomm2014_signature* signature;            \
  } u;

#define LABCOMM_SIGDEF_BYTES(l, b) { l, .u.bytes=b }
#define LABCOMM_SIGDEF_SIGNATURE(s) { 0, .u.signature=&s } // addressof, as s is pointing at the sig struct, not directly the the sig_bytes[]
#define LABCOMM_SIGDEF_END { -1, .u.bytes=0 }

#else

#define LABCOMM_SIGDEF_BYTES_OR_SIGNATURE          \
  struct {                                  \
    char *bytes;                            \
    const struct labcomm2014_signature *signature;            \
  } u;

#define LABCOMM_SIGDEF_BYTES(l, b) { l, { b, 0 } }
#define LABCOMM_SIGDEF_SIGNATURE(s) { 0, { 0, &s } }
#define LABCOMM_SIGDEF_END { -1, { 0, 0 } }

#endif

struct labcomm2014_signature_data {
  int length;
  LABCOMM_SIGDEF_BYTES_OR_SIGNATURE
};

#endif
struct labcomm2014_sample_ref;

struct labcomm2014_signature {
  char *name;
  int (*encoded_size)(const struct labcomm2014_signature *sig, void *); /* void* refers to sample_data */
  int size;
  unsigned char *signature; 
  int index;
#ifndef LABCOMM_NO_TYPEDECL
  int tdsize;
  struct labcomm2014_signature_data *treedata;
#endif  
#ifdef LABCOMM_EXPERIMENTAL_CACHED_ENCODED_SIZE
  int cached_encoded_size; // -1 if not initialized or type is variable size
#endif
};

/* a struct for "raw" type_defs, to be used as an intermediate representation
 * between decoder and signature parser
 */

struct labcomm2014_raw_type_def {
    char *name;
    int index;
    int length;
    char *signature_data;
};

/* a struct for type bindings
 */

struct labcomm2014_type_binding {
    int sample_index;
    int type_index;
};

/*
 * functions
 */


/* register a handler for type_defs and type bindings
 */

int labcomm2014_decoder_register_labcomm2014_type_def(
  struct labcomm2014_decoder *d,
  void (*handler)(
    struct labcomm2014_raw_type_def *v,
    void *context
  ),
  void *context
);

int labcomm2014_decoder_register_labcomm2014_type_binding(
  struct labcomm2014_decoder *d,
  void (*handler)(
    struct labcomm2014_type_binding *v,
    void *context
  ),
  void *context
);

/* Dump signature bytes on stdout 
 */

void labcomm2014_signature_print(struct labcomm2014_signature_data *signature);

/* compare signatures (flattened, if needed) to other
*  return LABCOMM2014_TRUE if equal
*/
labcomm2014_bool labcomm2014_signature_cmp( struct labcomm2014_signature_data *s2,
                           struct labcomm2014_signature_data *s1);

/* flatten and dump signature to a byte array.
 * buf (out)   : byte array to write signature into
 * len (in/out): input: buf size, out: signature length
 *
 * return LABCOMM2014_TRUE if aborted due to overrun
 */
labcomm2014_bool labcomm2014_signature_dump(struct labcomm2014_signature_data *signature, 
                           char *buf, int *len);

/* maps function f on each byte in the signature
 * if flatten, the signature is flattened, and the second argument of
 * f is always zero
 * otherwise, when a type ref is encountered, f is called with the first
 * argument zero and the type ref as the second argument.
 */ 
void map_signature( void(*f)(char, const struct labcomm2014_signature *, void *), 
                    void *context,
                    const struct labcomm2014_signature *signature, labcomm2014_bool flatten);

#endif
