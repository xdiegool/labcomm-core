#include <stdio.h>
#include "ethaddr.h"


/* parse a string on the format 00:01:02:0d:0e:0f into a char array
 * the mac_addr argument must be at least six bytes.
 * returns 0 on success, or -1 on error
 */
int parse_MAC_address(const char *str, unsigned char mac_addr[])
{
	int res = sscanf(str, "%x:%x:%x:%x:%x:%x", &mac_addr[0], &mac_addr[1], 
		&mac_addr[2], &mac_addr[3], &mac_addr[4], &mac_addr[5]);	
	return (res == 6? 0 : -1);

} 

#if 0
/* test program */
int main(int argc, char *argv[])
{
	int i;
	unsigned char a[6];
	int res = parse_MAC_address(argv[1], a);
	if(res) {
		perror("Failed to parse MAC address");
	}
	printf("%x.%x.%x.%x.%x.%x\n",  a[0], a[1], a[2], a[3], a[4], a[5]);
	return 0;
}
#endif
