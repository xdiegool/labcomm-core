#!/usr/bin/python

import re
import sys
import random

def split_match(pattern, multiline):
    def match(s):
        m = re.match(pattern, s)
        if m:
            return m.group(1)
        pass
    return filter(lambda s: s != None, map(match, multiline.split('\n')))
   
def shuffle(l):
    result = list(l)
    random.shuffle(result)
    return result

if __name__ == '__main__':
    f = open(sys.argv[1])
    sample = []
    for l in map(lambda s: s.strip(), f):
        lang,kind,func,arg,dummy = l[1:].split(l[0])
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
      |  EncoderChannel encoder;
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
      |    DecoderChannel decoder = new DecoderChannel(InFile);
      |    FileStream OutFile = new FileStream(OutName,
      |                                        FileMode.OpenOrCreate,
      |                                        FileAccess.Write);
      |    encoder = new EncoderChannel(OutFile);
      |
    """))
    for func,arg in shuffle(sample):
        result.append('    %s.register(decoder, this);' % func)
        pass
    for func,arg in shuffle(sample):
        result.append('    %s.registerSampleRef(decoder);' % func)
        pass
    for func,arg in shuffle(sample):
        result.append('    %s.register(encoder);' % func)
        pass
    for func,arg in shuffle(sample):
        result.append('    %s.registerSampleRef(encoder);' % func)
        pass
    
    result.extend(split_match('^[^|]*\|(.*)$', """
      |    try {
      |      decoder.run();
      |    } catch (EndOfStreamException) {
      |    }
      |  }
      |  static void Main(String[] arg) {
      |    new cs_relay(arg[0], arg[1]);
      |  }
      |}
    """))
    print "\n".join(result)
    pass
