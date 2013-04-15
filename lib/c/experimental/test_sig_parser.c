/* labcomm_sig_parser.c:
 * a main program for the example labcomm signatures parser
 */

#include <stdio.h>

#include "labcomm_sig_parser.h"

#undef DEBUG_READ

#define BUF_SIZE 1024
#define STACK_SIZE 16

void test_read(buffer *buf) {
	int r = read_file(stdin, buf);
#ifdef DEBUG_READ
	printf("read %d bytes:\n\n", r);
	int i;
	for(i=0; i<r; i++) {
		printf("%x ", buf->c[i]);
		if(i%8 == 7) printf("\n");
	}
	printf("\n");
#endif
}
int main() {
	buffer buf;

	if( init_buffer(&buf, BUF_SIZE, STACK_SIZE) ) {
		printf("failed to init buffer\n");
		exit(1);
	}
	test_read(&buf);
	do{
		printf("--------------------------------------------- new packet: \n");
	} while(more(&buf) && accept_packet(&buf)); 
	printf("EOF\n");
}

