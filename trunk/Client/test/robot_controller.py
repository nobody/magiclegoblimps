#!/usr/bin/python
import socket
import sys
#HOST = '0-22-19-17-37-ad.dyn.utdallas.edu'
#HOST = "titus.collegiumv.org"
HOST = "localhost"
PORT = 10001              # Arbitrary non-privileged port

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((HOST, PORT))
string = ''
for arg in sys.argv[1:]:
    string += arg+'%'
print string
s.send(string+'\n')
s.close()
