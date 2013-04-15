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
 *   is currently no longer used
 * 
 * - TYPE_DECL is not tested
 */
      


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "labcomm_sig_parser.h"

static void error(char *s) {
	fprintf(stderr, "%s", s);
	fprintf(stderr, "\nexiting\n");
	exit(1);
}

#define STACK_SIZE 16

/* aux method for reading a big endian uint32 from a char* (i.e. ntohl but for explicit char*) */
static unsigned int unpack32(unsigned char *c, unsigned int idx) {
       unsigned int b0=(c[idx]) << 3 ; 
       unsigned int b1=(c[idx+1]) << 2 ;
       unsigned int b2=(c[idx+2]) << 1 ;
       unsigned int b3=c[idx+3];

       return  b0 | b1 | b2 | b3;
}

static inline unsigned int get32(buffer *b) {
       unsigned int res = unpack32(b->c, b->idx);
       b->idx+=4;
       return res;
}


/* aux method for reading labcomm varint from a char* 
   size is an out parameter: if not NULL the number of bytes read will be written here
*/
static int unpack_varint(unsigned char *buf, unsigned int idx, unsigned char *size) {
        unsigned int res = 0;
        unsigned int i=0;
        unsigned char cont = TRUE;

        do {
                unsigned char c = buf[idx+i];
                res |= (c & 0x7f) << 7*i;
                cont = c & 0x80;
                i++;
        } while(cont);

	if(size != NULL)
		*size = i; 
        return res;
}

void dumpValStack(buffer *b) {
#ifdef DEBUG_STACK
	int i;
	printf("=== value stack: ");
	for(i=0; i<STACK_SIZE; i++) { //HERE BE DRAGONS
		printf("%2.2x ", b->val_stack[i]);
	}
	printf("    top==%d\n", b->val_top);
#endif
}
void dumpPtrStack(buffer *b) {
#ifdef DEBUG_STACK
	int i;
	printf("=== pointer stack: ");
	for(i=0; i<STACK_SIZE; i++) { //HERE BE DRAGONS
		printf("%2.2x ", b->ptr_stack[i]);
	}
	printf("    top==%d\n", b->ptr_top);
#endif
}

void push_val(buffer *b, unsigned int e) {
	b->val_stack[b->val_top]=e;
	b->val_top=b->val_top-1;
#ifdef DEBUG
	dumpValStack(b);
#endif
}
unsigned int pop_val(buffer *b) {
	b->val_top=b->val_top+1;
	return b->val_stack[b->val_top];
}
void push_ptr(buffer *b, void* e) {
	b->ptr_stack[b->ptr_top]=e;
	b->ptr_top=b->ptr_top-1;
#ifdef DEBUG
	dumpPtrStack(b);
#endif
}
void* pop_ptr(buffer *b) {
	b->ptr_top=b->ptr_top+1;
	return b->ptr_stack[b->ptr_top];
}
	
int init_buffer(buffer *b, size_t size, size_t stacksize) {
	b->c = malloc(size);
	b->capacity = size;
	b->size = 0;
	b->idx = 0;

	b->stacksize = stacksize;

	b->val_stack = calloc(stacksize, sizeof(b->val_stack));
	b->val_top = stacksize-1;

	b->ptr_stack = calloc(stacksize, sizeof(b->ptr_stack));
	b->ptr_top = stacksize-1;

	return b->c == NULL || b->val_stack == NULL || b->ptr_stack == NULL;
}

int read_file(FILE *f, buffer *b) {
        int s = fread(b->c, sizeof(char), b->capacity, f);
        b->size = s;
        b->idx=0;
        return s;
}

int more(buffer *b) 
{
	return b->idx < b->size;
}

unsigned char get(buffer *b) {
	return b->c[b->idx++];
}

unsigned char peek(buffer *b) {
	return b->c[b->idx];
}

void advance(buffer *b) {
	b->idx++;
}

void advancen(buffer *b, size_t n) {
	b->idx+=n;
}

unsigned int peek_varint(buffer *b, unsigned char *size) {
        return unpack_varint(b->c, b->idx, size);
}

