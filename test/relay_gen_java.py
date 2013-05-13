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
        if lang == 'Java' and kind == 'sample':
            sample.append((func, arg))
            pass
        pass
    result = []
    result.extend(split_match('^[^|]*\|(.*)$', """
      |import java.io.FileInputStream;
      |import java.io.FileOutputStream;
      |import java.io.IOException;
      |import se.lth.control.labcomm.LabCommDecoderChannel;
      |import se.lth.control.labcomm.LabCommEncoderChannel;
      |
      |public class java_relay implements
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
              |  public void handle_%(func)s() throws IOException {
              |    %(func)s.encode(encoder);
              |  }
            """ % { 'func': func}))
            pass
        else:
            result.extend(split_match('^[^|]*\|(.*)$', """
              |  public void handle_%(func)s(%(arg)s data) throws IOException {
              |    %(func)s.encode(encoder, data);
              |  }
            """ % { 'func': func, 'arg': arg}))
            pass
        pass
    result.extend(split_match('^[^|]*\|(.*)$', """
      |  public java_relay(String InName, String OutName) throws Exception {
      |    FileInputStream InFile = new FileInputStream(InName);
      |    LabCommDecoderChannel d = new LabCommDecoderChannel(InFile);
      |    FileOutputStream OutFile = new FileOutputStream(OutName);
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
      |    } catch (java.io.EOFException e) {
      |    }
      |  }
      |  public static void main(String[] arg) throws Exception {
      |    new java_relay(arg[0], arg[1]);
      |  }
      |}
    """))
    print "\n".join(result)
    pass
