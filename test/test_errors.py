#!/usr/bin/python

import sys
import argparse
import subprocess

# returns true if test fails
def test_labcomm_compile_OK(lc, args):
    cmd = args.labcomm.split() + [lc]
    try:
        res = subprocess.check_call(cmd)
        print "sucess!"
        return False
    except subprocess.CalledProcessError as ex:
        print ex.output
        print ex.returncode
        return True

def test_labcomm_compile_fail(lc, args):
    cmd = args.labcomm.split() + [lc]
    try:
        res = subprocess.check_call(cmd)
        print "failed! (should have produced an error)"
        return True
    except subprocess.CalledProcessError as ex:
        print "sucess!"
        print ex.output
        print ex.returncode
        return False;

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Run test of error messages.')

    parser.add_argument('--labcomm');
    parser.add_argument('--testOK', nargs='*', default=[])
    parser.add_argument('--testNOK', nargs='*', default=[])
    args = parser.parse_args()

    fail = False;
    for test in args.testOK:
        fail = fail or test_labcomm_compile_OK(test, args)
        pass
    for test in args.testNOK:
        fail = fail or test_labcomm_compile_fail(test, args)
        pass
    if fail:
        print "*** fail ***"
    else:
        print "*** success ***"
