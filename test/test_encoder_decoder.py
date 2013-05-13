#!/usr/bin/python
# -*- coding: utf-8 -*-

import argparse
import imp
import labcomm
import math
import os
import re
import struct
import subprocess
import sys
import threading

class hexwriter(object):
    def __init__(self, outfile):
        self.pos = 0
        self.ascii = ''
        self.outfile = outfile

    def write(self, data):
        for c in data:
            if ' ' <= c and c <= '}':
                self.ascii += c
            else:
                self.ascii += '.'
            sys.stdout.write("%2.2x " % ord(c))
            self.pos += 1
            if self.pos >= 15:
                sys.stdout.write("%s\n" % self.ascii)
                self.pos = 0
                self.ascii = ""
        #self.outfile.write(data)
        
    def mark(self):
        self.flush()
        pass

    def flush(self):
        for i in range(self.pos, 15):
            sys.stdout.write("   ")
        sys.stdout.write("%s\n" % self.ascii)
        self.pos = 0
        self.ascii = ""
        if self.outfile:
            self.outfile.flush()


def generate(decl):
    if decl.__class__ == labcomm.sample:
        result = []
        for values in generate(decl.decl):
            result.append((decl, values))
        return result

    elif decl.__class__ == labcomm.struct:
        result = []
        if len(decl.field) == 0:
            result.append({})
        else:
            values1 = generate(decl.field[0][1])
            values2 = generate(labcomm.struct(decl.field[1:]))
            for v1 in values1:
                for v2 in values2:
                    v = dict(v2)
                    v[decl.field[0][0]] = v1
                    result.append(v)
        return result
    
    elif decl.__class__ == labcomm.array:
        if len(decl.indices) == 1:
            values = generate(decl.decl)
            if decl.indices[0] == 0:
                lengths = [0, 1, 2]
            else:
                lengths = [ decl.indices[0] ]
        else:
            values = generate(labcomm.array(decl.indices[1:],decl.decl))
            if decl.indices[0] == 0:
                lengths = [1, 2]
            else:
                lengths = [ decl.indices[0] ]
        result = []
        for v in values:
            for i in lengths:
                element = []
                for j in range(i):
                    element.append(v)
                result.append(element)
        return result

    elif decl.__class__ == labcomm.BOOLEAN:
        return [False, True]

    elif decl.__class__ == labcomm.BYTE:
        return [-128, 0, 127]

    elif decl.__class__ == labcomm.SHORT:
        return [-32768, 0, 32767]

    elif decl.__class__ == labcomm.INTEGER:
        return [-2147483648, 0, 2147483647]

    elif decl.__class__ == labcomm.LONG:
        return [-9223372036854775808, 0, 9223372036854775807]

    elif decl.__class__ == labcomm.FLOAT:
        def tofloat(v):
            return struct.unpack('f', struct.pack('f', v))[0]
        return [tofloat(-math.pi), 0.0, tofloat(math.pi)]

    elif decl.__class__ == labcomm.DOUBLE:
        return [-math.pi, 0.0, math.pi]

    elif decl.__class__ == labcomm.STRING:
        return ['string', u'sträng' ]

    print decl
    raise Exception("unhandled decl %s" % decl.__class__)

def labcomm_compile(lc, name, args):
    for lang in [ 'c', 'csharp', 'java', 'python']:
        destdir = 'gen/%s/%s' % (name, lang)
        if not os.path.exists(destdir):
            os.makedirs(destdir)
            pass
        pass
    cmd = args.labcomm.split() + [ 
        "--c=gen/%s/c/%s.c" % (name, name),
        "--h=gen/%s/c/%s.h" % (name, name),
        "--cs=gen/%s/csharp/%s.cs" % (name, name),
        "--python=gen/%s/python/%s.py" % (name, name),
        "--java=gen/%s/java/" % name,
        "--typeinfo=gen/%s/%s.typeinfo" % (name, name),
        lc]
    subprocess.check_call(cmd)
    pass

def get_signatures(path):
    fp, pathname, description = imp.find_module(os.path.basename(path)[0:-3], 
                                                [ os.path.dirname(path) ])
    with fp as fp:
        m = imp.load_module('signatures', fp, pathname, description)
        pass
    return m.signatures

class Test:
    
    def __init__(self, program, signatures):
        self.program = program
        self.signatures = signatures
        pass

    def run(self):
        print 'Testing', self.program
        p = subprocess.Popen(self.program, 
                             stdin=subprocess.PIPE,
                             stdout=subprocess.PIPE)
        self.expected = None
        self.failed = False
        self.next = threading.Semaphore(0)
        decoder = threading.Thread(target=self.decode, args=(p.stdout,))
        decoder.start()
        class Writer:
            def write(self, data):
                p.stdin.write(data)
                pass
            def mark(self):
                p.stdin.flush()
                pass
            pass
        h = hexwriter(p.stdin)
        encoder = labcomm.Encoder(Writer())
        for name,signature in self.signatures:
            encoder.add_decl(signature)
            pass
        print self.signatures
        for name,signature in self.signatures:
            print "Checking", name,
            sys.stdout.flush()
            for decl,value in generate(signature):
                sys.stdout.write('.')
                #print name,decl,value,value.__class__
                self.expected = value
                encoder.encode(value, decl)
                self.next.acquire()
                if self.failed:
                    p.terminate()
                    exit(1)
                pass
            print
            pass
        p.stdin.close()
        print p.wait()
        pass

    def decode(self, f):
        class Reader:
            def read(self, count):
                result = f.read(count)
                if len(result) == 0:
                    raise EOFError()
                return result
            def mark(self, value, decl):
                pass
            pass
        decoder = labcomm.Decoder(Reader())
        try:
            while True:
                value,decl = decoder.decode()
                if value != None:
                    if value != self.expected:
                        print "Coding error", value, self.expected, decl
                        self.failed = True
                    self.next.release()
                pass
            pass
        except EOFError:
            pass
        print 'Done'
        pass

    pass
    
def run(test, signatures):
    t = Test(test, signatures)
    t.run()
    pass

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Run encoding test.')
    class test_action(argparse.Action):
        def __call__(self, parser, namespace, values, option_string=None):
            old = getattr(namespace, self.dest)
            old.append(values)
    parser.add_argument('--signatures')
    parser.add_argument('--test', nargs='*', action=test_action, default=[])

    args = parser.parse_args()
    signatures = get_signatures(args.signatures)
    for test in args.test:
        run(test, signatures)
    exit(0)
    
    for lc in args.lc:
        run(lc, args)
        pass
    exit(0)


    print os.getcwd(), sys.argv
    if not os.path.exists("gen"):
        os.makedirs("gen")
    if len(sys.argv) > 1:
        files = [s[0:-3] for s in sys.argv[1:] if s.endswith('.lc')]
    else:
        files = [s[0:-3] for s in os.listdir(".") if s.endswith('.lc')]

    for f in files:
        cscode.test_program("gen/csharp/test_%s.cs" % f,
                            "gen/%s.typeinfo" % f)


    sys.path.insert(0, "../lib/python")    
    import labcomm;
    sys.path.insert(0, "gen/python")    
    for f in files:
        print f
        h = hexwriter("gen/%s.vec" % f)
        encoder = labcomm.Encoder(h)
        signatures = []
        i = __import__(f)
        for k in dir(i):
            v = getattr(i, k)
            try:
                s = v.signature
            except:
                s = None
            if s:
                signatures.append(s)
                encoder.add_decl(s)
        for s in signatures:
            for e in generate(s):
                encoder.encode(e[1], s)

        h.flush()

        ccode.gen(f, signatures)
        
        
