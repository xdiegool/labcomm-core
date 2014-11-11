#!/usr/bin/python

import labcomm
import sys
import test

if __name__ == '__main__':
    version = sys.argv[2] if len(sys.argv) == 3 else "LabComm2014"
    encoder = labcomm.Encoder(labcomm.StreamWriter(open(sys.argv[1], 'w')), version)
    encoder.add_decl(test.twoLines.signature)
    tl = dict(
        l1=dict(
            start=dict(
                x=dict(val=1), 
                y=dict(val=11)
            ),
            end=dict(
                x=dict(val=2), 
                y=dict(val=22)
            )
        ),
        l2=dict(
            start=dict(
                x=dict(val=3), 
                y=dict(val=33)
            ),
            end=dict(
                x=dict(val=4), 
                y=dict(val=44)
            )
        ),
        f=dict(
            a=10, 
            b=20, 
            c=False
        )
    )
    encoder.encode(tl, test.twoLines.signature)
