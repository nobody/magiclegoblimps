#!/usr/bin/python3

import socket
import time
import random
from datetime import datetime

opts = {
    'max_devices': 12,
    'max_objects': 20,
    'max_obj/cam': 3,
}

devices = []

# how often each of these should change in seconds
obj_update_interval = [3,30]
obj_wait = 10.0
last_obj_update = time.time()

dev_wait = 32
dev_update_interval = [31,90]
last_dev_update = time.time()

class Device():
    def __init__(self):
        self.url = ''
        self.objects = []
        self.online = False

def init_data():
    """
    Read test data needed for simulation into python data structures.
    """
    url_template = 'http://10.176.14.{0}/video.cgi'
    base_ip = 60

    for i in range(0, opts['max_devices']):
        d = Device()
        d.url = url_template.format(str(base_ip + i))
        d.objects = update_objects()
        devices.append(d)
    update_devices()

def update_objects():
    """
    Get a list of objects viewable by a camera.
    """
    count = random.randint(0, opts['max_obj/cam'])
    obj_list = []
    nums_seen = [0]
    for i in range(0, count):
        o = {'id': 0, 'qos': 0.0}
        x = 0
        while x in nums_seen:
            x = random.randint(1, opts['max_objects'])
        o['id'] = x
        o['qos'] = random.random()
        obj_list.append(o)
    return obj_list

def update_qos():
    """
    Change qos data for each object viewable by each camera.
    """
    for dev in devices:
        for o in dev.objects:
            o['qos'] = random.random()

def update_devices():
    """
    Add/remove cameras from the system.
    """
    for d in devices:
        d.online = random.randint(0,2) == 1

def get_new_wait(update_interval):
    return update_interval[0] + random.random() * (update_interval[1]
            - update_interval[0])

def get_updates():
    global last_dev_update, dev_wait, last_obj_update, obj_wait
    now = time.time()
    if now - last_dev_update > dev_wait:
        update_devices()
        update_qos()
        last_dev_update = now
        dev_wait = get_new_wait(dev_update_interval)
    elif now - last_obj_update > obj_wait:
        for d in devices:
            d.objects = update_objects()
        last_obj_update = now
        obj_wait = get_new_wait(obj_update_interval)
    else:
        update_qos()

def get_status():
    """
    Generate a status update to send to the client.
    """
    get_updates()
    stat = str(datetime.today()) + '\n\n'
    for d in devices:
        if d.online:
            ln = d.url + ';'
            for o in d.objects:
                ln += str(o['id']) + ';'
                ln += str(o['qos']) + ';'
            stat += ln + '\n'
    return stat.encode()

def handle_response():
    """
    Handle data from the client in response to a push.
    """
    pass

def get_live_urls():
    """
    Reads the contents of the file and encodes them for sending over the
    socket. Used to test restreaming.
    """
    with open('./live-feeds.txt', 'r') as f:
        txt = f.read()
    return txt.encode()

def main():
    random.seed()
    init_data()

    s = socket.socket()
    addr = ('localhost', 5678)
    s.bind(addr)
    s.listen(5)

    (conn, caddr) = s.accept()
    while True:
        try:
            # push = get_status()
            push = get_live_urls()
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
    s.close()

if __name__ == '__main__':
    main()
    print('bye')

