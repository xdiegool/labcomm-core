#!/usr/bin/python
# -*- coding: utf-8 -*-

import argparse
import imp
import labcomm2014
import math
import os
import re
import struct
import subprocess
import sys
import threading


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
    return map(lambda s: s.signature, m.sample)

class Test:
    
    def __init__(self, program, signatures):
        self.program = program
        self.signatures = signatures
        pass

    def generate(self, decl):
        if decl.__class__ == labcomm2014.sample:
            result = []
            for values in self.generate(decl.decl):
                result.append((decl, values))
            return result
    
        elif decl.__class__ == labcomm2014.typedef:
            result = []
            for values in self.generate(decl.decl):
                result.append(values)
            return result

        elif decl.__class__ == labcomm2014.struct:
            result = []
            if len(decl.field) == 0:
                result.append({})
            else:
                values1 = self.generate(decl.field[0][1])
                values2 = self.generate(labcomm2014.struct(decl.field[1:]))
                for v1 in values1:
                    for v2 in values2:
                        v = dict(v2)
                        v[decl.field[0][0]] = v1
                        result.append(v)
            return result
        
        elif decl.__class__ == labcomm2014.array:
            if len(decl.indices) == 1:
                values = self.generate(decl.decl)
                if decl.indices[0] == 0:
                    lengths = [0, 1, 2]
                else:
                    lengths = [ decl.indices[0] ]
            else:
                values = self.generate(labcomm2014.array(decl.indices[1:],
                                                     decl.decl))
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
    
        elif decl.__class__ == labcomm2014.BOOLEAN:
            return [False, True]
    
        elif decl.__class__ == labcomm2014.BYTE:
            return [0, 127, 128, 255]
    
        elif decl.__class__ == labcomm2014.SHORT:
            return [-32768, 0, 32767]
    
        elif decl.__class__ == labcomm2014.INTEGER:
            return [-2147483648, 0, 2147483647]
    
        elif decl.__class__ == labcomm2014.LONG:
            return [-9223372036854775808, 0, 9223372036854775807]
    
        elif decl.__class__ == labcomm2014.FLOAT:
            def tofloat(v):
                return struct.unpack('f', struct.pack('f', v))[0]
            return [tofloat(-math.pi), 0.0, tofloat(math.pi)]
    
        elif decl.__class__ == labcomm2014.DOUBLE:
            return [-math.pi, 0.0, math.pi]
    
        elif decl.__class__ == labcomm2014.STRING:
            return ['string', u'sträng' ]
    
        elif decl.__class__ == labcomm2014.SAMPLE:
            return self.signatures
    
        print>>sys.stderr, decl
        raise Exception("unhandled decl %s" % decl.__class__)

    def uses_refs(self, decls):
        for decl in decls:
            if decl.__class__ == labcomm2014.sample:
                if self.uses_refs([ decl.decl ]):
                    return True
    
            elif decl.__class__ == labcomm2014.struct:
                if self.uses_refs([ d for n,d in decl.field ]):
                    return True
        
            elif decl.__class__ == labcomm2014.array:
                if self.uses_refs([ decl.decl ]):
                    return True

            elif decl.__class__ == labcomm2014.SAMPLE:
                return True

        return False
        

    def run(self):
        print>>sys.stderr, 'Testing', self.program
        p = subprocess.Popen(self.program, 
                             stdin=subprocess.PIPE,
                             stdout=subprocess.PIPE,
                             stderr=sys.stderr)
        self.expected = None
        self.failed = False
        self.next = threading.Condition()
        decoder = threading.Thread(target=self.decode, args=(p.stdout,))
        decoder.start()
        encoder = labcomm2014.Encoder(labcomm2014.StreamWriter(p.stdin))
        for signature in self.signatures:
            encoder.add_decl(signature)
            pass
        if self.uses_refs(self.signatures):
            for signature in self.signatures:
                encoder.add_ref(signature)
        for signature in self.signatures:
            print>>sys.stderr, "Checking", signature.name,
            for decl,value in self.generate(signature):
                sys.stderr.write('.')
                self.next.acquire()
                self.received_value = None
                self.received_decl = None
                encoder.encode(value, decl)
                self.next.wait(2)
                self.next.release()
                if p.poll() != None:
                    print>>sys.stderr, "Failed with:", p.poll()
                    self.failed = True
                elif value != self.received_value or decl != self.received_decl:
                    print>>sys.stderr, "Coding error"
                    print>>sys.stderr, value == self.received_value
                    print>>sys.stderr, "Got:     ", self.received_value 
                    print>>sys.stderr, "         ", self.received_decl 
                    print>>sys.stderr, "Expected:", value
                    print>>sys.stderr, "         ", decl
                    self.failed = True
                    
                if self.failed: 
                    if p.poll() == None:
                        p.terminate()
                    exit(1)
                pass
            print>>sys.stderr
            pass
        p.stdin.close()
        if p.wait() != 0:
            exit(1)
            pass
        pass

    def decode(self, f):
        decoder = labcomm2014.Decoder(labcomm2014.StreamReader(f))
        try:
            while True:
                value,decl = decoder.decode()
                if value != None:
                    self.next.acquire()
                    self.received_value = value
                    self.received_decl = decl
                    self.expected = None
                    self.next.notify_all()
                    self.next.release()
                pass
            pass
        except EOFError:
            pass
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
            old = list(getattr(namespace, self.dest))
            def strip_slash(s):
                if s.startswith('\\'):
                    return s[1:]
                return s
            old.append(map(strip_slash, values))
            setattr(namespace, self.dest, old)
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
    pass
