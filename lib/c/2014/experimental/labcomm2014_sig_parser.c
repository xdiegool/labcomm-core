/* labcomm2014_sig_parser.c:
 * an example parser for labcomm signatures, illustrating how to skip samples
 * based on their signature. Intended as an embryo for introducing this
 * functionality into the lib to allow a channel to survive types with no
 * registered handler.
 */


/* TODO, and notes about strange quirks
 *
 * - the return values for the accept_ functions are not really used anymore
 *   as the parser "peeks" and calls the correct accept function instead.
 *   This should be refactored
 *
 * - The RETURN_STRINGS and where/if to allocate strings is to be decided, it
 *   is currently not used
 *
 * - The dynamic allocation of the parser is not quite dynamic, the sizes are
 *   set through the init function, and are then static.
 *   This should be adapted when allocation is parameterized/user-definable
 *   for the entire lib.
 */



#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "labcomm2014_sig_parser.h"

static void error(char *s) {
	fprintf(stderr, "ERROR: %s", s);
	fprintf(stderr, "\nexiting\n");
	exit(1);
}

/* aux method for reading a big endian uint32 from a char* (i.e. ntohl but for explicit char*) */
static int unpack32(unsigned char *c, int idx) {
       int b0=(c[idx]) << 3 ;
       int b1=(c[idx+1]) << 2 ;
       int b2=(c[idx+2]) << 1 ;
       int b3=c[idx+3];

       return  b0 | b1 | b2 | b3;
}

static inline int get32(labcomm2014_sig_parser_t *b) {
       int res = unpack32(b->c, b->idx);
       b->idx+=4;
       return res;
}


/* aux method for reading labcomm varint from a char*
   size is an out parameter: if not NULL the number of bytes read will be written here
*/
static int unpack_varint(unsigned char *buf,
				  int idx,
				  size_t *size)
{
        int res = 0;
        unsigned int i=0;
        unsigned char cont = LABCOMM2014_TRUE;

        do {
                unsigned char c = buf[idx+i];
                res = (res << 7) | (c & 0x7f);
                cont = c & 0x80;
                i++;
        } while(cont);

	if(size != NULL)
		*size = i;
        return res;
}

void dumpValStack(labcomm2014_sig_parser_t *b) {
	printf("=== value stack: ");
#ifdef DEBUG_STACK_VERBOSE
	int i;
	for(i=0; i<STACK_SIZE; i++) { //HERE BE DRAGONS
		printf("%2.2x ", b->val_stack[i]);
	}
#endif
	printf("    top==%d\n", b->val_top);
}
void dumpPtrStack(labcomm2014_sig_parser_t *b) {
	printf("=== pointer stack: ");
#ifdef DEBUG_STACK_VERBOSE
	int i;
	for(i=0; i<STACK_SIZE; i++) { //HERE BE DRAGONS
		printf("%2.2x ", b->ptr_stack[i]);
	}
#endif
	printf("    top==%d\n", b->ptr_top);
}

void push_val(labcomm2014_sig_parser_t *b, unsigned int e) {
	b->val_stack[b->val_top]=e;
	b->val_top=b->val_top-1;
	if(b->val_top<0) error("value stack overrun");
#if defined DEBUG && !defined QUIET_STACK
	dumpValStack(b);
#endif
}
unsigned int pop_val(labcomm2014_sig_parser_t *b) {
	b->val_top=b->val_top+1;
	if(b->val_top>b->stacksize) error("value stack underrun");
#if defined DEBUG && !defined QUIET_STACK
	dumpValStack(b);
#endif
	return b->val_stack[b->val_top];
}
void push_ptr(labcomm2014_sig_parser_t *b, void* e) {
	b->ptr_stack[b->ptr_top]=e;
	b->ptr_top=b->ptr_top-1;
	if(b->ptr_top<0) error("pointer stack overrun");
#ifdef DEBUG
	dumpPtrStack(b);
#endif
}
void* pop_ptr(labcomm2014_sig_parser_t *b) {
	b->ptr_top=b->ptr_top+1;
	if(b->ptr_top>b->stacksize) error("pointer stack underrun");
#ifdef DEBUG
	dumpPtrStack(b);
#endif
	return b->ptr_stack[b->ptr_top];
}

