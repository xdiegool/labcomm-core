#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <labcomm_thr_reader_writer.h>
#include "gen/simple.h"
#include "ThrottleDrv/throttle_drv.h"



static void handle_simple_TwoInts_foo(simple_TwoInts *v,void *context) {
  struct ether_addr *src = get_sender_addr((struct thr_chn_t *)context); 
  printf("Got TwoInts. a=%d, b=%d\n", v->a, v->b);
  printf("... src addr: %x:%x:%x:%x:%x:%x\n", src->ether_addr_octet[0],  src->ether_addr_octet[1], src->ether_addr_octet[2], src->ether_addr_octet[3], src->ether_addr_octet[4], src->ether_addr_octet[5]);
}

static int encode(int argc, char *argv[]) {
  struct thr_chn_t *p_thr_chn = NULL;
  struct labcomm_encoder *encoder;
  struct labcomm_decoder *decoder;
  int i, j;
  struct ether_addr dest_mac;
  unsigned char chn_id = 0x01;
  unsigned short frag_size = 60;
  unsigned short freq = 1000;  /* milliseconds */

  char *ifname = argv[1];
  char *dest_mac_str = argv[2];

  if(parse_MAC_address(dest_mac_str, &dest_mac)) {
	printf("failed to parse dest MAC address\n");
	return 1;
  }
  
  if (-1 == thr_init(ifname))
  {
     printf("Throttle Init failure.");
  }
  else
  {
    p_thr_chn = thr_open_chn(&dest_mac, chn_id, frag_size, freq,(thr_msg_handler_t)labcomm_decoder_decode_one);
    encoder = labcomm_encoder_new(labcomm_thr_writer, p_thr_chn);
    labcomm_encoder_register_simple_TwoInts(encoder);
    labcomm_encoder_register_simple_IntString(encoder);
    decoder = labcomm_decoder_new(labcomm_thr_reader, p_thr_chn);
    if (!decoder)
    {
      printf("Failed to allocate decoder %s:%d\n", __FUNCTION__, __LINE__);
      return 1;
    }
    labcomm_decoder_register_simple_TwoInts(decoder, handle_simple_TwoInts_foo, p_thr_chn);

    int ret;
    unsigned char data[200];
    simple_IntString is;
    is.x = 24;
    is.s = "Hello, LabComm!";
    printf("Encoding IntString, x=%d, s=%s\n", is.x, is.s);
    labcomm_encode_simple_IntString(encoder, &is);

    simple_TwoInts ti;
    ti.a = 13;
    ti.b = 37;
    printf("Encoding TwoInts, a=%d, b=%d\n", ti.a, ti.b);
    labcomm_encode_simple_TwoInts(encoder, &ti);
    printf("Decoding:\n");
    thr_receive(p_thr_chn, data, decoder);
    thr_receive(p_thr_chn, data, decoder);

    labcomm_encoder_free(encoder);
    labcomm_decoder_free(decoder);
    thr_close_chn(p_thr_chn);
    //client_exit(fd);
 }
}

static void handle_simple_TwoInts(simple_TwoInts *v,void *context) {
  struct ether_addr *src = get_sender_addr((struct thr_chn_t *)context); 
  unsigned char ch_id = get_channel((struct thr_chn_t *)context); 
  printf("Got TwoInts. a=%d, b=%d\n", v->a, v->b);
  printf("... src addr: %x:%x:%x:%x:%x:%x\n", src->ether_addr_octet[0],  src->ether_addr_octet[1], src->ether_addr_octet[2], src->ether_addr_octet[3], src->ether_addr_octet[4], src->ether_addr_octet[5]);
  struct thr_chn_t *ch = thr_open_chn(src, ch_id, (unsigned short)50, (unsigned short)100, NULL);
  struct labcomm_encoder *enc = labcomm_encoder_new(labcomm_thr_writer, ch);
  labcomm_encoder_register_simple_TwoInts(enc);
  v->a *= 2;
  v->b *= 3;
  labcomm_encode_simple_TwoInts(enc, v);
  labcomm_encoder_free(enc);
  thr_close_chn(ch);
}

static void handle_simple_IntString(simple_IntString *v,void *context) {
  printf("Got IntString. x=%d, s=%s\n", v->x, v->s);
  struct ether_addr *src = get_sender_addr((struct thr_chn_t *)context); 
  printf("... src addr: %x:%x:%x:%x:%x:%x\n", src->ether_addr_octet[0],  src->ether_addr_octet[1], src->ether_addr_octet[2], src->ether_addr_octet[3], src->ether_addr_octet[4], src->ether_addr_octet[5]);
}

static int decode(int argc, char *argv[]) {
  struct thr_chn_t *p_thr_chn = NULL;
  struct labcomm_decoder *decoder;
  struct ether_addr dest_mac; 	// Not used in this simple decoder example.
				// The return channel is opened in the handler
  int ret = 0;
  unsigned char chn_id = 0x01;
  unsigned short frag_size = 60;
  unsigned short freq = 1000;  /* milliseconds */
  unsigned char data[200];

  char *ifname = argv[1];
  if (-1 == thr_init(ifname))
  {
     printf("Throttle Init failure.");
  }
  else
  {
    p_thr_chn = thr_open_chn(&dest_mac, chn_id, frag_size, freq, (thr_msg_handler_t)labcomm_decoder_decode_one);
    decoder = labcomm_decoder_new(labcomm_thr_reader, p_thr_chn);
    if (!decoder)
    {
      printf("Failed to allocate decoder %s:%d\n", __FUNCTION__, __LINE__);
      return 1;
    }
    labcomm_decoder_register_simple_TwoInts(decoder, handle_simple_TwoInts, p_thr_chn);
    labcomm_decoder_register_simple_IntString(decoder, handle_simple_IntString, p_thr_chn);

    printf("Decoding:\n");
    do {
    ret = thr_receive(p_thr_chn, data, decoder);
    } while(-1 != ret);
    {
      printf("Throttle Send receive failure.");
    }
    printf("--- End Of File ---:\n");
    labcomm_decoder_free(decoder);

    //server_exit(fd);
  }
}

int main(int argc, char *argv[]) {
	if(argc == 2) {
		return decode(argc, argv);
	} else if(argc == 3) {
		return encode(argc, argv);
	} else {
		printf("Usage: For encoding ethN xx:xx:xx:xx:xx:xx\n       For decoding ethN\n       where ethN is the ethernet interface to use\n       and xx:xx:...:xx is the destination MAC address"); 
		return 1;
	}
}