unsigned int get_varint(buffer *b) {
        unsigned char size;
        unsigned int res = peek_varint(b, &size);
        advancen(b, size);
        return res;
}
unsigned int get_varint_size(buffer *b, unsigned char *size) {
        unsigned int res = peek_varint(b, size);
        advancen(b, *size);
        return res;
}

void getStr(buffer *b, char *dest, size_t size) {
	int rem = b->size - b->idx;
	if( size > rem ) 
		size = rem;
	strncpy(dest, &b->c[b->idx], size);
	b->idx += size;
}

labcomm_signature_t sig_ts[MAX_SIGNATURES];

unsigned int signatures_length[MAX_SIGNATURES];
unsigned int signatures_name_length[MAX_SIGNATURES];
unsigned char signatures_name[MAX_SIGNATURES][MAX_NAME_LEN]; //HERE BE DRAGONS: add range checks
unsigned char signatures[MAX_SIGNATURES][MAX_SIG_LEN]; 

labcomm_signature_t *get_sig_t(unsigned int uid) 
{
	return &sig_ts[uid-LABCOMM_USER];
}

unsigned int get_signature_len(unsigned int uid){
	//return signatures_length[uid-LABCOMM_USER];
	return sig_ts[uid-LABCOMM_USER].size;
}
unsigned char* get_signature(unsigned int uid){
	//return signatures[uid-LABCOMM_USER];
	return sig_ts[uid-LABCOMM_USER].signature;
}

//is this needed?
//unsigned int get_signature_name_len(unsigned int uid){
//	return signatures_name_length[uid-LABCOMM_USER];
//}

unsigned char* get_signature_name(unsigned int uid){
	//return signatures_name[uid-LABCOMM_USER];
	return sig_ts[uid-LABCOMM_USER].name;
}

void dump_signature(unsigned int uid){
	int i;
	unsigned int len = get_signature_len(uid);
	printf("signature for uid %x : %s (len=%d):\n", uid, get_signature_name(uid), len);
	unsigned char* sig = get_signature(uid);
	for(i=0; i<len; i++) {
		printf("%2.2x ",sig[i]);
		if( i < len-1 && (i+1)%8==0 ) printf("\n");
	}
	printf("\n");
}

static inline void buffer_set_varsize(buffer *b)
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
//static int accept_type_decl(buffer *d);
//static int accept_sample_decl(buffer *d);
static int accept_user_id(buffer *d);
static int accept_string(buffer *d);
static int accept_string_length(buffer *d);
static int accept_char(buffer *d);
static int accept_type(buffer *d);
static int accept_array_decl(buffer *d);
static int accept_number_of_indices(buffer *d);
static int accept_indices(buffer *d);
static int accept_variable_index(buffer *d);
static int accept_fixed_index(buffer *d);
static int accept_struct_decl(buffer *d);
static int accept_number_of_fields(buffer *d);
static int accept_field(buffer *d);
static int accept_sample_data(buffer *d);

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

int encoded_size_static(labcomm_signature_t *sig, void *unused)
{
	if(sig->cached_encoded_size == -1) {
		error("encoded_size_static called for var_size sample or uninitialized signature\n");
	}
	return sig->cached_encoded_size;
}

/* This function probably never will be implemented, as it would be 
   similar to skip_packed_sample_data. And if unregistered variable size 
   samples is to be handled, the proper way is to generate and register
   a handler. Thus, the most probable use of the encoded_size function
   on the receiver side, is to skip unhandled samples.
*/
  
int encoded_size_parse_sig(labcomm_signature_t *sig, void *sample)
{
	printf("Warning: encoded_size_parse_sig not implemented\n");
	return -1;
}

