__all__ = [ 'LabComm' ]

import LabComm

from StreamReader import StreamReader
from StreamWriter import StreamWriter

Decoder = LabComm.Decoder
Encoder = LabComm.Encoder

sample = LabComm.sample_def
sample_def = LabComm.sample_def
sample_ref = LabComm.sample_ref
typedef = LabComm.typedef

array = LabComm.array
struct = LabComm.struct
primitive = LabComm.primitive

BOOLEAN = LabComm.BOOLEAN
BYTE = LabComm.BYTE
SHORT = LabComm.SHORT
INTEGER = LabComm.INTEGER
LONG = LabComm.LONG
FLOAT = LabComm.FLOAT
DOUBLE = LabComm.DOUBLE
STRING = LabComm.STRING
SAMPLE = LabComm.SAMPLE

decl_from_signature = LabComm.decl_from_signature