int labcomm2014_sig_parser_init(labcomm2014_sig_parser_t *b, size_t buffer_size,
                            size_t stacksize, size_t num_signatures,
                            size_t max_name_len, size_t max_sig_len)
{
	b->c = malloc(buffer_size);
	b->capacity = buffer_size;
	b->size = 0;
	b->idx = 0;

	b->stacksize = stacksize;

	b->val_stack = calloc(stacksize, sizeof(b->val_stack));
	b->val_top = stacksize-1;

	b->ptr_stack = calloc(stacksize, sizeof(b->ptr_stack));
	b->ptr_top = stacksize-1;

	b->max_signatures = num_signatures;
	b->max_name_len = max_name_len;
	b->max_sig_len = max_sig_len;

#ifdef STATIC_ALLOCATION
	printf("warning: labcomm2014_sig_parser_t_init: size params ignored, using defaults from .h file \n");
#else
	b->sig_ts=calloc(num_signatures, sizeof(struct labcomm2014_signature));
	b->signatures_length=calloc(num_signatures, sizeof(int));
	b->signatures_name_length=calloc(num_signatures, sizeof(int));
	b->signatures_name=calloc(num_signatures, sizeof(void *)); //HERE BE DRAGONS: add range checks
	b->signatures=calloc(num_signatures, sizeof(void *));
	int i;
	for(i = 0; i<num_signatures; i++) {
		b->signatures[i] = calloc(b->max_sig_len, sizeof(char));
		b->signatures_name[i] = calloc(b->max_name_len, sizeof(char));
	}
#endif
	return b->c == NULL || b->val_stack == NULL || b->ptr_stack == NULL;
}

/* free the objects allocated by labcomm_sig_parser_init(b)
 * NB! does not free b itself */
void labcomm2014_sig_parser_free(labcomm2014_sig_parser_t *b)
{
    int i;
	free(b->c);
	free(b->val_stack);
	free(b->ptr_stack);
#ifndef STATIC_ALLOCATION
	for(i = 0; i<b->max_signatures; i++) {
		free(b->signatures[i]);
		free(b->signatures_name[i]);
	}
	free(b->signatures);
	free(b->signatures_name);
	free(b->signatures_name_length);
	free(b->signatures_length);
	free(b->sig_ts);
#endif
}

int labcomm2014_sig_parser_read_file(labcomm2014_sig_parser_t *b, FILE *f) {
        int s = fread(b->c, sizeof(char), b->capacity, f);
        b->size = s;
        b->idx=0;
        return s;
}

int more(labcomm2014_sig_parser_t *b)
{
	return b->idx < b->size;
}

unsigned char get(labcomm2014_sig_parser_t *b) {
	return b->c[b->idx++];
}

unsigned char peek(labcomm2014_sig_parser_t *b) {
	return b->c[b->idx];
}

void advance(labcomm2014_sig_parser_t *b) {
	b->idx++;
}

void advancen(labcomm2014_sig_parser_t *b, size_t n) {
	b->idx+=n;
}

int peek_varint(labcomm2014_sig_parser_t *b, size_t *size) {
        return unpack_varint(b->c, b->idx, size);
}

int get_varint(labcomm2014_sig_parser_t *b) {
        size_t size;
        int res = peek_varint(b, &size);
        advancen(b, size);
        return res;
}
int get_varint_size(labcomm2014_sig_parser_t *b, size_t *size) {
        unsigned int res = peek_varint(b, size);
        advancen(b, *size);
        return res;
}

void getStr(labcomm2014_sig_parser_t *b, unsigned char *dest, size_t size) {
	int rem = b->size - b->idx;
	if( size > rem )
		size = rem;
	strncpy((char *)dest, (char *)&b->c[b->idx], size);
    dest[size] = 0;
	b->idx += size;
}

struct labcomm2014_signature *get_sig_t(labcomm2014_sig_parser_t *p, unsigned int uid)
{
	return &(p->sig_ts[uid-LABCOMM_USER]);
}

unsigned int get_signature_len(labcomm2014_sig_parser_t *p, unsigned int uid){
	//return signatures_length[uid-LABCOMM_USER];
	return p->sig_ts[uid-LABCOMM_USER].size;
}
unsigned char* get_signature(labcomm2014_sig_parser_t *p, unsigned int uid){
	//return signatures[uid-LABCOMM_USER];
	return p->sig_ts[uid-LABCOMM_USER].signature;
}

