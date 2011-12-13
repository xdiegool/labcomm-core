#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <labcomm_thr_reader_writer.h>
#include "gen/simple.h"
#include "ThrottleDrv/throttle_drv.h"

static void handle_simple_TwoInts(simple_TwoInts *v,void *context) {
  printf("Got TwoInts. a=%d, b=%d\n", v->a, v->b);
}

static void handle_simple_IntString(simple_IntString *v,void *context) {
  printf("Got IntString. x=%d, s=%s\n", v->x, v->s);
}

int main(int argc, char *argv[]) {
  struct thr_chn_t *p_thr_chn = NULL;
  struct labcomm_decoder *decoder;
  void  *context = NULL;
  int ret = 0;
  unsigned char dest_mac[ETH_ADR_SIZE] = {0x06, 0x05, 0x04, 0x03, 0x02, 0x01};	/* other host MAC address, hardcoded...... :-( */
  unsigned char chn_id = 0x01;
  unsigned short frag_size = 60;
  unsigned short freq = 1000;  /* milliseconds */
  unsigned char data[200];

  char *filename = argv[1];
  if (-1 == thr_init("eth2"))
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
