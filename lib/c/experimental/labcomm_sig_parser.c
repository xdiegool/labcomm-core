/* labcomm_sig_parser.c:
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
 * - TYPE_DECL is not tested (is it ever sent?)
 * 
 * - The dynamic allocation of the parser is not quite dynamic, the sizes are
 *   set through the init function, and are then static.
 *   This should be adapted when allocation is parameterized/user-definable
 *   for the entire lib.
 */
      


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "labcomm_sig_parser.h"

static void error(char *s) {
	fprintf(stderr, "ERROR: %s", s);
	fprintf(stderr, "\nexiting\n");
	exit(1);
}

/* aux method for reading a big endian uint32 from a char* (i.e. ntohl but for explicit char*) */
static unsigned int unpack32(unsigned char *c, unsigned int idx) {
       unsigned int b0=(c[idx]) << 3 ; 
       unsigned int b1=(c[idx+1]) << 2 ;
       unsigned int b2=(c[idx+2]) << 1 ;
       unsigned int b3=c[idx+3];

       return  b0 | b1 | b2 | b3;
}

static inline unsigned int get32(labcomm_sig_parser_t *b) {
       unsigned int res = unpack32(b->c, b->idx);
       b->idx+=4;
       return res;
}


/* aux method for reading labcomm varint from a char* 
   size is an out parameter: if not NULL the number of bytes read will be written here
*/
static unsigned int unpack_varint(unsigned char *buf, 
				  unsigned int idx, 
				  unsigned char *size) 
{
        unsigned int res = 0;
        unsigned int i=0;
        unsigned char cont = TRUE;

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

void dumpValStack(labcomm_sig_parser_t *b) {
	int i;
	printf("=== value stack: ");
#ifdef DEBUG_STACK_VERBOSE
	for(i=0; i<STACK_SIZE; i++) { //HERE BE DRAGONS
		printf("%2.2x ", b->val_stack[i]);
	}
#endif
	printf("    top==%d\n", b->val_top);
}
void dumpPtrStack(labcomm_sig_parser_t *b) {
	int i;
	printf("=== pointer stack: ");
#ifdef DEBUG_STACK_VERBOSE
	for(i=0; i<STACK_SIZE; i++) { //HERE BE DRAGONS
		printf("%2.2x ", b->ptr_stack[i]);
	}
#endif
	printf("    top==%d\n", b->ptr_top);
}

void push_val(labcomm_sig_parser_t *b, unsigned int e) {
	b->val_stack[b->val_top]=e;
	b->val_top=b->val_top-1;
	if(b->val_top<0) error("value stack overrun");
#if defined DEBUG && !defined QUIET_STACK
	dumpValStack(b);
#endif
}
unsigned int pop_val(labcomm_sig_parser_t *b) {
	b->val_top=b->val_top+1;
	if(b->val_top>b->stacksize) error("value stack underrun");
#if defined DEBUG && !defined QUIET_STACK
	dumpValStack(b);
#endif
	return b->val_stack[b->val_top];
}
void push_ptr(labcomm_sig_parser_t *b, void* e) {
	b->ptr_stack[b->ptr_top]=e;
	b->ptr_top=b->ptr_top-1;
	if(b->ptr_top<0) error("pointer stack overrun");
#ifdef DEBUG
	dumpPtrStack(b);
#endif
}
void* pop_ptr(labcomm_sig_parser_t *b) {
	b->ptr_top=b->ptr_top+1;
	if(b->ptr_top>b->stacksize) error("pointer stack underrun");
#ifdef DEBUG
	dumpPtrStack(b);
#endif
	return b->ptr_stack[b->ptr_top];
}
	
int labcomm_sig_parser_init(labcomm_sig_parser_t *b, size_t buffer_size, 
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
	printf("warning: labcomm_sig_parser_t_init: size params ignored, using defaults from .h file \n");
#else
	b->sig_ts=calloc(num_signatures, sizeof(struct labcomm_signature));
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

int labcomm_sig_parser_read_file(labcomm_sig_parser_t *b, FILE *f) {
        int s = fread(b->c, sizeof(char), b->capacity, f);
        b->size = s;
        b->idx=0;
        return s;
}

int more(labcomm_sig_parser_t *b) 
{
	return b->idx < b->size;
}

unsigned char get(labcomm_sig_parser_t *b) {
	return b->c[b->idx++];
}

unsigned char peek(labcomm_sig_parser_t *b) {
	return b->c[b->idx];
}

void advance(labcomm_sig_parser_t *b) {
	b->idx++;
}

void advancen(labcomm_sig_parser_t *b, size_t n) {
	b->idx+=n;
}

unsigned int peek_varint(labcomm_sig_parser_t *b, unsigned char *size) {
        return unpack_varint(b->c, b->idx, size);
}

unsigned int get_varint(labcomm_sig_parser_t *b) {
        unsigned char size;
        unsigned int res = peek_varint(b, &size);
        advancen(b, size);
        return res;
}
unsigned int get_varint_size(labcomm_sig_parser_t *b, unsigned char *size) {
        unsigned int res = peek_varint(b, size);
        advancen(b, *size);
        return res;
}

void getStr(labcomm_sig_parser_t *b, char *dest, size_t size) {
	int rem = b->size - b->idx;
	if( size > rem ) 
		size = rem;
	strncpy(dest, &b->c[b->idx], size);
	b->idx += size;
}

struct labcomm_signature *get_sig_t(labcomm_sig_parser_t *p, unsigned int uid) 
{
	return &(p->sig_ts[uid-LABCOMM_USER]);
}

unsigned int get_signature_len(labcomm_sig_parser_t *p, unsigned int uid){
	//return signatures_length[uid-LABCOMM_USER];
	return p->sig_ts[uid-LABCOMM_USER].size;
}
unsigned char* get_signature(labcomm_sig_parser_t *p, unsigned int uid){
	//return signatures[uid-LABCOMM_USER];
	return p->sig_ts[uid-LABCOMM_USER].signature;
}

//is this needed?
//unsigned int get_signature_name_len(labcomm_sig_parser_t *p, unsigned int uid){
//	return signatures_name_length[uid-LABCOMM_USER];
//}

unsigned char* get_signature_name(labcomm_sig_parser_t *p, unsigned int uid){
	//return signatures_name[uid-LABCOMM_USER];
	return p->sig_ts[uid-LABCOMM_USER].name;
}

void dump_signature(labcomm_sig_parser_t *p, unsigned int uid){
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

static inline void labcomm_sig_parser_t_set_varsize(labcomm_sig_parser_t *b)
{
	b->current_decl_is_varsize = TRUE;
}
static size_t labcomm_sizeof_primitive(unsigned int type)
{
	switch(type) {
		case TYPE_BOOLEAN :
		case TYPE_BYTE : 
			return 1;
		case TYPE_SHORT : 
			return 2;
		case TYPE_INTEGER :
		case TYPE_FLOAT : 
			return 4;
		case TYPE_LONG :
		case TYPE_DOUBLE : 
			return 8;
		default:
			printf("labcomm_sizeof_primitive(%x)\n", type);
		 	error("labcomm_sizeof_primitive should only be called for primitive types");
	}
}

//these are inlined in do_accept_packet
//static int accept_type_decl(labcomm_sig_parser_t *d);
//static int accept_sample_decl(labcomm_sig_parser_t *d);
static int accept_user_id(labcomm_sig_parser_t *d);
static int accept_string(labcomm_sig_parser_t *d);
static int accept_string_length(labcomm_sig_parser_t *d);
static int accept_char(labcomm_sig_parser_t *d);
static int accept_type(labcomm_sig_parser_t *d);
static int accept_array_decl(labcomm_sig_parser_t *d);
static int accept_number_of_indices(labcomm_sig_parser_t *d);
static int accept_indices(labcomm_sig_parser_t *d);
static int accept_variable_index(labcomm_sig_parser_t *d);
static int accept_fixed_index(labcomm_sig_parser_t *d);
static int accept_struct_decl(labcomm_sig_parser_t *d);
static int accept_number_of_fields(labcomm_sig_parser_t *d);
static int accept_field(labcomm_sig_parser_t *d);
static int accept_sample_data(labcomm_sig_parser_t *d);

static unsigned char labcomm_varint_sizeof(unsigned int i)
{
	if(i < 128) {
		return 1;
	} else {
		unsigned char res = 1;
		while (i >>= 7) ++res;

		return res;
	}
}
int encoded_size_static(struct labcomm_signature *sig, void *unused)
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
  
int encoded_size_parse_sig(struct labcomm_signature *sig, void *sample)
{
	printf("Warning: encoded_size_parse_sig not implemented\n");
	return -1;
}

static int accept_signature(labcomm_sig_parser_t *d)
{
	if(accept_user_id(d)) {
		unsigned int uid = pop_val(d);
		unsigned int nstart = d->idx;
		VERBOSE_PRINTF(", name = ");
		accept_string(d);
		unsigned int start = d->idx;
		unsigned int nlen = pop_val(d);
#ifdef RETURN_STRINGS
		char *str = (char *) pop_ptr(d);
		free(str);
#endif
		unsigned char lenlen = labcomm_varint_sizeof(nlen);
		VERBOSE_PRINTF("\ntype = ");
		accept_type(d);
		//printf(" : ");
		//unsigned int dt = pop(d);
		unsigned int type = pop_val(d);
		unsigned int enc_size = pop_val(d);
		unsigned int end = d->idx;
		unsigned int len = end-start;

		struct labcomm_signature *newsig = get_sig_t(d, uid);
//		newsig->type = type;
		if(len <= d->max_sig_len) {
			d->signatures_length[uid-LABCOMM_USER] = len;
			memcpy(d->signatures[uid-LABCOMM_USER], &d->c[start], len);
			newsig->size = len;
			newsig->signature = d->signatures[uid-LABCOMM_USER];
		} else {
			error("sig longer than max length (this ought to be dynamic...)");
		}

		if(nlen < d->max_name_len) { // leave 1 byte for terminating NULL
			d->signatures_name_length[uid-LABCOMM_USER] = nlen;
			memcpy(d->signatures_name[uid-LABCOMM_USER], &d->c[nstart+lenlen], nlen);
			d->signatures_name[uid-LABCOMM_USER][nlen]=0;
			newsig->name = d->signatures_name[uid-LABCOMM_USER];
		} else {
			error("sig name longer than max length (this ought to be dynamic...");
		}
		VERBOSE_PRINTF("signature for uid %x: %s (start=%x,end=%x, nlen=%d,len=%d)\n", uid, get_signature_name(d, uid), start,end, nlen, len);
		INFO_PRINTF("SIG: %s\n", newsig->name);	
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
		return TRUE;
	} else {
		error("sample_decl with uid < LABCOMM_USER");	
		return FALSE;
	}
}

// HERE BE DRAGONS! what does the return value mean?
int accept_packet(labcomm_sig_parser_t *d) {
        unsigned char nbytes;
        unsigned int type = peek_varint(d, &nbytes) ;
    if(type == VERSION ) {
		//XXX is this used? If so, is it correct?
		advancen(d, nbytes);
		VERBOSE_PRINTF("ignoring version ");
        int len = get_varint(d); //ignore length field
        advancen(d, len);
    }else if(type == TYPE_DECL ) {
		//XXX is this used? If so, is it correct?
		advancen(d, nbytes);
		d->current_decl_is_varsize = FALSE; // <-- a conveniance flag in labcomm_sig_parser_t
		VERBOSE_PRINTF("type_decl ");
        get_varint(d); //ignore length field
		accept_signature(d);
	} else if (type == SAMPLE_DECL) {
		d->current_decl_is_varsize = FALSE; // <-- a conveniance flag in labcomm_sig_parser_t
		advancen(d, nbytes);
		VERBOSE_PRINTF("sample_decl ");
        get_varint(d); //ignore length field
		accept_signature(d);
	} else if(type >= LABCOMM_USER) {
#ifdef EXIT_WHEN_RECEIVING_DATA
		printf("*** got sample data, exiting\n");
		exit(0);
#else
		accept_sample_data(d);
#endif
	} else {
		error("got unknown type (<LABCOMM_USER)");
		exit(1);
	}
}

static int accept_user_id(labcomm_sig_parser_t *d){
        unsigned char nbytes;
	unsigned int uid = peek_varint(d, &nbytes);
	if(uid >= LABCOMM_USER) {
		advancen(d, nbytes);
		VERBOSE_PRINTF("uid = %x ", uid);
		push_val(d, uid);
		return TRUE;
	} else {
		return FALSE;
	}
}

static int accept_string(labcomm_sig_parser_t *d){
	unsigned int len = get_varint(d);
	char *str=malloc(len);
	getStr(d, str, len); 
	VERBOSE_PRINTF("%s", str);
#ifdef RETURN_STRINGS
	push_ptr(d, str);
#else
	free(str);
#endif
	push_val(d, len);
	return TRUE;
}
/* pushes size and type id */
static int accept_type(labcomm_sig_parser_t *d){
	unsigned char nbytes;
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
			labcomm_sig_parser_t_set_varsize(d);
			push_val(d, 0);
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
			printf("accept_type default (type==%x) should not happen\n", type);
			push_val(d, 0);
			push_val(d, type);
			return FALSE;
	}
	push_val(d, type);
	return TRUE;
}

/* pushes size and element type */
static int accept_array_decl(labcomm_sig_parser_t *d){
        unsigned char nbytes;
        unsigned int tid = peek_varint(d, &nbytes) ;
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
				labcomm_sig_parser_t_set_varsize(d);
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
		return TRUE;
	} else {
		printf("accept_array_decl: type=%x, should not happen\n",tid);
		push_val(d, 0);
		push_val(d, tid);
		return FALSE;
	}
}

/* pushes size */
static int accept_struct_decl(labcomm_sig_parser_t *d){
	unsigned char nbytes;
        unsigned int tid = peek_varint(d, &nbytes) ;
	if(tid == STRUCT_DECL) {
		advancen(d, nbytes);
		unsigned int nf = get_varint(d);
		VERBOSE_PRINTF("%d field struct:\n", nf);
		int i;
		int numVar=0;
		int size=0;
		unsigned int fieldsizes=0;
		for(i=0; i<nf; i++) {
			accept_field(d);
			fieldsizes += pop_val(d);
		}
		push_val(d, fieldsizes);
		return TRUE;
	} else {
		printf("accept_struct_decl: type=%x, should not happen\n",tid);
		push_val(d, 0);
		return FALSE;
	}
}

/* pushes field size */
static int accept_field(labcomm_sig_parser_t *d){
	VERBOSE_PRINTF("\tfield name: ");
	accept_string(d);
	pop_val(d); // ignore length, for now
#ifdef RETURN_STRINGS
		char *str = (char *) pop_ptr(d);
		free(str);
#endif
	VERBOSE_PRINTF("\n\ttype: ");
	accept_type(d);
	pop_val(d); // ignore type, for now
	// push(pop() is really a NOP , leave size on the stack when debugging done
	VERBOSE_PRINTF("\n");
}
static int accept_sample_data(labcomm_sig_parser_t *d){
	accept_user_id(d);
	unsigned int uid = pop_val(d);	
	printf("sample data... uid=0x%x\n", uid);
    int len = get_varint(d); //length field
#ifdef DEBUG
	dump_signature(d, uid);
#endif
#ifdef SKIP_BY_PARSING
	struct labcomm_signature *sigt = get_sig_t(d, uid);
	int encoded_size = sigt->encoded_size(NULL);
	INFO_PRINTF("encoded_size from sig: %d\n", encoded_size);
	struct labcomm_signature *sig = get_sig_t(d, uid);
	skip_packed_sample_data(d, sig);
#else
	advancen(d, len);
#endif
	return TRUE;
}

static int skip_type(unsigned int,labcomm_sig_parser_t*,unsigned char*,unsigned int,unsigned int*) ;

static int skip_array(labcomm_sig_parser_t *d, unsigned char *sig, unsigned int len, unsigned int *pos) {
	unsigned int skip = 0;
	unsigned int tot_nbr_elem_tmp = 1;
	unsigned char nbytes;
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
	unsigned int var[nVar];

	unsigned char varSize=0; // total number of bytes required for var size fields
	for(i=0; i<nVar; i++) {
		var[i] = get_varint_size(d, &nbytes);	
		varSize += nbytes;
		VERBOSE_PRINTF("skip_array: var[%d]=%d (from sample)\n", i, var[i]);
		tot_nbr_elem_tmp *= var[i];
	}

	unsigned int type = unpack_varint(sig, *pos, &nbytes);
	*pos+=nbytes;

	unsigned int elemSize = labcomm_sizeof_primitive(type);

	skip = elemSize * tot_nbr_elem_tmp;

	VERBOSE_PRINTF("skip_array: skip: %d * %d = %d\n", tot_nbr_elem_tmp, elemSize ,skip);
	
	advancen(d, skip);

	//return skip + 4*nVar;
	return skip + varSize;
}

int skip_struct(labcomm_sig_parser_t *d, unsigned char *sig, unsigned int len, unsigned int *pos) {
	unsigned char nbytes;
	unsigned int nFields = unpack_varint(sig,*pos, &nbytes);
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
		strncpy(name, sig+*pos+nbytes, namelen);
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
int skip_type(unsigned int type, labcomm_sig_parser_t *d, 
		unsigned char *sig, unsigned int len, unsigned int *pos) 
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
			unsigned char nbytes;
			unsigned int len = get_varint_size(d, &nbytes);
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
int skip_type(unsigned int type, labcomm_sig_parser_t *d, 
		unsigned char *sig, unsigned int len, unsigned int *pos) 
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
			unsigned char nbytes;
			unsigned int len = get_varint_size(d, &nbytes);
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

/* parse signature and skip the corresponding bytes in the labcomm_sig_parser_t 
 */
int skip_packed_sample_data(labcomm_sig_parser_t *d, struct labcomm_signature *sig) {
	unsigned int pos = 0; 		//current position in signature
	unsigned int skipped = 0;	//skipped byte counter
	while(pos < sig->size) {
		unsigned char nbytes;
		unsigned int type = unpack_varint(sig->signature,pos, &nbytes);
		pos+=nbytes;
		skipped += skip_type(type, d, sig->signature, sig->size, &pos);
	}	
	printf("skipped %d bytes\n", skipped);
	return TRUE;
}
