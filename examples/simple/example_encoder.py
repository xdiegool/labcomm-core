#!/usr/bin/python

import labcomm
import sys
import simple

if __name__ == '__main__':
    encoder = labcomm.Encoder(labcomm.StreamWriter(open(sys.argv[1], 'w')))
    encoder.add_decl(simple.theTwoInts.signature)
    encoder.add_decl(simple.IntString.signature)
    foo = simple.theTwoInts()
    foo.a = 13
    foo.b = 37
    encoder.encode(foo, simple.theTwoInts.signature)

    bar = simple.IntString()
    bar.x = 1742
    bar.s = "A string from Python"
    encoder.encode(bar, simple.IntString.signature)
