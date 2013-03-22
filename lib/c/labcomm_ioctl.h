#include "labcomm.h"

struct labcomm_ioctl_register_signature {
  int index;
  labcomm_signature_t *signature;
};

#define LABCOMM_IOCTL_REGISTER_SIGNATURE         0x0001
#define LABCOMM_IOCTL_WRITER_GET_BYTES_WRITTEN   0x0002
#define LABCOMM_IOCTL_WRITER_GET_BYTE_POINTER    0x0003
#define LABCOMM_IOCTL_WRITER_COPY_BYTES          0x0004
