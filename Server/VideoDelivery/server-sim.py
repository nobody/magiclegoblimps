#!/usr/bin/python3

from __future__ import print_function
import socket
import time
import random
from datetime import datetime
import sys
import settings

def usage():
    print('usage: python server-sim.py test-file.txt')

def handle_response():
    """
    Handle data from the client in response to a push.
    """
    pass

def get_message_sequence():
    """
    Reads the contents of the file and encodes them for sending over the
    socket. Used to test restreaming.
    """
    if len(sys.argv) != 2:
        usage()
        exit(1)
    text = ''
    try:
        with open(sys.argv[1], 'r') as f:
            text = f.read()
    except:
        print('count not read file: ' + sys.argv[1])
        exit(1)
    messages = text.split('>>>\n')
    return messages

def get_message(messages, i):
    message = "{0}\n\n{1}"
    lines = messages[i].splitlines()
    url_lines = []
    for i in range(len(lines)):
        if lines[i][0] == '#':
            pass
        else:
            url_lines.append(lines[i])
    urls = '\n'.join(url_lines)
    timestamp = datetime.today().strftime('%Y-%b-%d %H:%M:%S')
    message = message.format(timestamp, urls)
    print('\n\nMessage:\n' + message)
    return message.encode()
    

def main():
    messages = get_message_sequence()
    s = socket.socket()
    addr = ('localhost', settings.QOS_SERVER_PORT)
    s.bind(addr)
    s.listen(5)

    (conn, caddr) = s.accept()
    for i in range(len(messages)):
        try:
            push = get_message(messages, i)
            conn.send(push)
            req = conn.recv(4096)
            time.sleep(1.0)
            if len(req) == 0:
                break
        except socket.error as ex1:
            print('socket.error: ' + str(ex1))
            break # simulator does not attempt to recover
        except Exception as ex2:
            print('error: ' + str(ex2))
            break
        except KeyboardInterrupt:
            break
    conn.close()

if __name__ == '__main__':
    main()
    print('bye')

