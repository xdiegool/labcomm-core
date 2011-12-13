#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <labcomm_thr_reader_writer.h>
#include "gen/simple.h"
#include "ThrottleDrv/throttle_drv.h"


int main(int argc, char *argv[]) {
  struct thr_chn_t *p_thr_chn = NULL;
  struct labcomm_encoder *encoder;
  int i, j;
  unsigned char dest_mac[ETH_ADR_SIZE] = {0x00, 0x09, 0x6b, 0x10, 0xf3, 0x80};	/* other host MAC address, hardcoded...... :-( */
  unsigned char chn_id = 0x01;
  unsigned short frag_size = 60;
  unsigned short freq = 1000;  /* milliseconds */

  char *filename = argv[1];
  if (-1 == thr_init("eth0"))
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
