#!/usr/bin/python3

import socket
import time

def wait_for_server(interval=1.0):
    """
    Establish a connection with the server. If a connection cannot be
    established, wait for the given interval (in seconds) and try again and
    again and again until a connection can be established or the user
    terminates the process.
    """
    while True:
        try:
            s = socket.socket()
            addr = ('localhost', 5678)
            s.connect(addr)
        except socket.error as ex:
            print(ex)
            time.sleep(interval)
        else:
            return s

def run_client():
    """
    Continuously sends and receives data from the server. If the connection to
    the server gets interrupted, wait until a new connection can be established
    then resume as normal.
    """
    s = wait_for_server()
    while True:
        try:
            resp = s.recv(4096)
            s.send(b'got it')
        except socket.error as ex:
            print(ex)
            s = wait_for_server()
            continue

        if len(resp) == 0:
            pass
        print('\n>>>\n')
        print(resp.decode('utf-8'))

if __name__ == '__main__':
    try:
        run_client()
    except KeyboardInterrupt:
        print('User terminated the process')
