#ifndef LABCOMM_TYPE_SIGNATURE_H
#define LABCOMM_TYPE_SIGNATURE_H

//XXX move to common.h
#ifndef labcomm_bool
#define labcomm_bool char
#define TRUE 1
#define FALSE 0
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
    struct labcomm_signature* signature;            \
  } u;

#define LABCOMM_SIGDEF_BYTES(l, b) { l, .u.bytes=b }
#define LABCOMM_SIGDEF_SIGNATURE(s) { 0, .u.signature=&s } // addressof, as s is pointing at the sig struct, not directly the the sig_bytes[]
#define LABCOMM_SIGDEF_END { -1, .u.bytes=0 }

#else

#define LABCOMM_SIGDEF_BYTES_OR_SIGNATURE          \
  struct {                                  \
    char *bytes;                            \
    const struct labcomm_signature *signature;            \
  } u;

#define LABCOMM_SIGDEF_BYTES(l, b) { l, { b, 0 } }
#define LABCOMM_SIGDEF_SIGNATURE(s) { 0, { 0, &s } }
#define LABCOMM_SIGDEF_END { -1, { 0, 0 } }

#endif

struct labcomm_signature_data {
  int length;
  LABCOMM_SIGDEF_BYTES_OR_SIGNATURE
};

#endif
struct labcomm_signature {
  char *name;
  int (*encoded_size)(void *); /* void* refers to sample_data */
  int size;
  unsigned char *signature; 
  int index;
#ifndef LABCOMM_NO_TYPEDECL
  int tdsize;
  struct labcomm_signature_data *treedata;
#endif  
#ifdef LABCOMM_EXPERIMENTAL_CACHED_ENCODED_SIZE
  int cached_encoded_size; // -1 if not initialized or type is variable size
#endif
};

#if 0
/*
 * Signature entry
 */

#ifdef USE_UNIONS

/* Useful for C99 and up (or GCC without -pedantic) */ 

#define LABCOMM_SIGDEF_BYTES_OR_SIGNATURE          \
  union {                                   \
    char *bytes;                            \
    struct labcomm_signature* signature;            \
  } u;

#define LABCOMM_SIGDEF_BYTES(l, b) { l, .u.bytes=b }
#define LABCOMM_SIGDEF_SIGNATURE(s) { 0, .u.signature=&s } // addressof, as s is pointing at the sig struct, not directly the the sig_bytes[]
#define LABCOMM_SIGDEF_END { -1, .u.bytes=0 }

#else

#define LABCOMM_SIGDEF_BYTES_OR_SIGNATURE          \
  struct {                                  \
    char *bytes;                            \
    struct labcomm_signature *signature;            \
  } u;

#define LABCOMM_SIGDEF_BYTES(l, b) { l, { b, 0 } }
#define LABCOMM_SIGDEF_SIGNATURE(s) { 0, { 0, &s } }
#define LABCOMM_SIGDEF_END { -1, { 0, 0 } }

#endif

struct labcomm_signature_data {
  int length;
  LABCOMM_SIGDEF_BYTES_OR_SIGNATURE
};

struct labcomm_signature {
  int type;
  char *name;
  int (*encoded_size)(struct labcomm_signature *, void *); // void * == encoded_sample *
  struct {
    int size;
    unsigned char *data; 
  } flat;
  struct {
    int size;
    struct labcomm_signature_data *data;
  } tree;
  int index;
#ifdef LABCOMM_EXPERIMENTAL_CACHED_ENCODED_SIZE
  int cached_encoded_size; // -1 if not initialized or type is variable size
#endif
};
#endif
/*
 * functions
 */


/* Dump signature bytes on stdout 
 */

void labcomm_signature_print(struct labcomm_signature_data *signature);

/* compare signatures (flattened, if needed) to other
*  return TRUE if equal
*/
labcomm_bool labcomm_signature_cmp( struct labcomm_signature_data *s2,
                           struct labcomm_signature_data *s1);

/* flatten and dump signature to a byte array.
 * buf (out)   : byte array to write signature into
 * len (in/out): input: buf size, out: signature length
 *
 * return TRUE if aborted due to overrun
 */
labcomm_bool labcomm_signature_dump(struct labcomm_signature_data *signature, 
                           char *buf, int *len);

/* maps function f on each byte in the signature
 * if flatten, the signature is flattened, and the second argument of
 * f is always zero
 * otherwise, when a type ref is encountered, f is called with the first
 * argument zero and the type ref as the second argument.
 */ 
void map_signature( void(*f)(char, const struct labcomm_signature *, void *), 
                    void *context,
                    const struct labcomm_signature *signature, labcomm_bool flatten);

#endif
