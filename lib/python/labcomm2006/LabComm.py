#!/usr/bin/python
#
# LabComm2006 packets has the following layout
#
#   +----+----+----+----+
#   | id                |
#   +----+----+----+----+
#   | data
#   | ...
#   +----+--
#
# LabComm2006 SAMPLE:
#
#   +----+----+----+----+
#   | id = 0x00000002   |
#   +----+----+----+----+
#   | type number       |
#   +----+----+----+----+
#   | type name (UTF8)
#   | ...
#   +----+----+----+----+
#   | type signature
#   | ...
#   +----+--
#
#
# LabComm2006 User data:
#
#   +----+----+----+----+
#   | id >= 0x00000040  |
#   +----+----+----+----+
#   | user data
#   | ...
#   +----+--
#   

#??  | field1 name
#??  | (length + UTF8)...
#??  +----+----+----+----+
#??  | indices           |
#??  +----+----+----+----+
#??  | index1            | (0 -> variable size)
#??  +----+----+----+----+
#??  | ...
#??  +----+----+----+----+
#??  | indexN            |
#??  +----+----+----+----+
#??  | ...
#??  +----+----+----+----+
#??  | fieldN type       |
#??  +----+----+----+----+
#??  | ...
#??  +----+----+----+----+
#??  | END_TYPE          |
#??  +----+----+----+----+
#??
#??type = <user defined type>
#??
#??  +----+----+----+----+
#??  | type number       |
#??  +----+----+----+----+
#??  | field1 varindices |
#??  | ...               |
#??  +----+----+----+----+
#??  | field1 data       |
#??  | ...               |
#??  +----+----+----+----+
#??  | ...
#??  +----+----+----+----+
#??  | fieldN data       |
#??  | ...               |
#??  +----+----+----+----+
#  
#   
# type numbers and lengths do not have a fixed lenght, but are packed into 
# sequences of 7 bit chunks, represented in bytes with the high bit meaning 
# that more data is to come.
 
import types
import struct as packer

# Allowed packet tags
i_SAMPLE_DEF  = 0x02
i_USER        = 0x40 # ..0xffffffff

# Predefined types
i_ARRAY   = 0x10
i_STRUCT  = 0x11

i_BOOLEAN = 0x20 
i_BYTE    = 0x21
i_SHORT   = 0x22
i_INTEGER = 0x23
i_LONG    = 0x24
i_FLOAT   = 0x25
i_DOUBLE  = 0x26
i_STRING  = 0x27


# Version testing
def usePacketLength(version):
    if version != "LabComm2006":
        import sys
        print>>sys.stderr, "No version please %s" % version
    return False

class length_encoder:
    def __init__(self, encoder):
        self.encoder = encoder
        self.version = encoder.version
        self.data = ""

    def write(self, data):
        self.data += data

    def __enter__(self):
        return Encoder(writer=self, version=None, codec=self.encoder)

    def __exit__(self, type, value, traceback):
        if usePacketLength(self.version):
             self.encoder.encode_int(len(self.data))
        self.encoder.pack("%ds" % len(self.data), self.data)

def indent(i, s):
    return ("\n%s" % (" " * i)).join(s.split("\n"))

#
# Base type for all decl's
#
class type_decl(object):
    pass

#
# Primitive types
#
class primitive(type_decl):
    def decode_decl(self, decoder):
        return self

    def __eq__(self, other):
        return self.__class__ == other.__class__

    def __ne__(self, other):
        return not self.__eq__(other)
    
    def __hash__(self):
        return hash(self.__class__)

class BOOLEAN(primitive):
    def encode_decl(self, encoder):
        return encoder.encode_type(i_BOOLEAN)

    def encode(self, encoder, value):
        return encoder.encode_boolean(value)
    
    def decode(self, decoder, obj=None):
        return decoder.decode_boolean()

    def new_instance(self):
        return False

    def __repr__(self):
        return "labcomm.BOOLEAN()"

