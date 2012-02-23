#ifndef _ETHERNET_DRV_H_
#define _ETHERNET_DRV_H_

#include <net/ethernet.h>

struct eth_int_t;

struct eth_int_t* eth_open(const char* eth_int_name);
int eth_close(struct eth_int_t* eth_int);
int eth_getMACadr(const struct eth_int_t* eth_int, struct ether_addr *mac_adr);
int eth_send(const struct eth_int_t* eth_int, const unsigned char* eth_frame, unsigned short length);
int eth_receive (const struct eth_int_t* eth_int, unsigned char* eth_frame, unsigned short length);

#endif