//is this needed?
//unsigned int get_signature_name_len(labcomm2014_sig_parser_t *p, unsigned int uid){
//	return signatures_name_length[uid-LABCOMM_USER];
//}

char* get_signature_name(labcomm2014_sig_parser_t *p, unsigned int uid){
	//return signatures_name[uid-LABCOMM_USER];
	return p->sig_ts[uid-LABCOMM_USER].name;
}

void dump_signature(labcomm2014_sig_parser_t *p, unsigned int uid){
	int i;
	unsigned int len = get_signature_len(p, uid);
	printf("signature for uid %x : %s (len=%d):\n", uid, get_signature_name(p, uid), len);
	unsigned char* sig = get_signature(p, uid);
	for(i=0; i<len; i++) {
		printf("%2.2x ",sig[i]);
		if( i < len-1 && (i+1)%8==0 ) printf("\n");
	}
	printf("\n");
}

static inline void labcomm2014_sig_parser_t_set_varsize(labcomm2014_sig_parser_t *b)
{
	b->current_decl_is_varsize = LABCOMM2014_TRUE;
}
static size_t labcomm2014_sizeof_primitive(unsigned int type)
{
	switch(type) {
		case TYPE_BOOLEAN :
		case TYPE_BYTE :
			return 1;
		case TYPE_SHORT :
			return 2;
		case TYPE_INTEGER :
		case TYPE_FLOAT :
		case TYPE_SAMPLE_REF :
			return 4;
		case TYPE_LONG :
		case TYPE_DOUBLE :
			return 8;
		default:
			printf("labcomm2014_sizeof_primitive(%x)\n", type);
		 	error("labcomm2014_sizeof_primitive should only be called for primitive types");
            return 0;
	}
}

//these are inlined in do_accept_packet
//static int accept_type_decl(labcomm2014_sig_parser_t *d);
//static int accept_sample_decl(labcomm2014_sig_parser_t *d);
static int accept_user_id(labcomm2014_sig_parser_t *d);
static int accept_string(labcomm2014_sig_parser_t *d);
static int accept_type(labcomm2014_sig_parser_t *d);
static int accept_array_decl(labcomm2014_sig_parser_t *d);
static int accept_intentions(labcomm2014_sig_parser_t *d);
#if 0  
// UNUSED declarations
static int accept_string_length(labcomm2014_sig_parser_t *d);
static int accept_char(labcomm2014_sig_parser_t *d);
static int accept_number_of_indices(labcomm2014_sig_parser_t *d);
static int accept_indices(labcomm2014_sig_parser_t *d);
static int accept_variable_index(labcomm2014_sig_parser_t *d);
static int accept_fixed_index(labcomm2014_sig_parser_t *d);
static int accept_number_of_fields(labcomm2014_sig_parser_t *d);
#endif
static int accept_struct_decl(labcomm2014_sig_parser_t *d);
static int accept_field(labcomm2014_sig_parser_t *d);
static int accept_sample_data(labcomm2014_sig_parser_t *d);

static unsigned char labcomm2014_varint_sizeof(unsigned int i)
{
	if(i < 128) {
		return 1;
	} else {
		unsigned char res = 1;
		while (i >>= 7) ++res;

		return res;
	}
}
int encoded_size_static(struct labcomm2014_signature *sig, void *unused)
{
#ifdef LABCOMM_EXPERIMENTAL_CACHED_ENCODED_SIZE
	if(sig->cached_encoded_size == -1) {
		error("encoded_size_static called for var_size sample or uninitialized signature");
	}
	return sig->cached_encoded_size;
#else
	printf("Warning: encoded_size_static currently broken\n");
	return -1;
#endif
}

/* This function probably never will be implemented, as it would be
   similar to skip_packed_sample_data. And if unregistered variable size
   samples is to be handled, the proper way is to generate and register
   a handler. Thus, the most probable use of the encoded_size function
   on the receiver side, is to skip unhandled samples.
*/

int encoded_size_parse_sig(struct labcomm2014_signature *sig, void *sample)
{
	printf("Warning: encoded_size_parse_sig not implemented\n");
	return -1;
}