// HERE BE DRAGONS! what does the return value mean?
int accept_packet(buffer *d) {
        unsigned char nbytes;
        unsigned int type = peek_varint(d, &nbytes) ;
	if(type == TYPE_DECL ) {
		advancen(d, nbytes);
		int b = accept_user_id(d);
		if(accept_user_id(d)) {
			unsigned int uid = pop_val(d);
			VERBOSE_PRINTF(", name = ");
			accept_string(d);
			pop_val(d); // ignore length, for now. 
	#ifdef RETURN_STRINGS
			char *str = (char *) pop_ptr(d);
			free(str);
	#endif
			VERBOSE_PRINTF(" : ");
			accept_type(d);
			unsigned int type = pop_val(d);
			pop_val(d); // ignore size
			//push(d, type);
			VERBOSE_PRINTF("\n");
			return TRUE;
		} else {
			error("type_decl with uid < LABCOMM_USER\n");	
		}
	} else if (type == SAMPLE_DECL) {
		d->current_decl_is_varsize = FALSE; // <-- a conveniance flag in buffer
		advancen(d, nbytes);
		VERBOSE_PRINTF("sample_decl ");
		if(accept_user_id(d)) {
			unsigned int uid = pop_val(d);
			unsigned int nstart = d->idx;
			labcomm_signature_t *newsig = get_sig_t(uid);
			newsig->type = type;
			VERBOSE_PRINTF(", name = ");
			accept_string(d);
			unsigned int start = d->idx;
			unsigned int nlen = pop_val(d);
#ifdef RETURN_STRINGS
			char *str = (char *) pop_ptr(d);
			free(str);
#endif
			unsigned char lenlen = labcomm_varint_sizeof(nlen);
			accept_type(d);
			//printf(" : ");
			//unsigned int dt = pop(d);
			pop_val(d); // ignore type, for now
			unsigned int enc_size = pop_val(d);
			unsigned int end = d->idx;
			unsigned int len = end-start;

			if(len <= MAX_SIG_LEN) {
				signatures_length[uid-LABCOMM_USER] = len;
				memcpy(signatures[uid-LABCOMM_USER], &d->c[start], len);
				newsig->size = len;
				newsig->signature = signatures[uid-LABCOMM_USER];
			} else {
				error("sig longer than max length (this ought to be dynamic...)");
			}

			if(nlen < MAX_NAME_LEN) { // leave 1 byte for terminating NULL
				signatures_name_length[uid-LABCOMM_USER] = nlen;
				memcpy(signatures_name[uid-LABCOMM_USER], &d->c[nstart+lenlen], nlen);
				signatures_name[uid-LABCOMM_USER][nlen+1]=0;
				newsig->name = signatures_name[uid-LABCOMM_USER];
			} else {
				error("sig name longer than max length (this ought to be dynamic...");
			}
			VERBOSE_PRINTF("signature for uid %x: %s (start=%x,end=%x, nlen=%d,len=%d)\n", uid, get_signature_name(uid), start,end, nlen, len);
			INFO_PRINTF("SIG: %s\n", newsig->name);	
			if(! d->current_decl_is_varsize) {
				newsig->cached_encoded_size = enc_size;
				newsig->encoded_size = encoded_size_static;
				INFO_PRINTF(".... is static size = %d\n", enc_size);
			} else {
				newsig->cached_encoded_size = -1;
				newsig->encoded_size = encoded_size_parse_sig;
				INFO_PRINTF(".... is variable size\n");
			}
		} else {
			error("sample_decl with uid < LABCOMM_USER\n");	
		}
	} else if(type >= LABCOMM_USER) {
#ifdef EXIT_WHEN_RECEIVING_DATA
		printf("*** got sample data, exiting\n");
		exit(0);
#else
		accept_sample_data(d);
#endif
	} else {
		error("got unknown type (<LABCOMM_USER)\n");
		exit(1);
	}
}