class BYTE(primitive):
    def encode_decl(self, encoder):
        return encoder.encode_type(i_BYTE)

    def encode(self, encoder, value):
        return encoder.encode_byte(value)
    
    def decode(self, decoder, obj=None):
        return decoder.decode_byte()
    
    def new_instance(self):
        return 0

    def __repr__(self):
        return "labcomm.BYTE()"

class SHORT(primitive):
    def encode_decl(self, encoder):
        return encoder.encode_type(i_SHORT)

    def encode(self, encoder, value):
        return encoder.encode_short(value)
    
    def decode(self, decoder, obj=None):
        return decoder.decode_short()
    
    def new_instance(self):
        return 0

    def __repr__(self):
        return "labcomm.SHORT()"

class INTEGER(primitive):
    def encode_decl(self, encoder):
        return encoder.encode_type(i_INTEGER)

    def encode(self, encoder, value):
        return encoder.encode_int(value)
    
    def decode(self, decoder, obj=None):
        return decoder.decode_int()
    
    def new_instance(self):
        return 0

    def __repr__(self):
        return "labcomm.INTEGER()"

class LONG(primitive):
    def encode_decl(self, encoder):
        return encoder.encode_type(i_LONG)

    def encode(self, encoder, value):
        return encoder.encode_long(value)
    
    def decode(self, decoder, obj=None):
        return decoder.decode_long()
    
    def new_instance(self):
        return long(0)

    def __repr__(self):
        return "labcomm.LONG()"

class FLOAT(primitive):
    def encode_decl(self, encoder):
        return encoder.encode_type(i_FLOAT)

    def encode(self, encoder, value):
        return encoder.encode_float(value)
    
    def decode(self, decoder, obj=None):
        return decoder.decode_float()
    
    def new_instance(self):
        return 0.0

    def __repr__(self):
        return "labcomm.FLOAT()"

class DOUBLE(primitive):
    def encode_decl(self, encoder):
        return encoder.encode_type(i_DOUBLE)

    def encode(self, encoder, value):
        return encoder.encode_double(value)
    
    def decode(self, decoder, obj=None):
        return decoder.decode_double()
    
    def new_instance(self):
        return 0.0

    def __repr__(self):
        return "labcomm.DOUBLE()"

class STRING(primitive):
    def encode_decl(self, encoder):
        return encoder.encode_type(i_STRING)

    def encode(self, encoder, value):
        return encoder.encode_string(value)
    
    def decode(self, decoder, obj=None):
        return decoder.decode_string()
    
    def new_instance(self):
        return ""

    def __repr__(self):
        return "labcomm.STRING()"

#
# Aggregate types
#
class sampledef(type_decl):
    def __init__(self, name=None, decl=None):
        self.name = name
        self.decl = decl

    def encode_decl(self, encoder):
        encoder.encode_type(self.type_index)
        encoder.encode_type(self.get_index(encoder))
        encoder.encode_string(self.name)
        self.decl.encode_decl(encoder)

    def encode(self, encoder, value):
        self.decl.encode(encoder, value)

    def decode_decl(self, decoder):
        index = decoder.decode_type_number()
        name = decoder.decode_string()
        decl = decoder.decode_decl()
        result = self.__class__.__new__(self.__class__)
        result.__init__(name=name, decl=decl)
        self.add_index(decoder, index, result)
        return result

    def decode(self, decoder, obj=None):
        obj = decoder.create_object(self.name)
        obj = self.decl.decode(decoder, obj)
        return obj

    def new_instance(self):
        return self.decl.new_instance()

    def __eq__(self, other):
        return (self.__class__ == other.__class__ and 
                self.name == other.name and
                self.decl == other.decl)
        
    def __ne__(self, other):
        return not self.__eq__(other)

    def __hash__(self):
        return hash(self.__class__) ^ hash(self.name) ^ hash(self.decl)
    
    def __repr__(self):
        return "%s('%s', %s)" % (self.type_name, self.name, self.decl)

class sample_def(sampledef):
    type_index = i_SAMPLE_DEF
    type_name = 'sample'

    def get_index(self, encoder):
        return encoder.decl_to_index[self]

    def add_index(self, decoder, index, decl):
        decoder.add_decl(decl, index)
    
