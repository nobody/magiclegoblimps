"""
This module handles the details of communicating with the QoS server.
"""

from datetime import datetime
from vidcontrol import VidFeed, VidControl
from qosupdate import parse, prepare

# Test for sample response
# NOTE: The server-sim.py program must be running for this test to execute
# successfully. This is a side effect of using the VidControl class.
if __name__ == '__main__':
    # test reading a QoS server response
    with open('./sample-qos-response.txt', 'r') as f:
        lines = f.readlines()
    feed_data = parse(lines)
    print(feed_data[0]) # timestamp
    for vf in feed_data[1]:
        print(vf.feed_url + ': ', end='')
        for o in vf.objects:
            print('({0}, {1}) '.format(o[0], o[1]), end='')
        print()

    # test writing to the QoS server
    # we launch the feeds to generate the required stream urls
    vc = VidControl()
    for vf in feed_data[1]:
        vc.launch_feed(vf)

    s = prepare(feed_data[1])
    print()
    print(s)

    for vf in feed_data[1]:
        try:
            vc.kill_feed(vf)
        except:
            pass # we can ignore this error
