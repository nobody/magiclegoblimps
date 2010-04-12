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
        # complete info about all active feeds/streams
        self.feeds = []
        # redundant information may make certain operations eaiser
        self.ports = []
        self.config_files = []
        self.next_port = 8090
        # used to generate configuration files
        self.config_gen = config.Generator()

    def connect_QoS(self, addr=settings.QOS_SERVER_URL,
                    port=settings.QOS_SERVER_PORT):
        """
        Setup a socket for communicating with the QoS server. Returns True if
        connection could be established and false otherwise. This function will
        retry connection attempts up to a max number of times at an interval
        specified in the settings file.
        """
        self.QoS_addr = (addr, port)
        for i in range(0, settings.MAX_CONNECTION_ATTEMPTS):
            try:
                self.QoS_server = socket.socket()
                self.QoS_server.connect(self.QoS_addr)
            except socket.error as ex: # connection failed, retry
                log(ex)
                log('Attempt ' + str(i) + ' to connect to the QoS server '
                    + ' failed miserably.')
                time.sleep(settings.CONNECTION_RETRY_INTERVAL)
            else: # we have a valid connection
                log('Connected to QoS server at ' + addr + ':' + str(port))
                return True
        # give up
        log('Could not connect to QoS server at ' + addr + ':' + str(port))
        return False

    def poll_QoS(self):
        """
        Gets the latest camera/object metrics from the QoS server.
        """
        #TODO: handle errors communicating to server
        response = self.QoS_server.recv(self.BUFFER_SIZE)
        pr = qosupdate.parse(response.decode('utf-8').splitlines(True))
        timestamp = pr[0]
        return pr[1]

    def reply_to_QoS(self):
        """
        Send a response back to the QoS server, so it knows we received correct
        data and that we are ready to receive more updates.
        """
        # TODO: handle error sending reply
        reply = qosupdate.prepare(self.feeds)
        self.QoS_server.send(reply)

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
        vfeed.feed_name = settings.FEED_NAME.format(strport)
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
        if not self.connect_QoS():
            return
        while True:
            try:
                latest_feeds = self.poll_QoS()
            except socket.error as ex:
                log(ex)
                print('lost connection to QoS server')
                if not self.connect_QoS():
                    break
                else:
                    continue
                
            self.update_feeds(latest_feeds)
            # TODO: QoS and archiving
            self.reply_to_QoS()

    def killserver(self):
        """
        Cleanup all existing feeds then shutdown the server.
        """
        while len(self.feeds) > 0:
            self.kill_feed(self.feeds[0])
            # TODO: handle spurious errors when killing feeds
        self.QoS_server.close()

def change_working_directory():
    """
    Change the working directory to the first valid directory given in the
    settings.ROOT_DIR list. If none of those is a valid directory, the program
    defaults to running in the current working directory.
    """
    for d in settings.ROOT_DIR:
        try:
            os.chdir(d)
        except:
            continue
        else:
            break

if __name__ == '__main__':
    change_working_directory()
    try:
        vc = VidControl()
        vc.runserver()
    except Exception as ex:
        log('Fatal error: ' + str(ex))
        vc.killserver()
        log('Video Delivery server shutdown normally')
