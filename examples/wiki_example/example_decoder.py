#!/usr/bin/python

import labcomm2014
import sys

if __name__ == "__main__":
    version = sys.argv[2] if len(sys.argv) == 3 else "LabComm2014"
    d = labcomm2014.Decoder(labcomm2014.StreamReader(open(sys.argv[1])), version)

    while True:
        try:
            data,decl = d.decode()
            if data:
                print data
        except Exception, e:
            print e
            break
