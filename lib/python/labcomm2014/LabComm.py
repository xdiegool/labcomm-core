#!/usr/bin/python
#
# LabComm2014 packet has the following layout
#
#   +----+----+----+----+
#   | id                    (packed32)
#   +----+----+----+----+
#   | length                (packed32)
#   +----+----+----+----+
#   | data
#   | ...
#   +----+--
#
# LabComm2014 SAMPLE_DEF:
#
#   +----+----+----+----+
#   | id = 0x02             (packed32)
#   +----+----+----+----+
#   | length                (packed32)
#   +----+----+----+----+
#   | type number           (packed32)
#   +----+----+----+----+
#   | type name (UTF8)
#   | ...
#   +----+----+----+----+
#   | signature length      (packed32)
#   +----+----+----+----+
#   | type signature
#   | ...
#   +----+--
#
# LabComm2014 SAMPLE_REF:
#
#   +----+----+----+----+
#   | id = 0x03             (packed32)
#   +----+----+----+----+
#   | length                (packed32)
#   +----+----+----+----+
#   | type number           (packed32)
#   +----+----+----+----+
#   | type name (UTF8)
#   | ...
#   +----+----+----+----+
#   | signature length      (packed32)
#   +----+----+----+----+
#   | type signature
#   | ...
#   +----+--
#
# LabComm2014 TYPE_DEF: (as SAMPLE_DEF, but signatures are hierarchical,
#                         i.e., may contain references to other types
#
#   +----+----+----+----+
#   | id = 0x04             (packed32)
#   +----+----+----+----+
#   | length                (packed32)
#   +----+----+----+----+
#   | type number           (packed32)
#   +----+----+----+----+
#   | type name (UTF8)
#   | ...
#   +----+----+----+----+
#   | signature length      (packed32)
#   +----+----+----+----+
#   | type signature
#   | ...
#   +----+--
#
# LabComm2014 TYPE_BINDING
#
#   +----+----+----+----+
#   | id = 0x05             (packed32)
#   +----+----+----+----+
#   | length                (packed32)
#   +----+----+----+----+
#   | sample number         (packed32)
#   +----+----+----+----+
#   | type number           (packed32)
#   +----+----+----+----+
#
# LabComm2014 User data:
#
#   +----+----+----+----+
#   | id >= 0x00000040      (packed32)
#   +----+----+----+----+
#   | length                (packed32)
#   +----+----+----+----+
#   | user data
#   | ...
#   +----+--
#   
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

DEFAULT_VERSION = "LabComm2014.v1"

# Allowed packet tags
i_VERSION     = 0x01
i_SAMPLE_DEF  = 0x02
i_SAMPLE_REF  = 0x03
i_TYPE_DEF    = 0x04  
i_TYPE_BINDING= 0x05 
i_PRAGMA      = 0x3f
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
i_SAMPLE  = 0x28


# Version testing
def usePacketLength(version):
    return version in [ None, DEFAULT_VERSION ]

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
             self.encoder.encode_packed32(len(self.data))
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

class SAMPLE(primitive):

    def encode_decl(self, encoder):
        return encoder.encode_type(i_SAMPLE)

    def encode(self, encoder, value):
        if not isinstance(value, type_decl):
            # Probably trying to encode a sample class, grab signature
            value = value.signature
        return encoder.encode_int(encoder.ref_to_index.get(value, 0))
    
    def decode(self, decoder, obj=None):
        return decoder.decode_ref()

    def new_instance(self):
        return ""

    def __repr__(self):
        return "labcomm.SAMPLE()"

#
# Aggregate types
#
class sampledef_or_sampleref_or_typedef(type_decl):
    def __init__(self, name=None, decl=None):
        self.name = name
        self.decl = decl

    def encode_decl(self, encoder):
        encoder.encode_type(self.type_index)
        with length_encoder(encoder) as e1:
            e1.encode_type(self.get_index(encoder))
            e1.encode_string(self.name)
            with length_encoder(e1) as e2:
                self.decl.encode_decl(e2)

    def encode(self, encoder, value):
        self.decl.encode(encoder, value)

    def decode_decl(self, decoder):
        index = decoder.decode_type_number()
        name = decoder.decode_string()
        if usePacketLength(decoder.version):
            length = decoder.decode_packed32()
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

class sample_def(sampledef_or_sampleref_or_typedef):
    type_index = i_SAMPLE_DEF
    type_name = 'sample'

    def get_index(self, encoder):
        return encoder.decl_to_index[self]

    def add_index(self, decoder, index, decl):
        decoder.add_decl(decl, index)

    def rename(self, name):
        return sample_def(name=name, decl=self.decl)
        
    
class sample_ref(sampledef_or_sampleref_or_typedef):
    type_index = i_SAMPLE_REF
    type_name = 'sample_ref'
    
    def __init__(self, name=None, decl=None, sample=None):
        self.name = name
        self.decl = decl
        if sample == None and name != None and decl != None:
            self.sample = sample_def(name, decl)
        else:
            self.sample = sample

    def get_index(self, encoder):
        return encoder.ref_to_index[self.sample]

    def add_index(self, decoder, index, decl):
        decoder.add_ref(decl, index)

