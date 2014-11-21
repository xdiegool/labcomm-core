/* labcomm_sig_parser.h:
 * an example parser for labcomm signatures, illustrating how to skip samples
 * based on their signature. Intended as an embryo for introducing this 
 * functionality into the lib to allow a channel to survive types with no
 * registered handler.
 */

#ifndef LABCOMM_SIG_PARSER_H
#define LABCOMM_SIG_PARSER_H

#include "../labcomm_private.h"

#undef DEBUG 
#undef QUIET_STACK	   // don't print anything for push/pop
#undef DEBUG_STACK_VERBOSE // dump stack, otherwise just print value of top

#undef QUIET 		//just print type and size when skipping data
#undef VERBOSE 		// print in great detail

#undef STATIC_ALLOCATION  //dynamic allocation not completely implemented

#ifdef STATIC_ALLOCATION

#define MAX_SIGNATURES 16
#define MAX_NAME_LEN 32 
#define MAX_SIG_LEN 128

#endif

/* internal type: stack &c. for the parser */
typedef struct {
        char* c;
        size_t size;
        size_t capacity;
        unsigned int idx;
        int val_top;
        int * val_stack;
        int ptr_top;
        void** ptr_stack;
        size_t stacksize;
        int current_decl_is_varsize;

	size_t max_signatures;                 // set by init(...)
	size_t max_name_len;
	size_t max_sig_len; 
#ifdef STATIC_ALLOCATION
	struct labcomm_signature sig_ts[MAX_SIGNATURES];

	unsigned int signatures_length[MAX_SIGNATURES];
	unsigned int signatures_name_length[MAX_SIGNATURES];
	unsigned char signatures_name[MAX_SIGNATURES][MAX_NAME_LEN]; 
	unsigned char signatures[MAX_SIGNATURES][MAX_SIG_LEN];
#else
	struct labcomm_signature *sig_ts;           // [MAX_SIGNATURES]

	unsigned int *signatures_length;       // [MAX_SIGNATURES]
	unsigned char **signatures;            // [MAX_SIGNATURES][MAX_SIG_LEN];

	unsigned int *signatures_name_length;  // [MAX_SIGNATURES]
	unsigned char **signatures_name;       // [MAX_SIGNATURES][MAX_NAME_LEN];
#endif

} labcomm_sig_parser_t;


int labcomm_sig_parser_init(labcomm_sig_parser_t *p, size_t size, 
                            size_t stacksize, size_t max_num_signatures, 
                            size_t max_name_len, size_t max_sig_len);
int labcomm_sig_parser_read_file(labcomm_sig_parser_t *p, FILE *f);

int accept_packet(labcomm_sig_parser_t *p);

struct labcomm_signature *get_sig_t(labcomm_sig_parser_t *p,unsigned int uid);

unsigned int get_signature_len(labcomm_sig_parser_t *p,unsigned int uid);
unsigned char* get_signature_name(labcomm_sig_parser_t *p,unsigned int uid);
unsigned char* get_signature(labcomm_sig_parser_t *p,unsigned int uid);
void dump_signature(labcomm_sig_parser_t *p,unsigned int uid);


int more(labcomm_sig_parser_t *b);


/* parse signature and skip the corresponding bytes in the labcomm_sig_parser 
 */
int skip_packed_sample_data(labcomm_sig_parser_t *p, struct labcomm_signature *sig);

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

#ifndef TRUE

#define FALSE 0
#define TRUE 1

#endif

typedef enum{
        VERSION = LABCOMM_VERSION,
        SAMPLE_DECL = LABCOMM_SAMPLE_DEF,
        TYPE_DECL = LABCOMM_TYPEDEF,

        ARRAY_DECL = LABCOMM_ARRAY,
        STRUCT_DECL = LABCOMM_STRUCT,

        TYPE_BOOLEAN  = LABCOMM_BOOLEAN,
        TYPE_BYTE  = LABCOMM_BYTE,
        TYPE_SHORT  = LABCOMM_SHORT,
        TYPE_INTEGER  = LABCOMM_INT,
        TYPE_LONG  = LABCOMM_LONG,
        TYPE_FLOAT  = LABCOMM_FLOAT,
        TYPE_DOUBLE  = LABCOMM_DOUBLE,
        TYPE_STRING  = LABCOMM_STRING
} labcomm_type ;
#endif
