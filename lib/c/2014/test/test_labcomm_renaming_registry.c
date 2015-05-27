#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <err.h>

#include "labcomm2014.h"
#include "labcomm2014_private.h"
#include "labcomm2014_default_error_handler.h"
#include "labcomm2014_default_memory.h"
#include "labcomm2014_default_scheduler.h"
#include "labcomm2014_renaming.h"
#include "labcomm2014_renaming_private.h"
#include "test/gen/generated_encoding.h"

static int do_test(int argc, char **argv)
{
  struct labcomm2014_renaming_registry *registry;
  struct labcomm2014_renaming_rename *r1, *r2, *r3, *r4;

  registry = labcomm2014_renaming_registry_new(
    labcomm2014_default_error_handler,
    labcomm2014_default_memory,
    labcomm2014_default_scheduler);
  r1 = labcomm2014_renaming_rename_new(
    registry,
    labcomm2014_signature_generated_encoding_R,
    labcomm2014_renaming_prefix, "p:");
  r2 = labcomm2014_renaming_rename_new(
    registry,
    labcomm2014_renaming_rename_signature(r1),
    labcomm2014_renaming_suffix, ":s");
  r3 = labcomm2014_renaming_rename_new(
    registry,
    labcomm2014_signature_generated_encoding_R,
    labcomm2014_renaming_suffix, ":s");
  r4 = labcomm2014_renaming_rename_new(
    registry,
    labcomm2014_renaming_rename_signature(r3),
    labcomm2014_renaming_prefix, "p:");
  assert(r2 == r4);
  labcomm2014_renaming_rename_free(registry, r1);
  labcomm2014_renaming_rename_free(registry, r2);
  labcomm2014_renaming_rename_free(registry, r3);
  labcomm2014_renaming_rename_free(registry, r4);
  labcomm2014_renaming_registry_free(registry);
  return 0;
}

int main(int argc, char **argv)
{
  return do_test(argc, argv);
}
