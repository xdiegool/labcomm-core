#include <stdlib.h>
#include <stdio.h>
#include "labcomm2014_private.h"
#include "test/gen/another_encoding.h"
#include "test/gen/generated_encoding.h"

static void info(char *name, char *full_name, 
		 const struct labcomm2014_signature *signature) {
  printf("%s %s %p -> %d\n", name,  full_name, signature, 
	 labcomm2014_get_local_index(signature));
  if (labcomm2014_get_local_index(signature) < 0x40) {
    exit(1);
  }
};

int main(int argc, char *argv[])
{
#define FUNC(name, full_name) \
  info( #name, #full_name, labcomm2014_signature_##full_name)

  LABCOMM_FORALL_SAMPLES_generated_encoding(FUNC, ;);
  LABCOMM_FORALL_SAMPLES_another_encoding(FUNC, ;);
  return 0;
}
