def gen(f, signatures):
    print """
#include <labcomm.h>
#include <labcomm_fd_reader_writer.h>
#include "%(f)s.h"
""" % { 'f': f }
    for s in [s.name for s in signatures]:
        t = "%s_%s" % (f, s)
        print """
void handle_%(t)s(%(t)s *v, void *context)
{
  struct labcomm_encoder *e = context;

  labcomm_encode_%(t)s(e, v);
}
""" % {'t':t}
    print """
int main(int argc, char *argv[]) {
  struct labcomm_decoder *d;
  struct labcomm_encoder *e;
"""
    
