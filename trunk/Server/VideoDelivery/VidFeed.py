#!/usr/bin/python2.6

class VidFeed():
    """
    A container for all information regarding a particular video feed.
    """

    def __init__(self, vfeed=None):
        self.port = 0
        self.feed_url = '' # input from IP cams
        self.cam_id = 0 # database id for camera

        self.config_file = '' # ffserver config file
        self.feed_name = '' # from ffmpeg
        self.stream_name = '' # filename w/o ext. for flv stream

        self.stream_url = '' # url for the flv stream
        self.objects = [] # list of tuples (object_id, QoS_rating)
        self.last_update = None # timestamp

        self.encoding = {}
        self.ffserver_proc = None
        self.ffmpeg_proc = None

        self.last_archived = None # timestamp for start of last archive
        self.archive_proc = None

        if not vfeed is None:
            self.feed_url = vfeed.feed_url
            self.objects = vfeed.objects
            self.cam_id = vfeed.cam_id

    def __eq__(self, vfeed):
        return vfeed.feed_url == self.feed_url

    def update(self, vfeed):
        self.objects = vfeed.objects

    def __repr__(self):
        return "VidFeed(cam:{0} feed:'{1}', stream:'{2}', obj/qos:{3})".format(
            self.cam_id, self.feed_url, self.stream_url, str(self.objects))