static int accept_signature(labcomm2014_sig_parser_t *d, 
                            labcomm2014_type type,
                            unsigned int start,
                            unsigned int uid, char *name)
{
    get_varint(d); // ignore sig len
    VERBOSE_PRINTF("\ntype = ");
    accept_type(d);
    //printf(" : ");
    //unsigned int dt = pop(d);
#ifdef USE_TYPE_AND_SIZE
    unsigned int type = pop_val(d);
    unsigned int enc_size = pop_val(d);
#else
    pop_val(d); // unsigned int type
    pop_val(d); // unsigned int enc_size
#endif
    if(type != PKG_SAMPLE_DECL) {
        if(type == PKG_SAMPLE_REF) {
            INFO_PRINTF("accept_signature: ignoring sample ref\n");
            return LABCOMM2014_TRUE;
        } else if (type == PKG_TYPE_DECL) {
            INFO_PRINTF("accept_signature: ignoring typedef\n");
            return LABCOMM2014_TRUE;
        } else {
            error("decl is neither sample, ref, or typedef???");
            return LABCOMM2014_FALSE;
        }
    }
    unsigned int end = d->idx;
    unsigned int len = end-start;

    struct labcomm2014_signature *newsig = get_sig_t(d, uid);
//		newsig->type = type;
    if(len <= d->max_sig_len) {
        d->signatures_length[uid-LABCOMM_USER] = len;
        memcpy(d->signatures[uid-LABCOMM_USER], &d->c[start], len);
        newsig->size = len;
        newsig->signature = d->signatures[uid-LABCOMM_USER];
        newsig->name = name;
    } else {
        error("sig longer than max length (this ought to be dynamic...)");
    }
    VERBOSE_PRINTF("signature for uid %x: %s (start=%x,end=%x,len=%d)\n", uid, get_signature_name(d, uid), start,end, len);
    INFO_PRINTF("accept_signature: %s\n", newsig->name);
#ifdef LABCOMM_EXPERIMENTAL_CACHED_ENCODED_SIZE
    if(! d->current_decl_is_varsize) {
        newsig->cached_encoded_size = enc_size;
        newsig->encoded_size = encoded_size_static;
        INFO_PRINTF(".... is static size = %d\n", enc_size);
    } else {
        newsig->cached_encoded_size = -1;
        newsig->encoded_size = encoded_size_parse_sig;
        INFO_PRINTF(".... is variable size\n");
    }
#endif
    return LABCOMM2014_TRUE;
}
static int accept_decl(labcomm2014_sig_parser_t *d, labcomm2014_type type)
{
	if(accept_user_id(d)) {
		unsigned int uid = pop_val(d);
		VERBOSE_PRINTF(", intentions = ");
		accept_intentions(d);
		unsigned int start = d->idx;
		unsigned int nstart = pop_val(d);
		unsigned int nlen = pop_val(d);
#ifdef RETURN_STRINGS
		char *str = (char *) pop_ptr(d);
		free(str);
#endif
		unsigned char lenlen = labcomm2014_varint_sizeof(nlen);

        if(type != PKG_SAMPLE_DECL) {
            // don't record typedefs and samplerefs (for now)
            // to avoid number clashes with sample defs
            // in the parser struct 
            return accept_signature(d, type, start, uid, (char *) &d->c[nstart+lenlen]);
        }
		if(nlen < d->max_name_len) { // leave 1 byte for terminating NULL
            char *name;
			d->signatures_name_length[uid-LABCOMM_USER] = nlen;
			memcpy(d->signatures_name[uid-LABCOMM_USER], &d->c[nstart+lenlen], nlen);
			d->signatures_name[uid-LABCOMM_USER][nlen]=0;
			name = d->signatures_name[uid-LABCOMM_USER];
            return accept_signature(d, type, start, uid, name);
		} else {
			error("sig name longer than max length (this ought to be dynamic...");
            return LABCOMM2014_FALSE;
		}
	} else {
		error("sample_decl with uid < LABCOMM_USER");
		return LABCOMM2014_FALSE;
	}
}

