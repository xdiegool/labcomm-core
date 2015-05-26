#!/usr/bin/python

import labcomm2014
import sys
import test

if __name__ == '__main__':
    version = sys.argv[2] if len(sys.argv) == 3 else "LabComm2014"
    encoder = labcomm2014.Encoder(labcomm2014.StreamWriter(open(sys.argv[1], 'w')), version)
    encoder.add_decl(test.twoLines.signature)
    tl = {
        (('','l1'),):{
            (('','start'),):{
                (('','x'),):{(('','val'),):1},
                (('','y'),):{(('','val'),):11}
            },
            (('','end'),):{
                (('','x'),):{(('','val'),):2},
                (('','y'),):{(('','val'),):22}
            }
        },
        (('','l2'),):{
            (('','start'),):{
                (('','x'),):{(('','val'),):3},
                (('','y'),):{(('','val'),):33}
            },
            (('','end'),):{
                (('','x'),):{(('','val'),):4},
                (('','y'),):{(('','val'),):44}
            }
        },
        (('','f'),):{
            (('','a'),):10,
            (('','b'),):20,
            (('','c'),):False
        }
    }
    encoder.encode(tl, test.twoLines.signature)
