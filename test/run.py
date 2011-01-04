#!/usr/bin/python
# -*- coding: iso8859-15 -*-

import labcomm
import math
import os
import sys
import re
import traceback

def run_labcomm(base):
    if not os.path.exists("gen/java/%s" % base):
        os.makedirs("gen/java/%s" % base)
    cmd = " ".join([
        "java -jar ../labComm.jar",
        "--c=gen/%s.c" % base,
        "--h=gen/%s.h" % base,
        "--python=gen/%s.py" % base,
        "--java=gen/java/%s" % base,
        "%s.lc" % base
        ])
    print cmd
    os.system(cmd)
    
class hexwriter(object):
    def __init__(self):
        self.pos = 0
        self.ascii = ''

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

    def mark(self):
        pass

    def flush(self):
        for i in range(self.pos, 15):
            sys.stdout.write("   ")
        sys.stdout.write("%s\n" % self.ascii)
        self.pos = 0
        self.ascii = ""

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
        return [-math.pi, 0.0, math.pi]

    elif decl.__class__ == labcomm.DOUBLE:
        return [-math.pi, 0.0, math.pi]

    elif decl.__class__ == labcomm.STRING:
        return ['string',
                'teckensträng'.decode("iso8859-15")]

    print decl
    raise Exception("unhandled decl %s" % decl.__class__)

if __name__ == "__main__":
    print os.getcwd(), sys.argv
    if not os.path.exists("gen"):
        os.makedirs("gen")
    files = sys.argv[1:]
    print sys.argv, files
    if len(files) == 0:
        files = os.listdir(".")
    for f in files:
        m = re.match("(.*).lc", f)
        if m:
            run_labcomm(m.group(1))

    sys.path.insert(0, "gen")    
    for f in files:
        m = re.match("(.*).lc", f)
        if m:
            print f
            h = hexwriter()
            encoder = labcomm.Encoder(h)
            signatures = []
            i = __import__(m.group(1))
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
