#!/usr/bin/python

import labcomm
import sys

if __name__ == "__main__":
    d = labcomm.Decoder(labcomm.StreamReader(open(sys.argv[1])))

    while True:
        try:
            data,decl = d.decode()
            if data:
                print data
        except:
            break
