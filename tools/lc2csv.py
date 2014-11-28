#!/usr/bin/env python

import sys
import labcomm


class Reader(object):
    def __init__(self, _file):
        self._file = open(_file)

    def read(self, count):
        data = self._file.read(count)
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
    if len(sys.argv) != 2:
        sys.exit("Give input file as argument\n")
    d = labcomm.Decoder(Reader(sys.argv[1]))
    seen = {}
    current = {}
    _type = {}

    # Do one pass through the file to find all registrations.
    while True:
        try:
            o, t = d.decode()
            if o is None:
                seen[t.name] = 0
                _type[t.name] = t
            else:
                current[t.name] = o
        except EOFError:
            break
    dump_labels(current, _type)

    # Do another pass to extract the data.
    current = {}
    d = labcomm.Decoder(Reader(sys.argv[1]))
    while True:
        try:
            o, t = d.decode()
            if o is not None:
                current[t.name] = o
                if len(current) == len(_type):
                    # Assume that samples arrive at different rates.
                    # Trigger on everything once we have a value for
                    # each column.
                    dump(current, _type)
        except EOFError:
            break


if __name__ == "__main__":
    main()