// HERE BE DRAGONS! what does the return value mean?
int accept_packet(labcomm2014_sig_parser_t *d) {
        size_t nbytes;
        unsigned int type = peek_varint(d, &nbytes) ;
    if(type == PKG_VERSION ) {
		advancen(d, nbytes);//consume type field
        get_varint(d); //ignore length field
		VERBOSE_PRINTF("got version.\n");
        accept_string(d);
	    pop_val(d); // ignore length, for now
#ifdef RETURN_STRINGS
            char *str = (char *) pop_ptr(d);
            free(str);
#endif
	} else if (type == PKG_SAMPLE_DECL) {
		d->current_decl_is_varsize = LABCOMM2014_FALSE; // <-- a conveniance flag in labcomm2014_sig_parser_t
		advancen(d, nbytes);
		VERBOSE_PRINTF("sample_decl ");
        get_varint(d); //ignore length field
		accept_decl(d, type);
	} else if (type == PKG_SAMPLE_REF) {
		d->current_decl_is_varsize = LABCOMM2014_FALSE; // <-- a conveniance flag in labcomm2014_sig_parser_t
		advancen(d, nbytes);
		VERBOSE_PRINTF("sample_ref ");
        get_varint(d); //ignore length field
		accept_decl(d, type);
    }else if(type == PKG_TYPE_DECL ) {
		d->current_decl_is_varsize = LABCOMM2014_FALSE; // <-- a conveniance flag in labcomm2014_sig_parser_t
		advancen(d, nbytes);//consume type field
		VERBOSE_PRINTF("type_decl ");
        get_varint(d); //ignore length field
		accept_decl(d, type);
	} else if (type == PKG_TYPE_BINDING) {
		VERBOSE_PRINTF("type_binding ");
		advancen(d, nbytes);
        get_varint(d); //ignore length field
#ifdef VERBOSE        
        int sid = 
#endif            
            get_varint(d); //ignore sample id field
#ifdef VERBOSE        
        int tid =
#endif            
            get_varint(d); //ignore type id field
		VERBOSE_PRINTF("sid=0x%x, tid=0x%x\n ", sid, tid);
	} else if(type >= LABCOMM_USER) {
#ifdef EXIT_WHEN_RECEIVING_DATA
		printf("*** got sample data, exiting\n");
		exit(0);
#else
		accept_sample_data(d);
#endif
	} else {
#ifdef EXIT_ON_UNKNOWN_TAG
        error("got unknown type (<LABCOMM_USER)");
		exit(1);
#else
        int len = get_varint(d); // length field
        printf("got unknown tag: 0x%x, skipping %d bytes\n",type, len);
        advancen(d, len);
#endif
	}
    return LABCOMM2014_TRUE;
}

static int accept_user_id(labcomm2014_sig_parser_t *d){
    size_t nbytes;
	int uid = peek_varint(d, &nbytes);
	if(uid >= LABCOMM_USER) {
		advancen(d, nbytes);
		VERBOSE_PRINTF("uid = 0x%x ", uid);
		push_val(d, uid);
		return LABCOMM2014_TRUE;
	} else {
        error("uid < LABCOMM_USER");
		return LABCOMM2014_FALSE;
	}
}

/** pushes (index in stream of name) and (length of name)  */
static int accept_intentions(labcomm2014_sig_parser_t *d){
	unsigned int num = get_varint(d);
    int i;
    int npos = 0;
    int nlen = 0;

    for(i=0; i<num; i++) {
        int klen, vlen;
        printf("( ");

#ifdef RETURN_STRINGS
        char *key;
        char *val;
#endif
        accept_string(d);
        klen = pop_val(d); 
        if(klen==0) {
            npos = d->idx;
        }
#ifdef RETURN_STRINGS
        key = (char *) pop_ptr(d);
        if(klen!=0) {
            printf("%s] : ",key);
        }
        free(key);
#else
        if(klen!=0) {
            printf(": ");
        } else {
            printf("name: ");
        }
#endif
        accept_string(d);
        printf(" ");
        vlen = pop_val(d);

        if(klen==0) {
            nlen = vlen;
        }
#ifdef RETURN_STRINGS
        val = (char *) pop_ptr(d);
        printf("%s %s",(klen?",val : ":""), val);
        free(val);
#endif
        printf(") ");
    }
    push_val(d, nlen);
    push_val(d, npos);
	return LABCOMM2014_TRUE;
}

