#!/usr/bin/python

import labcomm
import sys
import test

if __name__ == '__main__':
    version = sys.argv[2] if len(sys.argv) == 3 else "LabComm2013"
    encoder = labcomm.Encoder(labcomm.StreamWriter(open(sys.argv[1], 'w')), version)
    encoder.add_decl(test.twoLines.signature)
    l1 = test.line()
    c1s = test.point()
    c1sx = test.coord()
    c1sx.val = 1
    c1sy = test.coord()
    c1sy.val = 11
    c1s.x = c1sx;
    c1s.y = c1sy;
    c1e = test.point()
    c1ex = test.coord()
    c1ex.val = 2
    c1ey = test.coord()
    c1ey.val = 22
    c1e.x = c1ex;
    c1e.y = c1ey;
    l1.start=c1s
    l1.end = c1e
    l2 = test.line()
    c2s = test.point()
    c2sx = test.coord()
    c2sx.val = 3
    c2sy = test.coord()
    c2sy.val = 33
    c2s.x = c2sx;
    c2s.y = c2sy;
    c2e = test.point()
    c2ex = test.coord()
    c2ex.val = 4
    c2ey = test.coord()
    c2ey.val = 44
    c2e.x = c2ex;
    c2e.y = c2ey;
    l2.start=c2s
    l2.end = c2e
    tl = test.twoLines()
    tl.l1 = l1
    tl.l2 = l2
    tl.f = test.foo()
    tl.f.a = 10;
    tl.f.b = 20;
    tl.f.c = False;
    encoder.encode(tl, test.twoLines.signature)
