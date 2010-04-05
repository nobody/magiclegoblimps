#!/usr/bin/python
"""
Decides when to start or kill new processes. Maintains a list of active
processes. Keeps track of all open ports, all generated config files, all
incoming camera feeds, and all live feed urls. Polls the QoS server for updated
feed and QoS status.
"""

import os
import time
import socket
import sys

import config
import ffserver
import settings
from logger import log
import qosupdate
from VidFeed import VidFeed

class VidControl():
    """
    Responsible for the overall control of the video delivery server.
    """

    def __init__(self):
        # "constants"
        self.BUFFER_SIZE = 4096
        self.CONFIG_FILE_PREFIX = 'ffserver-'
        self.CONFIG_FILE_EXT = '.conf'
        self.POLL_TIME = 0.7
        
        self.config_gen = config.Generator()

        self.connect_QoS()
        log('Connected to QoS server at ' + settings.QOS_SERVER_URL + ':' +
                str(settings.QOS_SERVER_PORT))

        # complete info about all active feeds/streams
        self.feeds = []

        # redundant information may make certain operations eaiser
        self.ports = []
        self.config_files = []
        self.next_port = 8090

    def connect_QoS(self, addr=settings.QOS_SERVER_URL,
                    port=settings.QOS_SERVER_PORT):
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
        pr = qosupdate.parse(response.decode('utf-8').splitlines(True))
        timestamp = pr[0]
        return pr[1]

    def update_feeds(self, vfeeds):
        """
        Finds any new/missing feeds from QoS server then launches/kills
        corresponding processes to reflect the current state of the system.

        1. Find and kill feeds in self.feeds that are not in vfeeds
        2. Find and launch feeds not in self.feeds that are in vfeeds
        """
        for f in self.feeds:
            if not f in vfeeds: # kill feed
                self.kill_feed(f)

        for f in vfeeds:
            if f in self.feeds: # update feed
                i = self.feeds.index(f)
                self.feeds[i].update(f)
            else: # launch feed
                feed = VidFeed(f)
                self.feeds.append(feed)
                self.launch_feed(feed)

    def launch_feed(self, vfeed):
        """
        - generate config files
        - launch procs
        - update data structures
        - update csv files
        """
        # initialize vfeed object
        vfeed.port = self.next_port
        strport = str(vfeed.port)
        vfeed.config_file = settings.CONFIG_FILE_DIR + \
                            settings.CONFIG_FNAME.format(strport)
        vfeed.stream_name = settings.STREAM_NAME.format(strport)
        vfeed.feed_name = settings.FEED_NAME
        vfeed.stream_url = self.get_stream_url(vfeed)
        self.next_port += 1
        log('Launching feed in_url:{0}, out_url:{1}, config:{2}'.format(
                vfeed.feed_url, vfeed.stream_name, vfeed.config_file))

        # generate a new config file
        config_data = {
            'port': strport,
            'feed': vfeed.feed_name,
            'stream': vfeed.stream_name
        }
        self.config_gen.render(config_data, settings.CONFIG_TEMPLATE,
                               vfeed.config_file)

        ffserver.launch(vfeed)
        # TODO: update csv files

    def kill_feed(self, vfeed):
        """
        - kill procs
        - delete config files
        - update data structures
        - update csv files
        """
        log('Killing feed in_url:{0}, out_url:{1}, config:{2}'.format(
                vfeed.feed_url, vfeed.stream_name, vfeed.config_file))
        ffserver.kill(vfeed)
        os.remove(vfeed.config_file)
        self.feeds.remove(vfeed)
        # TODO: update csv files

    def get_stream_url(self, vfeed):
        return settings.LIVE_FEED_URLS.format(settings.CURRENT_IP, vfeed.port,
                                              vfeed.stream_name)

    def runserver(self):
        """
        1. poll QoS
        2. remove old feeds/streams
        3. add new feeds/streams
        4. examine QoS for feeds
        5. archive video if necessary
        6. repeat
        """
        log('Video Delivery server running at ' + settings.CURRENT_IP)
        while True:
            try:
                latest_feeds = self.poll_QoS()
            except socket.error as ex:
                print('lost connection to server')
                # raise ex
                break
                
            self.update_feeds(latest_feeds)
            # TODO: QoS and archiving
            time.sleep(self.POLL_TIME)

    def killserver(self):
        """
        Cleanup all existing feeds then shutdown the server.
        """
        while len(self.feeds) > 0:
            self.kill_feed(self.feeds[0])
        self.QoS_server.close()

if __name__ == '__main__':
    # root directory is different based on which computer I'm developing on
    for d in settings.ROOT_DIR:
        try:
            os.chdir(d)
        except:
            continue
        else:
            break
    # Fall back to current directory if no root given

    try:
        vc = VidControl()
        vc.runserver()
    except KeyboardInterrupt:
        vc.killserver()
        log('Video Delivery server shutdown normally')
