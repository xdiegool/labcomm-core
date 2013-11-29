import socket
import types

def flush(self):
    pass

socket.socket.write = socket.socket.send
socket.socket.read = socket.socket.recv
socket.socket.flush = flush

# class rwsocket(socket.socket):
#     write=socket.socket.send
#     read=socket.socket.recv

#     def flush(self):
#         pass

#     def accept(self):
#         sock, addr = super(rwsocket, self).accept()
#         # sock.write = types.MethodType(self.write, sock, sock.__class__)
#         # sock.read = types.MethodType(self.write, sock, sock.__class__)
#         sock.__class__ = rwsocket

#         return (sock, addr)
