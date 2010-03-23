#!/usr/bin/python3

import socket

def get_status():
    status = [
        {'cam':'192.168.1.120', 'object':3, 'QoS': 0.34},
        {'cam':'192.168.1.121', 'object':1, 'QoS': 0.45},
        {'cam':'192.168.1.122', 'object':0, 'QoS': 0.0},
        {'cam':'192.168.1.123', 'object':4, 'QoS': 0.19},
    ]
    return str(status).encode()

if __name__ == '__main__':
    s = socket.socket()
    addr = ('localhost', 5678)
    s.bind(addr)
    s.listen(5)

    (conn, caddr) = s.accept()
    while True:
        req = conn.recv(4096)
        if len(req) == 0:
            break
        print(str(req))
        if req == b'update':
            resp = get_status()
            conn.send(resp)
        else:
            conn.send(b'error')
