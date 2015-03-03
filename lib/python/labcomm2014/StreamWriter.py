class StreamWriter:
    
    def __init__(self, stream):
        self.stream = stream
        pass
            
    def write(self, data):
        self.stream.write(data)
        pass

    def mark_begin(self, decl, value):
        pass

    def mark_end(self, decl, value):
        self.stream.flush()
        pass

    pass