static int accept_user_id(buffer *d){
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

static int accept_string(buffer *d){
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
static int accept_type(buffer *d){
	unsigned char nbytes;
        unsigned int type = peek_varint(d, &nbytes) ;
	switch(type) {
		case TYPE_BOOLEAN :
			VERBOSE_PRINTF("boolean");
			advancen(d, nbytes);
			push_val(d, 1);
			break;
		case TYPE_BYTE :
			VERBOSE_PRINTF("byte");
			advancen(d, nbytes);
			push_val(d,  1);
			break;
		case TYPE_SHORT :
			VERBOSE_PRINTF("short");
			advancen(d, nbytes);
			push_val(d,  2);
			break;
		case TYPE_INTEGER :
			VERBOSE_PRINTF("integer");
			advancen(d, nbytes);
			push_val(d,  4);
			break;
		case TYPE_LONG :
			VERBOSE_PRINTF("long");
			advancen(d, nbytes);
			push_val(d,  8);
			break;
		case TYPE_FLOAT :
			VERBOSE_PRINTF("float");
			advancen(d, nbytes);
			push_val(d,  4);
			break;
		case TYPE_DOUBLE :
			VERBOSE_PRINTF("double");
			advancen(d, nbytes);
			push_val(d,  8);
			break;
		case TYPE_STRING :
			VERBOSE_PRINTF("string");
			advancen(d, nbytes);
			buffer_set_varsize(d);
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
			printf("accept_basic_type default (type==%x) should not happen\n", type);
			push_val(d, 0);
			push_val(d, type);
			return FALSE;
	}
	push_val(d, type);
	return TRUE;
}

/* pushes size and element type */
static int accept_array_decl(buffer *d){
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
				buffer_set_varsize(d);
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
static int accept_struct_decl(buffer *d){
	unsigned char nbytes;
        unsigned int tid = peek_varint(d, &nbytes) ;
	if(tid == STRUCT_DECL) {
		advancen(d, nbytes);
		unsigned int nf = get_varint(d);
		VERBOSE_PRINTF(", %d field struct:\n", nf);
		int i;
		int numVar=0;
		int size=0;
		unsigned int fieldsizes=0;
		for(i=0; i<nf; i++) {
			VERBOSE_PRINTF("\t");
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
static int accept_field(buffer *d){
	VERBOSE_PRINTF("field ");
	accept_string(d);
	pop_val(d); // ignore length, for now
#ifdef RETURN_STRINGS
		char *str = (char *) pop_ptr(d);
		free(str);
#endif
	VERBOSE_PRINTF(" : ");
	accept_type(d);
	pop_val(d); // ignore type, for now
	// push(pop() is really a NOP , leave size on the stack when debugging done
	VERBOSE_PRINTF(" : ");
	VERBOSE_PRINTF("\n");
}
static int accept_sample_data(buffer *d){
	accept_user_id(d);
	unsigned int uid = pop_val(d);	
	printf("sample data... uid=0x%x\n", uid);
#ifdef DEBUG
	dump_signature(uid);
#endif
	labcomm_signature_t *sigt = get_sig_t(uid);
	int encoded_size = sigt->encoded_size(sigt, NULL);
	INFO_PRINTF("encoded_size from sig: %d\n", encoded_size);
	labcomm_signature_t *sig = get_sig_t(uid);
	skip_packed_sample_data(d, sig);
	return TRUE;
}

static int skip_type(unsigned int,buffer*,unsigned char*,unsigned int,unsigned int*) ;

static int skip_array(buffer *d, unsigned char *sig, unsigned int len, unsigned int *pos) {
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

int skip_struct(buffer *d, unsigned char *sig, unsigned int len, unsigned int *pos) {
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
		VERBOSE_PRINTF("namelen==%d",namelen);
		char name[namelen+1]; //HERE BE DRAGONS. alloca?
		strncpy(name, sig+*pos+nbytes, namelen);
		name[namelen]=0;
		VERBOSE_PRINTF(", name = %s",name);
#endif
		*pos += (nbytes+namelen); // 32bit len + actual string

		unsigned int type = unpack_varint(sig, *pos, &nbytes);
		*pos += nbytes;
#ifdef DEBUG
		VERBOSE_PRINTF(": type == %x\n", type);
#endif
		skipped += skip_type(type, d, sig, len, pos);
	}
	return skipped;
}
#ifndef QUIET
/* print and skip */
int skip_type(unsigned int type, buffer *d, 
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
int skip_type(unsigned int type, buffer *d, 
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

/* parse signature and skip the corresponding bytes in the buffer 
 */
int skip_packed_sample_data(buffer *d, labcomm_signature_t *sig) {
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
