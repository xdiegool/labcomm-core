#!/usr/bin/python

import labcomm2014
import sys,traceback
import test

def handle_twoInts(val):
    print "got twoInts: %s" % val

def handle_twoLines(val):
    print "got twoLines: %s" % val


if __name__ == "__main__":
    version = sys.argv[2] if len(sys.argv) == 3 else "LabComm2014"
    d = labcomm2014.Decoder(labcomm2014.StreamReader(open(sys.argv[1])), version)

    d.register_handler(test.twoInts.signature, handle_twoInts)
    d.register_handler(test.twoLines.signature, handle_twoLines)

    while True:
        try:
            d.runOne()
        except EOFError:
            print "got EOF"
            break
        except Exception, e:
            print "got Exception"
            print e
            traceback.print_exc()
            break