class array(type_decl):
    def __init__(self, indices, decl):
        self.indices = tuple(indices)
        self.decl = decl
        
    def __eq__(self, other):
        return (self.__class__ == other.__class__ and 
                self.indices == other.indices and
                self.decl == other.decl)
        
    def __ne__(self, other):
        return not self.__eq__(other)

    def __hash__(self):
        return hash(self.__class__) ^ hash(self.indices) ^ hash(self.decl)
    
    def encode_decl(self, encoder):
        encoder.encode_type(i_ARRAY)
        encoder.encode_int(len(self.indices))
        for i in self.indices:
            encoder.encode_int(i)
        encoder.encode_type_number(self.decl)

    def min_max_shape(self, l, depth, shape):
        if isinstance(l, types.StringTypes):
            return shape
        try:
            length = len(l)
            if len(shape) <= depth:
                shape.append((length, length))
                pass
            else:
                (low, high) = shape[depth]
                low = min(low, length)
                high = max(high, length)
                shape[depth] = (low, high)
                pass
            for e in l:
                shape = self.min_max_shape(e, depth + 1, shape)
                pass
            pass
        except TypeError:
            pass
        return shape    

    def shape(self, l):
        shape = self.min_max_shape(l, 0, [])
        result = []
        for (low, high) in shape:
            if low != high:
                raise Exception("Non rectangular shape %s" % shape)
            result.append(low)
        return result

    
    def encode_indices(self, encoder, value):
        depth = len(self.indices)
        shape = self.shape(value)
        if len(shape) < len(self.indices):
            raise Exception("Actual dimension %s differs from declared %s" %
                            (shape, self.indices))
        for i in range(len(self.indices)):
            if self.indices[i] == 0:
                encoder.encode_int(shape[i])
            elif self.indices[i] != shape[i]:
                raise Exception("Actual dimension %d in %s differs from %s" %
                                (i, shape, self.indices))
        return depth

    def encode_value(self, encoder, value, depth):
        if depth:
            for e in value:
                self.encode_value(encoder, e, depth - 1)
        else:
            self.decl.encode(encoder, value)
    
    def encode(self, encoder, value):
        depth = self.encode_indices(encoder, value)
        self.encode_value(encoder, value, depth)

    def decode_decl(self, decoder):
        n_indices = decoder.decode_int()
        indices = []
        for i in range(n_indices):
            index = decoder.decode_int()
            indices.append(index)
        elem_decl = decoder.decode_decl()
        return array(indices, elem_decl)

    def decode_value(self, decoder, indices):
        if not len(indices):
            result = self.decl.decode(decoder)
        else:
            result = []
            for i in range(indices[0]):
                result.append(self.decode_value(decoder, indices[1:]))
        return result

    def decode(self, decoder, obj=None):
        indices = []
        for i in self.indices:
            if i == 0:
                i = decoder.decode_int()
            indices.append(i)
        return self.decode_value(decoder, indices)

    def new_instance_value(self, indices):
        if not len(indices):
            result = self.decl.new_instance()
        else:
            result = []
            for i in range(indices[0]):
                result.append(self.new_instance_value(indices[1:]))
        return result
    
    def new_instance(self):
        indices = []
        for i in self.indices:
            if i == 0:
                i = decoder.decode_int()
            indices.append(i)
        return self.new_instance_value(indices)
    
    def __repr__(self):
        return "labcomm.array(%s,\n    %s)" % (
            self.indices, indent(4, self.decl.__repr__()))
    
