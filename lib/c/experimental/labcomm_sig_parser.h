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
#undef DEBUG_STACK

#undef QUIET 		//just print type and size when skipping data
#undef VERBOSE 		// print in great detail

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

#define RETURN_STRINGS  //  not really tested

#ifndef TRUE

#define FALSE 0
#define TRUE 1

#endif

typedef enum{
        TYPE_DECL = LABCOMM_TYPEDEF,
        SAMPLE_DECL = LABCOMM_SAMPLE,

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

/* internal type: stack for the parser */
typedef struct {
	unsigned char* c;
	size_t size;
	size_t capacity;
	unsigned int idx;
	unsigned int val_top;
	int * val_stack;
	unsigned int ptr_top;
	void** ptr_stack;
	size_t stacksize;
	int current_decl_is_varsize;
} buffer;

int init_buffer(buffer *b, size_t size, size_t stacksize) ;
int read_file(FILE *f, buffer *b);

int accept_packet(buffer *d);

//XXX experimental
#define MAX_SIGNATURES 10
#define MAX_NAME_LEN 32 
#define MAX_SIG_LEN 128

#define STACK_SIZE 16

labcomm_signature_t *get_sig_t(unsigned int uid);

unsigned int get_signature_len(unsigned int uid);
unsigned char* get_signature_name(unsigned int uid);
unsigned char* get_signature(unsigned int uid);
void dump_signature(unsigned int uid);


/* parse signature and skip the corresponding bytes in the buffer 
 */
int skip_packed_sample_data(buffer *d, labcomm_signature_t *sig);

#endif
