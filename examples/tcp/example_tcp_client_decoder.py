#!/usr/bin/python

import os
import sys
import socket
import rwsocket

if not any('labcomm' in p for p in sys.path):
    sys.path.append('../../lib/python')
import labcomm


if __name__ == "__main__":
    print "Trying to connect..."
    host = sys.argv[1] #'localhost'
    port = sys.argv[2] #'8081'
    addr = (host,int(port))
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(addr)
    print "Connected!"

    d = labcomm.Decoder(labcomm.StreamReader(sock))

    while True:
        try:
            data,decl = d.decode()
            if data:
                print data
        except KeyError as e:
            print 'KeyError : "%s"' % str(e)
            break 
        except:
            print 'exception...', sys.exc_info()[0]
            break
