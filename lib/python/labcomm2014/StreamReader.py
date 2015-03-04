class StreamReader:

    def __init__(self, stream):
        self.stream = stream
        pass

    def read(self, count):
        result = self.stream.read(count)
        if len(result) == 0:
            raise EOFError()
        return result

    def mark(self, value, decl):
        pass
        
    pass