static int accept_string(labcomm2014_sig_parser_t *d){
	unsigned int len = get_varint(d);
	unsigned char *str=malloc(len+1); // len is without terminating null
	getStr(d, str, len);
	VERBOSE_PRINTF("%s", str);
#ifdef RETURN_STRINGS
	push_ptr(d, str);
#else
	free(str);
#endif
	push_val(d, len);
	return LABCOMM2014_TRUE;
}
/* pushes size and type id */
static int accept_type(labcomm2014_sig_parser_t *d){
	size_t nbytes;
        unsigned int type = peek_varint(d, &nbytes) ;
	switch(type) {
		case TYPE_BOOLEAN :
			VERBOSE_PRINTF("boolean\n");
			advancen(d, nbytes);
			push_val(d, 1);
			break;
		case TYPE_BYTE :
			VERBOSE_PRINTF("byte\n");
			advancen(d, nbytes);
			push_val(d,  1);
			break;
		case TYPE_SHORT :
			VERBOSE_PRINTF("short\n");
			advancen(d, nbytes);
			push_val(d,  2);
			break;
		case TYPE_INTEGER :
			VERBOSE_PRINTF("integer\n");
			advancen(d, nbytes);
			push_val(d,  4);
			break;
		case TYPE_LONG :
			VERBOSE_PRINTF("long\n");
			advancen(d, nbytes);
			push_val(d,  8);
			break;
		case TYPE_FLOAT :
			VERBOSE_PRINTF("float\n");
			advancen(d, nbytes);
			push_val(d,  4);
			break;
		case TYPE_DOUBLE :
			VERBOSE_PRINTF("double\n");
			advancen(d, nbytes);
			push_val(d,  8);
			break;
		case TYPE_STRING :
			VERBOSE_PRINTF("string\n");
			advancen(d, nbytes);
			labcomm2014_sig_parser_t_set_varsize(d);
			push_val(d, 0);
			break;
		case TYPE_SAMPLE_REF :
			VERBOSE_PRINTF("sample\n");
			advancen(d, nbytes);
			push_val(d,  4);
			break;
		case ARRAY_DECL :
			accept_array_decl(d);
			pop_val(d); // ignore element type
			// push(d, pop(d) is a NOP --> leave size on stack
			break;
		case STRUCT_DECL :
			accept_struct_decl(d);
			// push(d, pop(d) is a NOP --> leave size on stack
			break;
		default :
            //should we distinguish between SAMPLE_DEF and TYPE_DEF here?
			//printf("accept_type default (type==%x) should not happen\n", type);
			VERBOSE_PRINTF("user type 0x%x\n",type);
			advancen(d, nbytes);
			push_val(d, 0);
			push_val(d, type);
			return LABCOMM2014_FALSE;
	}
	push_val(d, type);
	return LABCOMM2014_TRUE;
}

/* pushes size and element type */
static int accept_array_decl(labcomm2014_sig_parser_t *d){
        size_t nbytes;
        int tid = peek_varint(d, &nbytes) ;
	if(tid == ARRAY_DECL) {
		advancen(d, nbytes);
		unsigned int nidx = get_varint(d);
		VERBOSE_PRINTF("%d dim array", nidx);
		int i;
		unsigned int numVar=0;
		unsigned int size=1;
		for(i=0; i<nidx; i++) {
			unsigned int idx = get_varint(d);
			if(idx == 0) {
				numVar++;
				VERBOSE_PRINTF("[_] ");
				labcomm2014_sig_parser_t_set_varsize(d);
			} else {
				VERBOSE_PRINTF("[%d] ", idx);
				size*=idx;
			}
		}
		VERBOSE_PRINTF(" of ");
		accept_type(d);
		unsigned int et= pop_val(d);  // type
		unsigned int es= pop_val(d);  // (encoded) size
#ifdef DEBUG
		printf("accept_array_decl: et = %x\n", et);
#endif
		if(numVar == 0) {
#ifdef DEBUG
			printf("size=%d, es=%d\n", size, es);
#endif
			push_val(d,  (size*es));
		} else {
//HERE BE DRAGONS! push a (non-) size for variable size arrays?
			push_val(d, 0);
		}
		push_val(d, et);
		return LABCOMM2014_TRUE;
	} else {
		printf("accept_array_decl: type=%x, should not happen\n",tid);
		push_val(d, 0);
		push_val(d, tid);
		return LABCOMM2014_FALSE;
	}
}

