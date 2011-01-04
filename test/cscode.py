#!/usr/bin/python

import os
import re
import subprocess
import sys


def gen(classname, namespace, signatures):
    yield 'using System;'
    yield 'using System.IO;'
    yield 'using se.lth.control.labcomm;'
    yield ''
    yield 'class %s :' % classname
    for s in signatures[0:-1]:
        yield '%s%s.Handler,' % (namespace, s.name)
    yield '%s%s.Handler' % (namespace, signatures[-1].name)
    yield '{'
    yield ''
    yield '  LabCommEncoderChannel encoder;'
    for s in signatures:
        yield ''
        if s.type == 'void':
            yield '  void %s%s.Handler.handle() {' % ( namespace, s.name)
        else:
            yield '  void %s%s.Handler.handle(%s data) {' % (
                namespace, s.name, s.type)
        yield '    Console.Error.WriteLine("%s%s");' % (namespace, s.name)
        if s.type == 'void':
            yield '    %s%s.encode(encoder);' % (namespace, s.name)
        else:
            yield '    %s%s.encode(encoder, data);' % (namespace, s.name)
        yield '  }'    
    yield ''
    yield '  public %s(String InName, String OutName) {' % (classname)
    yield '    FileStream InFile = new FileStream(InName,' 
    yield '                                       FileMode.Open, '
    yield '                                       FileAccess.Read);'
    yield '    LabCommDecoderChannel d = new LabCommDecoderChannel(InFile);'
    yield '    FileStream OutFile = new FileStream(OutName, '
    yield '                                        FileMode.OpenOrCreate, '
    yield '                                        FileAccess.Write);'
    yield '    encoder = new LabCommEncoderChannel(OutFile);'
    yield ''
    for s in signatures:
        yield '    %s%s.register(d, this);' % (namespace, s.name)
    yield ''
    for s in signatures:
        yield '    %s%s.register(encoder);' % (namespace, s.name)
    yield ''
    yield '    try {'
    yield '      d.run();'
    yield '    } catch (EndOfStreamException) {'
    yield '    }'
    yield ''
    yield '  }'
    yield ''
    yield '  static void Main(String[] arg) {'
    yield '    new %s(arg[0], arg[1]);' % (classname)
    yield '  }'
    yield ''
    yield '}'

class Signature:
    def __init__(self, name, type):
        self.name = name
        self.type = type

def test_program(target, typeinfo):
    f = open(typeinfo)
    signature = []
    for l in f:
        m = re.match('C#,sample,([^,]+),(.*)', l)
        if m:
            signature.append(Signature(m.group(1), m.group(2)))
    f.close()
    f = open(target, "w")
    for l in gen("x", "", signature):
        f.write(l + "\n")
    f.close()
   
if __name__ == "__main__":
    #os.system("labcomm --typeinfo=/dev/stdout %s" % sys.argv[1])
    typeinfo = subprocess.Popen(["labcomm",
                            "--typeinfo=/dev/stdout %s" % sys.argv[1]],
                           stdout=subprocess.PIPE)
    signature = []
    for l in typeinfo.stdout:
        m = re.match('C#,sample,([^,]+),(.*)', l)
        if m:
            signature.append(Signature(m.group(1), m.group(2)))
    for l in gen("x", "", signature):
        print l
        
