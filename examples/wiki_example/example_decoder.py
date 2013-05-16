#!/usr/bin/python

import labcomm
import sys

class FileReader:
    def __init__(self, name):
        self.f = open(name)
        pass

    def start(self, decoder, version):
        other_version = decoder.decode_string()
        if version != other_version:
            raise Exception("LabComm version mismatch %s != %s" %
                            (version, other_version))
        pass

    def read(self, count):
        s = self.f.read(count)
        if len(s) == 0:
            raise Exception("EOF")
        return s

    def mark(self, value, decl):
        pass
        
if __name__ == "__main__":
    d = labcomm.Decoder(FileReader(sys.argv[1]))

    while True:
        try:
            data,decl = d.decode()
            if data:
                print data
        except:
            break