class typedef(sampledef_or_sampleref_or_typedef):
    type_index = i_TYPE_DEF
    type_name = 'typedef'

    def encode_decl(self, encoder):
        raise Exception("typedef's are disabled")
        self.decl.encode_decl(encoder)

    def encode(self, encoder, value):
        raise Exception("typedef's are disabled")
        self.decl.encode(encoder, value)

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
        encoder.encode_packed32(len(self.indices))
        for i in self.indices:
            encoder.encode_packed32(i)
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
                encoder.encode_packed32(shape[i])
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
        n_indices = decoder.decode_packed32()
        indices = []
        for i in range(n_indices):
            index = decoder.decode_packed32()
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
                i = decoder.decode_packed32()
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
                i = decoder.decode_packed32()
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
        encoder.encode_packed32(len(self.field))
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
        n_field = decoder.decode_packed32()
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
SAMPLE_REF = sample_ref()

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
        self.add_decl(SAMPLE_REF, i_SAMPLE_REF)

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
        self.add_decl(SAMPLE(), i_SAMPLE)
        
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
        if not isinstance(ref, type_decl):
            # Probably trying to register a sample class, grab signature
            ref = ref.signature
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
    def __init__(self, writer, version=DEFAULT_VERSION, codec=None):
        super(Encoder, self).__init__(codec)
        self.writer = writer
        self.version = version
        if self.version ==  DEFAULT_VERSION:
            self.encode_type(i_VERSION)
            with length_encoder(self) as e:
                e.encode_string(version)
        elif self.version == None:
            pass # Don't send version on a length encoder
        else:
            raise Exception("Unsupported labcomm version %s" % self.version)    

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
 
    def add_ref(self, ref, index=0):
        if not isinstance(ref, type_decl):
            # Trying to register a sample class
            ref = ref.signature
        decl = sample_ref(name=ref.name, decl=ref.decl, sample=ref)
        if index == 0:
            self.writer.mark_begin(ref, None)
            if super(Encoder, self).add_ref(decl, index):
                decl.encode_decl(self)
            self.writer.mark_end(ref, None)
 
    def encode(self, object, decl=None):
        if decl == None:
            name = self.type_to_name[object.__class__]
            decl = self.name_to_decl[name]
        if not isinstance(decl, type_decl):
            decl = decl.signature
        self.writer.mark_begin(decl, object)
        self.encode_type_number(decl)
        with length_encoder(self) as e:
            decl.encode(e, object)
        self.writer.mark_end(decl, object)

    def encode_type_number(self, decl):
        try:
            self.encode_type(self.decl_to_index[decl])
        except KeyError:
            decl.encode_decl(self)
            
    def encode_packed32(self, v):
        if self.version in [ None, DEFAULT_VERSION ]:
            v = v & 0xffffffff
            tmp = [ v & 0x7f ]
            v = v >> 7
            while v:
                tmp.append(v & 0x7f | 0x80)
                v = v >> 7
            for c in reversed(tmp):
                self.encode_byte(c) 
        else :
            raise Exception("Unsupported labcomm version %s" % self.version)

    def encode_type(self, index):
        self.encode_packed32(index)
            
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
	self.encode_packed32(len(s));
	self.pack("%ds" % len(s),s)

class Decoder(Codec):
    def __init__(self, reader, version=DEFAULT_VERSION):
        super(Decoder, self).__init__()
        self.reader = reader
        self.version = version
        self.handlers = {}

    def register_handler(self, sig, handler):
        self.handlers[str(sig)] = handler

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

    def skip(self, length):
        for _ in xrange(length):
            self.decode_byte()

    # kludge, should really check if the index exists in self.version
    def skip_or_raise(self, length, index):
        if usePacketLength(self.version):
            self.skip(length)
        else:    
            raise Exception("Invalid type index %d" % index)

    def runOne(self):
        data,decl = self.decode()
        if decl:
            if data:
                if str(decl) in self.handlers:
                    handler = self.handlers[str(decl)]
                    handler(data)
                else:
                    print ("No handler for %s" % decl.name )


    def decode(self):
        while True:
            index = self.decode_type_number()
            if usePacketLength(self.version):
                length = self.decode_packed32()
            if index != i_VERSION:
                break
            else:
                other_version = self.decode_string()  
                if self.version != other_version:
                    raise Exception("LabComm version mismatch %s != %s" %
                                    (version, other_version))
        if index == i_SAMPLE_DEF:
            decl = self.index_to_decl[index].decode_decl(self)
            value = None
        elif index == i_SAMPLE_REF:
            decl = self.index_to_decl[index].decode_decl(self)
            value = None
        elif index == i_TYPE_DEF:
            raise Exception("typedef's are disabled")
            self.skip_or_raise(length, index) 
            decl = None
            value = None
        elif index == i_TYPE_BINDING:
            raise Exception("typebindings's are disabled")
            self.skip_or_raise(length, index) 
            decl = None
            value = None
        elif index == i_PRAGMA:
            raise Exception("pragma's are disabled")
            self.skip_or_raise(length, index) 
            decl = None
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
    
    def decode_packed32(self):
        if self.version in [ DEFAULT_VERSION ] :
            result = 0
            while True:
                tmp = self.decode_byte()
                result = (result << 7) | (tmp & 0x7f)
                if (tmp & 0x80) == 0:
                    break
            return result
        else :
            raise Exception("Unsupported labcomm version %s" % self.version)

    def decode_type_number(self):
        return self.decode_packed32()
        
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
        length = self.decode_packed32()
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

