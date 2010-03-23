#!/usr/bin/python
"""
Decides when to start or kill new processes. Maintains a list of active
processes. Keeps track of all open ports, all generated config files, all
incoming camera feeds, and all live feed urls. Polls the QoS server for updated
feed and QoS status.
"""

import time
import socket

class VidFeed():
    """
    A container for all information regarding a particular video feed.
    """

    URL_TEMPLATE = 'rtmp://{0}:{1}/stream-{2}.flv'

    def __init__(self, obj=None):
        self.port = 0
        self.config_file = ''
        self.ffserver_proc = None
        self.ffmpeg_proc = None
        self.feed_url = ''
        self.stream_urls = ('', '') # (in feed, out stream)
        self.encoding = {}
        self.objects = [] # list of tuples (object_id, QoS_rating)

        if not obj == None:
            self.stream_urls = (obj['cam'], '')
            self.objects.append((obj['object'], obj['QoS']))

    def __eq__(self, obj):
        if type(obj) is VidFeed:
            pass
        else:
            return self.stream_urls[0] == obj['cam'] \
                   and self.objects[0][0] == obj['object'] \
                   and self.objects[0][1] == obj['QoS']

class VidControl():
    """
    Responsible for the overall control of the video delivery server.
    """

    DEFAULT_PORT = 5678
    DEFAULT_ADDR = 'localhost'
    
    def __init__(self):
        # "constants"
        self.BUFFER_SIZE = 4096
        self.CONFIG_FILE_PREFIX = 'ffserver-'
        self.CONFIG_FILE_EXT = '.conf'
        self.POLL_TIME = 0.7

        self.connect_QoS()

        # complete info about all active feeds/streams
        self.feeds = []

        # redundant information may make certain operations eaiser
        self.ports = []
        self.config_files = []

    def connect_QoS(self, addr=DEFAULT_ADDR, port=DEFAULT_PORT):
        """
        Setup a socket for communicating with the QoS server
        """
        self.QoS_addr = (addr, port)
        self.QoS_server = socket.socket()
        self.QoS_server.connect(self.QoS_addr)

    def poll_QoS(self):
        """
        Gets the latest camera/object metrics from the QoS server.
        """
        self.QoS_server.send(b'update')
        response = self.QoS_server.recv(self.BUFFER_SIZE)

        # TODO: parse response into a list of VidFeeds
        return eval(response)

    def update_feeds(self, vfeeds):
        """
        Finds any new/missing feeds from QoS server then launches/kills
        corresponding processes to reflect the current state of the system.

        1. Find and kill feeds in self.feeds that are not in vfeeds
        2. Find and launch feeds not in self.feeds that are in vfeeds
        """
        for f in vfeeds:
            print(f['cam'], f['object'], f['QoS'])
            if not f in self.feeds:
                # TODO: create feed
                pass
            else:
                #TODO: update feed
                pass
        pass

    def launch_feed(self, vfeed):
        """
        - generate config files
        - launch procs
        - update data structures
        - update database
        """
        pass

    def kill_feed(self, vfeed):
        """
        - kill procs
        - delete config files
        - update data structures
        - update database
        """
        pass

    def runserver(self):
        """
        1. poll QoS
        2. remove old feeds/streams
        3. add new feeds/streams
        4. examine QoS for feeds
        5. archive video if necessary
        6. repeat
        """
        while True:
            try:
                latest_feeds = self.poll_QoS()
            except socket.error as ex:
                print('lost connection to server')
                # raise ex
                break
                
            self.update_feeds(latest_feeds)
            time.sleep(self.POLL_TIME)

    def killserver(self):
        self.QoS_server.close()

if __name__ == '__main__':
    try:
        vc = VidControl()
        vc.runserver()
    except KeyboardInterrupt:
        vc.killserver()
