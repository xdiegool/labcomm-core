/* labcomm_sig_parser.c:
 * a main program for the example labcomm signatures parser
 */

#include <stdio.h>
#include <stdlib.h>

#include "labcomm_sig_parser.h"

#define DEBUG_READ

#define BUF_SIZE 1024
#define STACK_SIZE 16
#define MAX_NUM_SIGNATURES 10
#define MAX_SIGNATURES 16
#define MAX_NAME_LEN 32
#define MAX_SIG_LEN 128

void test_read(labcomm_sig_parser_t *p) {
	int r = labcomm_sig_parser_read_file(p, stdin);
#ifdef DEBUG_READ
	printf("read %d bytes:\n\n", r);
	int i;
	for(i=0; i<r; i++) {
		printf("%x ", p->c[i]);
		if(i%8 == 7) printf("\n");
	}
	printf("\n");
#endif
}
int main() {
	labcomm_sig_parser_t p;

	if(labcomm_sig_parser_init(&p, BUF_SIZE, STACK_SIZE, 
                                   MAX_NUM_SIGNATURES, MAX_NAME_LEN, MAX_SIG_LEN) ) {
		printf("failed to init buffer\n");
		exit(1);
	}
	test_read(&p);
	do{
		printf("--------------------------------------------- new packet: \n");
	} while(more(&p) && accept_packet(&p)); 
	printf("EOF\n");
}

