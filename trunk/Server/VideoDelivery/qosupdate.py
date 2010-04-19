"""
This module handles the details of communicating with the QoS server.
"""

import settings
from datetime import datetime
from VidFeed import VidFeed
from logger import transcribe

def parse(in_lines):
    """
    Takes input from the QoS server and returns a list of VidFeed objects
    corresponding to the server input. This function returns a 3-tuple
    (timestamp, add/update feeds list, delete feeds list). This function
    throws an exception if the message could not be parsed.
    """
    if settings.DEBUG:
        s = '>>> From QoS Server\n'
        for ln in in_lines:
            s += ln + '\n'
        transcribe(s)

    if len(in_lines) < 2 or in_lines[1] != '':
        raise Exception('could not parse QoS message')

    timestamp = in_lines[0] #TODO: parse into datetime object
    if len(in_lines) == 2:
        return (timestamp, [], [])

    addrobots = []
    rmrobots = []
    for ln in in_lines[2:]:
        delmode = ln.startswith('DELETE')
        if delmode:
            ln = ln[7:]
        rbt = get_robot(ln)
        if rbt != None:
            if delmode:
                rmrobots.append(rbt)
            else:
                addrobots.append(rbt)

    addfeeds = []
    for r in addrobots:
        addfeeds.append(make_vfeed(r))

    rmfeeds = []
    for r in rmrobots:
        rmfeeds.append(make_vfeed(r))

    return (timestamp, addfeeds, rmfeeds)

def make_vfeed(r):
    """
    Takes a parsed robot tuple and creates a VidFeed object.
    """
    vf = VidFeed()
    vf.feed_url = r[0]
    for o in r[1]:
        vf.objects.append(o)
    return vf

def get_robot(line):
    """
    Returns a tuple (cam_url, qos_objects) for the input line where
    qos_objects is a list of (obj_id, QoS) tuples. Returns None if
    the format is invalid.
    """
    parts = line.split(';')

    # Only URL is given for delete statements
    lp = len(parts)
    if lp == 1 or lp == 2:
        return (parts[0], [])

    # Check for unbalanced object;qos; pairs
    if lp == 0 or lp % 2 != 0:
        return None

    # Get list of obj;qos; pairs as [(obj, qos), ...]
    cam_url = parts[0]
    qos_objects = []
    last_object = None
    parts = parts[:-1]

    try:
        for i in range(1, len(parts)):
            if i % 2 == 0:
                qos_objects.append((last_object, float(parts[i])))
            else:
                last_object = int(parts[i])
    except:
        return None # Do not attempt to parse invalid input

    return (cam_url, qos_objects)
    
def prepare(vfeeds):
    """
    Take a list of VidFeed objects and return a bytes object representing those
    objects in a format suitable to send to the QoS server.
    """
    s = str(datetime.today()) + '\n'
    for vf in vfeeds:
        s += vf.feed_url + ';' + vf.stream_url + ';\n'
    if settings.DEBUG:
        s = '>>> To QoS Server\n' + s
        transcribe(s)
    return s.encode()

if __name__ == '__main__':
    print('To test this module, run `python qosupdate-test.py`')
