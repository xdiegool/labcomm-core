import labcomm.LabComm
    
from labcomm.StreamReader import StreamReader
from labcomm.StreamWriter import StreamWriter

Decoder = labcomm.LabComm.Decoder
Encoder = labcomm.LabComm.Encoder

sample = labcomm.LabComm.sample_def

array = labcomm.LabComm.array
struct = labcomm.LabComm.struct
primitive = labcomm.LabComm.primitive

BOOLEAN = labcomm.LabComm.BOOLEAN
BYTE = labcomm.LabComm.BYTE
SHORT = labcomm.LabComm.SHORT
INTEGER = labcomm.LabComm.INTEGER
LONG = labcomm.LabComm.LONG
FLOAT = labcomm.LabComm.FLOAT
DOUBLE = labcomm.LabComm.DOUBLE
STRING = labcomm.LabComm.STRING

decl_from_signature = labcomm.LabComm.decl_from_signature

try:
    import labcomm.TreeModel
    TreeModel = labcomm.TreeModel.TreeModel
except:
    pass

