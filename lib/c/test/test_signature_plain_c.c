#include <stdlib.h>
#include <stdio.h>
#include "test/gen/another_encoding.h"
#include "test/gen/generated_encoding.h"

extern int labcomm_signature_local_index(struct labcomm_signature *s);


static void info(char *name, char *full_name, 
		 struct labcomm_signature *signature) {
  printf("%s %s %p -> %d\n", name,  full_name, signature, 
	 labcomm_signature_local_index(signature));
  if (labcomm_signature_local_index(signature) < 0x40) {
    exit(1);
  }
};

int main(int argc, char *argv[])
{
#define FUNC(name, full_name) \
  extern struct labcomm_signature labcomm_signature_##full_name; \
  info( #name, #full_name, &labcomm_signature_##full_name)
  LABCOMM_FORALL_SAMPLES_generated_encoding(FUNC, ;);
  LABCOMM_FORALL_SAMPLES_another_encoding(FUNC, ;);
  return 0;
}
