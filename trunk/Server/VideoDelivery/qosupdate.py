"""
This module handles the details of communicating with the QoS server.
"""

from datetime import datetime
from VidFeed import VidFeed

def parse(in_lines):
    """
    Takes input from the QoS server and returns a list of VidFeed objects
    corresponding to the server input. This function returns a 2-tuple
    (timestamp, VidFeed list).
    """
    object_mode = True
    timestamp = in_lines[0] #TODO: parse into datetime object
    objects = []
    robots = []

    for ln in in_lines[1:]:
        if ln == '\n':
            object_mode = False
            continue
        elif object_mode: # object list
            obj = get_object(ln)
            if obj != None:
                objects.append(obj)
        else: # robot list
            rbt = get_robot(ln)
            if rbt != None:
                robots.append(rbt)

    vfeeds = []
    for r in robots:
        vf = VidFeed()
        vf.feed_url = r[0]
        for o in r[1]:
            vf.objects.append(o)
        vfeeds.append(vf)

    return (timestamp, vfeeds)

def get_object(line):
    """
    Returns a tuple (obj_id, obj_name) for the object represented by
    the given line. Returns None if the format is invalid.
    """
    parts = line.split(';')
    if len(parts) != 3:
        return None
    return (int(parts[0]), parts[1])

def get_robot(line):
    """
    Returns a tuple (cam_url, qos_objects) for the input line where
    qos_objects is a list of (obj_id, QoS) tuples. Returns None if
    the format is invalid.
    """
    qos_objects = []
    parts = line.split(';')
    if len(parts) < 2:
        return None
    parts = parts[:-1]
    cam_url = parts[0]
    last_object = None
    for i in range(1, len(parts)):
        if i % 2 == 0:
            qos_objects.append((last_object, float(parts[i])))
        else:
            last_object = int(parts[i])
    return (cam_url, qos_objects)
    
def prepare(vfeeds):
    """
    Take a list of VidFeed objects and return a string representing those
    objects in a format suitable to send to the QoS server.
    """
    s = str(datetime.today()) + '\n'
    for vf in vfeeds:
        s += vf.feed_url + ';' + vf.stream_url + ';\n'
    return s
