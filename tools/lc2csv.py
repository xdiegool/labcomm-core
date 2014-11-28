#!/usr/bin/env python

import sys
import labcomm


class Reader(object):
    def __init__(self, file):
        self.file = open(file)

    def read(self, count):
        data = self.file.read(count)
        if len(data) == 0:
            raise EOFError()
        return data

    def mark(self, value, decl):
        pass


def flatten(sample, _type):
    if isinstance(_type, labcomm.sample):
        flatten(sample, _type.decl)
    elif isinstance(_type, labcomm.array):
        for e in sample:
            flatten(e, _type.decl)
    elif isinstance(_type, labcomm.struct):
        for name, decl in _type.field:
            flatten(sample[name], decl)
    elif isinstance(_type, labcomm.BOOLEAN):
        print "%d," % sample,
    elif isinstance(_type, labcomm.STRING):
        print "\"%s\"," % sample,
    elif isinstance(_type, labcomm.primitive):
        print "%s," % sample,
    else:
        print sample, _type


def flatten_labels(sample, _type, prefix=""):
    if isinstance(_type, labcomm.sample):
        flatten_labels(sample, _type.decl, _type.name)
    elif isinstance(_type, labcomm.array):
        if len(_type.indices) != 1:
            raise Exception("Fix multidimensional arrays")
        if len(sample) == 0:
            raise Exception("We dont't handle dynamical sizes yet %s" % _type)
        for i in range(0, len(sample)):
            flatten_labels(sample[i], _type.decl, prefix + "[%d]" % i)
    elif isinstance(_type, labcomm.struct):
        for name, decl in _type.field:
            flatten_labels(sample[name], decl,
                           prefix + "." + name)
    elif isinstance(_type, labcomm.primitive):
        print '"%s",' % prefix,
    else:
        print sample, _type


def dump(sample, _type):
    for k in sorted(_type.keys()):
        flatten(sample[k], _type[k])
    print


def dump_labels(current, _type):
    for k in sorted(_type.keys()):
        flatten_labels(current[k], _type[k])
    print


def main():
    d = labcomm.Decoder(Reader(sys.argv[1]))
    seen = {}
    current = {}
    _type = {}
    while True:
        try:
            (o, t) = d.decode()
        except EOFError:
            break
        if o == None:
            # Type declaration
	    seen[t.name] = 0
	    _type[t.name] = t
        else:
            current[t.name] = o
            if len(current) == len(_type):
                # We have got one sample of each
                break
    dump_labels(current, _type)
    n = 0
    while o != None:
        if seen[t.name] > n:
            n = seen[t.name]
            dump(current, _type)
        current[t.name] = o
        seen[t.name] += 1
        try:
            (o, t) = d.decode()
        except EOFError:
            break
    dump(current, _type)


if __name__ == "__main__":
    main()