class struct(type_decl):
    def __init__(self, field):
        self.field = tuple(field)

    def __eq__(self, other):
        return (self.__class__ == other.__class__ and 
                self.field == other.field)
        
    def __ne__(self, other):
        return not self.__eq__(other)

    def __hash__(self):
        return hash(self.__class__) ^ hash(self.field)

    def encode_decl(self, encoder):
        encoder.encode_type(i_STRUCT)
        encoder.encode_int(len(self.field))
        for (name, decl) in self.field:
            encoder.encode_string(name)
            encoder.encode_type_number(decl)

    def encode(self, encoder, obj):
        if isinstance(obj, dict):
            for (name, decl) in self.field:
                decl.encode(encoder, obj[name])
        else:
            for (name, decl) in self.field:
                decl.encode(encoder, getattr(obj, name))

    def decode_decl(self, decoder):
        n_field = decoder.decode_int()
        field = []
        for i in range(n_field):
            name = decoder.decode_string()
            decl = decoder.decode_decl()
            field.append((name, decl))
        return struct(field)

    def decode(self, decoder, obj=None):
        if obj == None:
            obj = decoder.create_object()
        for (name, decl) in self.field:
            obj.__setattr__(name, decl.decode(decoder))
        return obj

    def new_instance(self):
        result = anonymous_object()
        for (name, decl) in self.field:
            result.__setattr__(name, decl.new_instance())
        return result
    
    def __repr__(self):
        delim = ""
        result = "labcomm.struct(["
        for (name, decl) in self.field:
            result += "%s\n  ('%s', %s)" % (delim, name, decl)
            delim = ","
        result += "\n])"
        return result

SAMPLE_DEF = sample_def()

ARRAY = array([], None)
STRUCT = struct([])

class anonymous_object(dict):
    def __setattr__(self, name, value):
        if name.startswith("_"):
            super(anonymous_object, self).__setattr__(name, value)
        else:
            self[name] = value

    def __getattr__(self, name):
        if name.startswith("_"):
            return super(anonymous_object, self).__getattr__(name)
        else:
            return self[name]

class Codec(object):
    def __init__(self, codec=None):
        self.type_to_name = codec and codec.type_to_name or {}
        self.name_to_type = codec and codec.name_to_type or {}
        self.index_to_decl = codec and codec.index_to_decl or {}
        self.decl_to_index = codec and codec.decl_to_index  or {}
        self.name_to_decl = codec and codec.name_to_decl  or {}
        self.index_to_ref = codec and codec.index_to_ref or {}
        self.ref_to_index = codec and codec.ref_to_index or {}
        self.name_to_ref = codec and codec.name_to_ref or {}
        self.decl_index = codec and codec.decl_index or i_USER
        self.ref_index = codec and codec.ref_index or i_USER
        if not codec:
            self.predefined_types()

    def predefined_types(self):
        self.add_decl(SAMPLE_DEF, i_SAMPLE_DEF)

        self.add_decl(ARRAY, i_ARRAY)
        self.add_decl(STRUCT, i_STRUCT)

        self.add_decl(BOOLEAN(), i_BOOLEAN)
        self.add_decl(BYTE(), i_BYTE)
        self.add_decl(SHORT(), i_SHORT)
        self.add_decl(INTEGER(), i_INTEGER)
        self.add_decl(LONG(), i_LONG)
        self.add_decl(FLOAT(), i_FLOAT)
        self.add_decl(DOUBLE(), i_DOUBLE)
        self.add_decl(STRING(), i_STRING)
        
    def add_decl(self, decl, index=0):
        if index == 0:
            if decl in self.decl_to_index:
                return False
            index = self.decl_index
            self.decl_index += 1
        self.index_to_decl[index] = decl
        self.decl_to_index[decl] = index
        try:
            self.name_to_decl[decl.name] = decl
        except:
            pass
        return True
        
    def add_ref(self, ref, index=0):
        if index == 0:
            if ref.sample in self.ref_to_index:
                return False
            index = self.ref_index
            self.ref_index += 1
        self.index_to_ref[index] = ref.sample
        self.ref_to_index[ref.sample] = index
        try:
            self.name_to_ref[ref.sample.name] = ref.sample
        except:
            pass
        return True
    
    def add_binding(self, name, decl):
        self.type_to_name[decl] = name
        self.name_to_type[name] = decl

    def samples(self):
        result = []
        index = self.index_to_decl.keys()
        index.sort()
        for i in index:
            e = self.index_to_decl[i]
            if i >= i_USER and isinstance(e, sample):
                result.append(e)
        return result
        

