#!/usr/bin/env python

import time
import argparse
import labcomm


class Reader(object):
    def __init__(self, file_):
        self._file = file_

    def read(self, count):
        data = self._file.read(count)
        if len(data) == 0:
            raise EOFError()
        return data

    def mark(self, value, decl):
        pass


class FollowingReader(Reader):
    def __init__(self, file_, interval, timeout):
        super(FollowingReader, self).__init__(file_)
        self._interval = interval
        self._timeout = timeout

    def read(self, count):
        data = ''
        t_start = time.time()
        while len(data) < count:
            tmp = self._file.read(count - len(data))
            if tmp:
                data += tmp
            else:
                time.sleep(self._interval)
                if self._timeout and time.time() - t_start > self._timeout:
                    raise EOFError()
        return data


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
    parser = argparse.ArgumentParser()
    parser.add_argument('elc', type=str, help="The log file.")
    parser.add_argument('-f', '--follow', action='store_true',
                        help="find all registrations that already "
                        "exist, then watch the file for changes. All "
                        "future registrations are ignored (because "
                        "the header has already been written).")
    parser.add_argument('-s', '--interval', action="store", type=float,
                        default=0.040,
                        help="time to sleep between failed reads. Requires -f.")
    parser.add_argument('-t', '--timeout', action="store", type=float,
                        help="timeout to terminate when no changes are detected. "
                        "Requires -f.")

    args = parser.parse_args()
    seen = {}
    current = {}
    _type = {}
    file_ = open(args.elc)
    d = labcomm.Decoder(Reader(file_))
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
    file_.seek(0)
    if args.follow:
        reader = FollowingReader(file_, args.interval, args.timeout)
    else:
        reader = Reader(file_)
    d = labcomm.Decoder(reader)
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
