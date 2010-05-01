#!/usr/bin/python2.6
"""
Decides when to start or kill new processes. Maintains a list of active
processes. Keeps track of all open ports, all generated config files, all
incoming camera feeds, and all live feed urls. Polls the QoS server for updated
feed and QoS status.
"""
from __future__ import print_function, with_statement
import os
import signal
import time
import socket
import sys

import config
import ffserver
import settings
from logger import log, transcribe, clearlogs
import qosupdate
from VidFeed import VidFeed
import archives
import db

active_video_controller = None

def sigterm_handler(signum, stack_frame):
    if active_video_controller is not None:
        active_video_controller.killserver()
    log('R.I.P.')
    exit(signum)

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
        log('Attempting to connect to database: ' + settings.MySQL_DATABASE)
        self.conn = db.connect()
        log('Connected to database: ' + settings.MySQL_DATABASE)

    def connect_QoS(self, addr=settings.QOS_SERVER_URL,
                    port=settings.QOS_SERVER_PORT):
        """
        Setup a socket for communicating with the QoS server. Returns True if
        connection could be established and false otherwise. This function will
        retry connection attempts up to a max number of times at an interval
        specified in the settings file.
        """
        self.QoS_addr = (addr, port)
        # for i in range(0, settings.MAX_CONNECTION_ATTEMPTS):
        while True:
            try:
                log('Attempting to connect to QoS server at '
                    + addr + ':' + str(port))
                self.QoS_server = socket.socket()
                self.QoS_server.connect(self.QoS_addr)
            except socket.error as ex: # connection failed, retry
                log('Failed to connect to QoS server: ' + str(ex))
                time.sleep(settings.CONNECTION_RETRY_INTERVAL)
            else: # we have a valid connection
                log('Connected to QoS server at ' + addr + ':' + str(port))
                return True
        # give up
        log('Could not connect to QoS server at ' + addr + ':' + str(port))
        return False

    def reconnect_QoS(self, addr=settings.QOS_SERVER_URL,
                      port=settings.QOS_SERVER_PORT):
        """
        If we think we've lost the connection to the QoS server, this function
        makes sure that the connection is either really dropped or closed (in
        the case that the connection was still good) before attempting to
        establish a new connection.
        """
        log('Lost connection to QoS server, attempting to reconnect')
        try:
            self.QoS_server.close()
        except Exception as e: # probably already closed
            log('Error closing connection to QoS server: ' + str(e))
        finally:
            time.sleep(1)
            return self.connect_QoS()

    def poll_QoS(self):
        """
        Gets the latest camera/object metrics from the QoS server. This
        function returns a list of VidFeed objects, or None if the input data
        was invalid.
        """
        response = self.QoS_server.recv(self.BUFFER_SIZE)
        if not response: # assume dropped connection
            raise socket.error('No data received from QoS server')

        try:
            lines = response.decode('utf-8').splitlines()
            pr = qosupdate.parse(lines)
        except Exception as ex:
            log('Invalid data received from server: ' + str(ex))
            return None
        return pr

    def reply_to_QoS(self, fail=False):
        """
        Send a response back to the QoS server, so it knows we received correct
        data and that we are ready to receive more updates.
        """
        if fail:
            reply = 'FAIL\n'.encode()
            transcribe('>>> To QoS Server:\nFAIL\n')
        else:
            reply = 'OK\n'.encode()
            transcribe('>>> To QoS Server:\nOK\n')

        try:
            self.QoS_server.send(reply)
        except socket.error as e:
            log('socket.error sending reply to server: ' + str(e))
            self.reconnect_QoS()

    def update_feeds(self, addfeeds, rmfeeds):
        """
        Finds any new/missing feeds from QoS server then launches/kills
        corresponding processes to reflect the current state of the system.

        1. Find and kill feeds in self.feeds that are not in vfeeds
        2. Find and launch feeds not in self.feeds that are in vfeeds
        """
        if rmfeeds is not None:
            for f in rmfeeds:
                i = self.feeds.index(f)
                self.kill_feed(self.feeds[i])

        if addfeeds is not None:
            for f in addfeeds:
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
        log('Launching feed in_url:{0}, out_url:{1}'.format(
                vfeed.feed_url, vfeed.stream_url))

        # generate a new config file
        config_data = {
            'port': strport,
            'feed': vfeed.feed_name,
            'stream': vfeed.stream_name
        }
        self.config_gen.render(config_data, settings.CONFIG_TEMPLATE,
                               vfeed.config_file)

        ffserver.launch(vfeed)

        # update client database
        db.addCameraFeed(self.conn, vfeed.cam_id, vfeed.stream_url)

    def kill_feed(self, vfeed):
        """
        - kill procs
        - delete config files
        - update data structures
        - update csv files
        """
        log('Killing feed in_url:{0}, out_url:{1}'.format(
                vfeed.feed_url, vfeed.stream_url, vfeed.config_file))
        ffserver.kill(vfeed)
        archives.cancel_archive(vfeed)
        os.remove(vfeed.config_file)
        self.feeds.remove(vfeed)

        # update client database
        db.removeCameraFeed(self.conn, vfeed.cam_id, vfeed.stream_url)

    def get_stream_url(self, vfeed):
        return settings.LIVE_FEED_URLS.format(settings.CURRENT_IP, vfeed.port,
                                              vfeed.stream_name)

    def do_archives(self):
        """
        Test feeds to see if any should be archived (the QoS for an object in
        view exceeds a certain threshold) and archives the feed if necessary.
        """
        for vf in self.feeds:
            for i in range(len(vf.objects)):
                if vf.objects[i][1] > settings.ARCHIVE_QOS_THRESHOLD:
                    archives.create_archive(vf, i)
                    break # archive max. of 1 object per feed at a given time

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
                feed_updates = self.poll_QoS()
            except socket.error as ex:
                log('Lost connection to QoS server: ' + str(ex))
                if not self.reconnect_QoS():
                    break
                else:
                    continue
            except Exception as e:
                log('Weird error: ' + str(e))
                continue
                
            if feed_updates is None:
                self.reply_to_QoS(fail=True)
                continue # QoS update could not be parsed
            else:
                self.update_feeds(feed_updates[1], feed_updates[2])

            try:
                self.do_archives()
            except Exception as e:
                log('Archiving failed: ' + str(e))

            self.reply_to_QoS()

    def killserver(self):
        """
        Cleanup all existing feeds then shutdown the server.
        """
        while len(self.feeds) > 0:
            self.kill_feed(self.feeds[0])
        self.QoS_server.close()
        db.close(self.conn)

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
    signal.signal(signal.SIGTERM, sigterm_handler)
    change_working_directory()
    clearlogs()
    try:
        vc = VidControl()
        active_video_controller = vc
        vc.runserver()
    except Exception as ex:
        log('Fatal error: ' + str(ex))
        if settings.DEBUG:
            raise ex
    finally:
        vc.killserver()
    log('Video Delivery server shutdown normally')
