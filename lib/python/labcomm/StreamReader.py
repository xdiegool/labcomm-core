class StreamReader:

    def __init__(self, stream):
        self.stream = stream
        pass

    def start(self, decoder, version):
        other_version = decoder.decode_string()
        if version != other_version:
            raise Exception("LabComm version mismatch %s != %s" %
                            (version, other_version))
        pass

    def read(self, count):
        result = self.stream.read(count)
        if len(result) == 0:
            raise EOFError()
        return result

    def mark(self, value, decl):
        pass
        
    pass