/* pushes size */
static int accept_struct_decl(labcomm2014_sig_parser_t *d){
	size_t nbytes;
        int tid = peek_varint(d, &nbytes) ;
	if(tid == STRUCT_DECL) {
		advancen(d, nbytes);
		unsigned int nf = get_varint(d);
        if(nf == 0) {
            VERBOSE_PRINTF("void\n");
        } else {
            VERBOSE_PRINTF("%d field struct:\n", nf);
        }
		int i;
#ifdef USE_UNUSED_VARS
		int numVar=0;
		int size=0;
#endif
		unsigned int fieldsizes=0;
		for(i=0; i<nf; i++) {
			accept_field(d);
			fieldsizes += pop_val(d);
		}
		push_val(d, fieldsizes);
		return LABCOMM2014_TRUE;
	} else {
		printf("accept_struct_decl: type=%x, should not happen\n",tid);
		push_val(d, 0);
		return LABCOMM2014_FALSE;
	}
}

/* pushes field size */
static int accept_field(labcomm2014_sig_parser_t *d){
	VERBOSE_PRINTF("\tfield: ");
	accept_intentions(d);
	pop_val(d); // ignore name pos, for now
	pop_val(d); // ignore name length, for now
#ifdef RETURN_STRINGS
		char *str = (char *) pop_ptr(d);
		free(str);
#endif
	VERBOSE_PRINTF("\n\ttype: ");
	accept_type(d);
	pop_val(d); // ignore type, for now
	// push(pop() is really a NOP , leave size on the stack when debugging done
	VERBOSE_PRINTF("\n");
	return LABCOMM2014_TRUE;
}
static int accept_sample_data(labcomm2014_sig_parser_t *d){
	accept_user_id(d);
	unsigned int uid = pop_val(d);
	printf("sample data... uid=0x%x\n", uid);
    int len = get_varint(d); //length field
#ifdef DEBUG
	dump_signature(d, uid);
#endif
#ifdef SKIP_BY_PARSING
	struct labcomm2014_signature *sigt = get_sig_t(d, uid);
	int encoded_size = sigt->encoded_size(NULL);
	INFO_PRINTF("encoded_size from sig: %d\n", encoded_size);
	struct labcomm2014_signature *sig = get_sig_t(d, uid);
	skip_packed_sample_data(d, sig);
#else
	advancen(d, len);
#endif
	return LABCOMM2014_TRUE;
}

static int skip_type(unsigned int,labcomm2014_sig_parser_t*,unsigned char*,unsigned int,int*) ;

static int skip_array(labcomm2014_sig_parser_t *d, unsigned char *sig, int len, int *pos) {
	unsigned int skip = 0;
	unsigned int tot_nbr_elem_tmp = 1;
	size_t nbytes;
	unsigned int nIdx = unpack_varint(sig, *pos, &nbytes);
	VERBOSE_PRINTF("skip_array: nIdx = %d (from sig)\n", nIdx);
	*pos +=nbytes;
	unsigned int idx[nIdx];
	unsigned int nVar=0;

	unsigned int i;

	for(i=0; i<nIdx; i++) {
		idx[i] = unpack_varint(sig, *pos, &nbytes);
		*pos += nbytes;
		VERBOSE_PRINTF("skip_array: idx[%d]=%d (from sig)\n", i, idx[i]);
		if(idx[i] == 0) {
			nVar++;
		} else {
			tot_nbr_elem_tmp *= idx[i];
		}
	}
	int var[nVar];

	unsigned char varSize=0; // total number of bytes required for var size fields
	for(i=0; i<nVar; i++) {
		var[i] = get_varint_size(d, &nbytes);
		varSize += nbytes;
		VERBOSE_PRINTF("skip_array: var[%d]=%d (from sample)\n", i, var[i]);
		tot_nbr_elem_tmp *= var[i];
	}

	int type = unpack_varint(sig, *pos, &nbytes);
	*pos+=nbytes;

	unsigned int elemSize = labcomm2014_sizeof_primitive(type);

	skip = elemSize * tot_nbr_elem_tmp;

	VERBOSE_PRINTF("skip_array: skip: %d * %d = %d\n", tot_nbr_elem_tmp, elemSize ,skip);

	advancen(d, skip);

	//return skip + 4*nVar;
	return skip + varSize;
}

