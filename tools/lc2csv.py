#!/usr/bin/env python

import argparse
import labcomm2006
import sys
import time


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
        raise Exception("Unhandled type. " + str(type(type_)) + " " + str(type_))


def flatten_labels(_type, prefix=""):
    if isinstance(_type, labcomm.sample):
        flatten_labels(_type.decl, _type.name)
    elif isinstance(_type, labcomm.array):
        if len(_type.indices) != 1:
            raise Exception("Fix multidimensional arrays")
        if len(_type.indices) == 0:
            raise Exception("We dont't handle dynamical sizes yet %s" % _type)
        for i in range(0, _type.indices[0]):
            flatten_labels(_type.decl, prefix + "[%d]" % i)
    elif isinstance(_type, labcomm.struct):
        for name, decl in _type.field:
            flatten_labels(decl,
                           prefix + "." + name)
    elif isinstance(_type, labcomm.primitive):
        print '"%s",' % prefix,
    else:
        raise Exception("Unhandled type. " + str(type(type_)) + " " + str(type_))


def default(type_):
    if isinstance(type_, labcomm.sample):
        return default(type_.decl)
    elif isinstance(type_, labcomm.array):
        if len(type_.indices) != 1:
            raise Exception("Fix multidimensional arrays")
        if len(type_.indices) == 0:
            raise Exception("We dont't handle dynamical sizes yet %s" % type_)
        for i in range(0, type_.indices[0]):
            return [default(type_.decl) for _ in range(type_.indices[0])]
    elif isinstance(type_, labcomm.struct):
        return {name: default(decl) for name, decl in type_.field}
    elif isinstance(type_, labcomm.STRING):
        return ''
    elif isinstance(type_, labcomm.BOOLEAN):
        return False
    elif (isinstance(type_, labcomm.FLOAT) or
          isinstance(type_, labcomm.DOUBLE)):
        return float('NaN')
    elif (isinstance(type_, labcomm.BYTE) or
          isinstance(type_, labcomm.SHORT) or
          isinstance(type_, labcomm.INTEGER) or
          isinstance(type_, labcomm.LONG)):
        return 0
    else:
        raise Exception("Unhandled type. " + str(type(type_)) + " " + str(type_))


def dump(sample, _type):
    for k in sorted(_type.keys()):
        flatten(sample[k], _type[k])
    print


def dump_labels(type_):
    for k in sorted(type_.keys()):
        flatten_labels(type_[k])
    print


def defaults(current, type_):
    for k in sorted(type_.keys()):
        if k not in current:
            current[k] = default(type_[k])


def main(main_args):
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
    parser.add_argument('-w', '--no-default-columns', action="store_true",
                        help="Do not fill columns for which there is no "
                        "data with default values. Wait instead until at least "
                        "one sample has arrived for each registration.")
    parser.add_argument('-a', '--trigger-all', action="store_true",
                        help="Output one line for each sample instead of for "
                        "each sample of the registration that has arrived with "
                        "the highest frequency.")
    args = parser.parse_args(main_args)
    n_samples = {}         # The number of received samples for each sample reg.
    current = {}           # The most recent sample for each sample reg.
    type_ = {}             # The type (declaration) of each sample reg.
    file_ = open(args.elc)
    if args.follow:
        reader = FollowingReader(file_, args.interval, args.timeout)
    else:
        reader = Reader(file_)
    d = labcomm.Decoder(reader)
    while True:
        try:
            o, t = d.decode()
            if o is None:
                n_samples[t.name] = 0
                type_[t.name] = t
            else:
                n_samples[t.name] += 1
                current[t.name] = o
                break
        except EOFError:
            break
    dump_labels(type_)
    if not args.no_default_columns:
        defaults(current, type_)
    n_rows = 0
    while True:
        try:
            o, t = d.decode()
            if o is None:
                continue
            current[t.name] = o
            n_samples[t.name] += 1
            if len(current) < len(type_):
                continue
            if args.trigger_all:
                dump(current, type_)
            else:
                if n_samples[t.name] > n_rows:
                    n_rows = n_samples[t.name]
                    dump(current, type_)
        except EOFError:
            break


if __name__ == "__main__":
    main(sys.argv[1:])
