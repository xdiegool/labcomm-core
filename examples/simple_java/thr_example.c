#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <labcomm_thr_reader_writer.h>
#include "gen/simple.h"
#include "ThrottleDrv/throttle_drv.h"


static int encode(int argc, char *argv[]) {
  struct thr_chn_t *p_thr_chn = NULL;
  struct labcomm_encoder *encoder;
  int i, j;
//  unsigned char dest_mac[ETH_ADR_SIZE] = {0x00, 0x09, 0x6b, 0x10, 0xf3, 0x80};	/* other host MAC address, hardcoded...... :-( */
  unsigned char dest_mac[ETH_ADR_SIZE] = {0x00, 0x09, 0x6b, 0xe3, 0x81, 0xbf};	/* other host MAC address, hardcoded...... :-( */
  unsigned char chn_id = 0x01;
  unsigned short frag_size = 60;
  unsigned short freq = 1000;  /* milliseconds */

  char *ifname = argv[1];
  char *dest_mac_str = argv[2];

  if(parse_MAC_address(dest_mac_str, dest_mac)) {
	printf("failed to parse dest MAC address\n");
	return 1;
  }
  
  if (-1 == thr_init(ifname))
  {
     printf("Throttle Init failure.");
  }
  else
  {
    p_thr_chn = thr_open_chn(dest_mac, chn_id, frag_size, freq, NULL);
    encoder = labcomm_encoder_new(labcomm_thr_writer, p_thr_chn);
    labcomm_encoder_register_simple_TwoInts(encoder);
    labcomm_encoder_register_simple_IntString(encoder);
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

    //client_exit(fd);
 }
}

static void handle_simple_TwoInts(simple_TwoInts *v,void *context) {
  printf("Got TwoInts. a=%d, b=%d\n", v->a, v->b);
}

static void handle_simple_IntString(simple_IntString *v,void *context) {
  printf("Got IntString. x=%d, s=%s\n", v->x, v->s);
}

static int decode(int argc, char *argv[]) {
  struct thr_chn_t *p_thr_chn = NULL;
  struct labcomm_decoder *decoder;
  void  *context = NULL;
  unsigned char dest_mac[ETH_ADR_SIZE] = {0x00, 0x09, 0x6b, 0x10, 0xf3, 0x80};	/* other host MAC address, hardcoded...... :-( */
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
    p_thr_chn = thr_open_chn(dest_mac, chn_id, frag_size, freq, labcomm_decoder_decode_one);
    decoder = labcomm_decoder_new(labcomm_thr_reader, p_thr_chn);
    if (!decoder)
    {
      printf("Failed to allocate decoder %s:%d\n", __FUNCTION__, __LINE__);
      return 1;
    }
    labcomm_decoder_register_simple_TwoInts(decoder, handle_simple_TwoInts, context);
    labcomm_decoder_register_simple_IntString(decoder, handle_simple_IntString, context);

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
