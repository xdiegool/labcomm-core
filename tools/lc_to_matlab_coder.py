#!/usr/bin/python

import sys
import imp
import subprocess
import os
import labcomm

TRANSLATE={
    labcomm.SHORT() : 'short',
    labcomm.DOUBLE() : 'double'
}

def compile_lc(lc):
    p = subprocess.Popen([ 'labcomm2014', '--python=/dev/stdout', lc],
                         stdout=subprocess.PIPE)
    src = p.stdout.read()
    code = compile(src, 'lc_import', 'exec')
    mod = sys.modules.setdefault('lc_import', imp.new_module('lc_import'))
    exec code in mod.__dict__
    import lc_import
    return (lc_import.typedef, lc_import.sample)

def gen_binding(decl, lc_prefix, prefix, suffix):
    if isinstance(decl, labcomm.sample):
        if isinstance(decl.decl, labcomm.typedef):
            print "%(n1)s = coder.cstructname(%(n1)s, '%(lc)s_%(n2)s')" % dict(
                n1=decl.name, n2=decl.decl.name, lc=lc_prefix)
        else:
            print "%(n1)s = coder.cstructname(%(n1)s, '%(lc)s_%(n2)s')" % dict(
                n1=decl.name, n2=decl.name, lc=lc_prefix)
        gen_binding(decl.decl, lc_prefix, '%s.' % decl.name, suffix)
    elif isinstance(decl, labcomm.typedef):
        print "%(p)s%(s)s = coder.cstructname(%(p)s%(s)s, '%(lc)s_%(n)s')" % dict(
            n=decl.name, lc=lc_prefix, p=prefix, s=suffix)
        gen_binding(decl.decl, lc_prefix, prefix, suffix)
    elif isinstance(decl, labcomm.array):
         raise Exception("Array unhandled")
    elif isinstance(decl, labcomm.struct):
        for n, d in decl.field:
            gen_binding(d, lc_prefix, '%sFields.%s' % (prefix, n), suffix)
    elif isinstance(decl, labcomm.primitive):
        pass
    else:
        raise Exception("Unhandled type. %s", decl)
    
def gen_sample(lc_prefix, decl):
    if isinstance(decl, labcomm.sample):
        print "%s = " % decl.name,
        gen_sample(lc_prefix, decl.decl)
        print
        gen_binding(decl, lc_prefix, '', '')
    elif isinstance(decl, labcomm.typedef):
        # Expand in place
        gen_sample(lc_prefix, decl.decl)
    elif isinstance(decl, labcomm.array):
         raise Exception("Array unhandled")
    elif isinstance(decl, labcomm.struct):
        print "struct(..."
        for n, d in decl.field:
            print "'%s, " % n,
            gen_sample(lc_prefix, d)
        print ")..."
    elif isinstance(decl, labcomm.primitive):
        print "%s(0), ..." % TRANSLATE[decl]
    else:
        raise Exception("Unhandled type. %s", decl)  

"""
robtarget = struct(...
	'orientation', ...
	struct(...
	    'q1', double(0), ...
	    'q2', double(0), ...
	    'q3', double(0), ...
	    'q4', double(0) ...
	), ...
	'translation', ...
	struct(...
	    'x', double(0), ...
	    'y', double(0), ...
	    'z', double(0) ...
	), ...
	'configuration', ...
	struct(...
	    'cf1', int16(0), ...
	    'cf4', int16(0), ...
	    'cf6', int16(0), ...
	    'cfx', int16(0) ...
	) ...
);
robtarget_types = coder.typeof(robtarget);

act = coder.cstructname(robtarget_types, 'egm_pb2lc_robtarget');
act.Fields.translation = coder.cstructname(act.Fields.translation, 'egm_pb2lc_cartesian');
act.Fields.orientation = coder.cstructname(act.Fields.orientation, 'egm_pb2lc_quaternion');
act.Fields.configuration = coder.cstructname(act.Fields.configuration, 'egm_pb2lc_confdata');
"""

def gen_matlab(lc):
    lc_prefix = os.path.basename(lc).split('.')[0]
    typedef, sample = compile_lc(lc)
    for s in sample:
        gen_sample(lc_prefix, s.signature)

if __name__ == '__main__':
    for lc in sys.argv[1:]:
        gen_matlab(lc)
