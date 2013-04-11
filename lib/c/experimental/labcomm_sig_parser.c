/* labcomm_sig_parser.c:
 * an example parser for labcomm signatures, illustrating how to skip samples
 * based on their signature. Intended as an embryo for introducing this 
 * functionality into the lib to allow a channel to survive types with no
 * registered handler.
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

void dumpStack(buffer *b) {
#ifdef DEBUG_STACK
	int i;
	printf("=== stack: ");
	for(i=0; i<STACK_SIZE; i++) { //HERE BE DRAGONS
		printf("%2.2x ", b->stack[i]);
	}
	printf("    top==%d\n", b->top);
#endif
}

void push(buffer *b, void* e) {
	b->stack[b->top]=e;
	b->top=b->top-1;
	dumpStack(b);
}
void* pop(buffer *b) {
	b->top=b->top+1;
	return b->stack[b->top];
}
	
int init_buffer(buffer *b, size_t size, size_t stacksize) {
	b->c = malloc(size);
	b->capacity = size;
	b->size = 0;
	b->idx = 0;

	b->stack = calloc(stacksize, sizeof(b->stack));
	b->stacksize = stacksize;
	b->top = stacksize-1;

	return b->c == NULL || b->stack == NULL;
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

unsigned int signatures_length[MAX_SIGNATURES];
unsigned char signatures_name[MAX_SIGNATURES][MAX_NAME_LEN]; //HERE BE DRAGONS: add range checks
unsigned char signatures[MAX_SIGNATURES][MAX_SIG_LEN]; 

unsigned int get_signature_len(unsigned int uid){
	return signatures_length[uid-LABCOMM_USER];
}
unsigned char* get_signature_name(unsigned int uid){
	return &signatures_name[uid-LABCOMM_USER][1];
}
unsigned char* get_signature(unsigned int uid){
	return signatures[uid-LABCOMM_USER];
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

int labcomm_sizeof(unsigned int type)
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
			printf("labcomm_sizeof(%x)\n", type);
		 	error("labcomm_sizeof should only be called for primitive types");
	}
}

static int accept_packet(buffer *d);
static int accept_type_decl(buffer *d);
static int accept_sample_decl(buffer *d);
static int accept_user_id(buffer *d);
static int accept_string(buffer *d);
static int accept_string_length(buffer *d);
static int accept_char(buffer *d);
static int accept_type(buffer *d);
static int accept_boolean_type(buffer *d);
static int accept_byte_type(buffer *d);
static int accept_short_type(buffer *d);
static int accept_integer_type(buffer *d);
static int accept_long_type(buffer *d);
static int accept_float_type(buffer *d);
static int accept_long_type(buffer *d);
static int accept_string_type(buffer *d);
static int accept_array_decl(buffer *d);
static int accept_number_of_indices(buffer *d);
static int accept_indices(buffer *d);
static int accept_variable_index(buffer *d);
static int accept_fixed_index(buffer *d);
static int accept_struct_decl(buffer *d);
static int accept_number_of_fields(buffer *d);
static int accept_field(buffer *d);
static int accept_sample_data(buffer *d);
static int accept_packed_sample_data(buffer *d);

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

int do_parse(buffer *d) {
        unsigned char nbytes;
        unsigned int type = peek_varint(d, &nbytes) ;
	if(type == TYPE_DECL ) {
		advancen(d, nbytes);
		accept_user_id(d);
		unsigned int uid = (unsigned int) (unsigned long) pop(d);
		VERBOSE_PRINTF(", name = ");
		accept_string(d);
		pop(d); // ignore, for now. 
#ifdef RETURN_STRINGS
		char *str = (char *) pop(d);
		free(str);
#endif
		VERBOSE_PRINTF(" : ");
		accept_type(d);
		unsigned int type = (unsigned int) (unsigned long) pop(d);

		//push(d, type);
		VERBOSE_PRINTF("\n");
		return TRUE;
	} else if (type == SAMPLE_DECL) {
		advancen(d, nbytes);
		VERBOSE_PRINTF("sample_decl ");
		accept_user_id(d);
		unsigned int nstart = d->idx;
		unsigned int uid = (unsigned int) (unsigned long) pop(d);
		VERBOSE_PRINTF(", name = ");
		accept_string(d);
		unsigned int start = d->idx;
		unsigned int nlen = (unsigned int) (unsigned long) pop(d);
#ifdef RETURN_STRINGS
		char *str = (char *) pop(d);
		free(str);
#endif
		unsigned char lenlen = labcomm_varint_sizeof(nlen);
		accept_type(d);
		//printf(" : ");
		//unsigned int dt = pop(d);
		unsigned int end = d->idx;
		unsigned int len = end-start;

		if(len <= MAX_SIG_LEN) {
			signatures_length[uid-LABCOMM_USER] = len;
			memcpy(signatures[uid-LABCOMM_USER], &d->c[start], len);
		} else {
			error("sig longer than max length (this ought to be dynamic...)");
		}

		if(nlen < MAX_NAME_LEN) { // leave 1 byte for terminating NULL
			signatures_name[uid-LABCOMM_USER][0] = nlen;
			memcpy(signatures_name[uid-LABCOMM_USER], &d->c[nstart+lenlen-1], nlen+1);
			signatures_name[uid-LABCOMM_USER][nlen+1]=0;
		} else {
			error("sig name longer than max length (this ought to be dynamic...");
		}
		VERBOSE_PRINTF("signature for uid %x: %s (start=%x,end=%x, nlen=%d,len=%d)\n", uid, get_signature_name(uid), start,end, nlen, len);
	} else {
#ifdef EXIT_WHEN_RECEIVING_DATA
		printf("*** got sample data, exiting\n");
		exit(0);
#else
		accept_sample_data(d);
#endif
	}
}

static int accept_user_id(buffer *d){
        unsigned char nbytes;
	unsigned int uid = peek_varint(d, &nbytes);
	if(uid >= LABCOMM_USER) {
		advancen(d, nbytes);
		VERBOSE_PRINTF("uid = %x ", uid);
		push(d, (void *) (unsigned long) uid);
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
	push(d, str);
#else
	free(str);
#endif
	push(d, (void *) (unsigned long) len);
	return TRUE;
}
static int accept_type(buffer *d){
	unsigned char nbytes;
        unsigned int type = peek_varint(d, &nbytes) ;
	switch(type) {
		case TYPE_BOOLEAN :
			VERBOSE_PRINTF("boolean");
			advancen(d, nbytes);
			break;
		case TYPE_BYTE :
			VERBOSE_PRINTF("byte");
			advancen(d, nbytes);
			break;
		case TYPE_SHORT :
			VERBOSE_PRINTF("short");
			advancen(d, nbytes);
			break;
		case TYPE_INTEGER :
			VERBOSE_PRINTF("integer");
			advancen(d, nbytes);
			break;
		case TYPE_LONG :
			VERBOSE_PRINTF("long");
			advancen(d, nbytes);
			break;
		case TYPE_FLOAT :
			VERBOSE_PRINTF("float");
			advancen(d, nbytes);
			break;
		case TYPE_DOUBLE :
			VERBOSE_PRINTF("double");
			advancen(d, nbytes);
			break;
		case TYPE_STRING :
			VERBOSE_PRINTF("string");
			advancen(d, nbytes);
			break;
		case ARRAY_DECL :
			accept_array_decl(d);
			break;
		case STRUCT_DECL :
			accept_struct_decl(d);
			break;
		default :
			printf("accept_basic_type default (type==%x) should not happen\n", type);
			return FALSE;
	}
	//push(d,type);
	return TRUE;
}

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
			} else {
				VERBOSE_PRINTF("[%d] ", idx);
				size*=idx;
			}
		}
		VERBOSE_PRINTF(" of ");
		unsigned int et=accept_type(d);
		//pop(d);
		//push(d,tid);
		return TRUE;
	} else {
		printf("accept_array_decl: type=%x, should not happen\n",tid);
		return FALSE;
	}
}
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
		for(i=0; i<nf; i++) {
			VERBOSE_PRINTF("\t");
			accept_field(d);
		}
//		push(d,tid);
		return TRUE;
	} else {
		printf("accept_struct_decl: type=%x, should not happen\n",tid);
		return FALSE;
	}
}
static int accept_field(buffer *d){
	VERBOSE_PRINTF("field ");
	accept_string(d);
	pop(d); // ignore, for now
#ifdef RETURN_STRINGS
		char *str = (char *) pop(d);
		free(str);
#endif
	VERBOSE_PRINTF(" : ");
	accept_type(d);
	VERBOSE_PRINTF("\n");
}
static int accept_sample_data(buffer *d){
	accept_user_id(d);
	unsigned int uid = (unsigned int) (unsigned long) pop(d);	
	printf("sample data... %x\n", uid);
#ifdef DEBUG
	dump_signature(uid);
#endif
	unsigned int siglen = get_signature_len(uid);
	unsigned char *sig = get_signature(uid);
	skip_packed_sample_data(d, sig, siglen);
	return TRUE;
}
//int accept_packed_sample_data(buffer *d){

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

	unsigned int elemSize = labcomm_sizeof(type);

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
int skip_packed_sample_data(buffer *d, unsigned char *sig, unsigned int siglen) {
	unsigned int pos = 0; 		//current position in signature
	unsigned int skipped = 0;	//skipped byte counter
	while(pos < siglen) {
		unsigned char nbytes;
		unsigned int type = unpack_varint(sig,pos, &nbytes);
		pos+=nbytes;
		skipped += skip_type(type, d, sig, siglen, &pos);
	}	
	printf("skipped %d bytes\n", skipped);
	return TRUE;
}
