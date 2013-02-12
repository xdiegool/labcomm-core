/* labcomm_parser.c:
 * an example parser for labcomm signatures, illustrating how to skip samples
 * based on their signature. Intended as an embryo for introducing this 
 * functionality into the lib to allow a channel to survive types with no
 * registered handler.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DEBUG 

#define FALSE 0
#define TRUE 1

#define	USER_ID_BASE	0x00000080

typedef enum{
	TYPE_DECL = 0x00000001,
	SAMPLE_DECL = 0x00000002,

	ARRAY_DECL = 0x00000010,
	STRUCT_DECL = 0x00000011,

	TYPE_BOOLEAN  = 0x00000020,
	TYPE_BYTE  = 0x00000021,
	TYPE_SHORT  = 0x00000022,
	TYPE_INTEGER  = 0x00000023,
	TYPE_LONG  = 0x00000024,
	TYPE_FLOAT  = 0x00000025,
	TYPE_DOUBLE  = 0x00000026,
	TYPE_STRING  = 0x00000027
} labcomm_type ;

void error(char *s) {
	fprintf(stderr, "%s", s);
	fprintf(stderr, "\nexiting\n");
	exit(1);
}

#define BUF_SIZE 1024
#define STACK_SIZE 16

/* internal type: stack for the parser */
typedef struct {
	unsigned char* c;
	unsigned int size;
	unsigned int capacity;
	unsigned int idx;
	unsigned int* stack;
	unsigned int top;
} buffer;

