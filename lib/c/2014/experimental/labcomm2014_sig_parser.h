/* labcomm2014_sig_parser.h:
 * an example parser for labcomm signatures, illustrating how to skip samples
 * based on their signature. Intended as an embryo for introducing this
 * functionality into the lib to allow a channel to survive types with no
 * registered handler.
 */

#ifndef LABCOMM_SIG_PARSER_H
#define LABCOMM_SIG_PARSER_H

#include "labcomm2014.h"
#include "labcomm2014_private.h"

#undef DEBUG
#define QUIET_STACK	   // don't print anything for push/pop
#undef DEBUG_STACK_VERBOSE // dump stack, otherwise just print value of top

#undef QUIET 		//just print type and size when skipping data
#define VERBOSE 		// print in great detail


#undef SKIP_BY_PARSING

#undef STATIC_ALLOCATION  //dynamic allocation not completely implemented

#ifdef STATIC_ALLOCATION

#define MAX_SIGNATURES 100
#define MAX_NAME_LEN 32
#define MAX_SIG_LEN 128
#define TYPEDEF_BASE MAX_SIGNATURES

#endif

/* internal type: stack &c. for the parser */
typedef struct {
        unsigned char* c;
        size_t size;
        size_t capacity;
        int idx;
        int val_top;
        int * val_stack;
        int ptr_top;
        void** ptr_stack;
        size_t stacksize;
        int current_decl_is_varsize;

	size_t max_signatures;                 // set by init(...)
	size_t max_name_len;
	size_t max_sig_len;
    // arrays for signatures and typedefs
    // signatures start at index 0
    // typedefs start at index MAX_SIGNATURES
#ifdef STATIC_ALLOCATION
	struct labcomm2014_signature sig_ts[2*MAX_SIGNATURES];

	unsigned int signatures_length[2*MAX_SIGNATURES];
	unsigned int signatures_name_length[2*MAX_SIGNATURES];
	unsigned char signatures_name[2*MAX_SIGNATURES][MAX_NAME_LEN];
	unsigned char signatures[2*MAX_SIGNATURES][MAX_SIG_LEN];
#else
	struct labcomm2014_signature *sig_ts;           // [2*MAX_SIGNATURES]

	unsigned int *signatures_length;       // [2*MAX_SIGNATURES]
	unsigned char **signatures;            // [2*MAX_SIGNATURES][MAX_SIG_LEN];

	unsigned int *signatures_name_length;  // [2*MAX_SIGNATURES]
	char **signatures_name;       // [2*MAX_SIGNATURES][MAX_NAME_LEN];
#endif

} labcomm2014_sig_parser_t;


int labcomm2014_sig_parser_init(labcomm2014_sig_parser_t *p, size_t size,
                            size_t stacksize, size_t max_num_signatures,
                            size_t max_name_len, size_t max_sig_len);
void labcomm2014_sig_parser_free(labcomm2014_sig_parser_t *b);
int labcomm2014_sig_parser_read_file(labcomm2014_sig_parser_t *p, FILE *f);

int accept_packet(labcomm2014_sig_parser_t *p);

struct labcomm2014_signature *get_sig_t(labcomm2014_sig_parser_t *p,unsigned int uid);

unsigned int get_signature_len(labcomm2014_sig_parser_t *p,unsigned int uid);
char* get_signature_name(labcomm2014_sig_parser_t *p,unsigned int uid);
unsigned char* get_signature(labcomm2014_sig_parser_t *p,unsigned int uid);
void dump_signature(labcomm2014_sig_parser_t *p,unsigned int uid);


int more(labcomm2014_sig_parser_t *b);


/* parse signature and skip the corresponding bytes in the labcomm2014_sig_parser
 */
int skip_packed_sample_data(labcomm2014_sig_parser_t *p, struct labcomm2014_signature *sig);

#ifdef QUIET
#define INFO_PRINTF(format, args...)
#undef VERBOSE
#else
#define INFO_PRINTF(format, args...)  \
      printf (format , ## args)
#endif

#ifdef VERBOSE
#define VERBOSE_PRINTF(format, args...)  \
      printf (format , ## args)
#else
#define VERBOSE_PRINTF(format, args...)
#endif

#undef EXIT_WHEN_RECEIVING_DATA

#undef RETURN_STRINGS  //  not really tested

typedef enum{
        PKG_VERSION = LABCOMM_VERSION,
        PKG_SAMPLE_DECL = LABCOMM_SAMPLE_DEF,
        PKG_SAMPLE_REF = LABCOMM_SAMPLE_REF,
        PKG_TYPE_DECL = LABCOMM_TYPE_DEF,
        PKG_TYPE_BINDING = LABCOMM_TYPE_BINDING,

        ARRAY_DECL = LABCOMM_ARRAY,
        STRUCT_DECL = LABCOMM_STRUCT,

        TYPE_BOOLEAN  = LABCOMM_BOOLEAN,
        TYPE_BYTE  = LABCOMM_BYTE,
        TYPE_SHORT  = LABCOMM_SHORT,
        TYPE_INTEGER  = LABCOMM_INT,
        TYPE_LONG  = LABCOMM_LONG,
        TYPE_FLOAT  = LABCOMM_FLOAT,
        TYPE_DOUBLE  = LABCOMM_DOUBLE,
        TYPE_STRING  = LABCOMM_STRING,
        TYPE_SAMPLE_REF  = LABCOMM_REF
} labcomm2014_type ;
#endif
