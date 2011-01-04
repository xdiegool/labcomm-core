#!/usr/bin/python
# Auto generated nested

import labcomm

class int_array_ss(object):
    no_signature = labcomm.typedef('int_array_ss',
        labcomm.array([1],
            labcomm.array([0, 3, 0],
                labcomm.array([5],
                    labcomm.INTEGER()))))

class struct_array_ss(object):
    signature = labcomm.sample('struct_array_ss', 
        labcomm.array([1],
            labcomm.array([0],
                labcomm.array([0],
                    labcomm.struct([
                        ('aa', labcomm.INTEGER()),
                        ('bb', labcomm.BOOLEAN()),
                        ('ias', labcomm.array([1],
                            labcomm.array([0, 3, 0],
                                labcomm.array([5],
                                    labcomm.INTEGER()))))])))))

class int_t(object):
    no_signature = labcomm.typedef('int_t',
        labcomm.INTEGER())

class int_array_t(object):
    no_signature = labcomm.typedef('int_array_t',
        labcomm.array([1],
            labcomm.array([2],
                labcomm.array([3, 4],
                    labcomm.INTEGER()))))

class struct_t(object):
    no_signature = labcomm.typedef('struct_t',
        labcomm.struct([
            ('a', labcomm.INTEGER())]))

class struct_array_t(object):
    no_signature = labcomm.typedef('struct_array_t',
        labcomm.array([2],
            labcomm.struct([
                ('a', labcomm.INTEGER())])))

class struct_struct_t(object):
    no_signature = labcomm.typedef('struct_struct_t',
        labcomm.struct([
            ('a', labcomm.struct([
                ('a', labcomm.INTEGER())]))]))

class int_s(object):
    signature = labcomm.sample('int_s', 
        labcomm.INTEGER())

class int_array_s(object):
    signature = labcomm.sample('int_array_s', 
        labcomm.array([1],
            labcomm.array([0, 3, 0],
                labcomm.array([5],
                    labcomm.INTEGER()))))

class struct_s(object):
    signature = labcomm.sample('struct_s', 
        labcomm.struct([
            ('a', labcomm.INTEGER()),
            ('bcd', labcomm.DOUBLE())]))

class struct_array_s(object):
    signature = labcomm.sample('struct_array_s', 
        labcomm.array([2],
            labcomm.struct([
                ('a', labcomm.INTEGER())])))

class struct_struct_s(object):
    signature = labcomm.sample('struct_struct_s', 
        labcomm.struct([
            ('a', labcomm.struct([
                ('a', labcomm.INTEGER())]))]))

class int_t_s(object):
    signature = labcomm.sample('int_t_s', 
        labcomm.INTEGER())

class int_array_t_s(object):
    signature = labcomm.sample('int_array_t_s', 
        labcomm.array([1],
            labcomm.array([2],
                labcomm.array([3, 4],
                    labcomm.INTEGER()))))

class struct_t_s(object):
    signature = labcomm.sample('struct_t_s', 
        labcomm.struct([
            ('a', labcomm.INTEGER())]))

class struct_array_t_s(object):
    signature = labcomm.sample('struct_array_t_s', 
        labcomm.array([2],
            labcomm.struct([
                ('a', labcomm.INTEGER())])))

class struct_struct_t_s(object):
    signature = labcomm.sample('struct_struct_t_s', 
        labcomm.struct([
            ('a', labcomm.struct([
                ('a', labcomm.INTEGER())]))]))

