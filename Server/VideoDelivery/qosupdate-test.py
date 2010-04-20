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
    with open('./sample-qos-response.txt', 'r') as f:
        text = f.read()

    messages = text.split('>>>\n')
    for m in messages[1:]:
        lines = m.splitlines()
        feed_data = parse(lines)

        print('\n>>>\n')
        print('Timestamp: ' + str(feed_data[0]))

        print('Add/update feeds:')
        for x in feed_data[1]:
            print('\t' + str(x))

        print('Delete feeds:')
        for x in feed_data[2]:
            print('\t' + str(x))