class Encoder(Codec):
    def __init__(self, writer, codec=None):
        super(Encoder, self).__init__(codec)
        self.writer = writer

    def pack(self, format, *args):
        self.writer.write(packer.pack(format, *args))

    def add_decl(self, decl, index=0):
        if not isinstance(decl, type_decl):
            decl = decl.signature
        if index == 0:
            self.writer.mark_begin(decl, None)
            if super(Encoder, self).add_decl(decl, index):
                decl.encode_decl(self)
            self.writer.mark_end(decl, None)
 
    def encode(self, object, decl=None):
        if decl == None:
            name = self.type_to_name[object.__class__]
            decl = self.name_to_decl[name]
        if not isinstance(decl, type_decl):
            decl = decl.signature
        self.writer.mark_begin(decl, object)
        self.encode_type_number(decl)
        decl.encode(self, object)
        self.writer.mark_end(decl, object)

    def encode_type_number(self, decl):
        try:
            self.encode_type(self.decl_to_index[decl])
        except KeyError:
            decl.encode_decl(self)
            
    def encode_int(self, v):
        v = v & 0xffffffff
        self.encode_int(v)

    def encode_type(self, index):
        self.encode_int(index)
            
    def encode_boolean(self, v):
        if v:
            self.pack("!b", 1)
        else:
            self.pack("!b", 0)

    def encode_byte(self, v):
        self.pack("!B", v)

    def encode_short(self, v):
        self.pack("!h", v)

    def encode_int(self, v):
        self.pack("!i", v)

    def encode_long(self, v):
        self.pack("!q", v)

    def encode_float(self, v):
        self.pack("!f", v)

    def encode_double(self, v):
        self.pack("!d", v)

    def encode_string(self, v):
        s = v.encode("utf8")
	self.encode_int(len(s));
	self.pack("%ds" % len(s),s)

class Decoder(Codec):
    def __init__(self, reader):
        super(Decoder, self).__init__()
        self.reader = reader
        
    def unpack(self, format):
        size = packer.calcsize(format)
        data = ""
        while len(data) < size:
            data += self.reader.read(size - len(data))
        result = packer.unpack(format, data)
        return result[0]

    def decode_decl(self):
        index = self.decode_type_number()
        result = self.index_to_decl[index]
        if index < i_USER:
            result = result.decode_decl(self)
        else:
            raise Exception('Should not be used')
        return result

    def decode(self):
        index = self.decode_type_number()
        if index == i_SAMPLE_DEF:
            decl = self.index_to_decl[index].decode_decl(self)
            value = None
        elif index < i_USER:
            raise Exception("Invalid type index %d" % index)
        else:
            decl = self.index_to_decl[index]
            value = decl.decode(self)
        self.reader.mark(value, decl)
        return (value, decl)

    def create_object(self, name=None):
        if isinstance(self, primitive):
            result = None
        else:
            try:
                decl = self.name_to_type[name]
                result = decl.__new__(decl)
                result.__init__()
            except KeyError:
                result = anonymous_object()

        return result
    
    def decode_int(self):
        return self.decode_int()

    def decode_type_number(self):
        return self.decode_int()
        
    def decode_boolean(self):
        return self.unpack("!b") != 0
    
    def decode_byte(self):
        return self.unpack("!B")
    
    def decode_short(self):
        return self.unpack("!h")
    
    def decode_int(self):
        return self.unpack("!i")
    
    def decode_long(self):
        return self.unpack("!q")
    
    def decode_float(self):
        return self.unpack("!f")
    
    def decode_double(self):
        return self.unpack("!d")
    
    def decode_string(self):
        length = self.decode_int()
        return self.unpack("!%ds" % length).decode("utf8")

    def decode_ref(self):
        index = self.decode_int()
        return self.index_to_ref.get(index, None)

class signature_reader:
    def __init__(self, signature):
        self.signature = packer.pack("!%db" % len(signature), *signature)

    def read(self, count):
        result = self.signature[0:count]
        self.signature = self.signature[count:]
        return result
        
def decl_from_signature(signature):
    decoder = Decoder(signature_reader(signature))
    t = decoder.decode_decl()
    return t

