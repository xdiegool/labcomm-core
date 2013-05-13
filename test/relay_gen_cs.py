#!/usr/bin/python

import re
import sys

def split_match(pattern, multiline):
    def match(s):
        m = re.match(pattern, s)
        if m:
            return m.group(1)
        pass
    return filter(lambda s: s != None, map(match, multiline.split('\n')))
   

if __name__ == '__main__':
    f = open(sys.argv[1])
    sample = []
    for l in map(lambda s: s.strip(), f):
        lang,kind,func,arg = l[1:].split(l[0])
        if lang == 'C#' and kind == 'sample':
            sample.append((func, arg))
            pass
        pass
    result = []
    result.extend(split_match('^[^|]*\|(.*)$', """
      |using System;
      |using System.IO;
      |using se.lth.control.labcomm;
      |
      |public class cs_relay :
    """))
    for func,arg in sample[0:-1]:
        result.append('  %s.Handler,' % func)
        pass
    result.append('  %s.Handler' % sample[-1][0])
    result.extend(split_match('^[^|]*\|(.*)$', """
      |{
      |  LabCommEncoderChannel encoder;
    """))
    for func,arg in sample:
        if arg == 'void':
            result.extend(split_match('^[^|]*\|(.*)$', """
              |  void %(func)s.Handler.handle() {
              |    %(func)s.encode(encoder);
              |  }
            """ % { 'func': func}))
            pass
        else:
            result.extend(split_match('^[^|]*\|(.*)$', """
              |  void %(func)s.Handler.handle(%(arg)s data) {
              |    %(func)s.encode(encoder, data);
              |  }
            """ % { 'func': func, 'arg': arg}))
            pass
        pass
    result.extend(split_match('^[^|]*\|(.*)$', """
      |  public cs_relay(String InName, String OutName) {
      |    FileStream InFile = new FileStream(InName,
      |                                       FileMode.Open,
      |                                       FileAccess.Read);
      |    LabCommDecoderChannel d = new LabCommDecoderChannel(InFile);
      |    FileStream OutFile = new FileStream(OutName,
      |                                        FileMode.OpenOrCreate,
      |                                        FileAccess.Write);
      |    encoder = new LabCommEncoderChannel(OutFile);
      |
    """))
    for func,arg in sample:
        result.append('    %s.register(d, this);' % func)
        pass
    for func,arg in sample:
        result.append('    %s.register(encoder);' % func)
        pass
    result.extend(split_match('^[^|]*\|(.*)$', """
      |    try {
      |      d.run();
      |    } catch (EndOfStreamException) {
      |    }
      |  }
      |  static void Main(String[] arg) {
      |    new cs_relay(arg[0], arg[1]);
      |  }
      |}
    """))
    print "\n".join(result)
    exit(0)

    for func,arg in sample:
        result.extend(split_match('^[^|]*\|(.*)$', """
          |void handle_%(func)s(%(arg)s *v, void *context)
          |{
          |  struct labcomm_encoder *e = context;
          |  labcomm_encode_%(func)s(e, v);
          |}""" % { 'func': func, 'arg': arg }))
        pass
    result.extend(split_match('^[^|]*\|(.*)$', """
      |int main(int argc, char *argv[]) {
      |  struct labcomm_encoder *e;
      |  struct labcomm_decoder *d;
      |  int in, out;
      |  
      |  if (argc < 3) { return 1; }
      |  in = open(argv[1], O_RDONLY);
      |  if (in < 0) { return 1; }
      |  out = open(argv[2], O_WRONLY);
      |  if (out < 0) { return 1; }
      |  e = labcomm_encoder_new(labcomm_fd_writer, &out);
      |  d = labcomm_decoder_new(labcomm_fd_reader, &in);
    """))
    for func,arg in sample:
        result.extend(split_match('^[^|]*\|(.*)$', """
          |  labcomm_encoder_register_%(func)s(e);
          |  labcomm_decoder_register_%(func)s(d, handle_%(func)s, e);
        """ % { 'func': func, 'arg': arg }))
    result.extend(split_match('^[^|]*\|(.*)$', """
      |  labcomm_decoder_run(d);
      |  return 0;
      |}
    """))
    print "\n".join(result)
    pass