int skip_struct(labcomm2014_sig_parser_t *d, unsigned char *sig, unsigned int len, int *pos) {
	size_t nbytes;
	int nFields = unpack_varint(sig,*pos, &nbytes);
	*pos += nbytes;
	unsigned int i;
	unsigned int skipped=0;
	VERBOSE_PRINTF("skip_struct (%d fields)\n", nFields);
	for(i=0; i<nFields; i++) {
		//skip name
		unsigned int namelen = unpack_varint(sig, *pos, &nbytes);
#ifdef DEBUG
		VERBOSE_PRINTF("field #%d:\n----namelen==%d\n",i,namelen);
		char name[namelen+1]; //HERE BE DRAGONS. alloca?
		strncpy(name, (const char *)sig+*pos+nbytes, namelen);
		name[namelen]=0;
		VERBOSE_PRINTF("----name = %s\n",name);
#endif
		*pos += (nbytes+namelen); // 32bit len + actual string

		unsigned int type = unpack_varint(sig, *pos, &nbytes);
		*pos += nbytes;
#ifdef DEBUG
		VERBOSE_PRINTF("----type == %x\n", type);
#endif
		skipped += skip_type(type, d, sig, len, pos);
	}
	return skipped;
}
#ifndef QUIET
/* print and skip */
int skip_type(unsigned int type, labcomm2014_sig_parser_t *d,
		unsigned char *sig, unsigned int len, int *pos)
{
	int skipped=0;
	printf("skip_type %x:", type);
	switch(type) {
		case TYPE_BOOLEAN :
			printf("boolean [%d]\n", get(d));
			skipped++;
			break;
		case TYPE_BYTE :
			printf("byte [%d]\n", get(d));
			skipped++;
			break;
		case TYPE_SHORT :
			//XXX not supported
			advancen(d,2);
			skipped+=2;
			break;
		case TYPE_INTEGER :
			printf("integer [%d]\n", get32(d));
			skipped +=4;
			break;
		case TYPE_FLOAT :
			//XXX not supported
			advancen(d,4);
			skipped+=4;
			break;
		case TYPE_LONG :
		case TYPE_DOUBLE :
			//XXX not supported
			advancen(d,8);
			skipped+=8;
			break;
		case TYPE_STRING :
			{
			size_t nbytes;
			int len = get_varint_size(d, &nbytes);
			int i;
			printf("string [");
			for(i=0; i<len; i++)
				printf("%c", get(d));
			printf("]\n");
			skipped+=len+nbytes;
			break;}
		case ARRAY_DECL :
			printf("array\n");
			skipped += skip_array(d, sig, len, pos);
			break;
		case STRUCT_DECL :
			printf("struct\n");
			skipped += skip_struct(d, sig, len, pos);
			break;
		default:
			printf("ERROR: skip_type: type = %x\n", type);
			exit(1);
	}
	return skipped;
}
#else
int skip_type(unsigned int type, labcomm2014_sig_parser_t *d,
		const char *sig, unsigned int len, int *pos)
{
	int skipped=0;
	VERBOSE_PRINTF("skip_type %x\n", type);
	switch(type) {
		case TYPE_BOOLEAN :
		case TYPE_BYTE :
			advancen(d,1);
			skipped++;
			break;
		case TYPE_SHORT :
			advancen(d,2);
			skipped+=2;
			break;
		case TYPE_INTEGER :
		case TYPE_FLOAT :
			advancen(d,4);
			skipped+=4;
			break;
		case TYPE_LONG :
		case TYPE_DOUBLE :
			advancen(d,8);
			skipped+=8;
			break;
		case TYPE_STRING :
			{
			size_t nbytes;
			int len = get_varint_size(d, &nbytes);
			advancen(d,len);
			skipped+=len+nbytes;
			break;}
		case ARRAY_DECL :
			skipped += skip_array(d, sig, len, pos);
			break;
		case STRUCT_DECL :
			skipped += skip_struct(d, sig, len, pos);
			break;
		default:
			printf("ERROR: skip_type: type = %x\n", type);
			exit(1);
	}
	return skipped;
}
#endif

/* parse signature and skip the corresponding bytes in the labcomm2014_sig_parser_t
 */
int skip_packed_sample_data(labcomm2014_sig_parser_t *d, struct labcomm2014_signature *sig) {
	int pos = 0; 		//current position in signature
	unsigned int skipped = 0;	//skipped byte counter
	while(pos < sig->size) {
		size_t nbytes;
		int type = unpack_varint(sig->signature,pos, &nbytes);
		pos+=nbytes;
		skipped += skip_type(type, d, sig->signature, sig->size, &pos);
	}
	printf("skipped %d bytes\n", skipped);
	return LABCOMM2014_TRUE;
}
