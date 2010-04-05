#!/usr/bin/python3

import socket

def get_status():
    with open('./sample-qos-response.txt', 'r') as f:
        response = f.read()
    return response.encode()

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
