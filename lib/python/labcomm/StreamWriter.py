import labcomm

class StreamWriter:
    
    def __init__(self, stream):
        self.stream = stream
        pass
            
    def write(self, data):
        self.stream.write(data)
        pass

    def mark(self):
        self.stream.flush()
        pass

    pass
