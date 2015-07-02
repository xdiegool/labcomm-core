#!/usr/bin/python

import argparse
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
    parser = argparse.ArgumentParser(description='Generate Java test relay.')
    parser.add_argument('--renaming', action='store_true')
    parser.add_argument('typeinfo', help='typeinfo file')

    options = parser.parse_args(sys.argv[1:])
    if options.renaming:
        relay_name = 'java_renaming_relay'
    else:
        relay_name = 'java_relay'

    f = open(options.typeinfo)
    sample = []
    for l in map(lambda s: s.strip(), f):
        lang,kind,func,arg,dummy = l[1:].split(l[0])
        if lang == 'Java' and kind == 'sample':
            sample.append((func, arg))
            pass
        pass
    result = []
    result.extend(split_match('^[^|]*\|(.*)$', """
      |import java.io.FileInputStream;
      |import java.io.FileOutputStream;
      |import java.io.IOException;
      |import se.lth.control.labcomm2014.*;
      |
    """))
    result.extend(split_match('^[^|]*\|(.*)$', """
    |public class %(relay_name)s implements
    """ % dict(relay_name = relay_name)))
    for func,arg in sample[0:-1]:
        result.append('  %s.Handler,' % func)
        pass
    result.append('  %s.Handler' % sample[-1][0])
    result.extend(split_match('^[^|]*\|(.*)$', """
      |{
      |  Encoder encoder;
      |  Decoder decoder;
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
      |  public %(relay_name)s(String InName, String OutName) throws Exception {
      |    FileInputStream InFile = new FileInputStream(InName);
      |    decoder = new DecoderChannel(InFile);
      |    FileOutputStream OutFile = new FileOutputStream(OutName);
      |    encoder = new EncoderChannel(OutFile);
      |
    """ % dict(relay_name=relay_name)))
    if options.renaming:
        result.extend(split_match('^[^|]*\|(.*)$', """
        |    RenamingRegistry registry = new RenamingRegistry();
        |    decoder = new RenamingDecoder(
        |        decoder, registry, new RenamingDecoder.Rename() {public String rename(String s){return "prefix:" + s + ":suffix";}});
        |    encoder = new RenamingEncoder(
        |        encoder, registry, new RenamingEncoder.Rename() {public String rename(String s){return "prefix:" + s + ":suffix";}});
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
      |    } catch (java.io.EOFException e) {
      |    }
      |  }
      |  public static void main(String[] arg) throws Exception {
      |    new %(relay_name)s(arg[0], arg[1]);
      |  }
      |}
    """ % dict(relay_name=relay_name)))
    print "\n".join(result)
    pass
