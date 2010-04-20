from __future__ import print_function, with_statement
import os
import signal
import subprocess
import settings
from VidFeed import VidFeed
from datetime import datetime
from time import time
from logger import log

def launch(vfeed):
    """
    Launch a new ffserver/ffmpeg live feed
    """
    if settings.DEBUG:
        dummy_launch(vfeed)
        return

    vfeed.ffserver_proc = subprocess.Popen(ffserver_args(vfeed),
                                           stdin=subprocess.PIPE,
                                           stdout=subprocess.PIPE)
    log('started ffserver proc {0} with config file {1}'.format(
        str(vfeed.ffserver_proc.pid), vfeed.config_file))

    vfeed.ffmpeg_proc = subprocess.Popen(ffmpeg_args(vfeed),
                                         stdin=subprocess.PIPE,
                                         stdout=subprocess.PIPE)
    log('started ffmpeg proc ' + str(vfeed.ffmpeg_proc.pid))

def kill(vfeed):
    """
    Kill an existing ffserver/ffmpeg live feed
    """
    os.kill(vfeed.ffserver_proc.pid, signal.SIGTERM)
    vfeed.ffserver_proc.wait()
    log('ffserver proc {0} terminated with status {1}'.format(
        str(vfeed.ffserver_proc.pid), vfeed.ffserver_proc.returncode))

    os.kill(vfeed.ffmpeg_proc.pid, signal.SIGTERM)
    vfeed.ffmpeg_proc.wait()
    log('ffmpeg proc {0} terminated with status {1}'.format(
        str(vfeed.ffmpeg_proc.pid), vfeed.ffmpeg_proc.returncode))

def ffserver_args(vfeed):
    return ['ffserver', '-f', vfeed.config_file]

def ffmpeg_args(vfeed):
    return [
        'ffmpeg',
        '-f', 'mjpeg',
        '-i', vfeed.feed_url,
        settings.FEEDER_URLS.format(vfeed.port, vfeed.feed_name)
    ]

def capture_archive(vfeed, object_id, qos):
    """
    Saves 30 seconds of the given feed to the archives directory.
    Returns the filename for the archive file.
    """
    # ffmpeg -f mjpeg -i http://10.176.14.66/video.cgi -t 30 -f flv clip.flv
    archive_file = '{0}-{1}-{2}.flv'.format(
        str(qos), str(object_id), str(int(time())))
    if not settings.DEBUG:
        subprocess.Popen(
            ['ffmpeg', '-f', 'mjpeg', '-i', vfeed.feed_url,
             '-t', str(settings.ARCHIVE_DURATION), '-f', 'flv',
             archive_file],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE)
    vfeed.last_archived = datetime.now()
    log('archiving feed: {0}, object:{1}, qos: {2}'.format(
        vfeed.feed_url, str(object_id), str(qos)))
    return archive_file

def capture_screenshot(vfeed, object_id, qos):
    """
    Creates a screenshot for the specified feed.
    Returns the filename for the screenshot.
    """
    # ffmpeg -f mjpeg -i http://10.176.14.66/video.cgi -t 30 -f flv clip.flv
    screenshot_file = '{0}-{1}-{2}.jpg'.format(
        str(qos), str(object_id), str(int(time())))
    if not settings.DEBUG:
        subprocess.Popen(
            ['ffmpeg', '-f', 'mjpeg', '-i', vfeed.feed_url,
             '-f', 'image2', '-t', '0.001', screenshot_file],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE)
    log('took screenshot for feed: {0}, object:{1}, qos: {2}'.format(
        vfeed.feed_url, str(object_id), str(qos)))
    return screenshot_file

def dummy_launch(vfeed):
    """
    Runs dummy processes that don't do anything in place of ffmpeg and
    ffserver processes.
    """
    vfeed.ffserver_proc = subprocess.Popen(['./ffserver-dummy'])
    log('started ffserver proc {0} with config file {1}'.format(
        str(vfeed.ffserver_proc.pid), vfeed.config_file))

    vfeed.ffmpeg_proc = subprocess.Popen(['./ffmpeg-dummy'])
    log('started ffmpeg proc ' + str(vfeed.ffmpeg_proc.pid))

# Testing for the archiving functions
if __name__ == '__main__':
    vf = VidFeed()
    vf.feed_url = 'http://fake-url/video.cgi'
    capture_archive(vf, 3, 0.76)
    capture_screenshot(vf, 3, 0.76)
