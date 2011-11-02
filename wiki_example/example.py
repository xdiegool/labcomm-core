#!/usr/bin/python
# Auto generated example

import labcomm

class log_message(object):
    signature = labcomm.sample('log_message', 
        labcomm.struct([
            ('sequence', labcomm.INTEGER()),
            ('line', labcomm.array([0],
                labcomm.struct([
                    ('last', labcomm.BOOLEAN()),
                    ('data', labcomm.STRING())])))]))

class data(object):
    signature = labcomm.sample('data', 
        labcomm.FLOAT())

