#!/usr/bin/python
# Auto generated simple

import labcomm

class an_int(object):
    signature = labcomm.sample('an_int', 
        labcomm.INTEGER())

class a_fixed_int_array(object):
    signature = labcomm.sample('a_fixed_int_array', 
        labcomm.array([2],
            labcomm.INTEGER()))

class a_fixed_int_multi_array(object):
    signature = labcomm.sample('a_fixed_int_multi_array', 
        labcomm.array([2, 2, 2],
            labcomm.INTEGER()))

class a_fixed_int_array_array_array(object):
    signature = labcomm.sample('a_fixed_int_array_array_array', 
        labcomm.array([2],
            labcomm.array([2],
                labcomm.array([2],
                    labcomm.INTEGER()))))

class a_variable_int_array(object):
    signature = labcomm.sample('a_variable_int_array', 
        labcomm.array([0],
            labcomm.INTEGER()))

class a_variable_int_multi_array(object):
    signature = labcomm.sample('a_variable_int_multi_array', 
        labcomm.array([0, 0, 0],
            labcomm.INTEGER()))

class a_variable_int_array_array_array(object):
    signature = labcomm.sample('a_variable_int_array_array_array', 
        labcomm.array([0],
            labcomm.array([0],
                labcomm.array([0],
                    labcomm.INTEGER()))))

class an_int_struct(object):
    signature = labcomm.sample('an_int_struct', 
        labcomm.struct([
            ('a', labcomm.INTEGER()),
            ('b', labcomm.INTEGER())]))

class a_void(object):
    signature = labcomm.sample('a_void', 
        labcomm.struct([])
)