/* aux method for reading a big endian uint32 from a char* (i.e. ntohl but for explicit char*) */
static int unpack32(unsigned char *c, unsigned int idx) {
	unsigned int b0=(c[idx]) << 3 ; 
	unsigned int b1=(c[idx+1]) << 2 ;
	unsigned int b2=(c[idx+2]) << 1 ;
	unsigned int b3=c[idx+3];

	return  b0 | b1 | b2 | b3;
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

void push(buffer *b, unsigned int e) {
	b->stack[b->top]=e;
	b->top=b->top-1;
	dumpStack(b);
}
unsigned int pop(buffer *b) {
	b->top=b->top+1;
	return b->stack[b->top];
}
	
int init_buffer(buffer *b, size_t size, size_t stacksize) {
	b->c = malloc(size);
	b->capacity = size;
	b->idx = 0;

	b->stack = calloc(stacksize, sizeof(b->stack));
	b->top = stacksize-1;

	return b->c == NULL || b->stack == NULL;
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

unsigned int peek32(buffer *b) {
	return unpack32(b->c, b->idx);
}

void advance32(buffer *b) {
	b->idx+=4;
}

unsigned int get32(buffer *b) {
	unsigned int res = peek32(b);
	advance32(b);
	return res;
}

void getStr(buffer *b, char *dest, size_t size) {
	int rem = b->size - b->idx;
	if( size > rem ) 
		size = rem;
	strncpy(dest, &b->c[b->idx], size);
	b->idx += size;
}

//XXX experimental
#define MAX_SIGNATURES 10
#define MAX_NAME_LEN 32 
#define MAX_SIG_LEN 128
unsigned int signatures_length[MAX_SIGNATURES];
unsigned char signatures_name[MAX_SIGNATURES][MAX_NAME_LEN]; //HERE BE DRAGONS: add range checks
unsigned char signatures[MAX_SIGNATURES][MAX_SIG_LEN]; 

unsigned int get_signature_len(unsigned int uid){
	return signatures_length[uid-USER_ID_BASE];
}
unsigned char* get_signature_name(unsigned int uid){
	return &signatures_name[uid-USER_ID_BASE][1];
}
unsigned char* get_signature(unsigned int uid){
	return signatures[uid-USER_ID_BASE];
}

void dump_signature(unsigned int uid){
	int i;
	unsigned int len = get_signature_len(uid);
	printf("signature for uid %x : %s (len=%d):\n", uid, get_signature_name(uid), len);
	unsigned char* sig = get_signature(uid);
	for(i=0; i<len; i++) {
		printf("%2.2x ",sig[i]);
		if( (i+1)%8==0 ) printf("\n");
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

int accept_packet(buffer *d);
int accept_type_decl(buffer *d);
int accept_sample_decl(buffer *d);
int accept_user_id(buffer *d);
int accept_string(buffer *d);
int accept_string_length(buffer *d);
int accept_char(buffer *d);
int accept_type(buffer *d);
int accept_boolean_type(buffer *d);
int accept_byte_type(buffer *d);
int accept_short_type(buffer *d);
int accept_integer_type(buffer *d);
int accept_long_type(buffer *d);
int accept_float_type(buffer *d);
int accept_long_type(buffer *d);
int accept_string_type(buffer *d);
int accept_array_decl(buffer *d);
int accept_number_of_indices(buffer *d);
int accept_indices(buffer *d);
int accept_variable_index(buffer *d);
int accept_fixed_index(buffer *d);
int accept_struct_decl(buffer *d);
int accept_number_of_fields(buffer *d);
int accept_field(buffer *d);
int accept_sample_data(buffer *d);
int accept_packed_sample_data(buffer *d);

int do_parse(buffer *d) {
	unsigned int type = peek32(d) ;
	if(type == TYPE_DECL ) {
		advance32(d);
		accept_user_id(d);
		unsigned int uid = pop(d);
		printf(", name = ");
		accept_string(d);
		// ignore, for now. This should do something as
		// char *name = (char*) pop(d);
		// store or print name
		// free(name)
		printf(" : ");
		accept_type(d);
		unsigned int type = pop(d);

		//push(d, type);
		printf("\n");
		return TRUE;
	} else if (type == SAMPLE_DECL) {
		advance32(d);
		printf("sample ");
		accept_user_id(d);
		unsigned int nstart = d->idx;
		unsigned int uid = pop(d);
		printf(", name = ");
		accept_string(d);
		unsigned int start = d->idx;
		unsigned int nlen = pop(d);
		accept_type(d);
		printf(" : ");
		//unsigned int dt = pop(d);
#if 0
		unsigned int end = d->idx;
		unsigned int len = end-start;
		if(len <= MAX_SIG_LEN) {
			signatures_length[uid-USER_ID_BASE] = len;
			memcpy(signatures_name[uid-USER_ID_BASE], &d->c[nstart+3], nlen+1);
		} else {
			error("sig longer than max length (this ought to be dynamic...)");
		}
		if(nlen < MAX_NAME_LEN) { // reserve space for terminating NULL
			signatures_name[uid-USER_ID_BASE][nlen+1]=0;
			memcpy(signatures[uid-USER_ID_BASE], &d->c[start], len);
		} else {
			error("sig name longer than max length (this ought to be dynamic...");
		}
		printf("signature for uid %x: %s (start=%x,end=%x, nlen=%d,len=%d)\n", uid, get_signature_name(uid), start,end, nlen, len);
#endif
	} else {
		printf("*** got sample data, exiting\n");
		exit(0);
	}
}

int accept_user_id(buffer *d){
	unsigned int uid = peek32(d);
	if(uid >= USER_ID_BASE) {
		advance32(d);
		printf("uid = %x ", uid);
		return TRUE;
	} else {
		return FALSE;
	}
}

int accept_string(buffer *d){
	unsigned int len = get32(d);
	push(d, len);
	char *str=malloc(len);
	getStr(d, str, len);
	printf("%s", str);
#ifdef RETURN_STRINGS
	push(d, str);
#else
	free(str);
#endif
	return TRUE;
}
// included above
// int accept_string_length(buffer *d){
//	unsigned int uid = get32(d);
//	return TRUE;
//}
//int accept_char(buffer *d){
//}

int accept_type(buffer *d){
	unsigned int type = peek32(d);
	switch(type) {
		case TYPE_BOOLEAN :
			printf("boolean");
			advance32(d);
			break;
		case TYPE_BYTE :
			printf("byte");
			advance32(d);
			break;
		case TYPE_SHORT :
			printf("short");
			advance32(d);
			break;
		case TYPE_INTEGER :
			printf("integer");
			advance32(d);
			break;
		case TYPE_LONG :
			printf("long");
			advance32(d);
			break;
		case TYPE_FLOAT :
			printf("float");
			advance32(d);
			break;
		case TYPE_DOUBLE :
			printf("double");
			advance32(d);
			break;
		case TYPE_STRING :
			printf("string");
			advance32(d);
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

int accept_array_decl(buffer *d){
	unsigned int tid = peek32(d);
	if(tid == ARRAY_DECL) {
		advance32(d);
		unsigned int nidx = get32(d);
		printf("%d dim array: ", nidx);
		int i;
		unsigned int numVar=0;
		unsigned int size=1;
		for(i=0; i<nidx; i++) {
			unsigned int idx = get32(d);
			if(idx == 0) {
				numVar++;
				printf("variable index (numVar=%d), ",numVar);
			} else {
				printf("fixed index: %d, ", idx);
				size*=idx;
			}
			printf("\n");
		}
		unsigned int et=accept_type(d);
		printf("array element type: %x\n", et);
		pop(d);
		//push(d,tid);
		return TRUE;
	} else {
		printf("accept_array_decl: type=%x, should not happen\n",tid);
		return FALSE;
	}
}
int accept_struct_decl(buffer *d){
	unsigned int tid = peek32(d);
	if(tid == STRUCT_DECL) {
		advance32(d);
		unsigned int nf = get32(d);
		printf(", %d field struct:\n", nf);
		int i;
		int numVar=0;
		int size=0;
		for(i=0; i<nf; i++) {
			printf("\t");
			accept_field(d);
		}
//		push(d,tid);
		printf("----\n");
		return TRUE;
	} else {
		printf("accept_struct_decl: type=%x, should not happen\n",tid);
		return FALSE;
	}
}
int accept_field(buffer *d){
	printf("field ");
	accept_string(d);
	// ignore, for now
	printf(" : ");
	accept_type(d);
	printf("\n");
}
int accept_sample_data(buffer *d){
	accept_user_id(d);
	unsigned int uid = pop(d);	
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

int skip_type(unsigned int,buffer*,unsigned char*,unsigned int,unsigned int*) ;

int skip_array(buffer *d, unsigned char *sig, unsigned int len, unsigned int *pos) {
	unsigned int skip = 0;
	unsigned int tot_nbr_elem_tmp = 1;
	unsigned int nIdx = unpack32(sig, *pos);
	printf("skip_array: nIdx = %d\n", nIdx);
	*pos +=4;
	unsigned int idx[nIdx];
	unsigned int nVar=0;

	unsigned int i;

	for(i=0; i<nIdx; i++) {
		idx[i] = unpack32(sig, *pos);
		*pos += 4;
		printf("skip_array: idx[%d]=%d\n", i, idx[i]);
		if(idx[i] == 0) {
			nVar++;
		} else {
			tot_nbr_elem_tmp *= idx[i];
		}
	}
	unsigned int var[nVar];

	for(i=0; i<nVar; i++) {
		var[i] = get32(d);	
		printf("skip_array: var[%d]=%d\n", i, var[i]);
	}

	unsigned int type = unpack32(sig, *pos);
	*pos+=4;

	unsigned int elemSize = labcomm_sizeof(type);

	skip = elemSize * tot_nbr_elem_tmp;

	printf("skip_array: skip: %d * %d = %d\n", elemSize, tot_nbr_elem_tmp, skip);
	
	advancen(d, skip);

	skip += nVar;

	return skip;
}

int skip_struct(buffer *d, unsigned char *sig, unsigned int len, unsigned int *pos) {
	unsigned int nFields = unpack32(sig,*pos);
	*pos += 4;
	unsigned int i;
	unsigned int skipped=0;
	printf("skip_struct (%d fields)\n", nFields);
	for(i=0; i<nFields; i++) {
		//skip name 
		unsigned int namelen = unpack32(sig, *pos);
		*pos += (4+namelen); // 32bit len + actual string
#ifdef DEBUG
		printf("namelen==%d \n",namelen);
#endif
		unsigned int type = unpack32(sig, *pos);
		*pos += 4;
#ifdef DEBUG
		printf("type == %x\n", type);
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
			{unsigned int len = get32(d);
			//advancen(d,len);
			int i;
			printf("string [");
			for(i=0; i<len; i++)
				printf("%c", get(d));
			printf("]\n");
			skipped+=len+4;
			break;}
		case ARRAY_DECL :
			printf("array\n");
			skipped += skip_array(d, sig, len, pos);
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
	printf("skip_type %x\n", type);
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
			{unsigned int len = get32(d);
			advancen(d,len);
			skipped+=len+4;
			break;}
		case ARRAY_DECL :
			printf("array\n");
			skipped += skip_array(d, sig, len, pos);
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
		unsigned int type = unpack32(sig,pos);
		pos+=4;
		skipped += skip_type(type, d, sig, siglen, &pos);
	}	
	printf("skipped %d bytes\n", skipped);
	return TRUE;
}

int read_file(FILE *f, buffer *b) {
	int s = fread(b->c, sizeof(char), b->capacity, f);
	b->size = s;
	b->idx=0;
	return s;
}

void test_read(buffer *buf) {
	int r = read_file(stdin, buf);
	printf("read %d bytes:\n\n", r);
	int i;
	for(i=0; i<r; i++) {
		printf("%x ", buf->c[i]);
		if(i%8 == 7) printf("\n");
	}
	printf("\n");
}
int main() {
	buffer buf;

	if( init_buffer(&buf, BUF_SIZE, STACK_SIZE) ) {
		printf("failed to init buffer\n");
		exit(1);
	}
	test_read(&buf);
	do{
		printf("trying to read another packet\n");
	} while(more(&buf) && do_parse(&buf)); 
	printf("done\n");
}

	
