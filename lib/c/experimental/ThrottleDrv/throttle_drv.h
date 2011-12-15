#ifndef _THROTTLE_DRV_H_
#define _THROTTLE_DRV_H_

#define ETH_ADR_SIZE 6

/* typedef of the callback used to manage the received Ethernet Frame (the User Data)*/
typedef int (*thr_msg_handler_t)(void* data);
struct thr_chn_t;


int thr_init(const char* eth_int);
struct thr_chn_t* thr_open_chn(const unsigned char* dst_adr, unsigned char chn_id, unsigned char frag_size, unsigned short freq, thr_msg_handler_t funct);
void thr_close_chn(struct thr_chn_t* c);
int thr_receive (struct thr_chn_t* thr_chn, unsigned char* data, void* param);
int thr_send(const struct thr_chn_t* thr_chn, const char* data, unsigned int length);
int thr_read(struct thr_chn_t* thr_chn, unsigned char* data, int length);

unsigned char* get_sender_addr(struct thr_chn_t* ch);
#endif
