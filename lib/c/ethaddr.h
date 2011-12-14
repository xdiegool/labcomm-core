#ifndef ETHADDR_H
#define ETHADDR_H
/* parse a string on the format 00:01:02:0d:0e:0f into a char array
 * the mac_addr argument must be at least six bytes.
 * returns 0 on success, or -1 on error
 */
int parse_MAC_address(const char *str, unsigned char mac_addr[]);
#endif
