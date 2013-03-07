#include <stdio.h>

typedef unsigned int number;

unsigned char do_pack(unsigned char *buf, number i) 
{
	printf("do_pack %lu == %lx\n", i, i);

	number tmp = i;
	unsigned char res = 0;

	while ( tmp >= 0x80 ) {
		buf[res] = (tmp & 0x7f) | 0x80;
	        tmp >>= 7;
		res++;
	}
	buf[res] = tmp;
	return res+1;
}

number do_unpack(unsigned char *buf) 
{
	number res=0;
	unsigned char i=0;
	unsigned char cont=1;
	do {
		unsigned char c = buf[i];
		res |= (c & 0x7f) << 7*i; 
		cont = c & 0x80;
		i++;
	} while(cont);

	return res;
}

void print_packed(unsigned char *buf, unsigned char len)
{
	int i;
	
	for(i=0; i<len;i++) {
		printf("%2x   ", buf[i]);
	}
	printf("\n");
}

int main()
{
	unsigned char buf[10];
	unsigned char len;

	len = do_pack(buf, 10);
	print_packed(buf, len);
	printf("... unpacks to %u\n\n", do_unpack(buf));

	len = do_pack(buf, 100);
	print_packed(buf, len);
	printf("... unpacks to %u\n\n", do_unpack(buf));

	len = do_pack(buf, 1000);
	print_packed(buf, len);
	printf("... unpacks to %u\n\n", do_unpack(buf));

	len = do_pack(buf, 100000);
	print_packed(buf, len);
	printf("... unpacks to %u\n\n", do_unpack(buf));

	len = do_pack(buf, 2345678901);
	print_packed(buf, len);
	printf("... unpacks to %u\n\n", do_unpack(buf));

	len = do_pack(buf, 0xffffffff);
	print_packed(buf, len);
	printf("... unpacks to %lx\n", do_unpack(buf));

	return 0;
}
