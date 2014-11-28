#!/usr/bin/python

import os
import sys
import difflib
import re

class File:

    def __init__(self, path, match, replacement):
        def replace(s):
            return re.sub('[ \t]+', ' ', s).replace(match, replacement)
        self.name = path.replace(match, replacement)
        self.path = path
        with open(path) as f:
            self.lines = map(replace, f.readlines())

    def __cmp__(self, other):
        if other == None:
            return cmp(self.name, other)
        return cmp(self.name, other.name)

def readfiles(root, match, replacement):
    result = []
    for n in os.listdir(root):
        path = os.path.join(root, n)
        if os.path.islink(path):
            pass
        elif os.path.isdir(path):
            for f in filter(None, readfiles(path, match, replacement)):
                result.append(f)
        else:
            result.append(File(path, match, replacement))
    for f in sorted(result):
        yield f
    yield None

if __name__ == '__main__':
    A = readfiles(*sys.argv[1:4])
    B = readfiles(*sys.argv[4:7])
    a = A.next()
    b = B.next()
    while a != None or b != None:
        if b == None or a.name < b.name:
            print "Only in %s:" %sys.argv[1], a.path
            a = A.next()
        elif a == None or a.name > b.name: 
            print "Only in %s:" %sys.argv[4], b.path
            b = B.next()
        else:
            print a.path, b.path
            print ''.join(difflib.unified_diff(a.lines, b.lines, 
                                                 a.path, b.path))
            a = A.next() 
            b = B.next() 
